#pragma once

#include "communication/writer/comm_writer.h"

// Windows implementation
#if defined(_WIN32) || defined(_WIN64)

#include "Windows.h"

class SerialCommWriter : public ICommWriter
{
public:
    explicit SerialCommWriter(const std::string &portName, unsigned int baudRate = 115200);
    ~SerialCommWriter() override;

    bool Connect() override;
    void Disconnect() override;
    bool IsConnected() override { return m_IsConnected; }
    void Write(const std::string &data) override;

private:
    std::string m_PortName;
    unsigned int m_BaudRate;
    bool m_IsConnected;

    HANDLE m_Handle;
};

#endif