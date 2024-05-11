cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

project(JVTDriver)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT PLATFORM)
    if(CMAKE_SIZEOF_VOID_P MATCHES 8)
        set(PROCESSOR_ARCH 64)
    else()
        set(PROCESSOR_ARCH 32)
    endif()
endif()
message(STATUS "Compilation will target ${PROCESSOR_ARCH}-bit architecture.")

if (WIN32)
    set(PLATFORM_NAME "win")
elseif (UNIX AND NOT APPLE)
    set(PLATFORM_NAME "linux")
elseif (APPLE)
    set(PLATFORM_NAME "osx")
endif ()

add_subdirectory(JVTDriver-OpenVR)
