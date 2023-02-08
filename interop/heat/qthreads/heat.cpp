#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <chrono>
#include <assert.h>
#include <mpi.h>
#include <mpi-ext.h>

#include <heat.hpp>

enum direction {NORTH, SOUTH, NORTHSOUTH, REGULAR};
enum rankName {FIRST, LAST, INTER, SINGLE};

MPI_Request cont_req;
volatile int do_progress;

qt_barrier_t * barrier;

aligned_t solveBlock_task(task_arg_t * args) {
	/* Unpack task data */
	int rank = args->rank;
	int rank_size = args->rank_size;

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

	debug("solveblock_task %i, %i, %i, %i, adr %p: %p, %p, %p, %p\n", bx, by, nbx, nby, &targetBlock, topBlock, bottomBlock, leftBlock, rightBlock);	

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

	qthread_fill((aligned_t*)&matrix[CENTER]);
	if(rank == rank_size - 1) {
		if(bx == args->nbx-2 && by == args->nby-2)
		{		
			qt_barrier_enter(barrier);
		}
	}
	return 0;
}

void solveBlock(block_t *matrix, task_arg_t * _args, int rank, int rank_size, int nbx, int nby, int bx, int by) {
	
	task_arg_t &args = _args[CENTER_ARGS];
	args.bx=bx;
	args.by=by;
	args.nbx=nbx;
	args.nby=nby;
	args.rank = rank;
	args.rank_size = rank_size;
	args.matrix = matrix;

	int region = (bx == 1 && bx != nbx - 2) ? NORTH
		: (bx == 1 && bx == nbx - 2) ? NORTHSOUTH 
			: (bx != 1 && bx == nbx - 2) ? SOUTH 
				: REGULAR;

	switch (region){
		case (NORTH):
		{
			debug("solveblock_N %i, %i, %i, %i, adr %p: %p, %p, %p, %p\n", bx, by, nbx, nby, &matrix[CENTER], &matrix[TOP],  &matrix[BOTTOM], &matrix[LEFT], &matrix[RIGHT]);	
			qthread_fork_precond((aligned_t(*)(void*))&solveBlock_task, 
				(void *) &args, 
				NULL,
				2,
				(aligned_t*)&matrix[TOP],				
				(aligned_t*)&matrix[LEFT]
			);
			break;
		}
		case (SOUTH):
		{
			debug("solveblock_S %i, %i, %i, %i, adr %p: %p, %p, %p, %p\n", bx, by, nbx, nby, &matrix[CENTER], &matrix[TOP],  &matrix[BOTTOM], &matrix[LEFT], &matrix[RIGHT]);	
			qthread_fork_precond((aligned_t(*)(void*))&solveBlock_task, 
				(void *) &args, 
				NULL,
				3,				
				(aligned_t*)&matrix[TOP],
				(aligned_t*)&matrix[LEFT],
				(aligned_t*)&matrix[BOTTOM]
			);
			break;
		}
		case (NORTHSOUTH):
		{
			debug("solveblock_NS %i, %i, %i, %i, adr %p: %p, %p, %p, %p\n", bx, by, nbx, nby, &matrix[CENTER], &matrix[TOP],  &matrix[BOTTOM], &matrix[LEFT], &matrix[RIGHT]);	
			qthread_fork_precond((aligned_t(*)(void*))&solveBlock_task, 
				(void *) &args, 
				NULL,
				3,				
				(aligned_t*)&matrix[TOP],
				(aligned_t*)&matrix[LEFT],
				(aligned_t*)&matrix[BOTTOM]
			);
			break;	
		}
		case (REGULAR):
		{
			debug("solveblock_R %i, %i, %i, %i, adr %p: %p, %p, %p, %p\n", bx, by, nbx, nby, &matrix[CENTER], &matrix[TOP],  &matrix[BOTTOM], &matrix[LEFT], &matrix[RIGHT]);	
			qthread_fork_precond((aligned_t(*)(void*))&solveBlock_task, 
				(void *) &args, 
				NULL,
				2,
				(aligned_t*)&matrix[TOP],
				(aligned_t*)&matrix[LEFT]
			);
			break;	
		}
		default:
		{
			debug("ERROR: Unexpected block.\n");	
			exit(1);
		}
	}
}

