//
// Created by 0x0 on 10/13/22.
//

#include "Natural.h"
#include "binary.h"

// allow this == &natural
Natural &Natural::assign_right_move(const Natural &natural, size_t n) {
  // Note that modifying the properties of *this may affect @natural;
  size_t offset = n % bits_width;
  n /= bits_width;
  if (n >= natural.size || (n == natural.size - 1 && (natural[natural.size - 1] >> offset) == 0)) {
    size = 0;
    return *this;
  }

  reserve_data(natural.size - n);
  size = natural.size - n;

  if (offset == 0) {
    for (size_t i = 0; i != size; ++i) { data[i] = natural[i + n]; }
  } else {
    assert(size > 0);
    size_t back = size - 1;
    for (size_t i = 0; i != back; ++i) {
      data[i] = Binary::cat(natural[i + n], natural[i + n + 1], offset);
    }
    data[back] = Binary::cat<uint_t>(natural[back + n], 0, offset);
    if (size != 0 && data[size - 1] == 0) {
      --size;
    }
    assert(size == 0 || data[size - 1] != 0);
  }
  return *this;
}

Natural Natural::operator>>(size_t n) const {
  Natural natural;
  natural.assign_right_move(*this, n);
  return natural;
}

Natural &Natural::operator>>=(size_t n) {
  return assign_right_move(*this, n);
}

Natural Natural::operator<<(size_t n) const {
  size_t offset = n % bits_width;
  n /= bits_width;

  if (size == 0) { return {}; }

  Natural natural;
  natural.reserve_data(size + n + (offset != 0));
  natural.size = natural.capacity;

  if (offset == 0) {
    for (size_t i = 0; i != n; ++i) { natural[i] = 0; }
    for (size_t i = 0; i != size; ++i) { natural[i + n] = data[i]; }
  } else {
    for (size_t i = 0; i != n; ++i) { natural[i] = 0; }
    uint_t prev = 0;
    for (size_t i = 0; i != size; ++i) {
      natural[i + n] = Binary::cat(prev, data[i], bits_width - offset);
      prev = data[i];
    }
    natural[size + n] = Binary::cat<uint_t>(prev, 0, bits_width - offset);
    if (natural.size != 0 && natural[natural.size - 1] == 0) {
      --natural.size;
    }
    assert(natural.size == 0 || natural[natural.size - 1] != 0);
  }
  return natural;
}

Natural &Natural::operator<<=(size_t n) {
  if (n % bits_width == 0) {
    if (size + n > capacity) {
      *this = operator<<(n);
    } else {
      n /= bits_width;
      for (size_t i = 0; i != size; ++i) { data[i + n] = data[i]; }
      for (size_t i = 0; i != n; ++i) { data[i] = 0; }
    }
  } else {
    *this = operator<<(n);
  }
  return *this;
}
