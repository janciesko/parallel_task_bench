#include "polling.hpp"
#include <unistd.h>
#include <cstdio>

Poller::Poller(void) : task_count(0), task_registration_complete(false), terminated(false) {}

void Poller::register_request(MPI_Request req) {
  printf("registration\n");
  #pragma omp critical (pq)
  q.push(req);
}

inline void Poller::manage_task_count(bool reg_comp, bool increment) {
  #pragma omp critical (ptc)
  {
    if (reg_comp) {
      task_registration_complete = true;
    } else {
      if(increment) {
        ++this->task_count;
      } else {
        --this->task_count;
      }
    }

    if (task_count == 0 && task_registration_complete) {
      terminated = true;
    }
  }
}

void Poller::register_task() {
  manage_task_count(false, true);
}

void Poller::task_complete() {
  manage_task_count(false, false);
}

inline void Poller::mark_tr_complete() {
  manage_task_count(true, false);
}

void Poller::polling_loop() {
  mark_tr_complete();
  bool loop_continue = true;
  while (loop_continue) {
    
    #pragma omp critical (pq)
    {
      if (!terminated || !q.empty()) {

        printf("polling\n");
        for (size_t i = 0; i < this->q.size(); i++) {
          int flag = 0;
          MPI_Test(&this->q.front(), &flag, MPI_STATUS_IGNORE);
          if (flag == 0) {
            printf("incomplete\n");
            this->q.push(this->q.front());
          } else {
            printf("complete\n");
          }
          this->q.pop();
        }
      } else {
        loop_continue = false;
      }
    }

    usleep(1);
  }
}
