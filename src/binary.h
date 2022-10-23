//
// Created by 0x0 on 10/20/22.
//

#ifndef BIGNUM_SRC_BINARY_H
#define BIGNUM_SRC_BINARY_H

namespace Binary {

template<class T>
concept Integral = std::is_integral<T>::value;
template<class T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
template<class T>
concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;

template<UnsignedIntegral U>
constexpr inline U &bits_set(U &u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos <= bits_width);
  u |= U{1} << pos;
  return u;
}

template<UnsignedIntegral U>
constexpr inline U &bits_reset(U &u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos <= bits_width);
  u &= ~U(U{1} << pos);
  return u;
}

template<UnsignedIntegral U>
constexpr inline U &bits_flip(U &u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos <= bits_width);
  u ^= U{1} << pos;
  return u;
}

template<UnsignedIntegral U>
constexpr inline bool bits_test(U u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos <= bits_width);
  return (u & U(U{1} << pos)) != 0;
}

template<UnsignedIntegral U>
constexpr inline U bits(size_t size) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size <= bits_width);
  return U(~0) >> (bits_width - size);
}

template<UnsignedIntegral U>
constexpr inline U high_bits(U u, size_t size) {
  constexpr size_t bits_width = sizeof(U) * 8;
  return u >> (bits_width - size);
}

template<UnsignedIntegral U>
constexpr inline U low_bits(U u, size_t size) {
  constexpr size_t bits_width = sizeof(U) * 8;
  return u & bits<U>(size);
}

template<UnsignedIntegral U>
constexpr inline U low_half(U n) {
  constexpr size_t bits_width = sizeof(U) * 8;
  constexpr size_t half_width = bits_width / 2;
  return n & bits<U>(half_width);
}

template<UnsignedIntegral U>
constexpr inline U high_half(U n) {
  constexpr size_t bits_width = sizeof(U) * 8;
  constexpr size_t half_width = bits_width / 2;
  return n >> half_width;
}

template<UnsignedIntegral U>
constexpr inline U carry_add(U lhs, U rhs) {
  U lhs_low = low_half(lhs);
  U rhs_low = low_half(rhs);
  U lhs_high = high_half(lhs);
  U rhs_high = high_half(rhs);
  U carry = high_half<U>(lhs_low + rhs_low) + (lhs_high + rhs_high);
  return high_half(carry);
}

template<>
constexpr inline uint8_t carry_add<uint8_t>(uint8_t lhs, uint8_t rhs) {
  return (uint32_t(lhs) + uint32_t(rhs)) >> 8;
}

template<>
constexpr inline uint16_t carry_add<uint16_t>(uint16_t lhs, uint16_t rhs) {
  return (uint32_t(lhs) + uint32_t(rhs)) >> 16;
}

template<>
constexpr inline uint32_t carry_add<uint32_t>(uint32_t lhs, uint32_t rhs) {
  return (uint64_t(lhs) + uint64_t(rhs)) >> 32;
}

template<UnsignedIntegral U>
constexpr inline U carry_mul(U lhs, U rhs) {
  constexpr size_t bits_width = sizeof(U) * 8;
  constexpr size_t half_width = bits_width / 2;
  U lhs_low = low_half(lhs);
  U rhs_low = low_half(rhs);
  U lhs_high = high_half(lhs);
  U rhs_high = high_half(rhs);
  U low_prod = high_half<U>(lhs_low * rhs_low);
  U mid_prod1 = lhs_low * rhs_high;
  U mid_prod2 = lhs_high * rhs_low;
  return U(carry_add<U>(low_prod + mid_prod1, mid_prod2) << half_width)
    + high_half<U>(low_prod + mid_prod1 + mid_prod2)
    + U(lhs_high * rhs_high);
}

template<>
constexpr inline uint8_t carry_mul<uint8_t>(uint8_t lhs, uint8_t rhs) {
  return (uint32_t(lhs) * uint32_t(rhs)) >> 8;
}

template<>
constexpr inline uint16_t carry_mul<uint16_t>(uint16_t lhs, uint16_t rhs) {
  return (uint32_t(lhs) * uint32_t(rhs)) >> 16;
}

template<>
constexpr inline uint32_t carry_mul<uint32_t>(uint32_t lhs, uint32_t rhs) {
  return (uint64_t(lhs) * uint64_t(rhs)) >> 32;
}

// concatenate the least significant (bits_width - @offset) bits of @high
// with the most significant @offset bits of @low
// return @low when @offset == 0
// return @high when @offset == bits_width
template<UnsignedIntegral U>
constexpr inline U cat(U low, U high, size_t offset) {
  constexpr size_t bits_width = sizeof(U) * 8;
  return (high << (bits_width - offset)) | (low >> offset);
}

template<UnsignedIntegral U>
constexpr inline void move_forward(U *dest, const U *src, size_t size) {
  for (size_t i = 0; i != size; ++i) { dest[i] = src[i]; }
}

template<UnsignedIntegral U>
constexpr inline void move_backward(U *dest, const U *src, size_t size) {
  for (size_t i = size; i != 0; --i) { dest[i - 1] = src[i - 1]; }
}

template<UnsignedIntegral U>
constexpr inline U bits_right_move(U *dest, const U *src, size_t size, size_t offset) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size != 0);
  size_t bound = size - 1;
  U result = Binary::cat(U{0}, src[0], offset);
  for (size_t i = 0; i != bound; ++i) {
    dest[i] = Binary::cat(src[i], src[i + 1], offset);
  }
  dest[bound] = Binary::cat(src[bound], U{0}, offset);
  return result;
}

template<UnsignedIntegral U>
constexpr inline U bits_left_move(U *dest, const U *src, size_t size, size_t offset) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size != 0);
  size_t bound = size - 1;
  U result = Binary::cat(src[bound], U{0}, bits_width - offset);
  for (size_t i = bound; i != 0; --i) {
    dest[i] = Binary::cat(src[i - 1], src[i], bits_width - offset);
  }
  dest[0] = Binary::cat(U{0}, src[0], bits_width - offset);
  return result;
}

}

#endif //BIGNUM_SRC_BINARY_H
