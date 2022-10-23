//
// Created by 0x0 on 10/21/22.
//

#include "Natural.h"
#include "binary.h"

Natural &Natural::operator+=(const Natural &rhs) {
//  if (capacity < 1 + std::max(size, rhs.size)) {
  return *this = operator+(rhs);
//  }
}

Natural Natural::operator+(const Natural &rhs) const {
  auto &lhs = *this;

  if (lhs.size > rhs.size) {
    return rhs + lhs;
  }

  Natural natural;
  natural.reserve_data(1 + rhs.size);

  uint_t carry = 0;
  size_t i = 0;
  for (; i != lhs.size; ++i) {
    uint_t sum = lhs[i] + rhs[i];
    natural[i] = sum + carry;
    carry = Binary::carry_add(lhs[i], rhs[i]) + Binary::carry_add(sum, carry);
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
  auto &lhs = *this;

  assert(lhs >= rhs);
  if (lhs.size < rhs.size) {
    throw std::runtime_error("overflow");
  } else {
    uint_t borrow = 0;
    size_t i = 0;
    for (; i < rhs.size; ++i) {
      if (lhs[i]) {
        lhs[i] -= borrow;
        borrow = lhs[i] < rhs[i];
        lhs[i] -= rhs[i];
      } else {
        lhs[i] -= borrow + rhs[i];
        borrow = rhs[i] || borrow;
      }
    }
    if (borrow && lhs.size == rhs.size) {
      throw std::runtime_error("overflow");
    }
    if (borrow) {
      while (lhs[i] == 0) {
        ++i;
      }
      lhs[i] -= borrow;
    }
    while (lhs.size != 0 && lhs.back() == 0) {
      --lhs.size;
    }
  }
  return *this;
}

Natural Natural::operator-(const Natural &rhs) const {
  return Natural(*this) -= rhs;
}

void Natural::carry_to(size_t index, uint_t carry) {
  while (carry) {
    uint_t sum = data[index] + carry;
    carry = Binary::carry_add(data[index], carry);
    data[index] = sum;
    ++index;
  }
}

Natural &Natural::operator*=(const Natural &rhs) {
  return *this = operator*(rhs);
}

Natural Natural::operator*(const Natural &rhs) const {
  auto &lhs = *this;
  if (lhs.size == 0 || rhs.size == 0) { return {}; }

  Natural natural;
  natural.reserve_data(lhs.size + rhs.size);
  natural.size = (lhs.size + rhs.size);

  natural.back() = 0;

  for (size_t l = 0; l != lhs.size; ++l) {
    for (size_t r = 0; r != rhs.size; ++r) {
      size_t index = l + r;
      uint_t prod = lhs[l] * rhs[r];
      auto prod_carry = Binary::carry_mul(lhs[l], rhs[r]);
      natural.carry_to(index, prod);
      natural.carry_to(index + 1, prod_carry);
    }
  }
  if (natural.back() == 0) {
    --natural.size;
  }
  assert(natural.back() != 0);

  return natural;
}

std::pair<Natural, Natural> Natural::div(const Natural &rhs) const {

}

Natural &Natural::operator/=(const Natural &rhs) {
  return *this = operator/(rhs);
}

Natural Natural::operator/(const Natural &rhs) const {
  return div(rhs).first;
}

Natural &Natural::operator%=(const Natural &rhs) {
  return *this = operator%(rhs);
}

Natural Natural::operator%(const Natural &rhs) const {
  return div(rhs).second;
}
