
#ifndef NBE_CHAT_EPOLLSERVER_H
#define NBE_CHAT_EPOLLSERVER_H

#include "BaseServer.h"
#include "Epoll.h"

const int MAX_EPOLL_EVENTS = 64;

class EpollServer: public Epoll, public NonCopyableMovable
{
protected:
    BaseServer *tcpServer;
    int epollId;
    struct epoll_event events[MAX_EPOLL_EVENTS];
public:
    EpollServer(BaseServer *bs): tcpServer(bs)
    {
        this->epollId = DEFAULT_INVALID_DESCRIPTOR;
    }

    ~EpollServer()
    {
        // TODO: check and remove
        cout << "~EpollServer(). closing EpollFD" << endl;
        if (this->epollId != DEFAULT_INVALID_DESCRIPTOR) {
            cout << "closed EpollFD" << endl;
            close(this->epollId);
        } else {
            cout << "NOT closed EpollFD" << endl;
        }
    }

    struct epoll_event* getEvent(int ind)
    {
        return &(this->events[ind]);
    }

    struct epoll_event* getEvents()
    {
        return this->events;
    }

    int create()
    {
        this->epollId = epoll_create1(0);
        if (this->epollId == -1) {
            AppException::Throw("epoll_create1 failed");
        }

        return this->epollId;
    }

    int configureServerSocket(int serverId)
    {
        // Регистрация сокета через epoll_ctl и структуру epoll_event
        // Сначала настраивается структура epoll_event:
        struct epoll_event ev{};
        ev.events   = EPOLLIN;     // Интересуют события чтения
        ev.data.fd  = serverId; // Дескриптор сокета, за которым следим

        // сокет регистрируется в epoll
        int result = epoll_ctl(this->epollId, EPOLL_CTL_ADD, serverId, &ev);
        if (result == -1) {
            AppException::Throw("epoll_ctl: serverSocketFd failed");
        }

        return result;
    }

    int waitEvents(int timeout = -1)
    {
        // Системный вызов epoll_wait отправляет поток в сон до тех пор,
        // пока ядро Linux не зафиксирует активность хотя бы на одном из зарегистрированных сокетов.
        return epoll_wait(this->epollId, this->events, MAX_EPOLL_EVENTS, timeout);
    }

    int addNewClient(int clientSocketId)
    {
        // Регистрируем клиентский сокет в epoll
        struct epoll_event newClientEvent{};
        newClientEvent.events = EPOLLIN | EPOLLRDHUP;
        newClientEvent.data.fd = clientSocketId;

        return epoll_ctl(this->epollId, EPOLL_CTL_ADD, clientSocketId, &newClientEvent);
    }

    int removeClient(int socketId)
    {
        // Удаляем из epoll
        return epoll_ctl(this->epollId, EPOLL_CTL_DEL, socketId, nullptr);
    }

    //////////////////////////////
    /// EVENTS
    //////////////////////////////
    bool isClientDisconnectedEvent(uint32_t hexEvents)
    {
        return (hexEvents & (EPOLLRDHUP | EPOLLERR | EPOLLHUP));
    }
    bool isClientReadyToRecieve(uint32_t hexEvents)
    {
        // Данные готовы для чтения (EPOLLIN)
        return hexEvents & EPOLLIN;
    }
    //////////////////////////////
    /// ERRORS
    //////////////////////////////
    bool checkIsInterrupted(int err)
    {
        // Сигнал прервал системный вызов, продолжаем
        return err == EINTR;
    }
    bool checkIsFatalError(int err)
    {
        return err != EAGAIN && err != EWOULDBLOCK;
    }
};

#endif //NBE_CHAT_EPOLLSERVER_H
