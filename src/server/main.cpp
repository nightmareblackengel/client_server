//#include <iostream>

//#include "include/server/Server01.h"
//int main()
//{
//    Server01::inst = new Server01();
//    int ret = Server01::inst->run();
//
//    delete Server01::inst;
//    return ret;
//}

#include "server/ServerV3.h"

int main()
{
    ServerV3::inst = new ServerV3();
    int ret = ServerV3::inst->run();

    delete ServerV3::inst;
    return ret;
}
