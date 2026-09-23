
#ifndef NBE_CHAT_EPOLL_H
#define NBE_CHAT_EPOLL_H

#include <fcntl.h>
#include "common/exceptions/Cs01Exception.h"

// TODO: arch: extract @throw Cs01Exception@
class Epoll
{
public:
    bool setFileDescriptorNonBlockFlag(int fileDescriptor)
    {
        // 1. Получаем текущие флаги дескриптора
        int flags = fcntl(fileDescriptor, F_GETFL, 0);
        if (flags == -1) {
            throw Cs01Exception("fcntl F_GETFL failed");
            return false;
        }
        // 2. Устанавливаем флаг O_NONBLOCK поверх существующих
        if (fcntl(fileDescriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
            throw Cs01Exception("fcntl F_SETFL O_NONBLOCK failed");
            return false;
        }
        // TODO: remove
        cout << "set non block success" << endl;
        return true;
    }
};

#endif //NBE_CHAT_EPOLL_H
