#ifndef NBE_CHAT_SERVERB3_H
#define NBE_CHAT_SERVERB3_H

#include <atomic>
#include <csignal>
#include <string>
#include <cstring>
#include <sys/epoll.h>

#include "common/bootstrap.h"
#include "ServersClientManager01.h"
#include "common/MyThreadPool.h"
#include "common/transmitter/EpollServer.h"
#include "common/interfaces/LinuxExitHandlers.h"
#include "common/static/AppException.h"

using std::atomic;
using std::string;

class ServerV3:
        public EpollServer,
        public LinuxExitHandlers<ServerV3>
{
private:
    ServersClientManager01 connectedClients;
    BaseServer tcpServer;
public:
    // TODO: extract EpollServer from
//    EpollServer epollServer;

    atomic<bool> isRun = true;
    static ServerV3* inst;

    ServerV3(): EpollServer(&this->tcpServer)
    {
        AppException::type = CS01_SERVER_TYPE;
    }

    ~ServerV3()
    {
        this->connectedClients.closeAll();
    }

    int createSocketAndListen()
    {
        this->registerExitHandlers();

        this->tcpServer.createSocket();
        int serverId = this->tcpServer.getSocket()->getId();
        this->setFileDescriptorNonBlockFlag(serverId);
        this->tcpServer.bindSocket();
        this->tcpServer.listenSocket();

        return serverId;
    }

    int run()
    {
        cout << IoTextColor::DEFAULT << "Сервер стартует..." << endl;
        ServerV3* app = ServerV3::inst;
        try {
            app->registerExitHandlers();
            int serverId = this->createSocketAndListen();

// TODO: TEST EPOL

            // Создание epoll-экземпляра
            app->create();
            app->configureServerSocket(serverId);
            const int bufferSize = 1024*1024;

            while(app->isRun) {
                // TODO: add try
                cout << "RUN APP" << endl;
                // Ждем событий от ядра Linux
                int fdCount = app->waitEvents();
                if (fdCount == -1) {
                    // Сигнал прервал системный вызов, продолжаем
                    if (this->checkIsInterrupted(errno)) {
                        AppException::Throw("NBE epoll_wait failed. Exiting.");
                        break;
                    }
                }
                // TODO: EPOLL Refactoring next part!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                // Обходим все сокеты, на которых что-то произошло
                for (int ind1 = 0; ind1 < fdCount; ind1++) {
                    // TODO: add try catch
                    int eventSocketId = eEvents[ind1].data.fd;
                    // Событие произошло на слушающем сокете сервера -> Новое подключение!
                    if (eventSocketId == serverId) {
                        int newClientId = this->defServer->acceptNewClient(serverId);
                        // Переводим сокет нового клиента в НЕБЛОКИРУЮЩИЙ режим!
                        this->setFileDescriptorNonBlockFlag(newClientId);
                        app->addNewClient(newClientId);

                        cout << "Новый клиент подключен: FD=[" << newClientId << "]" << endl;
                        // todo: MyServerClients Добавляем clientFd в свой список/мапу клиентов
                    }
                    // Событие на сокете существующего клиента -> Пришли данные или отключение
                    else {
                        // Проверяем на отключение (EPOLLRDHUP) или ошибку
                        if (this->isClientDisconnectedEvent(eEvents[ind1].events)) {
                            std::cout << "Клиент отключился: FD [" << eventSocketId << "]" << std::endl;
                            // Удаляем из epoll и закрываем
                            this->removeClient(eventSocketId);

                            close(eventSocketId);
                            // TODO: MyServerClients Удаляем из своего списка клиентов
                            continue;
                        }
                        // Данные готовы для чтения (EPOLLIN)
                        if (this->isClientReadyToRecieve(eEvents[ind1].events)) {
                            char buffer[bufferSize];
                            // TODO: move to default server
                            ssize_t bytesRead = this->tcpServer.simpleReceive(eventSocketId, buffer, sizeof(buffer) - 1, 0);
                            if (bytesRead > 0) {
                                std::cout << "Получено от FD " << eventSocketId << ": " << buffer << endl;
                                // Делаем бродкаст всем остальным клиентам
                                // TODO: MyServerClients
                                // broadcastMessage(buffer, currentFd);
                            }
                            else if (bytesRead == 0) {
                                buffer[bytesRead] = '\0';
                                // Клиент закрыл соединение
                                this->removeClient(eventSocketId);
                                close(eventSocketId);
                                // TODO: MyServerClients Удаляем из списка клиентов
                            } else if (this->checkIsFatalError(errno)) {
                                this->removeClient(eventSocketId);
                                // TODO: MyServerClients Удаляем из списка клиентов
                                close(eventSocketId);

                                // Реальная ошибка чтения
                                AppException::Throw("NBE recv failed");
                            }
                        } else {
                            // TODO: output unparsed events
                            cout << "NBE UNPROCESSED error. events=[" << eEvents[ind1].events << "]" << endl;
                        }
                    }
                }
            }
// TODO:002

            cout << "END OF epoll_create1" << endl;
// TODO: TEST EPOL
            return 0;
            // TODO: remove bellow
            while (app->isRun) {
                cout << "Run ..." << endl;
                sleep(1);
                // это должно выполнятся вне потока и блокироваться, чтобы не выполнять многократно  acceptNewClient - без наличия клиента
                //int clientId = app->acceptNewClient();
/*********************************/
                BaseServersClient *c1 = new BaseServersClient();

                cout << "Ожидание подключения нового клиента (accept)..." << endl;
                int clientId = -1;
                try {
                    clientId = c1->acceptFromServer(this->tcpServer.getSocket()->getId());
//            this->connectedClients.addClient(clientId, c1);
                } catch(TransmitterException &ex1) {
                    // TODO: add remove in destruct
                    delete c1;
                    c1 = nullptr;
                    cout << "Не получилось присоединить клиента" << endl;
//                    this->throwException("Не получилось присоединить клиента");
                }

                //return clientId;
/*********************************/
                cout << "clientId=[" << clientId << "]" << endl;
//
//                app->clientsThrPool.enqueueTask([&app, clientId] {
//                    app->acceptClientInThread(clientId);
//                });
            }
        }
        catch (TransmitterException& ex) {
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
//      TODO: remove method after "4"
//    int acceptNewClient()
//    {
//        // TODO: change there -> Epoll
//        BaseServersClient *c1 = new BaseServersClient();
//
//        cout << "Ожидание подключения нового клиента (accept)..." << endl;
//        int clientId = -1;
//        try {
//            clientId = c1->acceptFromServer(this->defaultServer.getSocket()->getId());
////            this->connectedClients.addClient(clientId, c1);
//        } catch(TransmitterException &ex1) {
//            delete c1;
//            c1 = nullptr;
//            AppException::Throw("Не получилось присоединить клиента");
//        }
//
//        return clientId;
//    }

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
        shutdown(ServerV3::inst->tcpServer.getSocket()->getId(), SHUT_RDWR);
    }
};

ServerV3* ServerV3::inst = nullptr;
// TODO:
// 3. вывести отдельно в композицию methods: "epoll_ctl(..., EPOLL_CTL_DEL, ...) / , ...."
// 4. add new client-server-v03
#endif //NBE_CHAT_SERVERB3_H
