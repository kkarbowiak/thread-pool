# C++11 Thread Pool #

This is my C++11-based implementation of a bounded thread pool. I wrote a simple thread pool some years ago, when C++11 was not yet standardised. Now, I decided to rewrite it using new language features to see how much more elegant it can get without platform-dependent code (POSIX vs. Win32).

## Why ##

C++11 finally brings threading into the language standard and the standard library. There are two primitives provided that allow asynchronous execution, namely `std::thread` and `std::async`. Both are fine for executing longer tasks, but when the need arises to execute a lot of small jobs in parallel, the overhead of creating and destroying lots of threads may get too big. This is when thread pools come into play.

## How ##

This thread pool creates a fixed number of threads. Upon creation, the threads block on a job queue waiting for assignments. A job can be anything that fits into an `std::function`, `std::function<void ()>` to be specific. This means it is easy to execute free and member functions (perhaps using `std::bind()`), function objects, and lambdas and closures.

My first implementation represented job as an abstract base class, from which clients had to derive, but I find the current version a much better and less restricting design.

## Features ##

This thread pool has the following features:

 - fixed number of threads
 - uses `std::function<void ()>` to represent jobs
 - ability to clear queued jobs (`clearPendingJobs()` function)
 - ability to wait for all queued jobs to complete (`waitUntilJobsCompleted()` function)
 - guaranteed thread cleanup if `ThreadPool`'s constructor throws

## Usage ##

The simplest usage is shown below.

```
#include "tpoolThreadPool.h"

#include <thread>
#include <chrono>
#include <iostream>

int main()
{
    tpool::ThreadPool tp(std::thread::hardware_concurrency());
    for (int i = 1; i <= 10; ++i)
    {
        auto job = [i]
        {
            std::this_thread::sleep_for(std::chrono::seconds(i));
            std::cout << i << '\n';
        };
        tp.addJob(job);
    }
}
```

Some other examples can be found in _examples_ directory.

## License ##

This project is released under **MIT/X11** license, so feel free to do anything you like with it.


