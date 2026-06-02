#ifndef CHAT_SERVER_BOOTSTRAP_H
#define CHAT_SERVER_BOOTSTRAP_H

#include <sys/socket.h>
#include <netinet/in.h>

const int SERVER_PORT       = 8899;
const char* SERVER_HOST     = "127.0.0.1";
const int SERVER_IP_TYPE    = AF_INET;

sockaddr_in getConfiguredAddress() {
    sockaddr_in address{};
    address.sin_family = SERVER_IP_TYPE;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    return address;
}

const int DEFAULT_INVALID_DESCRIPTOR    = -1;
const int ERROR_CANT_CREATE_SOCKET      = -2;
const int ERROR_CANT_BIND_SOCKET        = -3;
const int ERROR_CANT_START_LISTEN       = -4;
const int ERROR_CLIENT_CANT_ACCEPT      = -5;
const int ERROR_WHEN_CLIENT_READ        = -6;
const int ERROR_CLIENT_DISCONNECT       = -7;

#endif //CHAT_SERVER_BOOTSTRAP_H
