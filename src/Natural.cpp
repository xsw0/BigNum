//
// Created by 0x0 on 10/13/22.
//

#include "Natural.h"
#include "binary.h"
#include <optional>

std::allocator<Natural::uint_t> Natural::alloc;

Natural::Natural(uint64_t u) {
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

Natural::Natural(Natural &&natural) noexcept {
  size = natural.size;
  capacity = natural.capacity;
  data = natural.data;

  natural.size = 0;
  natural.capacity = 0;
  natural.data = nullptr;
}

Natural::Natural(const Natural &natural) {
  size = natural.size;
  capacity = natural.size;
  data = alloc.allocate(capacity);

  for (size_t i = 0; i != size; ++i) {
    data[i] = natural[i];
  }
}

Natural::~Natural() {
  alloc.deallocate(data, capacity);
}

Natural &Natural::operator=(Natural &&natural) noexcept {
  this->~Natural();

  size = natural.size;
  capacity = natural.capacity;
  data = natural.data;

  natural.size = 0;
  natural.capacity = 0;
  natural.data = nullptr;

  return *this;
}

Natural &Natural::operator=(const Natural &natural) {
  if (this == &natural) {
    return *this;
  }

  this->~Natural();

  size = natural.size;
  capacity = natural.size;
  data = alloc.allocate(capacity);

  for (size_t i = 0; i != size; ++i) {
    data[i] = natural[i];
  }

  return *this;
}

void Natural::reserve_data(size_t cap) {
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

bool Natural::operator==(const Natural &rhs) const {
  if (size != rhs.size) {
    return false;
  }
  for (size_t i = 0; i != size; ++i) {
    if (data[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

std::strong_ordering Natural::operator<=>(const Natural &rhs) const {
  if (size != rhs.size) {
    return size <=> rhs.size;
  }
  for (size_t i = size; i != 0; --i) {
    if (data[i - 1] != rhs[i - 1]) {
      return data[i - 1] <=> rhs[i - 1];
    }
  }
  return std::strong_ordering::equal;
}

std::optional<Natural> Natural::parse(std::string_view sv) {
  if (sv.empty()) { return {}; }
  if (sv[0] == '0' && sv.size() > 1) { return {}; }
  for (auto &&c: sv) { if (!isdigit(c)) { return {}; }}

  Natural natural;

  // 10^3 <= 2^10, plus two to round up
  natural.reserve((sv.size() * 10 + 2) / 3);

  for (auto c : sv) {
    // 10 times == 2 times + 8 times;
    natural <<= 1;
    auto two_times = natural;
    natural <<= 2;
    natural += two_times;

    natural += Natural(c - '0');
  }
  return natural;
}

std::string Natural::to_string(size_t base) const {
  std::string result = "0";
  for (size_t i = size; i != 0; --i) {
    for (size_t pos = bits_width; pos != 0; --pos) {
      int carry = Binary::bits_test(data[i - 1], pos - 1);
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

std::ostream &operator<<(std::ostream &os, const Natural &natural) {
  os << natural.to_string();
  return os;
}
