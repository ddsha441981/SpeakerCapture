cmake_minimum_required(VERSION 3.10)
project(SpeakerCapture)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(SimplePA)

include_directories(${CMAKE_SOURCE_DIR}/SimplePA)

add_executable(SpeakerCapture main.cpp)

target_link_libraries(SpeakerCapture SimplePA pulse pulse-simple)
