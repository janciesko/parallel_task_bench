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
#include "timers.h"
#include "omp.h"


#ifndef _OPENMP
#define omp_get_num_threads() 1
#endif

typedef struct
{
    int x, y;
} move_t;

typedef struct sol_node
{
    move_t pos;
    struct sol_node* prev;
} sol_node_t;


int visited_position(move_t m, sol_node_t* sol)
{
    sol_node_t* aux = sol;
    while (aux != 0)
    {
        if (aux->pos.x == m.x && aux->pos.y == m.y)
            return 1;

        aux = aux->prev;
    }
    return 0;
}


move_t vector_add (move_t *a, move_t *b)
{
    move_t c;
    c.x = a->x + b->x;
    c.y = a->y + b->y;
    return c;
}

move_t moves[8]={{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{-1,2},{1,-2},{-1,-2}};

int N, final;

int findtour(int move_num, move_t m, sol_node_t sol)
{
    //check final position
    if(move_num == (N*N))
    {
        return 1;
    }

    //test all new possible moves
    int i, sum = 0;
    for(i = 0; i < 8; i++)
    {
        if (move_num < 4)
        {
            fprintf(stderr, "testing movement: %d\n", i);
        }
        move_t new_move = vector_add(&m, &moves[i]);
        int pos = new_move.y * N + new_move.x;
        if (!((new_move.x < 0) || (new_move.x >= N) || (new_move.y < 0) || (new_move.y >= N))
                && !visited_position(new_move, &sol))

        {

            sol_node_t new_sol;
            new_sol.pos = new_move;
            new_sol.prev = &sol;

#pragma omp task reduction(+:sum) final(move_num > final)
            {
                int tmp = findtour(move_num+1, new_move, new_sol);
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


    int n_solutions = 0, i;
    timer_start(0);
    for (i = 0; i < N*N;++i)
    {
        move_t initial_move = { i % N, i / N};
        sol_node_t initial_sol = { initial_move, 0};
        fprintf(stderr, "initial_pos: (%d, %d)\n", initial_move.x, initial_move.y);
        n_solutions += findtour(1, initial_move, initial_sol);
        fprintf(stderr, "%i\n", n_solutions);
    }
    timer_stop(0);

    fprintf(stderr, "test, %s, time, %.6f, size, %i, result, %d, threads, %i, final, %i\n", argv[0], timer_read(0), N, n_solutions, omp_get_num_threads(), final);
    return 0;
}
