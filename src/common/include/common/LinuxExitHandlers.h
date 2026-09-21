
#ifndef NBE_CHAT_LINUXEXITHANDLERS_H
#define NBE_CHAT_LINUXEXITHANDLERS_H

#include <csignal>
#include <iostream>

using std::cout;

template<class T>
class LinuxExitHandlers
{
public:
    static void runExitHandlers(int signum)
    {

    }

    virtual void registerExitHandlers()
    {
        signal(SIGINT, T::runExitHandlers);
        signal(SIGTERM, T::runExitHandlers);
    }
};

#endif //NBE_CHAT_LINUXEXITHANDLERS_H
