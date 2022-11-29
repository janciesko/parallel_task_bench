//@HEADER
// ************************************************************************
//
//                      OPENMP TASK BENCH 1.0
//       Copyright 2022 National Technology & Engineering 
//                Solutions of Sandia, LLC (NTESS). 
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NTESS OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Jan Ciesko (jciesko@sandia.gov)
//
// ************************************************************************
//@HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<assert.h>
typedef struct
{
	int x, y;
} move;

#define true 1
#define false 0

move vector_add (move *a, move *b)
{
	move c;
	c.x = a->x + b->x;
	c.y = a->y + b->y;
	return c;
}

//move moves[8]={{2,1}, {1,2},{ -1,2}, {-2,1}, {-2,-1}, {-2,-1}, {-1,-2}, {2,-1}}; //valid positions for a move
move moves[8]={{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{-1,2},{1,-2},{-1,-2}}; //valid positions for a move

int N, *board, final;
int total_solutions;

int findtour(int move_num, move m, int _board [N*N])
{
	//check final position
	if(move_num == (N*N))
	{
        int j;
        for (j = 0; j < N*N; ++j)
        {
            assert(board[j]);
        }
		return 1;
	}

	//test all new possible moves
	int sum = 0, i;
	for(i = 0; i < 8; i++)
    {
        move new_move = vector_add(&m, &moves[i]);
        int pos = new_move.y * N + new_move.x;
        if (!((new_move.x < 0) || (new_move.x >= N) || (new_move.y < 0) || (new_move.y >= N))
                    && !board[pos])
        {
            #pragma omp task reduction(+:sum) firstprivate(_board) final(move_num > final)
            {
	            board[pos] = true;
                int tmp = findtour(move_num+1, new_move, _board);
	            board[pos] = false;
                sum = sum + tmp;
            }
        }
    }

	#pragma omp taskwait
	return sum;
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("./knights N FINAL\n");
        exit(1);
    }

    N = atoi(argv[1]);
    final = atoi(argv[2]);

    total_solutions = 0;

	int n_solutions = 0, i;
	move move = {0,0};
	board = malloc (sizeof(int) * N*N);
    for (i = 0; i < N*N;++i)
    {
        memset(board, false, sizeof(int) * N*N);
        move.x = i % N;
        move.y = i / N;
        board[i] = true;
         n_solutions += findtour(1, move, board);
        board[i] = false;

        printf("%i\n", n_solutions);
    }

		printf("Total:%i\n", n_solutions);
	return 0;
}

