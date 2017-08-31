/*
    Copyright 2017 Krzysztof Karbowiak
*/

#include "tpoolThreadPool.h"

#include <thread>
#include <functional>
#include <iostream>
#include <cstddef>


class Job
{
    public:
        explicit Job(std::size_t number) : mNumber(number), mActive(true), mExecuted(false) {}
        Job(Job const & other) : mNumber(other.mNumber), mActive(other.mActive), mExecuted(other.mExecuted) {}
        Job(Job && other) : mNumber(other.mNumber), mActive(other.mActive), mExecuted(other.mExecuted) { other.mActive = false; }
        ~Job() { if (mActive && !mExecuted) { std::cout << "j" << mNumber << ": canceled" << '\n'; } }
        Job & operator=(Job && other) { mNumber = other.mNumber; mActive = other.mActive; mExecuted = other.mExecuted; other.mActive = false; return *this; }
        void execute() { mExecuted = true; std::cout << "j" << mNumber << ": executed" << '\n'; }

    private:
        std::size_t mNumber;
        bool mActive;
        bool mExecuted;
};

int main()
{
    tpool::ThreadPool tp(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < 100; ++i)
    {
        tp.addJob(std::bind(&Job::execute, Job(i)));
    }
    tp.clearPendingJobs();
}

