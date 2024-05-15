#include "communication/listener/feedback_data_listener.h"

#include <memory>
#include <string>

FeedbackDataListener::FeedbackDataListener(HandSide handSide) : m_HandSide(handSide)
{
}

void FeedbackDataListener::Listen(std::function<void(const FeedbackData &)> onReceive)
{
    std::string pipeName = std::string(R"(\\.\pipe\jvt\input\feedback\)") + (m_HandSide == HandSide::Left ? "left" : "right");
    m_NamedPipeListener = std::make_unique<NamedPipeListener<FeedbackData>>(pipeName);

    m_NamedPipeListener->Listen(onReceive);

#ifdef OPENGLOVES_COMPATIBILITY
    std::string ogPipeName = std::string(R"(\\.\pipe\vrapplication\ffb\curl\)") + (m_HandSide == HandSide::Left ? "left" : "right");
    m_CompatibilityNamedPipeListener = std::make_unique<NamedPipeListener<FeedbackDataOpenGloves>>(ogPipeName);

    m_CompatibilityNamedPipeListener->Listen([onReceive](const FeedbackDataOpenGloves &data)
                                             {
        FeedbackData feedbackData {
            data.thumb,
            data.thumb,
            data.index,
            data.index,
            data.index,
            data.middle,
            data.middle,
            data.middle,
            data.ring,
            data.ring,
            data.ring,
            data.pinky,
            data.pinky,
            data.pinky
        };

        onReceive(feedbackData); });
#endif
}
