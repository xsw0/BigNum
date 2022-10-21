//
// Created by 0x0 on 10/21/22.
//

#include "Natural.h"
#include "binary.h"

Natural &Natural::operator+=(const Natural &rhs) {
  *this = *this + rhs;
  return *this;
}

Natural Natural::operator+(const Natural &rhs) const {
  if (size > rhs.size) {
    return rhs + *this;
  }

  Natural natural;
  natural.reserve_data(1 + rhs.size);

  uint_t carry = 0;
  size_t i = 0;
  for (; i != size; ++i) {
    uint_t sum = data[i] + rhs[i];
    natural[i] = sum + carry;
    carry = Binary::carry_add(data[i], rhs[i]) + Binary::carry_add(sum, carry);
  }

  for (; i != rhs.size; ++i) {
    natural[i] = rhs[i] + carry;
    carry = Binary::carry_add(carry, rhs[i]);
  }

  if (carry) {
    assert(carry == 1);
    natural[i] = carry;
    ++i;
  }

  natural.size = i;
  return natural;
}

Natural &Natural::operator-=(const Natural &rhs) {
  assert(*this >= rhs);
  if (size < rhs.size) {
    throw std::runtime_error("overflow");
  } else {
    uint_t borrow = 0;
    size_t i = 0;
    for (; i < rhs.size; ++i) {
      if (data[i]) {
        data[i] -= borrow;
        borrow = data[i] < rhs[i];
        data[i] -= rhs[i];
      } else {
        data[i] -= borrow + rhs[i];
        borrow = rhs[i] || borrow;
      }
    }
    if (borrow && size == rhs.size) {
      throw std::runtime_error("overflow");
    }
    if (borrow) {
      while (data[i] == 0) {
        ++i;
      }
      data[i] -= borrow;
    }
    while (size != 0 && data[size - 1] == 0) {
      --size;
    }
  }
  return *this;
}

Natural Natural::operator-(const Natural &rhs) const {
  Natural natural{*this};
  natural -= rhs;
  return natural;
}
