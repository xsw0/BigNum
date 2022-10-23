//
// Created by 0x0 on 10/13/22.
//

#include "Natural.h"
#include "binary.h"

Natural Natural::right_move(Natural *dest, const Natural &src, size_t offset) {
  size_t bit_offset = offset % bits_width;
  size_t index_offset = offset / bits_width;
  if (index_offset >= src.size) { return {}; }
  size_t target_size = src.size - index_offset - (src.back() >> bit_offset == 0);

  if (target_size == 0) { return {}; }

  Natural natural;
  if (!dest || dest->capacity < target_size) { dest = &natural; }
  natural.reserve_data(target_size);

  if (bit_offset == 0) {
    Binary::move_forward(dest->data, src.data + index_offset, src.size - index_offset);
  } else {
    Binary::bits_right_move(dest->data, src.data + index_offset, src.size - index_offset, bit_offset);
  }

  dest->size = target_size;
  return *dest;
}

Natural Natural::left_move(Natural *dest, const Natural &src, size_t offset) {
  size_t bit_offset = offset % bits_width;
  size_t index_offset = offset / bits_width;

  if (src.size == 0) { return {}; }

  uint_t carry = Binary::cat(src.back(), uint_t{0}, bits_width - bit_offset);
  size_t target_size = src.size + index_offset + (carry != 0);

  Natural natural;
  if (!dest || dest->capacity < target_size) { dest = &natural; }
  natural.reserve_data(target_size);

  if (bit_offset == 0) {
    Binary::move_backward(dest->data + index_offset, src.data, src.size);
  } else {
    uint_t res = Binary::bits_left_move(dest->data + index_offset, src.data, src.size, bit_offset);
    assert(carry == res);
  }

  for (size_t i = 0; i != index_offset; ++i) { dest->data[i] = 0; }

  dest->size = target_size;
  if (carry) { dest->back() = carry; }

  return *dest;
}
