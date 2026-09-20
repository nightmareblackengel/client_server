#ifndef NBE_CHAT_SERVERB3_H
#define NBE_CHAT_SERVERB3_H

#include <atomic>
#include <csignal>
#include <string>

#include "common/bootstrap.h"
#include "server/ServerClient01.h"
#include "common/exceptions/Cs01Exception.h"
#include "common/BaseTcpTransmitter.h"
#include "common/composition/IServerClientList.h"
#include "common/composition/MyThreadPool.h"

using std::atomic;
using std::string;

class ServerB3: public BaseTcpTransmitter
{
private:
    IServerClientList connectedClients;
public:
    atomic<bool> isRun = true;
    static ServerB3* inst;

    ServerB3(): BaseTcpTransmitter()
    {
        this->errorType = CS01_SERVER_TYPE;
    }

    ~ServerB3()
    {
        this->connectedClients.closeAll();
    }

    int run()
    {
        cout << IoTextColor::DEFAULT << "Сервер стартует..." << endl;
        ServerB3* app = ServerB3::inst;
        try {
            app->registerExitHandlers();

            app->createSocket();
            app->bindSocket();
            app->listenSocket();

            while (app->isRun) {
                cout << "Run ..." << endl;
                sleep(1);
                // это должно выполнятся вне потока и блокироваться, чтобы не выполнять многократно  acceptNewClient - без наличия клиента
//                int clientId = app->acceptNewClient();
//
//                app->clientsThrPool.enqueueTask([&app, clientId] {
//                    app->acceptClientInThread(clientId);
//                });
            }
        }
        catch (Cs01Exception& ex) {
            // если ошибка во время оставновки - не показываем
            if (ServerB3::inst->isRun)
            {
                cout << IoTextColor::RED << "Ошибка:" << ex.toString() << IoTextColor::DEFAULT << endl;
            }
        }
        catch(const char* msg) {
            cout << IoTextColor::RED << "ОШИБКА. Подробная информация: " << msg << IoTextColor::DEFAULT<< endl;
        }
        catch(...) {
            cout << IoTextColor::RED << "ОШИБКА. Экстренный выход из программы. Без дополнительной информации." << IoTextColor::DEFAULT << endl;
        }

        return 0;
    }

    // Регистрируем обработчик для SIGINT (Ctrl+C / кнопка Stop в CLion)
    // и для SIGTERM (команда kill в Linux)
    void registerExitHandlers()
    {
        signal(SIGINT, ServerB3::runExitHandlers);
        signal(SIGTERM, ServerB3::runExitHandlers);
    }
    static void runExitHandlers(int signum)
    {
        cout << "[Signal] Получен сигнал " << signum << ". Инициируем вежливую остановку..." << endl;
        ServerB3::inst->isRun = false;

        // остановим клиентов
        ServerB3::inst->connectedClients.terminateAnyDataTransmit();
        // остановим сервер
        // "Больше не передавай и не принимай данные."
        // Но сам файловый дескриптор остается существовать.
        shutdown(ServerB3::inst->socketId, SHUT_RDWR);
    }
};

ServerB3* ServerB3::inst = nullptr;

#endif //NBE_CHAT_SERVERB3_H
