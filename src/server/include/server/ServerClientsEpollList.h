
#ifndef NBE_CHAT_SERVERCLIENTSEPOLLLIST_H
#define NBE_CHAT_SERVERCLIENTSEPOLLLIST_H

#include <sys/socket.h>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>

#include "common/WrapperMutex.h"
#include "common/transmitter/BaseTransmitter.h"

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;

class ServerClientsEpollList
{
private:
    map<int, int> clientList;
    WrapperMutex wmutexClients;
    BaseTransmitter* transmitter;
public:

    ServerClientsEpollList(BaseTransmitter* _transmitter): transmitter(_transmitter)
    {

    }

    ~ServerClientsEpollList()
    {
        this->closeAll();
    }

    void closeAll()
    {
        this->wmutexClients.lock();
        this->clientList.clear();
        this->wmutexClients.unlock();
    }

    // используется для принудительное остановки "блокируемых" функций при передаче данных (recv, send....)
    void terminateAnyDataTransmit()
    {
        this->wmutexClients.lock();
        for (auto &item : this->clientList){
            // "Больше не передавай и не принимай данные."
            // Но сам файловый дескриптор остается существовать.
            shutdown(item.first, SHUT_RDWR);
        }
        this->wmutexClients.unlock();
    }

    void addClient(int clientId)
    {
        this->wmutexClients.lock();
        this->clientList[clientId] = clientId;
        this->wmutexClients.unlock();
    }

    void remove(int clientId)
    {
        this->wmutexClients.lock();
        auto iterator = this->clientList.find(clientId);
        if (iterator != this->clientList.end()) {
            this->clientList.erase(clientId);
        }

        this->wmutexClients.unlock();
    }

    void broadcastMessage(int fromSocketId, string& msg)
    {
        this->wmutexClients.lock();
        vector<int> clientIds(this->clientList.size());

        int clientIdsCount = 0, ind1;
        for (auto p1: this->clientList) {
            if (p1.first != fromSocketId) {
                clientIds[clientIdsCount] = p1.first;
                clientIdsCount++;
            }
        }
        this->wmutexClients.unlock();
        ///
        for (ind1 = 0; ind1 < clientIdsCount; ind1++) {
            try {
                int res = this->transmitter->sendStringToSocket(clientIds[ind1], msg);
                cout << "sending msg to client=[" << clientIds[ind1] << "] from client=[" << fromSocketId << "]. Result =[" << res << "]" << endl;
            } catch (...)
            {
                cout << "MY TMP ERROR" << endl;
            }
        }
    }
};

#endif //NBE_CHAT_SERVERCLIENTSEPOLLLIST_H
