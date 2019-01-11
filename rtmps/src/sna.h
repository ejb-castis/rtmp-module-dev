#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace serialnumberarithmetic {

// RFC 1982 의 구현
// 32 bit 만 사용해서,
// 더 큰 수를 표현할 때 rollover 하는 방식
// 따라서 rollover count 를 알면 원래 수를 구할 수 있다.

struct serialnumber {
  uint16_t serialbits{8};

  uint64_t divisor{0};
  uint64_t half_divisor{0};
  uint64_t number{0};
  uint64_t max_add_number{0};

  serialnumber(uint64_t value, uint16_t size_in_bits = 32)
      : serialbits{size_in_bits} {
    divisor = std::pow(2, serialbits);
    half_divisor = std::pow(2, serialbits - 1);
    max_add_number = half_divisor - 1;
    number = value % divisor;
  }

  bool eq(serialnumber const& o) const { return number == o.number; }
  bool ne(serialnumber const& o) const { return number != o.number; }
  bool lt(serialnumber const& o) const {
    return ((number < o.number) && (o.number - number < half_divisor)) ||
           ((number > o.number) && (number - o.number > half_divisor));
  }
  bool gt(serialnumber const& o) const {
    return ((number < o.number) && (o.number - number > half_divisor)) ||
           ((number > o.number) && (number - o.number < half_divisor));
  }
  bool le(serialnumber const& o) const { return eq(o) || lt(o); }
  bool ge(serialnumber const& o) const { return eq(o) || gt(o); }
  uint64_t add(serialnumber const& o) {
    if (serialbits != o.serialbits) {
      throw std::invalid_argument("different serialbits");
    }
    if (0 <= o.number && o.number <= max_add_number) {
      number = (number + o.number) % divisor;
      return number;
    } else {
      throw std::overflow_error("fails to add too big value");
    }
  }
  std::string to_str() const {
    std::ostringstream os;
    os << "serialbits[" << serialbits << "],"
       << "number[" << number << "],"
       << "divisor[" << divisor << "],"
       << "half_divisor[" << half_divisor << "],"
       << "max_add_number[" << max_add_number << "]";

    return os.str();
  }
  std::string to_hex() const {
    std::ostringstream os;
    os << "serialbits[0x" << std::hex << serialbits << "],"
       << "number[0x" << std::hex << number << "],"
       << "divisor[0x" << std::hex << divisor << "],"
       << "half_divisor[0x" << std::hex << half_divisor << "],"
       << "max_add_number[0x" << std::hex << max_add_number << "]";

    return os.str();
  }
};

bool is_rolled_over(serialnumber const pre, serialnumber const next) {
  return (pre.serialbits == next.serialbits) && (next.number < pre.number) &&
         next.gt(pre);
}

uint64_t unroll(serialnumber const sn, uint32_t const rollover_count) {
  uint64_t value = sn.divisor * rollover_count + sn.number;
  return value;
}

uint64_t unroll(serialnumber const sn, serialnumber const pre,
                uint32_t& rollover_count) {
  if (pre.serialbits != sn.serialbits)
    throw std::invalid_argument("different serialbits");

  if (is_rolled_over(pre, sn)) {
    ++rollover_count;
    return unroll(sn, rollover_count);
  } else {
    return unroll(sn, rollover_count);
  }
}

serialnumber add(serialnumber const s1, uint64_t delta,
                 uint32_t& rollover_count) {
  if (0 <= delta && delta <= s1.max_add_number) {
    serialnumber s2(s1.number + delta, s1.serialbits);

    if (is_rolled_over(s1, s2)) {
      ++rollover_count;
    }
    return s2;
  } else {
    throw std::overflow_error("fails to add too big value");
  }
}

}  // namespace serialnumberarithmetic
