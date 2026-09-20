//#include <iostream>

#include "include/server/ServerB3.h"


int main()
{
    ServerB3::inst = new ServerB3();
    int ret = ServerB3::inst->run();

    delete ServerB3::inst;
    return ret;
}

