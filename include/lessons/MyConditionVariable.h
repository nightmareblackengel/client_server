#ifndef NBE_CHAT_SERVER_MYCONDITIONVARIABLE_H
#define NBE_CHAT_SERVER_MYCONDITIONVARIABLE_H

// https://en.cppreference.com/cpp/thread/condition_variable

#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <iostream>

using std::mutex;
using std::condition_variable;
using std::string;
using std::thread;
using std::cout;
using std::endl;
using std::unique_lock;
using std::lock_guard;

class MyConditionalVariable
{
public:
    static string data;
    static bool isProcessReady;
    static bool isProcessed;

    static mutex mut;
    static condition_variable condVar;

public:

    void worker()
    {
        unique_lock myLock(MyConditionalVariable::mut);
        MyConditionalVariable::condVar.wait(myLock, [] {
            return MyConditionalVariable::isProcessReady;
        });

        // after the wait, we own the lock
        cout << "Worker thread is processing data\n";
        MyConditionalVariable::data+= " after processing";

        this->isProcessed = true;
        cout << "Worker thread signals data processing completed\n";

        // manual unlocking is done before notifying, to avoid waking up
        // the waiting thread only to block again (see notify_one for details)
        myLock.unlock();
        MyConditionalVariable::condVar.notify_one();
        sleep(3);
    }

    static void run()
    {
        MyConditionalVariable mcv;
        thread tWorker(&MyConditionalVariable::worker, mcv);

        MyConditionalVariable::data = "Example data";
        // send data to the worker thread
        {
            lock_guard lg(MyConditionalVariable::mut);
            MyConditionalVariable::isProcessReady = true;
            cout << "main() signals data ready for processing\n";
        }

        MyConditionalVariable::condVar.notify_one();
        cout << "MY notify end" << endl;
        {
            unique_lock ul(MyConditionalVariable::mut);
            MyConditionalVariable::condVar.wait(ul, [] {
                return MyConditionalVariable::isProcessed;
            });
        }
        cout << "Back in main(), data = " << data << '\n';

        tWorker.join();
    }

};

string MyConditionalVariable::data = "";
bool MyConditionalVariable::isProcessReady = false;
bool MyConditionalVariable::isProcessed = false;
mutex MyConditionalVariable::mut;
condition_variable MyConditionalVariable::condVar;

#endif //NBE_CHAT_SERVER_MYCONDITIONVARIABLE_H