int release_event_fill(int rc, void *data) {
	debug("*** Released Fill Event: %p\n", (aligned_t *)data);
	qthread_writeEF((aligned_t*)data, (aligned_t*)data);
	return MPI_SUCCESS;
}

int release_event_barrier(int rc, void *data) {
	debug("*** Released Barrier: %p\n", (aligned_t *)data);
	qt_barrier_enter(barrier);
	return MPI_SUCCESS;
}

aligned_t sendFirstComputeRow_task(task_arg_t * args) {
	/* Unpack task data */
	int by   = args->by;
	int rank = args->rank;
	block_t * matrix = args->matrix;

	MPI_Request request;
	debug("Sending first compute row to %d tag %d adr %p args %p\n", rank-1, by, matrix, args);
	MPI_Isend(&matrix[0], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, &request);
	// No continuation needed as the app only progresses after the corresponding Irecv has completed
	// via an MPI Continuation.
	MPI_Request_free(&request);
	return 0;
}

inline void sendFirstComputeRow(block_t *matrix, task_arg_t * _args, int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[0 * (nby - 2) + by - 1];
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
	debug("Sending last compute row to %d tag %d\n", rank+1, by);
	MPI_Isend(&matrix[BSX-1], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event_barrier, (void *) matrix, MPIX_CONT_REQBUF_VOLATILE, MPI_STATUS_IGNORE, cont_req);
	return 0;
}

inline void sendLastComputeRow(block_t *matrix, task_arg_t * _args, int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[0 * (nby - 2) + by - 1];
		args.by=by;
		args.rank = rank;
		args.matrix = &matrix[(nbx-2)*nby + by];
		qthread_fork_precond((aligned_t(*)(void*))&sendLastComputeRow_task, 
			(void *) &args, 
			NULL,
			1, 
			&matrix[(nbx-2)*nby + by]);
	}
}

aligned_t receiveUpperBorder_task(task_arg_t * args) {
	/* Unpack task data */
	int by = args->by;
	int rank = args->rank;
	block_t * matrix = args->matrix;

	MPI_Request request;
	debug("Receiving upper border from %d tag %d adr %p args %p\n", rank-1, by, matrix, args);
	MPI_Irecv(&matrix[BSX-1], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event_fill, (void *) matrix, MPIX_CONT_REQBUF_VOLATILE, MPI_STATUS_IGNORE, cont_req);
	return 0;
}

inline void receiveUpperBorder(block_t *matrix, task_arg_t * _args, int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[2 * (nby - 2) + by - 1];
		args.by = by;
		args.rank = rank;
		args.matrix = &matrix[by];
		qthread_fork((aligned_t(*)(void*))&receiveUpperBorder_task,
		(void *) &args,
		NULL);
	}
}

aligned_t receiveLowerBorder_task(task_arg_t * args) {
	/* Unpack task data */
	int by   = args->by;
	int rank = args->rank;
	block_t * matrix = args->matrix;

	MPI_Request request;
	debug("Receiving lower border from %d tag %d adr %p\n", rank+1, by, matrix);
	MPI_Irecv(&matrix[0], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD, &request);
	MPIX_Continue(&request, &release_event_fill, (void *) matrix, MPIX_CONT_REQBUF_VOLATILE, MPI_STATUS_IGNORE, cont_req);
	return 0;
}

inline void receiveLowerBorder(block_t *matrix, task_arg_t * _args,  int nbx, int nby, int rank, int rank_size) {
	for (int by = 1; by < nby-1; ++by) {	
		task_arg_t &args = _args[3 * (nby - 2) + by - 1];
		args.by = by;
		args.rank = rank;
		args.matrix = &matrix[(nbx-1)*nby + by];
		qthread_fork((aligned_t(*)(void*))&receiveLowerBorder_task,
		 (void *) &args,
		  NULL);
	}
}

