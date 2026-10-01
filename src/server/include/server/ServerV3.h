#ifndef NBE_CHAT_SERVERB3_H
#define NBE_CHAT_SERVERB3_H

#include <atomic>
#include <csignal>
#include <string>
#include <cstring>
#include <sys/epoll.h>

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
            int serverId = app->bSocket.getId();
            app->setFileDescriptorNonBlockFlag(serverId);
            app->bindSocket();
            app->listenSocket();

// TODO: TEST EPOL

            // Создание epoll-экземпляра
            int epollFd = -1;
            epollFd = epoll_create1(0);
            if (epollFd == -1) {
                this->throwException("epoll_create1 failed");
                // Обработка ошибки
            }
            // Регистрация сокета через epoll_ctl и структуру epoll_event
            // Сначала настраивается структура epoll_event:
            struct epoll_event ev{};
            ev.events = EPOLLIN;     // Интересуют события чтения
            ev.data.fd = serverId; // Дескриптор сокета, за которым следим

            // сокет регистрируется в epoll
            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverId, &ev) == -1) {
                this->throwException("epoll_ctl: serverSocketFd failed");
            }

            // Системный вызов epoll_wait отправляет поток в сон до тех пор,
            // пока ядро Linux не зафиксирует активность хотя бы на одном из зарегистрированных сокетов.
            int maxEvents = 100;
            int ewRes = epoll_wait(epollFd, &ev, maxEvents, 1);
// TODO:002
            const int MAX_EPOLL_EVENTS = 64;
            struct epoll_event eEvents[MAX_EPOLL_EVENTS];


            while(app->isRun) {
                // Ждем событий от ядра Linux
                int fdCount = epoll_wait(epollFd, eEvents, MAX_EPOLL_EVENTS, -1);
                if (fdCount == -1) {
                    // Сигнал прервал системный вызов, продолжаем
                    if (errno == EINTR) {
                        this->throwException("NBE epoll_wait failed. Exiting.");
                        break;
                    }
                }
                // Обходим все сокеты, на которых что-то произошло
                for (int ind1 = 0; ind1 < fdCount; ind1++) {
                    int eventSocketId = eEvents[ind1].data.fd;
                    // Событие произошло на слушающем сокете сервера -> Новое подключение!
                    if (eventSocketId == serverId) {
                        sockaddr_in clientAddr {};
                        socklen_t clientSize = sizeof(clientAddr);

                        int newClientId = accept(eventSocketId, (sockaddr*) &clientAddr, &clientSize);
                        if (newClientId >= 0) {
                            // Переводим сокет нового клиента в НЕБЛОКИРУЮЩИЙ режим!
                            app->setFileDescriptorNonBlockFlag(newClientId);
                            // Регистрируем клиентский сокет в epoll
                            struct epoll_event newClientEvent{};
                            newClientEvent.events = EPOLLIN | EPOLLRDHUP;
                            newClientEvent.data.fd = newClientId;

                            epoll_ctl(epollFd, EPOLL_CTL_ADD, newClientId, &newClientEvent);

                            cout << "Новый клиент подключен: FD=[" << newClientId << "]" << endl;
                            // todo: // Добавляем clientFd в свой список/мапу клиентов
                        }
                    }
                    // Событие на сокете существующего клиента -> Пришли данные или отключение
                    else {
                        // Проверяем на отключение (EPOLLRDHUP) или ошибку
                        if (eEvents[ind1].events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)) {
                            std::cout << "Клиент отключился: FD [" << eventSocketId << "]" << std::endl;
                            // Удаляем из epoll и закрываем
                            epoll_ctl(epollFd, EPOLL_CTL_DEL, eventSocketId, nullptr);
                            close(eventSocketId);
                            // TODO: Удаляем из своего списка клиентов
                            continue;
                        }
                        // Данные готовы для чтения (EPOLLIN)
                        if (eEvents[ind1].events & EPOLLIN) {
                            char buffer[1024];
                            ssize_t bytesRead = recv(eventSocketId, buffer, sizeof(buffer) - 1, 0);
                            if (bytesRead > 0) {
                                // todo: unsafe operation if (sizeof(buff)-1 == bytesRead)
                                // buffer[bytesRead] = '\0';
                                std::cout << "Получено от FD " << eventSocketId << ": " << buffer << endl;
                                // Делаем бродкаст всем остальным клиентам
                                // TODO:
                                // broadcastMessage(buffer, currentFd);
                            }
                            else if (bytesRead == 0) {
                                // Клиент закрыл соединение
                                epoll_ctl(epollFd, EPOLL_CTL_DEL, eventSocketId, nullptr);
                                close(eventSocketId);
                                // Удаляем из списка клиентов
                            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                                // Реальная ошибка чтения
                                this->throwException("NBE recv failed");
                                epoll_ctl(epollFd, EPOLL_CTL_DEL, eventSocketId, nullptr);
                                close(eventSocketId);
                            }
                        } else {
                            // TODO: output unparsed events
                            cout << "NBE UNPROCESSED error. events=[" << eEvents[ind1].events << "]" << endl;
                        }


                    }
                }
            }
// TODO:002

            // END
            close(epollFd);

            cout << "END OF epoll_create1" << endl;
// TODO: TEST EPOL
            return 0;
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
                    clientId = c1->acceptFromServer(this->bSocket.getId());
//            this->connectedClients.addClient(clientId, c1);
                } catch(Cs01Exception &ex1) {
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
//            this->connectedClients.addClient(clientId, c1);
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
// TODO:
// 1. реализовать статический класс "выброса ошибок" с инициализацией типа, как у нас
// 2. вывести отдельно в композицию methods: recv, close, accept...
// 3. вывести отдельно в композицию methods: "epoll_ctl(..., EPOLL_CTL_DEL, ...) / , ...."

#endif //NBE_CHAT_SERVERB3_H
