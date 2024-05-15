#pragma once

#include "communication/listener/named_pipe_listener.h"
#include "communication/writer/comm_writer_factory.h"
#include "communication/hand_side.h"

#include <functional>
#include <memory>
#include <sstream>

struct FeedbackData
{
    uint16_t thumbDistal;
    uint16_t thumbProximal;
    uint16_t indexDistal;
    uint16_t indexMiddle;
    uint16_t indexProximal;
    uint16_t middleDistal;
    uint16_t middleMiddle;
    uint16_t middleProximal;
    uint16_t ringDistal;
    uint16_t ringMiddle;
    uint16_t ringProximal;
    uint16_t pinkyDistal;
    uint16_t pinkyMiddle;
    uint16_t pinkyProximal;
    uint16_t palm;

    std::string encode() const
    {
        std::stringstream encoded;
        encoded << 'I' << thumbDistal << 'J' << thumbProximal << 'K' << indexDistal << 'L' << indexMiddle << 'M' << indexProximal
                << 'N' << middleDistal << 'O' << middleMiddle << 'P' << middleProximal << 'Q' << ringDistal << 'R' << ringMiddle
                << 'S' << ringProximal << 'T' << pinkyDistal << 'U' << pinkyMiddle << 'V' << pinkyProximal << 'W' << palm;
        return encoded.str();
    }
};

class FeedbackDataListener : public ICommListener<FeedbackData>
{
public:
    FeedbackDataListener(HandSide handSide);
    void Listen(std::function<void(const FeedbackData &)> onReceive) override;

private:
    HandSide m_HandSide;
    std::unique_ptr<NamedPipeListener<FeedbackData>> m_NamedPipeListener;

#ifdef OPENGLOVES_COMPATIBILITY
    struct FeedbackDataOpenGloves
    {
        uint16_t thumb;
        uint16_t index;
        uint16_t middle;
        uint16_t ring;
        uint16_t pinky;
    };
    std::unique_ptr<NamedPipeListener<FeedbackDataOpenGloves>> m_CompatibilityNamedPipeListener;
#endif
};