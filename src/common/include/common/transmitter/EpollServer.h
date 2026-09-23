
#ifndef NBE_CHAT_EPOLLSERVER_H
#define NBE_CHAT_EPOLLSERVER_H

#include "BaseServer.h"
#include "Epoll.h"

class EpollServer: public BaseServer, public Epoll
{

};

#endif //NBE_CHAT_EPOLLSERVER_H
