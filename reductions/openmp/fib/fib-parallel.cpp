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

// **** Use task reduction on parallel to update sum ****

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "helpers.h"
#include "timer.h"

#include "fib-seq.h"

int sum;
int cut_off;

void fib(int n, int &sum) {
  if (n < 2)
    sum += n;
  else {
    if (n < cut_off) {
      fib(n - 1, sum);
      fib(n - 2, sum);
    } else {
#pragma omp task firstprivate(n) in_reduction(+ : sum)
      fib(n - 1, sum);

#pragma omp task firstprivate(n) in_reduction(+ : sum)
      fib(n - 2, sum);
    }
  }
}

int main(int argc, char *argv[]) {
  partix_config_t conf;
  conf.n = argc > 1 ? atoi(argv[1]) : N_DEFAULT;
  conf.num_threads = argc > 2 ? atoi(argv[2]) : NUM_THREADS_DEFAULT;
  conf.num_tasks = argc > 3 ? atoi(argv[3]) : NUM_TASKS_DEFAULT; /*Unused*/
  conf.task_cut_off = argc > 4 ? atoi(argv[4]) : TASK_DEFAULT_CUT_OFF;

  double start_time, ellapsed_time;
  int n = conf.n;

  start_time = get_time_sec();
  // Run three times for good sequential average
  int ref = fib_seq(n, 3);
  ellapsed_time = get_time_sec() - start_time;
  timer[1] += ellapsed_time;
  timer[1] /= NUM_SEQ_REPETITIONS;

  assert(ref == reference_results.fib[n]);

  cut_off = conf.task_cut_off;

  for (int iters = 0; iters < ITERS; ++iters) {
    sum = 0;
    /*Timer start*/
    start_time = get_time_sec();

#pragma omp parallel reduction(task, + : sum) num_threads(conf.num_threads)
#pragma omp single
#pragma omp task firstprivate(n) in_reduction(+ : sum)
    fib(n, sum);
    /*Timer end*/
    ellapsed_time = get_time_sec() - start_time;
    timer[0] += ellapsed_time;
  }

  assert(sum == reference_results.fib[n]);
  int num_tasks = reference_num_tasks.fib[n - cut_off];

  std::cout << "fib, parallelred, " << conf.n << ", " << conf.num_threads << ", "
            << num_tasks << ", " << sizeof(decltype(sum)) << ", "
            << timer[0] / ITERS << ", " << timer[1] << ", "  << timer[1] / (timer[0] / ITERS) << ", "
            << to_nsec(timer[0] / ITERS / num_tasks) << std::endl;
  return 0;
}
