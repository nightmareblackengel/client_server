using namespace std;

//#include "client/Client01.h"
//int main()
//{
//    Client01::inst = new Client01();
//    Client01::inst->run();
//    delete Client01::inst;
//
//    return 0;
//}

#include "client/ClientV3.h"
int main()
{
    ClientV3::inst = new ClientV3();
    ClientV3::inst->run();
    delete ClientV3::inst;

    return 0;
}
