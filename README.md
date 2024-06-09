# JVTDriver

## Overview

JVTDriver is a project that includes multiple components for communication and interaction with JVT hardware. The project is organized into different modules, each handling specific aspects of the communication and control process.

## Directory Structure

- `JVTDriver-Communication`: Contains source files and headers for managing feedback communication, listeners, writers, and logging utilities.
- `JVTDriver-OpenVR`: Contains source files and headers for integrating with OpenVR, including device drivers, hand simulations, and resource files.

## Build Instructions

### Prerequisites

Ensure you have the following tools installed:

- [CMake](https://cmake.org/download/) (version 3.22 or higher)

### Steps to Build

1. **Clone the Repository**
    ```pwsh
    git clone https://github.com/JVTTeam/JVTDriver.git
    cd JVTDriver
    ```

2. **Install Dependencies**
    Use `vcpkg` to install necessary dependencies.

    ```pwsh
    git clone https://github.com/microsoft/vcpkg.git
    .\vcpkg\bootstrap-vcpkg.bat
    .\vcpkg\vcpkg install
    ```

3. **Configure the Project with CMake**
    Run CMake to configure the project:
    ```pwsh
    cmake --preset=default
    ```

4. **Build the Project**
    Compile the project using CMake:
    ```pwsh
    cmake --build build
    ```

5. **Run the Standalone Executable**
    After a successful build, the executables will be available in the `build` directory. You can run the standalone communication module as follows:
    ```pwsh
    .\build\JVTDriver-Communication\JVTDriver-Communication-Standalone.exe
    ```

6. **Install it for SteamVR (optional)**
    If you want the driver to automatically load with SteamVR games, copy `.\build\JVTDriver-OpenVR\jvt` folder into SteamVR drivers folder.

    ```pwsh
    Copy-Item -Path .\build\JVTDriver-OpenVR\jvt -Destination "C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers" -Force -Recurse
    ```

## Communicating Feedback Data

Applications can communicate `FeedbackData` to the driver using the named pipes `\\.\pipe\jvt\input\feedback\left` and `\\.\pipe\jvt\input\feedback\right`.

### Expected Data Structure

The `FeedbackData` is expected to be a structured binary format. Below is the expected structure:

```cpp
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
};
```

The integer values should be between 0-1023, 0 representing no feedback to that zone while 1023 repsents a strong tactile feedback.

### Sending Feedback Data

To send `FeedbackData` to the driver:

1. Open a connection to the named pipe `\\.\pipe\jvt\input\feedback\<left - right>`.
2. Write the `FeedbackData` structure to the pipe.

This allows the driver to process the feedback and communicate the appropriate response to the JVT device.