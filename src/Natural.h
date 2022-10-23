//
// Created by 0x0 on 10/13/22.
//

#ifndef BIGNUM__INTEGER_H
#define BIGNUM__INTEGER_H

#include <string>
#include <optional>

class Natural final {
private:
  using uint_t = uint8_t;

  constexpr static size_t bits_width = 8 * sizeof(uint_t);

  static std::allocator<uint_t> alloc;

public:
  Natural() : size{0}, capacity{0}, data{nullptr} {};
  explicit Natural(uint64_t u);
  static std::optional<Natural> parse(std::string_view sv);

  ~Natural();

  Natural(const Natural &natural);
  Natural(Natural &&natural) noexcept;
  Natural &operator=(Natural &&natural) noexcept;
  Natural &operator=(const Natural &natural);

  [[nodiscard]] std::string to_string(size_t base = 10) const;
  friend std::ostream &operator<<(std::ostream &os, const Natural &natural);

  void reserve(size_t cap) { reserve_data((cap + (bits_width - 1)) / bits_width); }

  [[nodiscard]] bool operator==(const Natural &rhs) const;
  [[nodiscard]] std::strong_ordering operator<=>(const Natural &rhs) const;

  [[nodiscard]] Natural operator>>(size_t offset) const { return right_move(nullptr, *this, offset); }
  [[nodiscard]] Natural operator<<(size_t offset) const { return left_move(nullptr, *this, offset); }
  Natural &operator>>=(size_t offset) { return *this = right_move(this, *this, offset); }
  Natural &operator<<=(size_t offset) { return *this = left_move(this, *this, offset); }

  [[nodiscard]] Natural operator+(const Natural &rhs) const { return add(nullptr, *this, rhs); }
  [[nodiscard]] Natural operator-(const Natural &rhs) const { return sub(nullptr, *this, rhs); }
  [[nodiscard]] Natural operator*(const Natural &rhs) const;
  [[nodiscard]] Natural operator/(const Natural &rhs) const;
  [[nodiscard]] Natural operator%(const Natural &rhs) const;

  Natural &operator+=(const Natural &rhs) { return *this = add(this, *this, rhs); }
  Natural &operator-=(const Natural &rhs) { return *this = sub(this, *this, rhs); }
  Natural &operator*=(const Natural &rhs);
  Natural &operator/=(const Natural &rhs);
  Natural &operator%=(const Natural &rhs);

  [[nodiscard]] std::pair<Natural, Natural> div(const Natural &rhs) const;

private:
  [[nodiscard]] static std::strong_ordering compare(const uint_t *lhs, const uint_t *rhs, size_t size);

  [[nodiscard]] static Natural add(Natural *dest, const Natural &lhs, const Natural &rhs);
  [[nodiscard]] static Natural sub(Natural *dest, const Natural &lhs, const Natural &rhs);

  [[nodiscard]] static Natural right_move(Natural *dest, const Natural &src, size_t offset);
  [[nodiscard]] static Natural left_move(Natural *dest, const Natural &src, size_t offset);

  void reserve_data(size_t cap);

  constexpr uint_t &back() { return data[size - 1]; };
  [[nodiscard]] constexpr uint_t back() const { return data[size - 1]; };

  constexpr uint_t &operator[](size_t index) { return data[index]; };
  [[nodiscard]] constexpr uint_t operator[](size_t index) const { return data[index]; };

  void carry_to(size_t index, uint_t n);

  uint_t *data;
  size_t size;
  size_t capacity;
};

std::ostream &operator<<(std::ostream &os, const Natural &natural);

#endif //BIGNUM__INTEGER_H
