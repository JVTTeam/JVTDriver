cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME "JVTDriver-OpenVR")
set(DRIVER "jvt")
set(DRIVER_NAME "driver_${DRIVER}")

project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_BINARY_DIR}/${DRIVER}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}>)

add_library(${DRIVER_NAME} SHARED src/hmd_driver_factory.cpp src/device_provider.cpp src/controller_device_driver.cpp src/hand_simulation.cpp src/driverlog.cpp)
target_include_directories(${DRIVER_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# OpenVR
find_package(OpenVR REQUIRED)
target_link_libraries(${DRIVER_NAME} PUBLIC OpenVR::OpenVR)

# JVT Libraries
target_link_libraries(${DRIVER_NAME} PUBLIC JVTDriver-Communication)

# Copy driver to output folder
add_custom_command(
        TARGET ${DRIVER_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/${DRIVER}
        ${CMAKE_CURRENT_BINARY_DIR}/${DRIVER}
)