cmake_minimum_required(VERSION 3.0)

project(rtpb
    DESCRIPTION
        "real-time playback simulator"
    LANGUAGES
        CXX
    )


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES main.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

#Docopt configuration
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE vendor/docopt)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DOCOPT_HEADER_ONLY)