#ifndef CHAT_SERVER_BOOTSTRAP_H
#define CHAT_SERVER_BOOTSTRAP_H

#include <sys/socket.h>
using std::string;

constexpr const int DEFAULT_INVALID_DESCRIPTOR      = -1;
constexpr const int SERVER_IP_TYPE                  = AF_INET;

namespace IoTextColor
{
    const string BLACK   = "\033[30m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string WHITE   = "\033[37m";
    const string GRAY    = "\033[90m";
    const string DEFAULT = IoTextColor::GRAY;

    const string TEXT_ATTR_RESET     = "\033[0m";
    const string TEXT_ATTR_BOLD      = "\033[1m";
    const string TEXT_ATTR_UNDERLINE = "\033[4m";
};

#endif //CHAT_SERVER_BOOTSTRAP_H
