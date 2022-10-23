//
// Created by 0x0 on 10/17/22.
//

#include <string>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include "Natural.h"

using namespace Catch::Generators;

TEST_CASE("Natural test") {
  auto num = GENERATE(
    as<uint64_t>(), 0, 1, 2, 3,
    take(20, random<uint64_t>(0, std::numeric_limits<uint32_t>::max())));

  auto str = std::to_string(num);

  Natural natural(num);

  SECTION("unary operator") {
    CAPTURE(natural);
    SECTION("test input and output") {
      CAPTURE(num);
      REQUIRE(natural.to_string() == std::to_string(num));
    }

    SECTION("parse") {
      CAPTURE(str);
      REQUIRE(natural == Natural::parse(str).value());
    }

    SECTION("move") {
      SECTION("left move 1") {
        REQUIRE(natural << 1 == (natural + natural));
      }

      SECTION("right move 1") {
        auto half = natural >> 1;
        CAPTURE(half);
        auto whole = half + half;
        CAPTURE(whole);
        REQUIRE(((whole == natural) || (whole + Natural{1} == natural)));
      }

      size_t offset = GENERATE(take(10, random(0, 100)));
      CAPTURE(offset);

      SECTION("left assign move") {
        Natural assign = natural;
        assign <<= offset;
        REQUIRE(assign == natural << offset);
      }

      SECTION("right assign move") {
        Natural assign = natural;
        assign >>= offset;
        REQUIRE(assign == natural >> offset);
      }

      SECTION("left and right") {
        CAPTURE((natural << offset));
        CAPTURE((natural << offset >> offset));
        REQUIRE(natural << offset >> offset == natural);
      }
    }
  }

  auto &lhs = natural;
  auto &lhs_num = num;
  auto &lhs_str = str;

  auto rhs_num = GENERATE_COPY(
    as<uint64_t>(), num, 0, 1, 2, 3,
    take(20, random<uint64_t>(0, std::numeric_limits<uint32_t>::max())));
  auto rhs_str = std::to_string(rhs_num);
  Natural rhs(rhs_num);

  SECTION("binary operator") {
    CAPTURE(lhs, rhs);

    auto sum = lhs + rhs;
    SECTION("add") {
      auto sum_num = lhs_num + rhs_num;
      REQUIRE(Natural(sum_num) == (lhs + rhs));
      REQUIRE(Natural(sum_num) == (rhs + lhs));
      REQUIRE(Natural(sum_num) == (Natural(lhs) += rhs));
      REQUIRE(Natural(sum_num) == (Natural(rhs) += lhs));
    }

    SECTION("sub") {
      CAPTURE(sum);
      REQUIRE(lhs == (sum - rhs));
      REQUIRE(rhs == (sum - lhs));
      REQUIRE(lhs == (Natural(sum) -= rhs));
      REQUIRE(rhs == (Natural(sum) -= lhs));
    }

    SECTION("mul") {
      auto mul_num = lhs_num * rhs_num;
      REQUIRE(Natural(mul_num) == (lhs * rhs));
      REQUIRE(Natural(mul_num) == (rhs * lhs));
      REQUIRE(Natural(mul_num) == (Natural(lhs) *= rhs));
      REQUIRE(Natural(mul_num) == (Natural(rhs) *= lhs));
    }

    SECTION("cmp") {
      SECTION("check equal") {
        REQUIRE(natural == natural);
        REQUIRE(natural <= natural);
        REQUIRE(natural >= natural);
        REQUIRE_FALSE(natural != natural);
        REQUIRE_FALSE(natural < natural);
        REQUIRE_FALSE(natural > natural);
      }

      auto check_le = [](auto &small, auto big) {
        CAPTURE(small, big);
        REQUIRE(small != big + Natural{1});
        REQUIRE_FALSE(small == big + Natural{1});
        REQUIRE(small <= big);
        REQUIRE(small < big + Natural{1});
        REQUIRE_FALSE(small > big);
        REQUIRE_FALSE(small >= big + Natural{1});

        REQUIRE(big + Natural{1} != small);
        REQUIRE_FALSE(big + Natural{1} == small);
        REQUIRE(big >= small);
        REQUIRE(big + Natural{1} > small);
        REQUIRE_FALSE(big < small);
        REQUIRE_FALSE(big + Natural{1} <= small);
      };

      check_le(lhs, sum);
      check_le(rhs, sum);
      check_le(lhs, lhs);
      check_le(rhs, rhs);
    }
  }
}
