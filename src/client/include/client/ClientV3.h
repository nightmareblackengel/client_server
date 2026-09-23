#ifndef NBE_CHAT_CLIENTB3_H
#define NBE_CHAT_CLIENTB3_H

#include "common/bootstrap.h"
#include "common/transmitter/EpollClient.h"

class ClientV3: public EpollClient
{
public:
    static ClientV3 *inst;

    void run()
    {

    }
};

ClientV3* ClientV3::inst = nullptr;

#endif //NBE_CHAT_CLIENTB3_H
