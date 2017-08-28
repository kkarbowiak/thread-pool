#include "tpoolThreadPool.h"

#include <iostream>
#include <thread>


int main()
{
    tpool::ThreadPool tp(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < 100; ++i)
    {
        auto job = [i]
        {
            std::cout << "j" << i << ": " << std::this_thread::get_id() << '\n';
        };
        tp.addJob(job);
    }
}

