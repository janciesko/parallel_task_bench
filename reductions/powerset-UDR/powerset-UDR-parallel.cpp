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

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "helpers.h"
#include "timer.h"

#include "powerset-UDR-seq.h"

using T = red_t;

int cut_off;
T sum;

T &T_red_func(T &out, T &in) {
  out.data[0] += in.data[0];
  return out;
}

void T_init_func(T &priv) { priv.data[0] = 0; }

#pragma omp declare reduction(T_sum:T                                          \
                              : omp_out = T_red_func(omp_out, omp_in))         \
    initializer(T_init_func(omp_priv))

void powerset(int n, int index, T &sum) {
  for (int i = index; i < n; ++i) {
#pragma omp task firstprivate(i) in_reduction(T_sum                            \
                                              : sum) final(cut_off <= index)   \
    mergeable
    {
      powerset(n, i + 1, sum);
      sum.data[0]++;
    }
  }
}

int main(int argc, char *argv[]) {
  partix_config_t conf;
  conf.n = argc > 1 ? atoi(argv[1]) : N_DEFAULT;
  conf.num_threads = argc > 2 ? atoi(argv[2]) : NUM_THREADS_DEFAULT;
  conf.num_tasks = argc > 3 ? atoi(argv[3]) : NUM_TASKS_DEFAULT; /*Unused*/
  conf.task_cut_off = argc > 4 ? atoi(argv[4]) : conf.n;
  conf.task_cut_off = conf.task_cut_off > conf.n ? conf.n : conf.task_cut_off;

  double start_time, ellapsed_time;
  int n = conf.n;

  start_time = get_time_sec();
  // Run three times for good sequential average
  int ref = powerset_seq(n, 0, NUM_SEQ_REPETITIONS);
  ellapsed_time = get_time_sec() - start_time;
  timer[1] += ellapsed_time;
  timer[1] /= NUM_SEQ_REPETITIONS;

  assert(ref == exp2(n));

  cut_off = conf.task_cut_off;

  for (int iters = 0; iters < ITERS; ++iters) {
    sum.data[0] = 1; /* by default is 1 because the powerset of any set always
                  contains the empty set */

    /*Timer start*/
    start_time = get_time_sec();

#pragma omp parallel reduction(task, T_sum : sum) num_threads(conf.num_threads)
#pragma omp single
#pragma omp task firstprivate(n) in_reduction(T_sum : sum)
    powerset(n, 0, sum);

    /*Timer end*/
    ellapsed_time = get_time_sec() - start_time;
    timer[0] += ellapsed_time;
  }

  assert(sum.data[0] == exp2(n));
  int num_tasks = exp2(cut_off);

  std::cout << "ps, parallelred-UDR, " << conf.n << ", " << conf.num_threads << ", "
            << num_tasks << ", " << sizeof(decltype(sum)) << ", "
            << timer[0] / ITERS << ", " << timer[1] << ", " << timer[1] / (timer[0] / ITERS) << ", "
            << to_nsec(timer[0] / ITERS / num_tasks) << std::endl;
}