inline void resetFEBs( block_t * matrix, int rank, int num_ranks, int nbx, int nby )
{
	//Fill northern and southern blocks if num_ranks == 1
	if(num_ranks <=1 )
	{
		int bx = 0;
		for (int by = 1; by < nby-1; ++by) {
			qthread_fill((aligned_t*)&matrix[bx * nby + by]);
		}
		bx = nbx-1;
		for (int by = 1; by < nby-1; ++by) {
			qthread_fill((aligned_t*)&matrix[bx * nby + by]);
		}
	}else
	{
		int bx = 0;
		if (rank == 0) {
			//Fill northern blocks if rank == 0
			for (int by = 1; by < nby-1; ++by) {
				qthread_fill((aligned_t*)&matrix[bx * nby + by]);
			}
		}else
		{
			//Empty northern blocks if rank != 0
			for (int by = 1; by < nby-1; ++by) {
				qthread_empty((aligned_t*)&matrix[bx * nby + by]);
			}
		}

		bx = nbx-1;
		if (rank == num_ranks-1) {
			//Fill southern blocks if rank == num_ranks-1
			for (int by = 1; by < nby-1; ++by) {
				qthread_fill((aligned_t*)&matrix[bx * nby + by]);
			}
		}else{
			//Empty southern blocks if rank == num_ranks-1
			for (int by = 1; by < nby-1; ++by) {
				qthread_empty((aligned_t*)&matrix[bx * nby + by]);
			}
		}
	}

	//Empty inner blocks
	for (int bx = 1; bx < nbx - 1; ++bx) {
		for (int by = 1; by < nby-1; ++by) {
			qthread_empty((aligned_t*)&matrix[bx* nby + by]);
		}
	}

	//Fill east and west block columns
	for (int bx = 1; bx < nbx-1; ++bx) {
		qthread_fill((aligned_t*)&matrix[bx * nby + 0]);
		qthread_fill((aligned_t*)&matrix[bx * nby + (nby-1)]);
	}
}

inline void solveGaussSeidel(block_t *matrix, task_arg_t * args, task_arg_t * args_border, int nbx, int nby, int rank, int rank_size) {	
	
	resetFEBs(matrix, rank, rank_size, nbx, nby);

	if (rank != 0) {
		sendFirstComputeRow(matrix, args_border, nbx, nby, rank, rank_size);
		receiveUpperBorder(matrix, args_border,  nbx, nby, rank, rank_size);
	}
	if (rank != rank_size - 1) {
		receiveLowerBorder(matrix, args_border, nbx, nby, rank, rank_size);
	}
	for (int bx = 1; bx < nbx-1; ++bx) {
		for (int by = 1; by < nby-1; ++by) {
			solveBlock(matrix, args, rank, rank_size, nbx, nby, bx, by);
		}
	}

	if (rank != rank_size - 1) {
		sendLastComputeRow(matrix, args_border, nbx, nby, rank, rank_size);
	}	

	debug("PRE_BARRIER 2\n");
	qt_barrier_enter(barrier);
	debug("POST_BARRIER 2\n");
}

aligned_t progress_task(void * args)
{
	int flag = 0;
	debug("Progress task\n");
	  while(do_progress) { 
	        MPI_Test(&cont_req, &flag, MPI_STATUS_IGNORE);
            if (flag) {
           		MPI_Start(&cont_req);
            }
        	qthread_yield();        
      }
	debug("Progress task done\n");
	return 0;
}

void solve(block_t *matrix, task_arg_t * args, task_arg_t * args_border, int rowBlocks, int colBlocks, int timesteps) {
	int rank, rank_size;
	aligned_t ret;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);

	barrier = (rank == rank_size -1 )?qt_barrier_create(2, REGION_BARRIER) :
			 qt_barrier_create(colBlocks-2 + 1, REGION_BARRIER);

	if(rank_size > 1)
	{
		MPIX_Continue_init(0, 0, MPI_INFO_NULL, &cont_req);
		MPI_Start(&cont_req);
		QCHECK(qthread_fork(progress_task, NULL, &ret));
		do_progress = 1; // whether to keep triggering progress
	} 

	for (int t = 0; t < timesteps; ++t) {
		solveGaussSeidel(matrix, args, args_border, rowBlocks, colBlocks, rank, rank_size);
	}

	qt_barrier_destroy(barrier);

	if(rank_size > 1)
	{
		do_progress = 0;
		QCHECK(qthread_readFF(NULL, &ret));
		MPI_Request_free(&cont_req);
	}
}
