cmake_minimum_required(VERSION 3.22)
project(VR_MaterialScience LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/EHsc)
endif()

# ---- AIChatModel Library ----
add_library(AIChatModelLib
    AIChatModel/AIChatModel.cpp
)

target_include_directories(AIChatModelLib PUBLIC AIChatModel)

# ---- NLP Executable ----
add_executable(nlp_app
    AIChatModel/NLPModule.cpp
)

target_include_directories(nlp_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    AIChatModel
)

target_link_libraries(nlp_app PRIVATE
    AIChatModelLib
)

# ---- Vosk Integration ----
set(VOSK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vosk")
set(VOSK_LIBRARY     "${CMAKE_CURRENT_SOURCE_DIR}/vosk/libvosk.lib")
set(VOSK_DLL         "${CMAKE_CURRENT_SOURCE_DIR}/vosk/libvosk.dll")

target_include_directories(nlp_app PRIVATE ${VOSK_INCLUDE_DIR})
target_link_libraries(nlp_app PRIVATE ${VOSK_LIBRARY})

# Windows SDK libs — needed for SAPI + WinMM
target_link_libraries(nlp_app PRIVATE
    sapi
    ole32
    winmm
)

# Copy vosk.dll next to .exe
add_custom_command(TARGET nlp_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VOSK_DLL}"
        $<TARGET_FILE_DIR:nlp_app>
)

# ---- GoogleTest for AIChatModel ----
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    test_AIChatModel
    tests/test_AIChatModel.cpp
)

target_link_libraries(
    test_AIChatModel
    PRIVATE AIChatModelLib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_AIChatModel)
