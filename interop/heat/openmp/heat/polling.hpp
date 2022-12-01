#include <queue>

#include <mpi.h>

#ifndef _HG_POLLING
#define _HG_POLLING

class Poller {
  private:
    std::queue<MPI_Request> q;
    bool terminated, task_registration_complete;
    int task_count;
    void manage_task_count(bool, bool);
    void mark_tr_complete(void);
  public:
    Poller(void);
    void register_request(MPI_Request req);
    void register_task(void);
    void task_complete(void);
    void polling_loop(void);
};
    

#endif
