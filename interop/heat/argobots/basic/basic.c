#include <mpi.h>
#include <openmpi/mpiext/mpiext_continue_c.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <abt.h>

#include <unistd.h>
#include <stdarg.h>

int debug(const char *format, ...) {
#if DEBUG
  va_list args;
  va_start(args, format);

  int ret = vprintf(format, args);

  va_end(args);

  return ret;
#else
  return -1;
#endif
}

void attach_loop(int rank) {
  volatile int i = 0;
  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  printf("rank %d PID %d on %s ready for attach\n", rank, getpid(), hostname);
  fflush(stdout);
  while (0 == i) {
    sleep(5);
  }
}

typedef struct args {
  MPI_Request *cr_ptr;
  int *val_ptr;
  ABT_eventual val_eventual;
  int comm_started_flag;
} args_t;

#define NUM_XSTREAMS 8

#define NUM_THREADS 3

int main(void);
void proc0(void);
void proc1(void);
void task0(void *arg_ptr_);
void task1(void *arg_ptr_);
void poll_task(void *arg_ptr_);
void release_event(MPI_Status *status, void *data);

int main(void) {
  MPI_Init(NULL, NULL);
  ABT_init(0, NULL);


  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size != 2) {
    printf("ERROR: comm size must be 2\n");
    return 1;
  }

  int rank; 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#if GDB
  attach_loop(rank);
#endif

  switch (rank) {
    case 0:
      proc0();
      break;

    case 1:
      proc1();
      break;

    default:
      printf("ERROR: invalid rank: %d", rank);
      break;
  }

  debug("proc %d ready to finalize\n", rank);

  ABT_finalize();
  MPI_Finalize();

  debug("proc %d exiting\n", rank);

  return 0;
}

void proc0(void) {
  debug("proc 0\n");

  size_t num_xstreams = NUM_XSTREAMS;
  size_t num_threads = NUM_THREADS;

  /* Create pools. */  
  ABT_pool *pools = (ABT_pool *) malloc(sizeof(ABT_pool) * num_xstreams);
  for (size_t i = 0; i < num_xstreams; i++) {
    ABT_pool_create_basic(ABT_POOL_FIFO, ABT_POOL_ACCESS_MPMC, ABT_TRUE, &pools[i]);
  }

  /* Create schedulers. */
  ABT_sched *scheds = (ABT_sched *) malloc(sizeof(ABT_sched) * num_xstreams);
  for (size_t i = 0; i < num_xstreams; i++) {
    ABT_pool *tmp = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
    for (int j = 0; j < num_xstreams; j++) {
      tmp[j] = pools[(i + j) % num_xstreams];
    }
    ABT_sched_create_basic(ABT_SCHED_DEFAULT, num_xstreams, tmp, ABT_SCHED_CONFIG_NULL, &scheds[i]);
    free(tmp);
  }

  /* allocate execution streams */
  ABT_xstream *xstreams = (ABT_xstream *) malloc(sizeof(ABT_xstream) * num_xstreams);

  /* Set up a primary execution stream. */
  ABT_xstream_self(&xstreams[0]);
  ABT_xstream_set_main_sched(xstreams[0], scheds[0]);

  /* Create secondary execution streams. */
  for (size_t i = 1; i < num_xstreams; i++) {
    ABT_xstream_create(scheds[i], &xstreams[i]);
  }

  /* allocate threads */
  ABT_thread *threads = (ABT_thread *) malloc(sizeof(ABT_thread) * num_threads);

  MPI_Request cont_req;
  MPIX_Continue_init(&cont_req, MPI_INFO_NULL);

  int value = -1;

  args_t args = { &cont_req, &value, (ABT_eventual) NULL, 0 };
  ABT_eventual_create(0, &args.val_eventual);

  debug("proc0 tasks\n");

  //#pragma omp task depend(out:value) shared(value) detach(event)
  //qthread_fork(&task0, (void *) &args, NULL);
  int tid = 0;
  int pool_id = tid % num_xstreams;
  ABT_thread_create(pools[pool_id], task0, &args, ABT_THREAD_ATTR_NULL, &threads[tid]);

  //#pragma omp task depend(in:value) shared(value)
  //qthread_fork(&task1, (void *) &args, NULL);
  tid = 1;
  pool_id = tid % num_xstreams;
  ABT_thread_create(pools[pool_id], task1, &args, ABT_THREAD_ATTR_NULL, &threads[tid]);

  //#pragma omp task
  //qthread_fork(&poll_task, (void *) &args, NULL);
  tid = 2;
  pool_id = tid % num_xstreams;
  ABT_thread_create(pools[pool_id], poll_task, &args, ABT_THREAD_ATTR_NULL, &threads[tid]);

  //#pragma omp taskwait
  //qt_barrier_enter(args.barrier);
  for (size_t i = 0; i < num_threads; i++) {
    // join and free associated thread
    ABT_thread_free(&threads[i]);
  }

  /* Join secondary execution streams. */
  for (size_t i = 1; i < num_xstreams; i++) {
    ABT_xstream_join(xstreams[i]);
    ABT_xstream_free(&xstreams[i]);
  }

  /* Free allocated memory. */
  free(xstreams);
  free(pools);
  free(scheds);
  free(threads);
}

void proc1(void) {
  debug("proc1\n");
  int send_val = 17;
  MPI_Send(&send_val, 1, MPI_INT, 0, 425, MPI_COMM_WORLD);
  debug("sent %d\n", send_val);
  debug("proc1-end\n");
}


void task0(void *args_ptr_) {
  args_t *args_ptr = (args_t *) args_ptr_;
  debug("task0\n");
  MPI_Request req;
  MPI_Irecv(args_ptr->val_ptr, 1, MPI_INT, 1, 425, MPI_COMM_WORLD, &req);
  debug("t0: IReceive complete\n");
  MPIX_Continue(&req, &release_event, args_ptr_, MPI_STATUS_IGNORE, *args_ptr->cr_ptr);
  debug("t0: Continue complete\n");
  args_ptr->comm_started_flag = 1;
}


void task1(void *args_ptr_) {
  args_t *args_ptr = (args_t *) args_ptr_;
  debug("task1\n");

  ABT_eventual_wait(args_ptr->val_eventual, NULL);
  printf("RECEIVED %d\n", *args_ptr->val_ptr);
}


void poll_task(void *args_ptr_){
  args_t *args_ptr = (args_t *) args_ptr_;

  int flag = 0;

  do {
    debug("poll\n");
    if (args_ptr->comm_started_flag) {
      MPI_Test(args_ptr->cr_ptr, &flag, MPI_STATUS_IGNORE);
    }
    //#pragma omp taskyield
    //qthread_yield();
    ABT_thread_yield();
  } while(!flag);

  debug("poll exit\n");
}


void release_event(MPI_Status *status, void *data) {
  debug("release event\n");
  debug("data: %p\n", data);
  args_t *args_ptr = (args_t *) data;
  debug("ptr convert\n");
  ABT_eventual_set(args_ptr->val_eventual, NULL, 0);
  debug("release event end\n");
}
