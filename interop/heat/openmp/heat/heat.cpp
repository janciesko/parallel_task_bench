#include <stdio.h>
#include <stdint.h>
#include <omp.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>

#include <mpi.h>
#include <mpi-ext.h>

#include <helpers.h>
#include <heat.hpp>

static MPI_Request cont_req;

int serial;

void release_event(MPI_Status *status, void *data) {
  omp_event_handle_t event = (omp_event_handle_t)(uintptr_t) data;
  omp_fulfill_event(event);
}

inline void solveBlock(block_t *matrix, int nbx, int nby, int bx, int by) {
	block_t &targetBlock = matrix[bx*nby + by];
	const block_t &centerBlock = matrix[bx*nby + by];
	const block_t &topBlock    = matrix[(bx-1)*nby + by];
	const block_t &leftBlock   = matrix[bx*nby + (by-1)];
	const block_t &rightBlock  = matrix[bx*nby + (by+1)];
	const block_t &bottomBlock = matrix[(bx+1)*nby + by];

	double sum = 0.0;
	for (int x = 0; x < BSX; ++x) {
		const row_t &topRow = (x > 0) ? centerBlock[x-1] : topBlock[BSX-1];
		const row_t &bottomRow = (x < BSX-1) ? centerBlock[x+1] : bottomBlock[0];

		for (int y = 0; y < BSY; ++y) {
			double left = (y > 0) ? centerBlock[x][y-1] : leftBlock[x][BSY-1];
			double right = (y < BSY-1) ? centerBlock[x][y+1] : rightBlock[x][0];

			double value = 0.25 * (topRow[y] + bottomRow[y] + left + right);
			double diff = value - targetBlock[x][y];
			sum += diff * diff;
			targetBlock[x][y] = value;
		}
	}
}

// TODO: Remove the serialization of communication tasks
inline void sendFirstComputeRow(block_t *matrix, int nbx, int nby, int rank, int rank_size)
{
	for (int by = 1; by < nby-1; ++by) {
    omp_event_handle_t event;
		#pragma omp task depend(in: matrix[nby + by]) detach(event) // depend(inout: serial)
		{
			// TODO: Switch to the non-blocking MPI counterpart
			// MPI_Send(&matrix[nby+by][0], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD);
			MPI_Request request;
			debug("Sending first compute row to %d tag %d\n", rank-1, by);
			MPI_Isend(&matrix[nby+by][0], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, &request);
			// TODO: Bind the completion of the task to the finalization of the MPI request (TAMPI_Iwait)
			//TAMPI_Iwait(&request, MPI_STATUS_IGNORE);
            MPIX_Continue(&request, &release_event, (void *) event, MPI_STATUS_IGNORE, cont_req);
		}
	}
}

inline void sendLastComputeRow(block_t *matrix, int nbx, int nby, int rank, int rank_size)
{
	for (int by = 1; by < nby-1; ++by) {
    omp_event_handle_t event;
		#pragma omp task depend(in: matrix[(nbx-2)*nby + by]) detach(event) // depend(inout: serial)
		{
			// MPI_Send(&matrix[(nbx-2)*nby + by][BSX-1], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD);
			// TODO ...
			MPI_Request request;
			debug("Sending last compute row to %d tag %d\n", rank+1, by);
			MPI_Isend(&matrix[(nbx-2)*nby + by][BSX-1], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, &request);
			//TAMPI_Iwait(&request, MPI_STATUS_IGNORE);
            MPIX_Continue(&request, &release_event, (void *) event, MPI_STATUS_IGNORE, cont_req);
		}
	}

}

inline void receiveUpperBorder(block_t *matrix, int nbx, int nby, int rank, int rank_size)
{
	for (int by = 1; by < nby-1; ++by) {
    omp_event_handle_t event;
		#pragma omp task depend(out: matrix[by]) detach(event) // depend(inout: serial)
		{
			// MPI_Recv(&matrix[by][BSX-1], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// TODO ...
			MPI_Request request;
			debug("Receiving upper border from %d tag %d\n", rank-1, by);
			MPI_Irecv(&matrix[by][BSX-1], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, &request);
			//TAMPI_Iwait(&request, MPI_STATUS_IGNORE);
            MPIX_Continue(&request, &release_event, (void *) event, MPI_STATUS_IGNORE, cont_req);
		}
	}
}

