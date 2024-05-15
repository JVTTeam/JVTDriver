#pragma once

#include <string>

class ICommWriter
{
public:
    virtual bool Connect() = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() = 0;

    virtual void Write(const std::string &message) = 0;
    virtual ~ICommWriter() = default;
};