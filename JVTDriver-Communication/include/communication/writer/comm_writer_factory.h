#pragma once

#include "communication/writer/comm_writer.h"
#include "communication/hand_side.h"

#include <memory>

class CommWriterFactory
{
public:
    static std::shared_ptr<ICommWriter> CreateCommWriter(HandSide handSide);

private:
    CommWriterFactory() = delete;
    CommWriterFactory(const CommWriterFactory &) = delete;
    CommWriterFactory &operator=(const CommWriterFactory &) = delete;
};