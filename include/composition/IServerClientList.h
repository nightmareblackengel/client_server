#ifndef NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
#define NBE_CHAT_SERVER_ISERVERCLIENTLIST_H

#include <map>

using std::map;

class IServerClientList
{
private:
    map<int, ServerClient01*> clientList;

public:
    IServerClientList()
    {

    }

    ~IServerClientList()
    {
        // & -> used for set nullptr to clientLIst item
        for (auto &item: this->clientList) {
            if (item.second != nullptr) {
                this->freeItem(item.second);
            }
        }
        this->clientList.clear();
    }

    void addClient(int clientId, ServerClient01* sClient)
    {
        this->clientList[clientId] = sClient;
    }

    void remove(int clientId)
    {
        this->freeItem(this->clientList[clientId]);
        this->clientList.erase(clientId);
    }

    void freeItem(ServerClient01* itemToFree)
    {
        delete itemToFree;
        itemToFree = nullptr;
    }
};

#endif //NBE_CHAT_SERVER_ISERVERCLIENTLIST_H
