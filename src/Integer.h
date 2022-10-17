//
// Created by 0x0 on 10/13/22.
//

#ifndef BIGNUM__INTEGER_H
#define BIGNUM__INTEGER_H

#include <memory>
#include <string>

class Integer {
  constexpr static size_t bits_width = 8;
  using uint_t = uint8_t;
  static_assert(bits_width % 8 == 0 && bits_width >= 8 && bits_width <= 64);

  static std::allocator<uint_t> alloc;

  constexpr static uint_t &bits_set(uint_t &u, size_t pos) {
    u |= 1 << pos;
    return u;
  }

  constexpr static uint_t &bits_reset(uint_t &u, size_t pos) {
    u &= ~(1 << pos);
    return u;
  }

  constexpr static uint_t &bits_flip(uint_t &u, size_t pos) {
    u ^= 1 << pos;
    return u;
  }

  constexpr static bool bits_test(uint_t u, size_t pos) {
    return u & (1 << pos);
  }

  constexpr static uint_t carry_add(uint_t lhs, uint_t rhs) {
    constexpr size_t half_width = bits_width / 2;
    constexpr uint_t mask = (1 << half_width) - 1;
    return ((((lhs & mask) + (rhs & mask)) >> half_width)
      + ((lhs >> half_width) + (rhs >> half_width))) >> half_width;
  }

  constexpr static uint_t carry_mul(uint_t lhs, uint_t rhs) {
    constexpr size_t half_width = bits_width / 2;
    constexpr uint_t mask = (1 << half_width) - 1;
    return ((((lhs & mask) + (rhs & mask)) >> half_width)
      * ((lhs >> half_width) * (rhs >> half_width))) >> half_width;
  }

public:
  explicit Integer(uint64_t u = 0);
  explicit Integer(const std::string &s);

  Integer(Integer &&integer) noexcept;
  Integer(const Integer &integer);

  Integer &operator=(Integer &&integer) noexcept;
  Integer &operator=(const Integer &integer);

  ~Integer();

  [[nodiscard]] std::string to_string(size_t base = 10) const;

  void reserve(size_t cap);

  [[nodiscard]] bool operator==(const Integer &rhs) const;
  [[nodiscard]] std::strong_ordering operator<=>(const Integer &rhs) const;

  Integer &operator+=(const Integer &rhs);
  Integer operator+(const Integer &rhs) const;

  Integer &operator-=(const Integer &rhs);
  Integer operator-(const Integer &rhs) const;

private:
  uint_t *data;
  size_t size;
  size_t capacity;
};

#endif //BIGNUM__INTEGER_H
