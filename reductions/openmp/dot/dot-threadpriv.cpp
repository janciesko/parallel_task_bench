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

// **** Use taskloop reduction to update sum ****

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <omp.h>

#include "helpers.h"
#include "timer.h"

#include "dot-seq.h"

double mysum;
#pragma omp threadprivate(mysum)

void init(double *x, double *y, unsigned long n) {
#pragma omp parallel for shared(n)
  for (unsigned long i = 0; i < n; ++i) {
    x[i] = 3.14;
    y[i] = i;
  }
}

int main(int argc, char *argv[]) {
  partix_config_t conf;
  conf.n = argc > 1 ? atoi(argv[1]) : N_DEFAULT;
  conf.num_threads = argc > 2 ? atoi(argv[2]) : NUM_THREADS_DEFAULT;
  conf.num_tasks = argc > 3 ? atoi(argv[3]) : NUM_TASKS_DEFAULT;
  conf.task_cut_off =
      argc > 4 ? atoi(argv[4]) : TASK_DEFAULT_CUT_OFF; /* unused */

  double start_time, ellapsed_time;
  unsigned long n = 1 << conf.n;

  double *x = new double[n];
  double *y = new double[n];
  double sum;
  int nthreads;

  init(x, y, n);

  start_time = get_time_sec();
  // Run three times for good sequential average
  double ref = dot_seq(x, y, n, NUM_SEQ_REPETITIONS);
  ellapsed_time = get_time_sec() - start_time;
  timer[1] += ellapsed_time;
  timer[1] /= NUM_SEQ_REPETITIONS;

  for (int iters = 0; iters < ITERS; ++iters) {
    sum = 0;
    /*Timer start*/
    double start_time = get_time_sec();

#pragma omp parallel shared(x, y, n) num_threads(conf.num_threads)
    {
      mysum = 0;
#pragma omp single
#pragma omp taskloop num_tasks(conf.num_tasks) shared(n)
      for (unsigned long i = 0; i < n; ++i) {
        double temp = x[i] * y[i];
        mysum += temp;
      }
    }

#pragma omp parallel num_threads(conf.num_threads)
    {
#pragma omp single
      nthreads = omp_get_num_threads();
#pragma omp for reduction(+ : sum)
      for (int i = 0; i < nthreads; i++)
        sum += mysum;
    }
    /*Timer end*/
    double ellapsed_time = get_time_sec() - start_time;
    timer[0] += ellapsed_time;
  }

  assert(fabs(ref - ref) < 1e-3);

  int num_tasks = conf.num_tasks;

  std::cout << "dot, threadpriv, " << conf.n << ", " << conf.num_threads << ", "
            << num_tasks << ", " << sizeof(decltype(sum)) << ", "
            << timer[0] / ITERS << ", " << timer[1] << ", "  << timer[1] / (timer[0] / ITERS) << ", "
            << to_nsec(timer[0] / ITERS / num_tasks) << std::endl;

  delete[] x;
  delete[] y;
  return 0;
}
