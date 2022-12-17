#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <chrono>
#include <assert.h>
#include <mpi.h>
#include <mpi-ext.h>

#include <qthread/qthread.h>
#include <qthread/barrier.h>

#include <heat.hpp>

MPI_Request cont_req;
volatile int do_progress;

qt_barrier_t * barrier1;
qt_barrier_t * barrier2;

int serial;

aligned_t solveBlock_task(task_arg_t * args) {
	/* Unpack task data */
	int bx  = args->bx;
	int by  = args->by;
	int nbx = args->nbx;
	int nby = args->nby;
	block_t * matrix = args->matrix;

	block_t &targetBlock       = matrix[CENTER];
	const block_t &centerBlock = matrix[CENTER];
	const block_t &topBlock    = matrix[TOP];
	const block_t &leftBlock   = matrix[LEFT];
	const block_t &rightBlock  = matrix[RIGHT];
	const block_t &bottomBlock = matrix[BOTTOM];

	debug("solveblock %i, %i, %i, %i\n", bx, by, nbx, nby);	

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
	qt_barrier_enter(barrier1);
	return 0;
}

inline void solveBlock(block_t *matrix, task_arg_t * _args, int nbx, int nby, int bx, int by) {
	task_arg_t &args = _args[CENTER];
	args.bx=bx;
	args.by=by;
	args.nbx=nbx;
	args.nby=nby;
	args.matrix = matrix;

	qthread_fork_precond((aligned_t(*)(void*))&solveBlock_task, (void *) &args, 
		(aligned_t*)&matrix[CENTER],
		4,
		(aligned_t*)&matrix[TOP],
		(aligned_t*)&matrix[BOTTOM],
		(aligned_t*)&matrix[LEFT],
		(aligned_t*)&matrix[RIGHT]
	);
}

int release_event(int rc, void *data) {
  qthread_writeF(data);
  return MPI_SUCCESS;
}

aligned_t sendFirstComputeRow_task(task_arg_t * _args) {
	/* Unpack task data */
	int by   = _args->by;
	int rank = _args->rank;
	block_t * matrix = _args->matrix;

	MPI_Request request;
	debug("Sending first compute row to %d tag %d\n", rank-1, by);
	MPI_Isend(&matrix[0], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event, (void *) matrix, MPIX_CONT_PERSISTENT, MPI_STATUS_IGNORE, cont_req);
	return 0;
}

inline void sendFirstComputeRow(block_t *matrix, task_arg_t * _args, int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[nby+by];
		args.by=by;
		args.nby=nby;
		args.rank = rank;
		args.rank_size = rank_size;
		args.matrix = &matrix[nby+by];
		qthread_fork((aligned_t(*)(void*))&sendFirstComputeRow_task, (void *) &args, NULL);
	}
}

aligned_t sendLastComputeRow_task(task_arg_t * args) {
	/* Unpack task data */
	int by   = args->by;
	int rank = args->rank;
	block_t * matrix = args->matrix;

	MPI_Request request;
	debug("Sending last compute row to %d tag %d\n", rank-1, by);
	MPI_Isend(&matrix[BSX-1], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event, (void *) matrix, MPIX_CONT_PERSISTENT, MPI_STATUS_IGNORE, cont_req);
	qt_barrier_enter(barrier2);
	return 0;
}

inline void sendLastComputeRow(block_t *matrix, task_arg_t * _args, int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[(nbx-2)*nby + by];
		args.by=by;
		args.rank = rank;
		args.matrix = matrix[(nbx-2)*nby + by];
		qthread_fork((aligned_t(*)(void*))&sendLastComputeRow_task, (void *) &args, NULL);
	}
}

aligned_t receiveUpperBorder_task(task_arg_t * args) {
	/* Unpack task data */
	int by   = args->by;
	int rank = args->rank;
	block_t * matrix = args->matrix;

	MPI_Request request;
	debug("Receiving upper border from %d tag %d\n", rank-1, by);
	MPI_Irecv(&matrix[by][BSX-1], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event, (void *) matrix, MPIX_CONT_PERSISTENT, MPI_STATUS_IGNORE, cont_req);
	return 0;
}

inline void receiveUpperBorder(block_t *matrix, task_arg_t * _args, int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[by];
		args.by=by;
		args.rank = rank;
		args.matrix = matrix[by];
		qthread_fork((aligned_t(*)(void*))&receiveUpperBorder_task, (void *) &args, NULL);
	}
}

aligned_t receiveLowerBorder_task(task_arg_t * args) {
	/* Unpack task data */
	int by   = args->by;
	int rank = args->rank;
	block_t * matrix = args->matrix;

	MPI_Request request;
	debug("Receiving lower border from %d tag %d\n", rank-1, by);
	MPI_Irecv(&matrix[0], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event, (void *) matrix, MPIX_CONT_PERSISTENT, MPI_STATUS_IGNORE, cont_req);
	return 0;
}

inline void receiveLowerBorder(block_t *matrix, task_arg_t * _args,  int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[(nbx-1)*nby + by];
		args.by=by;
		args.rank = rank;
		args.matrix = matrix[(nbx-1)*nby + by];
		qthread_fork((aligned_t(*)(void*))&receiveLowerBorder_task, (void *) &args, NULL);
	}
}

inline void solveGaussSeidel(block_t *matrix, task_arg_t * args, int nbx, int nby, int rank, int rank_size) {	
	if (rank != 0) {
		sendFirstComputeRow(matrix, args, nbx, nby, rank, rank_size);
		receiveUpperBorder(matrix, args,  nbx, nby, rank, rank_size);
	}

	if (rank != rank_size - 1) {
		receiveLowerBorder(matrix, args, nbx, nby, rank, rank_size);
	}

	for (int bx = 1; bx < nbx-1; ++bx) {
		for (int by = 1; by < nby-1; ++by) {
			solveBlock(matrix, args, nbx, nby, bx, by, barrier1);
		}
	}

	qt_barrier_enter(barrier1);

	if (rank != rank_size - 1) {
		sendLastComputeRow(matrix, args, nbx, nby, rank, rank_size, barrier2);
	}	

	qt_barrier_enter(barrier2);

}

aligned_t progress_task(void * void)
{
	int flag = 0;
	  while(do_progress) { 
	        MPI_Test(&cont_req, &flag, MPI_STATUS_IGNORE);
        	qthread_yield();        
      }
}

double solve(block_t *matrix, task_arg_t * args, int rowBlocks, int colBlocks, int timesteps) {
	int rank, rank_size;
	aligned_t ret;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
	MPIX_Continue_init(MPI_UNDEFINED, 0, MPI_INFO_NULL, &cont_req);
	MPI_Start(&cont_req);
	barrier1 = qt_barrier_create((nbx-2)*(nby*2), REGION_BARRIER)
	barrier2 = qt_barrier_create(nby-2, REGION_BARRIER)

	QCHECK(qthread_fork(progress_task, &ret, NULL));

    do_progress = 1; // whether to keep triggering progress
  
	for (int t = 0; t < timesteps; ++t) {
		solveGaussSeidel(matrix, args, rowBlocks, colBlocks, rank, rank_size);
	}

	do_progress = 0;

	QCHECK(qthread_readFF(NULL, &ret));
	MPI_Request_free(&cont_req);
	qthread_finalize();
	return 0.0;
}
