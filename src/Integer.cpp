//
// Created by 0x0 on 10/13/22.
//

#include "Integer.h"
#include <numeric>

std::allocator<Integer::uint_t> Integer::alloc;

Integer::Integer(uint64_t u) {
  capacity = 64 / bits_width;
  data = alloc.allocate(capacity);
  uint64_t mask = uint_t(~0);
  for (size_t i = 0; i != capacity; ++i) {
    data[i] = (u >> (bits_width * i)) & mask;
  }
  size = capacity;
  while (size != 0 && data[size - 1] == 0) {
    --size;
  }
}

Integer::Integer(const std::string &s) {
  assert(0);
}

Integer::Integer(Integer &&integer) noexcept {
  size = integer.size;
  capacity = integer.capacity;
  data = integer.data;

  integer.size = 0;
  integer.capacity = 0;
  integer.data = nullptr;
}

Integer::Integer(const Integer &integer) {
  size = integer.size;
  capacity = integer.size;
  data = alloc.allocate(capacity);

  for (size_t i = 0; i != size; ++i) {
    data[i] = integer.data[i];
  }
}

Integer::~Integer() {
  alloc.deallocate(data, capacity);
}

Integer &Integer::operator=(Integer &&integer) noexcept {
  this->~Integer();

  size = integer.size;
  capacity = integer.capacity;
  data = integer.data;

  integer.size = 0;
  integer.capacity = 0;
  integer.data = nullptr;

  return *this;
}

Integer &Integer::operator=(const Integer &integer) {
  if (this == &integer) {
    return *this;
  }

  this->~Integer();

  size = integer.size;
  capacity = integer.size;
  data = alloc.allocate(capacity);

  for (size_t i = 0; i != size; ++i) {
    data[i] = integer.data[i];
  }

  return *this;
}

std::string Integer::to_string(size_t base) const {
  std::string result = "0";
  for (size_t i = size; i != 0; --i) {
    for (size_t pos = bits_width; pos != 0; --pos) {
      int carry = bits_test(data[i - 1], pos - 1);
      for (size_t j = 0; j != result.size(); ++j) {
        int sum = (result[j] - '0') * 2 + carry;
        result[j] = static_cast<std::string::value_type>('0' + sum % 10);
        carry = sum / 10;
      }
      if (carry) {
        assert(carry == 1);
        result.push_back('1');
      }
    }
  }
  reverse(result.begin(), result.end());
  return result;
}

void Integer::reserve(size_t cap) {
  cap = (cap + (bits_width - 1)) / bits_width;
  if (capacity < cap) {
    uint_t *new_data = alloc.allocate(cap);
    for (size_t i = 0; i != size; ++i) {
      new_data[i] = data[i];
    }
    alloc.deallocate(data, capacity);
    capacity = cap;
    data = new_data;
  }
}

bool Integer::operator==(const Integer &rhs) const {
  if (size != rhs.size) {
    return false;
  }
  for (size_t i = 0; i != size; ++i) {
    if (data[i] != rhs.data[i]) {
      return false;
    }
  }
  return true;
}

std::strong_ordering Integer::operator<=>(const Integer &rhs) const {
  if (size != rhs.size) {
    return size <=> rhs.size;
  }
  for (size_t i = size; i != 0; --i) {
    if (data[i - 1] != rhs.data[i - 1]) {
      return data[i - 1] <=> rhs.data[i - 1];
    }
  }
  return std::strong_ordering::equal;
}

Integer &Integer::operator+=(const Integer &rhs) {
  *this = *this + rhs;
  return *this;
}

Integer Integer::operator+(const Integer &rhs) const {
  if (size > rhs.size) {
    return rhs + *this;
  }

  Integer integer;
  integer.reserve((1 + rhs.size) * bits_width);

  uint_t carry = 0;
  size_t i = 0;
  for (; i != size; ++i) {
    uint_t sum = data[i] + rhs.data[i];
    integer.data[i] = sum + carry;
    carry = carry_add(data[i], rhs.data[i]) + carry_add(sum, carry);
  }

  for (; i != rhs.size; ++i) {
    integer.data[i] = rhs.data[i] + carry;
    carry = carry_add(carry, rhs.data[i]);
  }

  if (carry) {
    assert(carry == 1);
    integer.data[i] = carry;
    ++i;
  }

  integer.size = i;
  return integer;
}

Integer &Integer::operator-=(const Integer &rhs) {
  assert(*this >= rhs);
  if (size < rhs.size) {
    throw std::runtime_error("overflow");
  } else {
    uint_t borrow = 0;
    size_t i = 0;
    for (; i < rhs.size; ++i) {
      if (data[i]) {
        data[i] -= borrow;
        borrow = data[i] < rhs.data[i];
        data[i] -= rhs.data[i];
      } else {
        data[i] -= borrow + rhs.data[i];
        borrow = rhs.data[i] || borrow;
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

Integer Integer::operator-(const Integer &rhs) const {
  Integer integer{*this};
  integer -= rhs;
  return integer;
}
