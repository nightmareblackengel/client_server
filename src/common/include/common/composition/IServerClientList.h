#ifndef NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
#define NBE_CHAT_SERVER_ISERVERCLIENTLIST_H

#include <map>
#include <mutex>
#include "common/interfaces/WrapperMutex.h"

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

    void sendStringToOtherClients(int fromSocketId, string& msg)
    {
        for (auto p1: this->clientList)
        {
            if (p1.first == fromSocketId) {
                continue;
            }
            try {
                int res = send(fromSocketId, msg.c_str(), msg.length(), 0);
                cout << "sending msg to client=[" << p1.first << "] from client=[" << fromSocketId << "]. Result =[" << res << "]" << endl;
            } catch (...)
            {
                cout << "MY TMP ERROR" << endl;
            }
        }
    }

    void freeItem(ServerClient01* &itemToFree)
    {
        delete itemToFree;
        itemToFree = nullptr;
    }
};

#endif //NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
