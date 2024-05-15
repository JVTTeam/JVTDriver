cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME "JVTDriver-Communication")

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC src/writer/comm_writer_factory.cpp src/writer/serial_comm_writer.cpp src/listener/feedback_data_listener.cpp src/feedback_comm_manager.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME}-Standalone src/standalone.cpp src/writer/comm_writer_factory.cpp src/writer/serial_comm_writer.cpp src/listener/feedback_data_listener.cpp src/feedback_comm_manager.cpp)
target_include_directories(${PROJECT_NAME}-Standalone PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(OPENGLOVES_COMPATIBILITY "Enable OpenGloves compatibilty" ON)
if(OPENGLOVES_COMPATIBILITY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE OPENGLOVES_COMPATIBILITY)
    target_compile_definitions(${PROJECT_NAME}-Standalone PRIVATE OPENGLOVES_COMPATIBILITY)
endif ()