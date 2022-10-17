add_library(Integer Integer.cpp)
add_executable(BigNum main.cpp)
target_link_libraries(BigNum Integer)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.1.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(BigNumTest test/IntegerTest.cpp)
target_link_libraries(BigNumTest PRIVATE Catch2::Catch2WithMain Integer)
add_test(BigNumTest BigNumTest)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(BigNumTest)
