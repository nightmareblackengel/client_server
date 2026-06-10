#ifndef NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
#define NBE_CHAT_SERVER_ISERVERCLIENTLIST_H

#include <map>
#include <mutex>
#include "interfaces/WrapperMutex.h"

using std::map;

class IServerClientList
{
private:
    map<int, ServerClient01*> clientList;
    WrapperMutex wmutexClients;
public:
    IServerClientList()
    {

    }

    ~IServerClientList()
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

    void addClient(int clientId, ServerClient01* sClient)
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

    void freeItem(ServerClient01* &itemToFree)
    {
        delete itemToFree;
        itemToFree = nullptr;
    }
};

#endif //NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
