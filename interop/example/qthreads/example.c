#include <mpi.h>
#include <openmpi/mpiext/mpiext_continue_c.h>
#include <stdio.h>
#include <stdint.h>
#include <qthread/qthread.h>
#include <qthread/barrier.h>

#include <unistd.h>
#include <stdarg.h>
#include <assert.h>

#include <helpers.h>

#define VALREF 17

typedef struct args {
  MPI_Request * cont_req;
  aligned_t * val;
  qt_barrier_t * barrier;
  int comm_started_flag;
} args_t;

int callback(int rc, void *data) {
  args_t *args = (args_t *) data;
  qthread_fill(args->val);
  return MPI_SUCCESS;
}

aligned_t task0(void * _args) {
  args_t *args = (args_t *) _args;
  debug("task0\n");
  MPI_Request op_req;
  MPI_Irecv(args->val, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &op_req);
  MPIX_Continue(&op_req, &callback, (void *) args, MPIX_CONT_PERSISTENT , MPI_STATUS_IGNORE, *args->cont_req);
  args->comm_started_flag = 1;
  qt_barrier_enter(args->barrier);
  return 0;
}

aligned_t task1(void *_args) {
  args_t *args = (args_t *) _args;
  debug("task1\n");
  qthread_readFF(NULL, args->val);
  assert (*args->val == VALREF);
  qt_barrier_enter(args->barrier);
  return 0;
}

aligned_t poll_task(void * _args){
  args_t *args = (args_t *) _args;
  int flag = 0;
  do {
    if (args->comm_started_flag) {
      MPI_Test(args->cont_req, &flag, MPI_STATUS_IGNORE);
    }
    qthread_yield();
  } while(!flag);
  return 0;
}

void proc0(void) {
  aligned_t value = 0;
  volatile int comm_started_flag = 0;
  MPI_Request cont_req;

  MPIX_Continue_init(MPI_UNDEFINED, 0, MPI_INFO_NULL, &cont_req);
  MPI_Start(&cont_req);

  args_t args = { &cont_req, &value, qt_barrier_create(3, REGION_BARRIER), 0 };

  qthread_empty(&value);
  qthread_fork(&task0, (void *) &args, NULL);
  qthread_fork(&task1, (void *) &args, NULL);
  qthread_fork(&poll_task, (void *) &args, NULL);
  qt_barrier_enter(args.barrier);
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


  qthread_initialize();

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
  
  qthread_finalize();
  MPI_Finalize();
  return 0;
}
