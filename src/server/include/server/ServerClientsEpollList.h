
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
    map<int, RaiiSocket*> clientList;
    WrapperMutex wmutexClients;
    BaseTransmitter* tcp;
public:
    ServerClientsEpollList(BaseTransmitter* _t): tcp(_t)
    {

    }

    ~ServerClientsEpollList()
    {
        this->closeAll();
    }

    void closeAll()
    {
        this->wmutexClients.lock();

        // & -> used for set nullptr to clientLIst item
        for (auto &item: this->clientList) {
            if (item.second != nullptr) {
                this->freeItem(item.second);
            }
        }
        this->clientList.clear();
        this->wmutexClients.unlock();
    }

    void addClient(int clientId, RaiiSocket* sClient)
    {
        this->wmutexClients.lock();
        this->clientList[clientId] = sClient;
        this->wmutexClients.unlock();
    }

    void remove(int clientId)
    {
        this->wmutexClients.lock();
        auto iterator = this->clientList.find(clientId);
        if (iterator != this->clientList.end()) {
            this->freeItem(iterator->second);
            this->clientList.erase(clientId);
        }

        this->wmutexClients.unlock();
    }

    void freeItem(RaiiSocket* &itemToFree)
    {
        delete itemToFree;
        itemToFree = nullptr;
    }

    int acceptNewClient(int serverSocketId)
    {
        int newClientId = this->tcp->acceptNewClient(serverSocketId);
        if (newClientId >= 0) {
            RaiiSocket *newSocket = new RaiiSocket(newClientId);
            // TODO: check when run desctructor for newSocket
            cout << "created new RAIISOCKET = [" << newClientId << "]" << endl;
            this->addClient(newClientId, newSocket);
        }

        return newClientId;
    }

    // используется для принудительное остановки "блокируемых" функций при передаче данных (recv, send....)
    // из-за вызова из signal - мьютексы убираем
    void terminateAnyDataTransmit()
    {
//        this->wmutexClients.lock();
        for (auto &item : this->clientList){
            this->tcp->terminateDataInSocket(item.first);
        }
//        this->wmutexClients.unlock();
    }

    void broadcastMessage(int fromSocketId, string& msg)
    {
        this->wmutexClients.lock();
        vector<int> clientIds(this->clientList.size());

        int clientIdsCount = 0, ind1;
        for (auto &client: this->clientList) {
            if (client.first != fromSocketId) {
                clientIds[clientIdsCount] = client.first;
                clientIdsCount++;
            }
        }
        this->wmutexClients.unlock();
        ///
        for (ind1 = 0; ind1 < clientIdsCount; ind1++) {
            try {
                int res = this->tcp->sendStringToSocket(clientIds[ind1], msg);
                cout << "sending msg to client=[" << clientIds[ind1] << "] from client=[" << fromSocketId << "]. Result =[" << res << "]" << endl;
            } catch (...)
            {
                cout << "MY TMP ERROR" << endl;
            }
        }
    }
};

#endif //NBE_CHAT_SERVERCLIENTSEPOLLLIST_H
