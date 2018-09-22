#include <atomic>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <stdlib.h>

std::atomic<bool> terminate;
int x, y, diff;

void check()
{
    __transaction_atomic
    {
        if (x != y)
            ++diff;
    }
}

void flattenedTransaction() {
    bool flag1 = false, flag2 = false;
    int A_commit = rand() % 2;
    int B_commit = rand() % 2;
    __transaction_atomic {
        flag1 = true;
        __transaction_atomic {
            y = 1;
            flag2 = true;
            if (!B_commit) __transaction_cancel;
        }
        //if (flag2 == true) x = 5;
        if (flag2 == false)  // Inner is aborted
            __transaction_cancel;

        // Inner committed
        if (!A_commit) { // Outer is aborted
            __transaction_cancel; // Outer is aborted while inner is committed
        }
        // Both inner and outer are commited
        x = 1;
    }
}

void closedTransaction() {
    bool flag1 = false, flag2 = false;
    int A_commit = rand() % 2;
    int B_commit = rand() % 2;
    __transaction_atomic {
        flag1 = true;
        __transaction_atomic {
            y = 1;
            flag2 = true;
            if (!B_commit) __transaction_cancel;
        }
        //if (flag2 == true) x = 5;
        if (flag2 == false) // Inner is aborted
            if (!A_commit) // Both inner and outer are aborted
                __transaction_cancel;
            else // Inner is aborted wihle outer is commited
                x = 1;

        // Inner committed
        if (!A_commit) { // Outer is aborted
            __transaction_cancel; // Outer is aborted while inner is committed
        }
        // Both inner and outer are commited
        x = 1;
    }
}

void openTransaction() {
    bool flag1 = false, flag2 = false;
    int A_commit = rand() % 2;
    int B_commit = rand() % 2;
    __transaction_atomic {
        flag1 = true;
        __transaction_atomic {
            y = 1;
            flag2 = true;
            if (!B_commit) __transaction_cancel;
        }
        //if (flag2 == true) x = 5;
        if (flag2 == false) // Inner is aborted
            if (!A_commit) // Both inner and outer are aborted
                __transaction_cancel;
            else // Inner is aborted wihle outer is commited
                x = 1;

        // Inner committed
        if (A_commit) { // Both inner and outer are commited
            x = 1;
        }
    }
}

void toggle()
{
    try {
        __transaction_atomic
        {
            x = x+1;
            __transaction_atomic
            {
                x = x+1;
                __transaction_cancel;
            }
        }
    } catch (int& e) {
        std::cout << "Caught exception - transaction aborted" << std::endl;
    };
}

void exceptions()
{
    try {
        __transaction_atomic
        {
            x = x-1;
            __transaction_cancel;
            throw 4;
            y = y-1;
        }
        std::cout << "Transaction commited" << std::endl;
    } catch (int& e) {
        std::cout << "Caught exception - transaction aborted" << std::endl;
    };
}

int main()
{
    srand(time(NULL));
    std::cout << "x: " << x << ", y: " << y << std::endl;
    closedTransaction();

    check();

    std::cout << "x: " << x << ", y: " << y << std::endl;

    return 0;
}
