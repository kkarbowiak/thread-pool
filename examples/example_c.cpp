/*
    Copyright 2017 Krzysztof Karbowiak
*/

#include "tpoolThreadPool.h"

#include <thread>
#include <iostream>
#include <cstddef>


int main()
{
    tpool::ThreadPool tp(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < 400; ++i)
    {
        auto job = [i]
        {
            std::cout << "j" << i << '\n';
        };
        tp.addJob(job);
    }
    std::cout << "all jobs added" << '\n';
    tp.waitUntilJobsCompleted();
    std::cout << "all jobs executed" << '\n';
}

