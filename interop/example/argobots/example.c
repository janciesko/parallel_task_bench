#include <mpi.h>
#include <openmpi/mpiext/mpiext_continue_c.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <abt.h>

#include <unistd.h>
#include <stdarg.h>
#include <assert.h>

#include <helpers.h>

#define VALREF 17
#define NUM_XSTREAMS 1
#define NUM_THREADS 8

typedef struct args {
  MPI_Request * cont_req;
  int * val;
  ABT_eventual val_eventual;
  int comm_started_flag;
} args_t;

typedef struct abt_config {
  ABT_pool * pools;
  ABT_sched *scheds;
  ABT_xstream * xstreams;
  ABT_thread * threads;
  int num_xstreams;
  int num_threads;

} abt_config_t;

int callback(int rc, void *data) {
  args_t *args = (args_t *) data;
  ABT_eventual_set(args->val_eventual, NULL, 0);
  return MPI_SUCCESS;
}

void task0(void * _args) {
  args_t *args = (args_t *) _args;
  debug("task0\n");
  MPI_Request op_req;
  MPI_Irecv(args->val, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &op_req);
  MPIX_Continue(&op_req, &callback, (void *) args, MPIX_CONT_PERSISTENT , MPI_STATUS_IGNORE, *args->cont_req);
  args->comm_started_flag = 1;
}

void task1(void *_args) {
  args_t *args = (args_t *) _args;
  debug("task1\n");
  ABT_eventual_wait(args->val_eventual, NULL);
  assert (*args->val == VALREF);
}

void poll_task(void * _args){
  args_t *args = (args_t *) _args;
  int flag = 0;
  do {
    if (args->comm_started_flag) {
      MPI_Test(args->cont_req, &flag, MPI_STATUS_IGNORE);
    }
    ABT_thread_yield();
  } while(!flag);
}

void init_argobots(abt_config_t * conf)
{
  conf->num_xstreams = NUM_XSTREAMS;
  conf->num_threads = NUM_THREADS;

  /* Create pools. */  
  conf->pools = (ABT_pool *) malloc(sizeof(ABT_pool) * conf->num_xstreams);
  for (size_t i = 0; i < conf->num_xstreams; i++) {
    ABT_pool_create_basic(ABT_POOL_FIFO, ABT_POOL_ACCESS_MPMC, ABT_TRUE, &conf->pools[i]);
  }

  /* Create schedulers. */
  conf->scheds = (ABT_sched *) malloc(sizeof(ABT_sched) * conf->num_xstreams);
  for (size_t i = 0; i < conf->num_xstreams; i++) {
    ABT_pool *tmp = (ABT_pool *)malloc(sizeof(ABT_pool) * conf->num_xstreams);
    for (int j = 0; j < conf->num_xstreams; j++) {
      tmp[j] = conf->pools[(i + j) % conf->num_xstreams];
    }
    ABT_sched_create_basic(ABT_SCHED_DEFAULT, conf->num_xstreams, tmp, ABT_SCHED_CONFIG_NULL, &conf->scheds[i]);
    free(tmp);
  }

  /* Allocate execution streams */
  conf->xstreams = (ABT_xstream *) malloc(sizeof(ABT_xstream) * conf->num_xstreams);

  /* Set up a primary execution stream. */
  ABT_xstream_self(&conf->xstreams[0]);
  ABT_xstream_set_main_sched(conf->xstreams[0], conf->scheds[0]);

  /* Create secondary execution streams. */
  for (size_t i = 1; i < conf->num_xstreams; i++) {
    ABT_xstream_create(conf->scheds[i], &conf->xstreams[i]);
  }
  /* Allocate threads */
  conf->threads = (ABT_thread *) malloc(sizeof(ABT_thread) * conf->num_threads);
}

void free_argobots(abt_config_t * conf)
{
  /* Free allocated memory. */
  free(conf->xstreams);
  free(conf->pools);
  free(conf->scheds);
  free(conf->threads);
}


void proc0(void) {
  int value = 0;
  volatile int comm_started_flag = 0;
  MPI_Request cont_req;
  abt_config_t conf;

  init_argobots(&conf);

  MPIX_Continue_init(MPI_UNDEFINED, 0, MPI_INFO_NULL, &cont_req);
  MPI_Start(&cont_req);

  args_t args = { &cont_req, &value, (ABT_eventual) NULL, 0 };
  ABT_eventual_create(0, &args.val_eventual);

  int tid = 0;
  int pool_id = tid % conf.num_xstreams;
  ABT_thread_create(conf.pools[pool_id], task0, &args, ABT_THREAD_ATTR_NULL, &conf.threads[tid]);

  tid = 1;
  pool_id = tid % conf.num_xstreams;
  ABT_thread_create(conf.pools[pool_id], task1, &args, ABT_THREAD_ATTR_NULL, &conf.threads[tid]);

  tid = 2;
  pool_id = tid % conf.num_xstreams;
  ABT_thread_create(conf.pools[pool_id], poll_task, &args, ABT_THREAD_ATTR_NULL, &conf.threads[tid]);

  for (size_t i = 0; i < conf.num_threads; i++) {
    ABT_thread_free(&conf.threads[i]);
  }

  /* Join secondary execution streams. */
  for (size_t i = 1; i < conf.num_xstreams; i++) {
    ABT_xstream_join(conf.xstreams[i]);
    ABT_xstream_free(&conf.xstreams[i]);
  }

  free_argobots(&conf);

  MPI_Request_free (&cont_req);
}

void proc1(void) {
  int send_val = VALREF;
  MPI_Send(&send_val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

int main (int argc , char ** argv) {
  int size, rank;
  MPI_Init(&argc , &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  ABT_init(0, NULL);

  if (size != 2) {
    printf("ERROR: Comm size must be 2\n");
    return 1;
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  #pragma omp parallel master
  switch (rank) {
    case 0:
      proc0();
      break;
    case 1:
      proc1();
      break;
    default:
      printf("ERROR: Invalid rank: %d", rank);      
      break;
  }
  
  ABT_finalize();
  MPI_Finalize();
  return 0;
}
