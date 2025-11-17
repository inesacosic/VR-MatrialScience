cmake_minimum_required(VERSION 3.14)
project(VR-MATERIALSCIENCE-1)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your library (implementation)
add_library(AIChatModelLib AIChatModel/AIChatModel.cpp)

# Make the header visible to anything linking this library
target_include_directories(AIChatModelLib PUBLIC AIChatModel)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# to enable test
enable_testing()

# defining executable target and its source file
add_executable(
  test_AIChatModel
  tests/test_AIChatModel.cpp
)
# to link the executable with GoogleTest main library
target_link_libraries(
  test_AIChatModel
  PRIVATE AIChatModelLib
  GTest::gtest_main
)

# to include GoogleTest module
include(GoogleTest)

# to discover and register all tests in the executable
gtest_discover_tests(test_AIChatModel)