inline void receiveLowerBorder(block_t *matrix, int nbx, int nby, int rank, int rank_size)
{
	for (int by = 1; by < nby-1; ++by) {
    omp_event_handle_t event;
		#pragma omp task depend(out: matrix[(nbx-1)*nby + by]) depend(inout: serial) detach(event)
		{
			// MPI_Recv(&matrix[(nbx-1)*nby + by][0], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// TODO ...
			MPI_Request request;
			debug("Receiving lower border from %d tag %d\n", rank+1, by);
			MPI_Irecv(&matrix[(nbx-1)*nby + by][0], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, &request);
			//TAMPI_Iwait(&request, MPI_STATUS_IGNORE);
           MPIX_Continue(&request, &release_event, (void *) event, MPI_STATUS_IGNORE, cont_req);
		}
	}
}

inline void solveGaussSeidel(block_t *matrix, int nbx, int nby, int rank, int rank_size)
{
	if (rank != 0) {
		sendFirstComputeRow(matrix, nbx, nby, rank, rank_size);
		receiveUpperBorder(matrix, nbx, nby, rank, rank_size);
	}

	if (rank != rank_size - 1) {
		receiveLowerBorder(matrix, nbx, nby, rank, rank_size);
	}

	for (int bx = 1; bx < nbx-1; ++bx) {
		for (int by = 1; by < nby-1; ++by) {
			#pragma omp task                            \
					depend(in: matrix[(bx-1)*nby + by]) \
					depend(in: matrix[(bx+1)*nby + by]) \
					depend(in: matrix[bx*nby + by-1])   \
					depend(in: matrix[bx*nby + by+1])   \
					depend(inout: matrix[bx*nby + by])
			solveBlock(matrix, nbx, nby, bx, by);
		}
	}

	if (rank != rank_size - 1) {
		sendLastComputeRow(matrix, nbx, nby, rank, rank_size);
	}
}

double solve(block_t *matrix, int rowBlocks, int colBlocks, int timesteps)
{
	int rank, rank_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
	MPIX_Continue_init(&cont_req, MPI_INFO_NULL);

    volatile int do_progress = 1; // whether to keep triggering progress
    volatile int in_compute  = 0; // whether the compute task has been launched

	#pragma omp parallel
	{
      #pragma omp task shared(do_progress, in_compute) untied
      {
        constexpr const auto poll_interval = std::chrono::microseconds{1};
        //using clock = std::chrono::high_resolution_clock;
        using clock = std::chrono::system_clock;
        auto last_progress_ts = clock::now();
        while(do_progress) {
          auto ts = clock::now();
          if (poll_interval < ts - last_progress_ts)
          {
            //std::cout << "Calling MPI poll service " << std::endl;
		    int flag;
	        MPI_Test(&cont_req, &flag, MPI_STATUS_IGNORE);
            last_progress_ts = clock::now();
          }
          // it is not safe to yield before compute has started because the thread might steal the compute task
          if (in_compute) {
            #pragma omp taskyield
          } else {
            usleep(1);
          }
        }
      }
	  #pragma omp single
      {

        #pragma omp task default(shared)
        {
          in_compute = 1; // signal that the progress task can start yielding without risking to take away this task
 	 	  for (int t = 0; t < timesteps; ++t) {
		  	solveGaussSeidel(matrix, rowBlocks, colBlocks, rank, rank_size);
		  }
          // wait for all tasks created inside this task
          #pragma omp taskwait
          // signal that the progress task can stop working
          do_progress = 0;
        }
      }
      // wait for the progress task to complete
	  #pragma omp taskwait
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Request_free(&cont_req);

	return 0.0;
}
