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

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
int main(int argc, char*argv[])
{

    if(argc != 3)
    {
        printf("./array_sum n bs\n");
        exit(1);
    }
    int N = atoi( argv[1] );
    int BS = atoi( argv[2] );
    
    int i, res = 0, *v;
    v = malloc(N*sizeof(int));

    for (i = 0; i < N; ++i) v[i] = i + 1;

    for (i = 0; i < N; i+=BS)
    {
        #pragma omp task reduction(+: res)
        {
            int j;
            for(j = 0; j < BS && (i+j) < N; ++j)
            {
                res += v[i+j]; 
            }
        }
    }

#pragma omp taskwait
    printf("computed result: %d vs. real result: %d\n", res, ((N*(N+1))/2));
    assert(res == ((N*(N+1))/2));
    printf("ok!\n");
}
