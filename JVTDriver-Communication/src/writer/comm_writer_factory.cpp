#include "communication/writer/comm_writer_factory.h"

#include "communication/writer/serial_comm_writer.h"

#include <memory>

std::shared_ptr<ICommWriter> CommWriterFactory::CreateCommWriter(HandSide handSide)
{
    // TODO: Read configuration data to decide type and parameters of the communication service
    std::shared_ptr<ICommWriter> commWriter;
    if (handSide == HandSide::Left)
    {
        commWriter = std::make_shared<SerialCommWriter>(R"(\\.\COM13)");
    }
    else
    {
        commWriter = std::make_shared<SerialCommWriter>(R"(\\.\COM1)");
    }
    return commWriter;
}