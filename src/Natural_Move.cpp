//
// Created by 0x0 on 10/13/22.
//

#include "Natural.h"
#include "binary.h"

Natural Natural::operator>>(size_t offset) const {
  auto &lhs = *this;
  size_t bit_offset = offset % bits_width;
  size_t index_offset = offset / bits_width;
  if (index_offset >= size || (index_offset == size - 1 && (back() >> bit_offset) == 0)) {
    return {};
  }

  Natural natural;
  natural.reserve_data(size - index_offset);
  natural.size = size - index_offset;

  if (bit_offset == 0) {
    for (size_t i = 0; i != natural.size; ++i) { natural[i] = data[i + index_offset]; }
  } else {
    assert(natural.size > 0);
    size_t back = natural.size - 1;
    for (size_t i = 0; i != back; ++i) {
      natural[i] = Binary::cat(data[i + index_offset], data[i + index_offset + 1], bit_offset);
    }
    natural[back] = Binary::cat<uint_t>(data[back + index_offset], 0, bit_offset);
    if (natural.size != 0 && natural.back() == 0) {
      --natural.size;
    }
    assert(natural.size == 0 || natural.back() != 0);
  }

  return natural;
}

Natural Natural::operator<<(size_t n) const {
  size_t bit_offset = n % bits_width;
  size_t index_offset = n / bits_width;

  if (size == 0) { return {}; }

  uint_t carry = Binary::cat(back(), uint_t{0}, bits_width - bit_offset);
  Natural natural;
  natural.reserve_data(size + index_offset + (carry != 0));
  natural.size = natural.capacity;

  if (bit_offset == 0) {
    for (size_t i = 0; i != index_offset; ++i) { natural[i] = 0; }
    for (size_t i = 0; i != size; ++i) { natural[i + index_offset] = data[i]; }
  } else {
    for (size_t i = 0; i != index_offset; ++i) { natural[i] = 0; }
    natural[index_offset] = Binary::cat<uint_t>(0, data[0], bits_width - bit_offset);
    for (size_t i = 1; i != size; ++i) {
      natural[i + index_offset] = Binary::cat(data[i - 1], data[i], bits_width - bit_offset);
    }
    if (carry) { natural.back() = carry; }
  }
  return natural;
}
