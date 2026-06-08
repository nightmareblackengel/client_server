#ifndef LESSONS_000_WRAPPERMUTEX_H
#define LESSONS_000_WRAPPERMUTEX_H

#include <mutex>

class WrapperMutex
{
private:
    bool status;
    std::mutex descriptor;
public:
    WrapperMutex()
    {
        this->status = false;
    }
    ~WrapperMutex()
    {
        if (this->status) {
            this->unlock();
        }
    }

    void lock()
    {
        this->descriptor.lock();
        this->status = true;
    }
    void unlock()
    {

        this->descriptor.unlock();
        this->status = false;
    }
};

#endif //LESSONS_000_WRAPPERMUTEX_H
