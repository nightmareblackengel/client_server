#ifndef NBE_CHAT_SERVER_IRESOURCE_H
#define NBE_CHAT_SERVER_IRESOURCE_H

#include <iostream>
#include <unistd.h>
#include "bootstrap.h"

using std::cout;
using std::endl;

class IResource
{
protected:
    int resourceId;

    IResource()
    {
        this->resourceId = DEFAULT_INVALID_DESCRIPTOR;
    }

    ~IResource()
    {
        this->Close();
    }

    int Close()
    {
        if (this->resourceId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->resourceId);
        if (closeRes == 0) {
            cout << "resource закрыт" << endl;
        } else {
            cout << "resource незакрыт. Код ошибки: " << closeRes << endl;
        }
        this->resourceId = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

#endif //NBE_CHAT_SERVER_IRESOURCE_H
