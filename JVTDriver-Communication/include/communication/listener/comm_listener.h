#pragma once

#include <functional>

template <typename T>
class ICommListener
{
public:
    virtual void Listen(std::function<void(const T&)> onReceive) = 0;
    virtual ~ICommListener() = default;
};