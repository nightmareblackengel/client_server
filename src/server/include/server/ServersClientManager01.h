#ifndef NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
#define NBE_CHAT_SERVER_ISERVERCLIENTLIST_H

#include <map>
#include <mutex>
#include "common/WrapperMutex.h"
#include "common/blocking/BlockingServersClient.h"

using std::map;

class ServersClientManager01
{
private:
    map<int, BlockingServersClient*> clientList;
    WrapperMutex wmutexClients;
public:
    ServersClientManager01()
    {

    }

    ~ServersClientManager01()
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

    void addClient(int clientId, BlockingServersClient* sClient)
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

    void broadcastMessage(int fromSocketId, string& msg)
    {
        this->wmutexClients.lock();
        vector<int> clientIds(this->clientList.size());

        int clientIdsCount = 0, ind1;
        for (auto p1: this->clientList) {
            if (p1.first != fromSocketId) {
                clientIds[clientIdsCount] = p1.first;
            }

            clientIdsCount++;
        }
        this->wmutexClients.unlock();
        ///
        for (ind1 = 0; ind1 < clientIdsCount; ind1++) {
            try {
                // check it when realize "get message" on client.
                int res = send(clientIds[ind1], msg.c_str(), msg.length(), 0);
                cout << "sending msg to client=[" << clientIds[ind1] << "] from client=[" << fromSocketId << "]. Result =[" << res << "]" << endl;
            } catch (...)
            {
                cout << "MY TMP ERROR" << endl;
            }
        }
    }

    void freeItem(BlockingServersClient* &itemToFree)
    {
        delete itemToFree;
        itemToFree = nullptr;
    }
};

#endif //NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
