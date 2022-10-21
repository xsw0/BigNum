cmake_minimum_required(VERSION 3.24)
project(BigNum)

set(CMAKE_CXX_STANDARD 23)

include(CTest)
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.1.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)

enable_testing()

add_subdirectory(src)
