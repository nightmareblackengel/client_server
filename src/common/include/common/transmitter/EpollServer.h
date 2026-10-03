
#ifndef NBE_CHAT_EPOLLSERVER_H
#define NBE_CHAT_EPOLLSERVER_H

#include "BaseServer.h"
#include "Epoll.h"

const int MAX_EPOLL_EVENTS = 64;

class EpollServer: public Epoll, public NonCopyableMovable
{
protected:
    BaseServer *defServer;
    int epollId;
    struct epoll_event eEvents[MAX_EPOLL_EVENTS];
public:
    EpollServer(BaseServer *bs): defServer(bs)
    {
        this->epollId = DEFAULT_INVALID_DESCRIPTOR;
    }

    ~EpollServer()
    {
        if (this->epollId != DEFAULT_INVALID_DESCRIPTOR) {
            close(this->epollId);
        }
    }

    void create()
    {
        this->epollId = epoll_create1(0);
        if (this->epollId == -1) {
            AppException::Throw("epoll_create1 failed");
        }
    }

    void configureServerSocket(int serverId)
    {
        // Регистрация сокета через epoll_ctl и структуру epoll_event
        // Сначала настраивается структура epoll_event:
        struct epoll_event ev{};
        ev.events   = EPOLLIN;     // Интересуют события чтения
        ev.data.fd  = serverId; // Дескриптор сокета, за которым следим

        // сокет регистрируется в epoll
        if (epoll_ctl(this->epollId, EPOLL_CTL_ADD, serverId, &ev) == -1) {
            AppException::Throw("epoll_ctl: serverSocketFd failed");
        }
    }

    int waitEvents(int timeout = -1)
    {
        // Системный вызов epoll_wait отправляет поток в сон до тех пор,
        // пока ядро Linux не зафиксирует активность хотя бы на одном из зарегистрированных сокетов.
        return epoll_wait(this->epollId, this->eEvents, MAX_EPOLL_EVENTS, timeout);
    }

    void addNewClient(int clientSocketId)
    {
        // Регистрируем клиентский сокет в epoll
        struct epoll_event newClientEvent{};
        newClientEvent.events = EPOLLIN | EPOLLRDHUP;
        newClientEvent.data.fd = clientSocketId;

        epoll_ctl(this->epollId, EPOLL_CTL_ADD, clientSocketId, &newClientEvent);
    }
};

#endif //NBE_CHAT_EPOLLSERVER_H
