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

#define N_DEFAULT 4
#define NUM_THREADS_DEFAULT 4
#define NUM_TASKS_DEFAULT NUM_THREADS_DEFAULT
#define TASK_DEFAULT_CUT_OFF 0
#define ITERS 3

#define NUM_SEQ_REPETITIONS 2

#define FIB_RESULTS_NUM 41
struct {
  int fib[FIB_RESULTS_NUM] = {
      0,       1,        1,        2,        3,        5,        8,
      13,      21,       34,       55,       89,       144,      233,
      377,     610,      987,      1597,     2584,     4181,     6765,
      10946,   17711,    28657,    46368,    75025,    121393,   196418,
      317811,  514229,   832040,   1346269,  2178309,  3524578,  5702887,
      9227465, 14930352, 24157817, 39088169, 63245986, 102334155};
} reference_results;

struct {
  int fib[FIB_RESULTS_NUM] = {
      1,        3,        5,         9,         15,       25,       41,
      67,       109,      177,       287,       465,      753,      1219,
      1973,     3193,     5167,      8361,      13529,    21891,    35421,
      57313,    92735,    150049,    242785,    392835,   635621,   1028457,
      1664079,  2692537,  4356617,   7049155,   11405773, 18454929, 29860703,
      48315633, 78176337, 126491971, 204668309, 331160281};
} reference_num_tasks;

double timer[2] = {0.0, 0.0};

#define to_nsec(x) x * 1000 * 1000 * 1000

typedef struct {
  int n;
  int num_threads;
  int num_tasks;
  int task_cut_off;
} partix_config_t;