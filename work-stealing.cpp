#include <atomic>
#include <iostream>
#include <random>
#include <thread>
#include <unistd.h>

std::atomic<bool> terminate;

////////////////////////////////////////////////////////////////////////////////

#include "deque.cpp"

const int N = 4; // number of processors

DQueue job_queue[N];

void processor(int n)
{
    while (!terminate)
    {
        if (!job_queue[n].isEmpty()) // Still has jobs to do
        {
            int sleep_time;
            __transaction_atomic
            {
                sleep_time = job_queue[n].PopLeft();
            }
            if (sleep_time == 0) {
                std::cout << "Processor " << n << ": executing job (duration: " << sleep_time << " s)." << std::endl;
            }
            if (sleep_time > 0) {
                std::cout << "Processor " << n << ": executing job (duration: " << sleep_time << " s)." << std::endl;
                sleep(sleep_time);
            }
        }
        else // Stealing
        {
            for (int i = 0; i < N; i++) {
                int duration;
                int flag = false;
                if (i == n) continue;
                __transaction_atomic
                {
                    duration = job_queue[i].PopRight();
                    if (duration == -1);
                    if (duration == 0) job_queue[i].PushRight(duration);
                }
                if (duration > 0) {
                    std::cout << "Processor " << n << ": stealing job (duration: " << duration << " s) from processor " << i << "."<< std::endl;
                    sleep(duration);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void user()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> processor(0, N-1);
    std::uniform_int_distribution<int> duration(0, N);

    unsigned int time = 0;

    // generates a new job about once per second
    while (!terminate)
    {
        int p = processor(gen);
        int d = duration(gen);

        __transaction_relaxed
        {
            std::cout << time << ": scheduling a job on processor " << p << " (duration: " << d << " s)." << std::endl;
            job_queue[p].PushRight(d);
        }

        sleep(1);
        ++time;
    }
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::thread user_thread(user);

    std::thread processors[N];
    for (int i=0; i<N; ++i)
    {
        processors[i] = std::thread(processor, i);
    }

    sleep(60);
    terminate = true;

    user_thread.join();
    for (int i=0; i<N; ++i)
    {
        processors[i].join();
    }

    return 0;
}
