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
#include <assert.h>


typedef struct sol_node
{
    int col;
    struct sol_node * prev;
} sol_node_t, *sol_t;

static inline int check_attack(const int col, const int i, sol_t sol)
{
    int j;
    for (j = 0; j < col; j++)
    {
        const int tmp = abs(sol->col - i);
        if(tmp == 0 || tmp == j+1)
            return 1;
        sol = sol->prev;
    }
    return  0;
}

int final_depth;
int global_var;
int solve(int n, const int col, sol_node_t sol)
{
    if (col == n)
    {
        return 1;
    }

    int i, result = 0;
    for (i = 0; i < n; i++)
    {
        if (!check_attack(col, i, &sol))
        {
            sol_node_t new_sol;
            new_sol.prev = &sol;
            new_sol.col = i;

            #pragma omp task final(final_depth <= col) reduction(+: global_var)
            {
                //smateo: Do not merge these two statements
                int tmp = solve(n, col + 1, new_sol);
                global_var += tmp;
            }
        }
    }
    #pragma omp taskwait
    return 0;
}

int main(int argc, char **argv)
{

    if(argc != 3)
    {
        printf("./nqueens n final_depth\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    final_depth = atoi(argv[2]);

    assert(n > 0);

    global_var = 0;
    sol_node_t initial_node = {-1, 0};
    solve(n, 0, initial_node);

    printf("Total solutions for %d x %d: %d\n", n, n, global_var);
    return 0;
}
