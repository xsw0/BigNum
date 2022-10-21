add_library(Natural Natural.cpp Natural_Move.cpp Natural_Arithmetic.cpp)
add_executable(BigNum main.cpp)
target_link_libraries(BigNum Natural)

add_executable(Test BinaryTest.cpp NaturalTest.cpp)
target_link_libraries(Test PRIVATE Catch2::Catch2WithMain Natural)
catch_discover_tests(Test)
