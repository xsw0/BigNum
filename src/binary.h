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
  assert(pos < bits_width);
  u |= U{1} << pos;
  return u;
}

template<UnsignedIntegral U>
constexpr inline U &bits_reset(U &u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos < bits_width);
  u &= ~U(U{1} << pos);
  return u;
}

template<UnsignedIntegral U>
constexpr inline U &bits_flip(U &u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos < bits_width);
  u ^= U{1} << pos;
  return u;
}

template<UnsignedIntegral U>
constexpr inline bool bits_test(U u, size_t pos) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(pos < bits_width);
  return (u & U(U{1} << pos)) != 0;
}

template<UnsignedIntegral U>
constexpr inline U bits(size_t size) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size <= bits_width);
  if (size == bits_width) { return ~0; }
  return (U(1) << size) - 1;
}

template<UnsignedIntegral U>
constexpr inline U high_bits(U u, size_t size) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size > 0);
  assert(size <= bits_width);
  return u >> (bits_width - size);
}

template<UnsignedIntegral U>
constexpr inline U low_bits(U u, size_t size) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size > 0);
  assert(size <= bits_width);
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
  assert(offset > 0);
  assert(offset < bits_width);
  return (high << (bits_width - offset)) | (low >> offset);
}

template<UnsignedIntegral U>
constexpr inline void move_forward(U dest[], const U src[], size_t size) {
  for (size_t i = 0; i != size; ++i) { dest[i] = src[i]; }
}

template<UnsignedIntegral U>
constexpr inline void move_backward(U dest[], const U src[], size_t size) {
  for (size_t i = size; i != 0; --i) { dest[i - 1] = src[i - 1]; }
}

template<UnsignedIntegral U>
constexpr inline U bits_move_forward(U *dest, const U *src, size_t size, size_t offset) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size > 0);
  size_t bound = size - 1;
  U result = Binary::cat(U{0}, src[0], offset);
  for (size_t i = 0; i != bound; ++i) {
    dest[i] = Binary::cat(src[i], src[i + 1], offset);
  }
  dest[bound] = Binary::cat(src[bound], U{0}, offset);
  return result;
}

template<UnsignedIntegral U>
constexpr inline U bits_move_backward(U *dest, const U *src, size_t size, size_t offset) {
  constexpr size_t bits_width = sizeof(U) * 8;
  assert(size > 0);
  size_t bound = size - 1;
  U result = Binary::cat(src[bound], U{0}, bits_width - offset);
  for (size_t i = bound; i != 0; --i) {
    dest[i] = Binary::cat(src[i - 1], src[i], bits_width - offset);
  }
  dest[0] = Binary::cat(U{0}, src[0], bits_width - offset);
  return result;
}

template<UnsignedIntegral U>
constexpr inline U half_add(U *dest, const U *src, size_t size, U carry = 1) {
  for (size_t i = 0; i != size; ++i) {
    U sum = src[i] + carry;
    carry = Binary::carry_add(carry, src[i]);
    dest[i] = sum;
  }
  return carry;
}

template<UnsignedIntegral U>
constexpr inline U add(U dest[], const U lhs[], const U rhs[], size_t size) {
  U carry = 0;
  for (size_t i = 0; i != size; ++i) {
    U sum = lhs[i] + rhs[i] + carry;
    carry = Binary::carry_add(lhs[i], rhs[i]) + Binary::carry_add<U>(lhs[i] + rhs[i], carry);
    dest[i] = sum;
  }
  return carry;
}

template<UnsignedIntegral U>
constexpr inline U sub(U dest[], const U lhs[], size_t lhs_size, const U rhs[], size_t rhs_size) {
  assert(lhs_size >= rhs_size);

  U borrow = 0;
  size_t i = 0;
  for (; i < rhs_size; ++i) {
    if (borrow) {
      borrow = lhs[i] <= rhs[i];
      dest[i] = lhs[i] - rhs[i] - 1;
    } else {
      borrow = lhs[i] < rhs[i];
      dest[i] = lhs[i] - rhs[i];
    }
  }
  while (i != lhs_size && borrow) {
    borrow = lhs[i] == 0;
    dest[i] = lhs[i] - 1;
    ++i;
  }
  if (dest != lhs) {
    for (; i != lhs_size; ++i) {
      dest[i] = lhs[i];
    }
  }
  return borrow;
}

template<UnsignedIntegral U>
constexpr inline void mul(U dest[], const U lhs[], size_t lhs_size, const U rhs[], size_t rhs_size) {
  assert(!(dest >= lhs && dest < lhs + lhs_size));
  assert(!(dest >= rhs && dest < rhs + rhs_size));
  for (size_t i = 0, bound = lhs_size + rhs_size; i != bound; ++i) { dest[i] = 0; }
  for (size_t l = 0; l != lhs_size; ++l) {
    for (size_t r = 0; r != rhs_size; ++r) {
      size_t index = l + r;
      U prod_carry[2]{U(lhs[l] * rhs[r]), Binary::carry_mul(lhs[l], rhs[r])};
      U carry = add(dest + index, dest + index, prod_carry, 2);
      index += 2;
      while (carry) {
        U sum = dest[index] + carry;
        carry = Binary::carry_add(carry, dest[index]);
        dest[index] = sum;
        ++index;
      }
    }
  }
}

}

#endif //BIGNUM_SRC_BINARY_H
