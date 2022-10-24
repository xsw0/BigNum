//
// Created by 0x0 on 10/21/22.
//

#include "Natural.h"
#include "binary.h"

Natural Natural::add(Natural *dest, const Natural &lhs, const Natural &rhs) {
  if (lhs.size > rhs.size) { return add(dest, rhs, lhs); }

  Natural natural;
  if (!dest || dest->capacity <= rhs.size) {
    natural.reserve_data(rhs.size + 1);
    dest = &natural;
  }

  uint_t carry = 0;

  carry = Binary::add(dest->data, lhs.data, rhs.data, lhs.size);
  carry = Binary::half_add(dest->data + lhs.size, rhs.data + lhs.size, rhs.size - lhs.size, carry);

  dest->data[rhs.size] = carry;
  dest->size = rhs.size + carry;

  return std::move(*dest);
}

Natural Natural::sub(Natural *dest, const Natural &lhs, const Natural &rhs) {
  if (lhs.size < rhs.size) { throw std::runtime_error("overflow"); }

  Natural natural;
  if (!dest || dest->capacity < lhs.size) {
    natural.reserve_data(lhs.size);
    dest = &natural;
  }

  uint_t borrow = Binary::sub(dest->data, lhs.data, lhs.size, rhs.data, rhs.size);
  dest->size = lhs.size;
  while (dest->size != 0 && dest->back() == 0) { --dest->size; }
  if (borrow) {
    if (dest != &natural) {
      *dest += rhs;
      assert(*dest >= Natural{1} << rhs.size * bits_width);
      *dest -= Natural{1} << rhs.size * bits_width;
    }
    throw std::runtime_error("overflow");
  }
  return std::move(*dest);
}

Natural Natural::mul(Natural *dest, const Natural &lhs, const Natural &rhs) {
  if (lhs.size == 0 || rhs.size == 0) { return {}; }

  Natural natural;
  if (!dest || dest->capacity < lhs.size + rhs.size) {
    natural.reserve_data(lhs.size + rhs.size);
    dest = &natural;
  }

  Binary::mul(dest->data, lhs.data, lhs.size, rhs.data, rhs.size);

  dest->size = lhs.size + rhs.size;
  if (dest->back() == 0) { --dest->size; }
  assert(dest->back() != 0);

  return std::move(*dest);
}

Natural Natural::div(Natural *dest, const Natural &lhs, const Natural &rhs) {
  Natural quotient;
  Natural remainder;

  if (rhs.size == 0) { throw std::runtime_error("division by zero exception"); }
  if (dest != &lhs) {
    remainder = lhs;
    dest = &remainder;
  }

  return quotient;
}

//std::pair<Natural, Natural> Natural::div(const Natural &rhs) const {
//}
//
//Natural &Natural::operator/=(const Natural &rhs) {
//  return *this = operator/(rhs);
//}
//
//Natural Natural::operator/(const Natural &rhs) const {
//  return div(rhs).first;
//}
//
//Natural &Natural::operator%=(const Natural &rhs) {
//  return *this = operator%(rhs);
//}
//
//Natural Natural::operator%(const Natural &rhs) const {
//  return div(rhs).second;
//}
