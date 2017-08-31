/*
    Copyright 2017 Krzysztof Karbowiak
*/

#ifndef TPOOL_WORKER_H
#define TPOOL_WORKER_H

#include <thread>


namespace tpool { class JobQueue; }

namespace tpool
{
    class Worker
    {
        public:
            ~Worker();

            void start(JobQueue & job_queue);

        private:
            std::thread mThread;
    };
}

#endif /* TPOOL_WORKER_H */

