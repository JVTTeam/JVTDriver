#pragma once

#include "communication/listener/feedback_data_listener.h"
#include "communication/writer/comm_writer_factory.h"

#include <memory>

class FeedbackCommManager
{
public:
    FeedbackCommManager(HandSide handSide);
    void Init();

private:
    std::shared_ptr<ICommWriter> m_CommWriter;
    std::unique_ptr<FeedbackDataListener> m_FeedbackDataListener;
    HandSide m_HandSide;
};