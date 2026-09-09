#include "client/Client01.h"

using namespace std;




int main()
{

    Client01::inst = new Client01();
    Client01::inst->run();
    delete Client01::inst;

    return 0;
}
