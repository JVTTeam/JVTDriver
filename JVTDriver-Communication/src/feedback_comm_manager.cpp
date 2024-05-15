#include "communication/feedback_comm_manager.h"

#include "communication/writer/comm_writer_factory.h"

#include <string>

FeedbackCommManager::FeedbackCommManager(HandSide handSide) : m_HandSide(handSide)
{
    m_CommWriter = CommWriterFactory::CreateCommWriter(m_HandSide);
}

void FeedbackCommManager::Init()
{
    m_CommWriter->Connect();
    m_FeedbackDataListener = std::make_unique<FeedbackDataListener>(m_HandSide);
    m_FeedbackDataListener->Listen([this](const FeedbackData &data)
                                   {
        std::string encoded = data.encode();

        m_CommWriter->Write(encoded + '\n'); });
}
