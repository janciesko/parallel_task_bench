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

int power(int base, int exp)
{
    int result = 1;
    int i;
    for (i = 0; i < exp; ++i)
    {
        result *= base;
    }
    return result;
}

int final_depth;
int nbinarywords(int n, int word_length)
{
    if (word_length == n)
    {
        return 1;
    }
    int i;
    int result = 0;
    for (i = 0; i < 2; ++i) // loop over the binary alphabet: {0, 1}
    {
        #pragma omp task reduction(+: result) final(final_depth <= word_length)
        {
            int tmp = nbinarywords(n, word_length + 1);
            result += tmp;
        }
    }
    #pragma omp taskwait
    return result;
}

int main(int argc, char*argv[])
{

    if(argc != 3)
    {
        printf("./nbinarywords n final_depth\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    final_depth = atoi(argv[2]);

    int result = nbinarywords(n, 0);

    printf("%d is %d?\n", result, power(2, n));
    assert(result == power(2, n));
    printf("ok!\n");
}
