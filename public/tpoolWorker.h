#ifndef TPOOL_WORKER_H
#define TPOOL_WORKER_H

#include <thread>


namespace tpool { class CommandQueue; }

namespace tpool
{
    class Worker
    {
        public:
            ~Worker();

            void start(CommandQueue & command_queue);

        private:
            std::thread mThread;
    };
}

#endif /* TPOOL_WORKER_H */

