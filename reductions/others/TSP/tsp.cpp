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

#define CITIES 3
#define true 1
#define false 0

typedef struct
{
	int visted;
	int value;
} city;

city STATE    [CITIES][1<<CITIES]; //this maintains the state (such as the board in the knights tour)
int  DISTANCE [CITIES][CITIES];    //connections
int  final;

inline static int MIN(int a, int b) {
	return (a>b)? b: a;
}

int tsp(int city, int mask) //supports max 32 cities
{
	//all visited
	if (city == 1<<CITIES-1) {
		STATE[city][mask].visted = true;
		return STATE[city][mask].value;
	}

	//if visited in the current path, return value
	if(STATE[city][mask].visted == true)
		return STATE[city][mask].value;

	//mark as visited
	STATE[city][mask].visted = true;

	int distance = 0;
	for(int i = 0; i < CITIES; i++)
		if( DISTANCE[city][i] != -1 && (mask & (1<<i) == 0) )    //if there is a path and i is not visited yet
		//pragma omp task reduction (MIN:distance)
		MIN(distance, tsp(i, mask | 1<<i) + DISTANCE[city][i]); //visit city and city to the configuration mark

	//#pragma omp taskwait
	//store max path in array
	STATE[city][mask].value = distance;
	return distance;
}

int main(int argc, char **argv) {
    if(argc != 2)
    {
        printf("./knights FINAL_VAL\n");
        exit(1);
    }

    final = atoi(argv[1]);

    memset(STATE,     0, sizeof(STATE)   );
    memset(DISTANCE, -1, sizeof(DISTANCE));

    for(int i = 1; i < CITIES; i++)
    	for(int j = 0; j < i; j++)
    		DISTANCE[i][j] = i+j;

	printf("%i\n", tsp(0,0));
	return 0;
}

