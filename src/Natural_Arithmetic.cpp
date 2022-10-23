//
// Created by 0x0 on 10/21/22.
//

#include "Natural.h"
#include "binary.h"

Natural Natural::add(Natural *dest, const Natural &lhs, const Natural &rhs) {
  if (lhs.size > rhs.size) { return add(dest, rhs, lhs); }

  Natural natural;
  if (!dest || dest->capacity <= rhs.size) {
    dest = &natural;
  }
  dest->reserve_data(rhs.size + 1);

  uint_t carry = 0;
  size_t i = 0;
  for (; i != lhs.size; ++i) {
    uint_t sum = lhs[i] + rhs[i] + carry;
    carry = Binary::carry_add(lhs[i], rhs[i]) + Binary::carry_add<uint_t>(lhs[i] + rhs[i], carry);
    dest->data[i] = sum;
  }

  for (; i != rhs.size; ++i) {
    uint_t sum = rhs[i] + carry;
    carry = Binary::carry_add(carry, rhs[i]);
    dest->data[i] = sum;
  }

  natural[rhs.size] = carry;
  natural.size = rhs.size + carry;

  return *dest;
}

Natural Natural::sub(Natural *dest, const Natural &lhs, const Natural &rhs) {
  if (lhs.size < rhs.size) { throw std::runtime_error("overflow"); }

  Natural natural;
  if (!dest || dest->capacity < lhs.size) {
    dest = &natural;
  }
  dest->reserve_data(lhs.size);

  uint_t borrow = 0;
  size_t i = 0;
  for (; i < rhs.size; ++i) {
    if (borrow) {
      borrow = lhs[i] <= rhs[i];
      dest->data[i] = lhs[i] - rhs[i] - 1;
    } else {
      borrow = lhs[i] < rhs[i];
      dest->data[i] = lhs[i] - rhs[i];
    }
  }
  while (i != lhs.size && borrow) {
    borrow = lhs[i] == 0;
    dest->data[i] = lhs[i] - 1;
    ++i;
  }
  if (dest != &lhs) {
    for (; i != lhs.size; ++i) {
      dest->data[i] = lhs[i];
    }
  }
  if (borrow) {
    if (dest != &natural) {
      *dest += rhs;
      assert(*dest >= Natural{1} << rhs.size * bits_width);
      *dest -= Natural{1} << rhs.size * bits_width;
    }
    throw std::runtime_error("overflow");
  }
  dest->size = lhs.size;
  while (dest->size != 0 && dest->back() == 0) { --dest->size; }
  return *dest;
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
//  auto &lhs = *this;
//  if (rhs.size == 0) { throw std::runtime_error("div zero error"); }
//  if (lhs.size < rhs.size) { return {{}, lhs}; }
//
//  size_t offset = lhs.size - rhs.size;
//
//  Natural remainder = *this;
//  Natural quotient;
//  quotient.reserve_data(offset + 1);
//
//  auto divisor = rhs << offset * bits_width;
//  while (true) {
//    assert(remainder.size == divisor.size);
//    auto so = remainder <=> divisor;
//    if (so == std::strong_ordering::less) {
//      if (offset == 0) { break; }
//      --offset;
//      quotient <<= bits_width;
//      divisor >>= bits_width;
//    } else if (so == std::strong_ordering::greater) {
//      if (remainder.back() == divisor.back()) {
//        remainder -= divisor;
//        quotient += Natural{1};
//        if (offset == 0) { break; }
//        --offset;
//        quotient <<= bits_width;
//        divisor >>= bits_width;
//      } else {
//        auto v = Natural(remainder.back() / (divisor.back() + 1));
//        if (v == Natural{0}) {
//          quotient += Natural{1};
//          remainder -= v;
//          quotient <<= bits_width;
//          divisor >>= bits_width;
//        } else {
//          quotient += v;
//          remainder -= divisor * v;
//        }
//      }
//    } else {
//      return {quotient + Natural{1}, {}};
//    }
//  }
//  return {quotient, remainder};
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
