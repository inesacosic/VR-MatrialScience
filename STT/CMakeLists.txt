cmake_minimum_required(VERSION 3.16)
project(STT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Whisper include directory
include_directories(${CMAKE_SOURCE_DIR}/external/include)

# Whisper source files
set(WHISPER_SRCS
    ${CMAKE_SOURCE_DIR}/external/src/whisper.cpp
)

# Main executable
add_executable(stt
    src/main.cpp
    src/audio_utils.cpp
    ${WHISPER_SRCS}
)

# --- SndFile works normally ---
find_package(SndFile CONFIG REQUIRED)

# --- mpg123 (no CMake config on Windows - manual link) ---
set(MPG123_INCLUDE_DIR
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)

set(MPG123_LIBRARY
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/mpg123.lib)

target_include_directories(stt PRIVATE
    ${MPG123_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/include
)

target_link_libraries(stt PRIVATE
    SndFile::sndfile
    ${MPG123_LIBRARY}
)
