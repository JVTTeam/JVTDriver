#pragma once

#include "communication/listener/comm_listener.h"
#include "communication/util/logger.h"

static Logger &logger = Logger::GetInstance();

// Windows Implementation
#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>

#include <atomic>
#include <functional>
#include <stdexcept>
#include <string>
#include <thread>

template <typename T>
class NamedPipeListener : public ICommListener<T>
{
public:
    explicit NamedPipeListener(const std::string &pipeName);
    ~NamedPipeListener() override;
    void Listen(std::function<void(const T &)> onReceive) override;

private:
    enum class State
    {
        Pending,
        Reading,
        Read
    };

    struct PipeInstance
    {
        OVERLAPPED overlapped;
        HANDLE handle;
        DWORD nBytesRead;
        bool pendingIO;
        State state;
        char request[sizeof(T)];
    };

    void ListenThread(std::function<void(const T &)> onReceive);
    bool Connect(PipeInstance &pipeInstance);
    void DisconnectAndReconnect(PipeInstance &pipeInstance);

private:
    std::string m_PipeName;

    std::atomic<bool> m_ThreadActive;
    std::thread m_Thread;
};

template <typename T>
NamedPipeListener<T>::NamedPipeListener(const std::string &pipeName)
    : m_PipeName(pipeName)
{
}

template <typename T>
NamedPipeListener<T>::~NamedPipeListener()
{
    if (m_ThreadActive.exchange(false) && m_Thread.joinable())
        m_Thread.join();
}

template <typename T>
void NamedPipeListener<T>::Listen(std::function<void(const T &)> onReceive)
{
    if (m_ThreadActive.exchange(true))
    {
        throw std::runtime_error("Listener is already active");
        return;
    }

    m_Thread = std::thread(&NamedPipeListener<T>::ListenThread, this, onReceive);
}

template <typename T>
void NamedPipeListener<T>::ListenThread(std::function<void(const T &)> onReceive)
{
    HANDLE eventHandle = CreateEventA(nullptr, TRUE, TRUE, nullptr);
    if (eventHandle == nullptr)
    {
        logger.Log(Logger::Level::Error, "Failed to create event handle, error code %d", GetLastError());
        return;
    }

    HANDLE pipeHandle = CreateNamedPipeA(
        m_PipeName.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        static_cast<DWORD>(sizeof(T)),
        static_cast<DWORD>(sizeof(T)),
        0,
        nullptr);

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        logger.Log(Logger::Level::Error, "Failed to create named pipe, error code %d", GetLastError());
        return;
    }

    PipeInstance pipeInstance;
    pipeInstance.overlapped.hEvent = eventHandle;
    pipeInstance.handle = pipeHandle;

    if (!Connect(pipeInstance))
    {
        m_ThreadActive.store(false);
        return;
    }

    while (m_ThreadActive)
    {
        DWORD waitStatus = WaitForSingleObject(pipeInstance.overlapped.hEvent, 5);
        switch (waitStatus)
        {
        case WAIT_TIMEOUT:
            continue;
        case WAIT_OBJECT_0:
            break;
        default:
            logger.Log(Logger::Level::Error, "Failed to wait for event, error code %d", GetLastError());
            DisconnectAndReconnect(pipeInstance);
            continue;
        }

        if (pipeInstance.pendingIO)
        {
            DWORD nBytesTransferred = 0;
            BOOL success = GetOverlappedResult(pipeInstance.handle, &pipeInstance.overlapped, &nBytesTransferred, FALSE);
            if (pipeInstance.state == State::Reading)
            {
                if (!success || nBytesTransferred == 0)
                {
                    logger.Log(Logger::Level::Error, "Failed to read data from pipe or read empty data");
                    DisconnectAndReconnect(pipeInstance);
                    continue;
                }
                pipeInstance.pendingIO = false;
                pipeInstance.state = State::Read;
                pipeInstance.nBytesRead = nBytesTransferred;
            }
            else
            { // Pending or Read
                if (!success)
                {
                    logger.Log(Logger::Level::Error, "Failed to complete pending IO, error code %d", GetLastError());
                    DisconnectAndReconnect(pipeInstance);
                    continue;
                }

                pipeInstance.state = State::Reading;
            }
        }

        if (pipeInstance.state == State::Reading)
        {
            BOOL success = ReadFile(pipeInstance.handle, pipeInstance.request, sizeof(T), &pipeInstance.nBytesRead, &pipeInstance.overlapped);
            if (success)
            {
                if (pipeInstance.nBytesRead > 0)
                {
                    pipeInstance.pendingIO = false;
                    pipeInstance.state = State::Read;
                }
                else
                {
                    logger.Log(Logger::Level::Error, "Read empty data from pipe");
                    DisconnectAndReconnect(pipeInstance);
                }
            }
            else
            {
                if (GetLastError() == ERROR_IO_PENDING)
                    pipeInstance.pendingIO = true;
                else
                {
                    logger.Log(Logger::Level::Error, "Failed to read data from pipe, error code %d", GetLastError());
                    DisconnectAndReconnect(pipeInstance);
                }
            }
        }
        else if (pipeInstance.state == State::Read)
        {
            if (pipeInstance.nBytesRead == sizeof(T))
            {
                pipeInstance.state = State::Reading;
                onReceive(*reinterpret_cast<T *>(pipeInstance.request));
            }
            else
            {
                logger.Log(Logger::Level::Error, "Received data with invalid size %d, expected %d", pipeInstance.nBytesRead, sizeof(T));
                DisconnectAndReconnect(pipeInstance);
            }
        }
    }

    DisconnectNamedPipe(pipeInstance.handle);
    CloseHandle(pipeInstance.handle);
    CloseHandle(pipeInstance.handle);
}

template <typename T>
bool NamedPipeListener<T>::Connect(typename NamedPipeListener<T>::PipeInstance &pipeInstance)
{
    if (!ConnectNamedPipe(pipeInstance.handle, &pipeInstance.overlapped))
    {
        DWORD error = GetLastError();
        switch (error)
        {
        case ERROR_IO_PENDING:
            pipeInstance.pendingIO = true;
            pipeInstance.state = State::Pending;
            return true;
        case ERROR_PIPE_CONNECTED:
            if (SetEvent(pipeInstance.overlapped.hEvent))
            {
                pipeInstance.pendingIO = false;
                pipeInstance.state = State::Reading;
                return true;
            }
        }
    }

    logger.Log(Logger::Level::Error, "Failed to connect named pipe, error code %d", GetLastError());
    return false;
}

template <typename T>
void NamedPipeListener<T>::DisconnectAndReconnect(typename NamedPipeListener<T>::PipeInstance &pipeInstance)
{
    if (!DisconnectNamedPipe(pipeInstance.handle))
    {
        logger.Log(Logger::Level::Error, "Failed to disconnect named pipe, error code %d", GetLastError());
    }

    if (!Connect(pipeInstance))
    {
        logger.Log(Logger::Level::Error, "Failed to reconnect named pipe, error code %d", GetLastError());
    }
}

#endif