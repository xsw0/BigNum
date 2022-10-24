//
// Created by 0x0 on 10/17/22.
//

#include <vector>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include "binary.h"

TEST_CASE("Binary test") {
  SECTION("bits") {
    uint64_t n = GENERATE_REF(range(0, 8));
    Binary::UnsignedIntegral auto res = Binary::bits<uint8_t>(n);
    uint64_t x = (1 << n) - 1;
    CAPTURE(n, res, x);
    REQUIRE(res == x);
  }

  SECTION("carry_add") {
    uint64_t a = GENERATE_REF(take(50, random(0, 255)));
    uint64_t b = GENERATE_REF(take(50, random(0, 255)));
    auto carry = (a + b) >> 8;
    CAPTURE(a, b, carry);
    REQUIRE(carry == Binary::carry_add<uint8_t>(a, b));
  }

  SECTION("carry_mul") {
    uint64_t a = GENERATE_REF(take(50, random(128, 255)));
    uint64_t b = GENERATE_REF(take(50, random(128, 255)));
    auto carry = (a * b) >> 8;
    CAPTURE(a, b, int(carry));
    REQUIRE(carry == uint64_t(Binary::carry_mul<uint8_t>(a, b)));
  }
}
