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
#include <stdint.h>
#include <assert.h>

typedef struct {
        int ID, weight, value;
} item_t;

item_t item[] = {
        {1, 9, 150},
        {2, 13, 35},
        {3, 153, 200},
        {4, 50, 160},
        {5, 15, 60},
        {6, 68, 45},
        {7, 27, 60},
        {8, 39, 40},
        {9, 23, 30},
        {10, 52, 10},
        {11, 11, 70},
        {12, 32, 30},
        {13, 24, 15},
        {14, 48, 10},
        {15, 73, 40},
        {16, 42, 70},
        {17, 43, 75},
        {18, 22, 80},
        {19, 7, 20},
        {20, 18, 12},
        {21, 4, 50},
        {22, 30, 10}
};

#define n_items (sizeof(item)/sizeof(item_t))

typedef struct {
        uint32_t bits; /* 32 bits, can solve up to 32 items */
        int value;
} solution;

//----------------------------------------
//----------ORIG--------------------------
//----------------------------------------
void optimal(int weight, int idx, solution *s)
{
	solution v1, v2;
	if (idx < 0) {
			s->bits = s->value = 0;
			return;
	}

	if (weight < item[idx].weight) {
			optimal(weight, idx - 1, s);
			return;
	 }

	optimal(weight, idx - 1, &v1);
	optimal(weight - item[idx].weight, idx - 1, &v2);

	v2.value += item[idx].value;
	v2.bits |= (1 << idx);

	*s = (v1.value >= v2.value) ? v1 : v2;
}

//----------------------------------------
//--------------PARALLEL REDUCTION--------
//----------------------------------------
void MAX(solution * v1, solution v2)
{
	*v1 = (v1->value >= v2.value) ? *v1 : v2; //reduction here defined as: read-check-write
}

solution optimal_reduction(int weight, int idx)
{
	solution v1 = {0,0}, v2;

	if (idx < 0) {
			return v1;
	}

	if (weight < item[idx].weight) {
			return optimal_reduction(weight, idx - 1);
	 }

	//#pragma omp task out(v1)
		v1 = optimal_reduction(weight, idx - 1);

	//#pragma omp task out(v2)
	{
		v2 = optimal_reduction(weight - item[idx].weight, idx - 1);
		v2.value += item[idx].value;
		v2.bits |= (1 << idx);
	}

	#pragma omp taskwait
	//#pragma omp task reduction(MAX:v1) in(v2)
	{
		v1 = (v1.value >= v2.value) ? v1 : v2;
	}

	#pragma omp taskwait
	return v1;
}

int main(void)
{
        int i = 0, w = 0;
        solution s_serial = {0, 0}, s_parallel;

        optimal(400, n_items - 1, &s_serial);
        s_parallel = optimal_reduction(400, n_items - 1);

        assert(s_parallel.bits == s_serial.bits);
        return 0;
}

