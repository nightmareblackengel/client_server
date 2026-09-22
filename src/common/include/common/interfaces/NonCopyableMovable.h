
#ifndef NBE_CHAT_NONCOPYABLEMOVABLE_H
#define NBE_CHAT_NONCOPYABLEMOVABLE_H

#include <iostream>

class NonCopyableMovable
{
protected:
    NonCopyableMovable() = default;
    ~NonCopyableMovable() = default;
public:
    // ЗАПРЕЩАЕМ копирование
    NonCopyableMovable(const NonCopyableMovable&) = delete;
    NonCopyableMovable& operator=(NonCopyableMovable&) = delete;
    // ЗАПРЕЩАЕМ перемещения
    NonCopyableMovable(NonCopyableMovable&&) = delete;
    NonCopyableMovable& operator= (NonCopyableMovable&&) = delete;
};

#endif //NBE_CHAT_NONCOPYABLEMOVABLE_H
