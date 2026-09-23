#ifndef NBE_CHAT_SERVERB3_H
#define NBE_CHAT_SERVERB3_H

#include <atomic>
#include <csignal>
#include <string>

#include "common/bootstrap.h"
#include "common/exceptions/Cs01Exception.h"
#include "ServersClientManager01.h"
#include "common/MyThreadPool.h"
#include "common/transmitter/EpollServer.h"
#include "common/interfaces/LinuxExitHandlers.h"

using std::atomic;
using std::string;

class ServerV3: public EpollServer, public LinuxExitHandlers<ServerV3>
{
private:
    ServersClientManager01 connectedClients;
public:
    atomic<bool> isRun = true;
    static ServerV3* inst;

    ServerV3(): EpollServer()
    {
        this->errorType = CS01_SERVER_TYPE;
    }

    ~ServerV3()
    {
        this->connectedClients.closeAll();
    }

    int run()
    {
        cout << IoTextColor::DEFAULT << "Сервер стартует..." << endl;
        ServerV3* app = ServerV3::inst;
        try {
            app->registerExitHandlers();

            app->createSocket();
//            app->setFileDescriptorNonBlockFlag(app->bSocket->getId());
            app->bindSocket();
            app->listenSocket();

            while (app->isRun) {
                cout << "Run ..." << endl;
                sleep(1);
                // это должно выполнятся вне потока и блокироваться, чтобы не выполнять многократно  acceptNewClient - без наличия клиента
                int clientId = app->acceptNewClient();
                cout << "clientId=[" << clientId << "]" << endl;
//
//                app->clientsThrPool.enqueueTask([&app, clientId] {
//                    app->acceptClientInThread(clientId);
//                });
            }
        }
        catch (Cs01Exception& ex) {
            // если ошибка во время оставновки - не показываем
            if (ServerV3::inst->isRun)
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

    int acceptNewClient()
    {
        // TODO: change there -> Epoll
        BaseServersClient *c1 = new BaseServersClient();

        cout << "Ожидание подключения нового клиента (accept)..." << endl;
        int clientId = -1;
        try {
            clientId = c1->acceptFromServer(this->bSocket.getId());
            this->connectedClients.addClient(clientId, c1);
        } catch(Cs01Exception &ex1) {
            delete c1;
            c1 = nullptr;
            this->throwException("Не получилось присоединить клиента");
        }

        return clientId;
    }

    // Регистрируем обработчик для SIGINT (Ctrl+C / кнопка Stop в CLion)
    // и для SIGTERM (команда kill в Linux)
    void registerExitHandlers()
    {
        signal(SIGINT, ServerV3::runExitHandlers);
        signal(SIGTERM, ServerV3::runExitHandlers);
    }
    static void runExitHandlers(int signum)
    {
        cout << "[Signal] Получен сигнал " << signum << ". Инициируем вежливую остановку..." << endl;
        ServerV3::inst->isRun = false;

        // остановим клиентов
        ServerV3::inst->connectedClients.terminateAnyDataTransmit();
        // остановим сервер
        // "Больше не передавай и не принимай данные."
        // Но сам файловый дескриптор остается существовать.
        shutdown(ServerV3::inst->bSocket.getId(), SHUT_RDWR);
    }
};

ServerV3* ServerV3::inst = nullptr;

#endif //NBE_CHAT_SERVERB3_H
