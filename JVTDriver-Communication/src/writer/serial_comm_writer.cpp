#include "communication/writer/serial_comm_writer.h"

#include "communication/util/logger.h"

#include <iostream>

static Logger &logger = Logger::GetInstance();

// Windows implementation
#if defined(_WIN32) || defined(_WIN64)

SerialCommWriter::SerialCommWriter(const std::string &portName, unsigned int baudRate) : m_PortName(portName), m_BaudRate(baudRate), m_IsConnected(false)
{
}

SerialCommWriter::~SerialCommWriter()
{
    if (m_IsConnected)
        Disconnect();
}

bool SerialCommWriter::Connect()
{
    if (m_IsConnected)
    {
        logger.Log(Logger::Level::Warning, "Serial port %s is already connected", m_PortName.c_str());
        return true;
    }

    m_Handle = CreateFile(m_PortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (m_Handle == INVALID_HANDLE_VALUE)
    {
        logger.Log(Logger::Level::Error, "Failed to open serial port %s, error code %d", m_PortName.c_str(), GetLastError());
        return false;
    }

    DCB dcb;
    if (!GetCommState(m_Handle, &dcb))
    {
        logger.Log(Logger::Level::Error, "Failed to get serial port %s state, error code %d", m_PortName.c_str(), GetLastError());
        CloseHandle(m_Handle);
        return false;
    }

    dcb.BaudRate = m_BaudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(m_Handle, &dcb))
    {
        logger.Log(Logger::Level::Error, "Failed to set serial port %s state, error code %d", m_PortName.c_str(), GetLastError());
        CloseHandle(m_Handle);
        return false;
    }

    if (!SetupComm(m_Handle, 150, 150))
    {
        logger.Log(Logger::Level::Error, "Failed to setup serial port %s, error code %d", m_PortName.c_str(), GetLastError());
        CloseHandle(m_Handle);
        return false;
    }

    m_IsConnected = true;
    return true;
}

void SerialCommWriter::Disconnect()
{
    if (!m_IsConnected)
    {
        logger.Log(Logger::Level::Warning, "Serial port %s is not connected", m_PortName.c_str());
        return;
    }

    CancelIo(m_Handle);
    if (!CloseHandle(m_Handle))
    {
        logger.Log(Logger::Level::Error, "Failed to close serial port %s, error code %d", m_PortName.c_str(), GetLastError());
        return;
    }

    m_IsConnected = false;
}

void SerialCommWriter::Write(const std::string &data)
{
    if (!m_IsConnected)
    {
        logger.Log(Logger::Level::Error, "Serial port %s is not connected", m_PortName.c_str());
        return;
    }

    logger.Log(Logger::Level::Info, "Serial write: %s", data.c_str());

    DWORD bytesWritten;
    if (!WriteFile(m_Handle, data.c_str(), data.size(), &bytesWritten, nullptr))
    {
        logger.Log(Logger::Level::Error, "Failed to write to serial port %s, error code %d", m_PortName.c_str(), GetLastError());
        return;
    }
}

#endif