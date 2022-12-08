#include <mpi.h>
#include <openmpi/mpiext/mpiext_continue_c.h>
#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <stdarg.h>
#include <assert.h>

#include <omp.h>
#include <helpers.h>

#define VALREF 17

int callback(int rc, void *data) {
  omp_event_handle_t event = (omp_event_handle_t)(uintptr_t) data;
  omp_fulfill_event(event);
  return MPI_SUCCESS;
}

void proc0(void) {
  omp_event_handle_t event;
  int value = 0;
  volatile int comm_started_flag = 0;
  MPI_Request cont_req;

  MPIX_Continue_init(MPI_UNDEFINED, 0, MPI_INFO_NULL, &cont_req);
 
  MPI_Start(&cont_req);

  #pragma omp task depend(out:value) shared(value, comm_started_flag) detach(event)
  {
    debug("task0\n");
    MPI_Request req;
    int flag;
    MPI_Irecv(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &req);
    MPIX_Continue(&req, &callback, (void *) event, MPIX_CONT_PERSISTENT , MPI_STATUS_IGNORE, cont_req);
    comm_started_flag = 1;
  }

  #pragma omp task depend(in:value) shared(value, comm_started_flag)
  {
    debug("task1\n");
    assert (value == VALREF);
  }

  #pragma omp task
  {
    int flag = 0;
    debug("task2\n");
    do {
      if (comm_started_flag) {
        MPI_Test(&cont_req, &flag, MPI_STATUS_IGNORE);
      }
      #pragma omp taskyield
    } while(!flag);
  }
  #pragma omp taskwait
  MPI_Request_free (& cont_req);
}

void proc1(void) {
  int send_val = VALREF;
  MPI_Send(&send_val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

int main (int argc , char ** argv) {
  int size, rank;
  MPI_Init(&argc , &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

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
  
  MPI_Finalize();
  return 0;
}
