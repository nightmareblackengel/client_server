//#include <iostream>

#include "Server01.h"


int main()
{
    Server01::inst = new Server01();
    int ret = Server01::inst->run();
    delete Server01::inst;
    return ret;
}

