
#ifndef NBE_CHAT_RAIISOCKET_H
#define NBE_CHAT_RAIISOCKET_H

#include "RaiiFileDescriptor.h"

class RaiiSocket: public RaiiFileDescriptor
{
public:
    RaiiSocket(int _id = DEFAULT_INVALID_DESCRIPTOR): RaiiFileDescriptor(_id)
    {

    }
};

#endif //NBE_CHAT_RAIISOCKET_H
