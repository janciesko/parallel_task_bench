#include <mpi.h>
#include <openmpi/mpiext/mpiext_continue_c.h>
#include <stdio.h>
#include <stdint.h>
#include <qthread/qthread.h>
#include <qthread/barrier.h>

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
  aligned_t *val_ptr;
  qt_barrier_t *barrier;
  int comm_started_flag;
} args_t;

int main(void);
void proc0(void);
void proc1(void);
static aligned_t task0(void *arg_ptr_);
static aligned_t task1(void *arg_ptr_);
static aligned_t poll_task(void *arg_ptr_);
void release_event(MPI_Status *status, void *data);

int main(void) {
  MPI_Init(NULL, NULL);
  qthread_initialize();

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

  MPI_Finalize();

  debug("proc %d exiting\n", rank);

  return 0;
}

void proc0(void) {
  debug("proc 0\n");

  MPI_Request cont_req;
  MPIX_Continue_init(&cont_req, MPI_INFO_NULL);

  aligned_t value = 0;

  args_t args = { &cont_req, &value, qt_barrier_create(3, REGION_BARRIER), 0 };

  debug("proc0 tasks\n");

  qthread_empty(&value);

  //#pragma omp task depend(out:value) shared(value) detach(event)
  qthread_fork(&task0, (void *) &args, NULL);

  //#pragma omp task depend(in:value) shared(value)
  qthread_fork(&task1, (void *) &args, NULL);

  //#pragma omp task
  qthread_fork(&poll_task, (void *) &args, NULL);

  //#pragma omp taskwait
  qt_barrier_enter(args.barrier);
}

void proc1(void) {
  debug("proc1\n");
  int send_val = 17;
  MPI_Send(&send_val, 1, MPI_INT, 0, 425, MPI_COMM_WORLD);
  debug("sent %d\n", send_val);
  debug("proc1-end\n");
}


static aligned_t task0(void *args_ptr_) {
  args_t *args_ptr = (args_t *) args_ptr_;
  debug("task0\n");
  MPI_Request req;
  MPI_Irecv(args_ptr->val_ptr, 1, MPI_INT, 1, 425, MPI_COMM_WORLD, &req);
  debug("t0: IReceive complete\n");
  MPIX_Continue(&req, &release_event, args_ptr_, MPI_STATUS_IGNORE, *args_ptr->cr_ptr);
  debug("t0: Continue complete\n");
  args_ptr->comm_started_flag = 1;

  qt_barrier_enter(args_ptr->barrier);

  return 0;
}


static aligned_t task1(void *args_ptr_) {
  args_t *args_ptr = (args_t *) args_ptr_;
  debug("task1\n");

  qthread_readFF(NULL, args_ptr->val_ptr);
  printf("RECEIVED %lu\n", *args_ptr->val_ptr);

  qt_barrier_enter(args_ptr->barrier);

  return 0;
}


static aligned_t poll_task(void *args_ptr_){
  args_t *args_ptr = (args_t *) args_ptr_;

  int flag = 0;

  do {
    debug("poll\n");
    if (args_ptr->comm_started_flag) {
      MPI_Test(args_ptr->cr_ptr, &flag, MPI_STATUS_IGNORE);
    }
    //#pragma omp taskyield
    qthread_yield();
  } while(!flag);

  debug("poll exit\n");
  return 0;
}


void release_event(MPI_Status *status, void *data) {
  debug("release event\n");
  debug("data: %p\n", data);
  args_t *args_ptr = (args_t *) data;
  debug("ptr convert\n");
  qthread_fill(args_ptr->val_ptr);
  debug("release event end\n");
}
