#include "communication/feedback_comm_manager.h"

#include "communication/util/logger.h"

#include <iostream>

int main()
{
    Logger::GetInstance().Subscribe([](const std::string &message, Logger::Level level)
                                    {
                                        switch (level)
                                        {
                                        case Logger::Level::Info:
                                            std::cout << "[INFO] " << message << '\n';
                                            break;
                                        case Logger::Level::Warning:
                                            std::cout << "[WARNING] " << message << '\n';
                                            break;
                                        case Logger::Level::Error:
                                            std::cerr << "[ERROR] " << message << '\n';
                                            break;
                                        } });

    FeedbackCommManager feedbackCommManagerLeft(HandSide::Left);
    feedbackCommManagerLeft.Init();

    FeedbackCommManager feedbackCommManagerRight(HandSide::Right);
    feedbackCommManagerRight.Init();

    while (true)
        std::cin.get();
    return 0;
}