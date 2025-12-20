#include "runtime/ArgonRuntime.h"

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <limits>

namespace ArgonLang {
namespace Runtime {
// ===== i128 CLASS IMPLEMENTATIONS =====

i128::i128(const std::string& str) : high(0), low(0) {
    bool negative = false;
    size_t start = 0;
    if (!str.empty() && str[0] == '-') {
        negative = true;
        start = 1;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            *this = *this * i128(10) + i128(static_cast<i64>(str[i] - '0'));
        }
    }
    
    if (negative) {
        *this = -*this;
    }
}

i128::operator int64_t() const {
    return static_cast<int64_t>(low);
}

i128::operator double() const {
    double result = static_cast<double>(low);
    result += static_cast<double>(high) * 18446744073709551616.0; // 2^64
    return result;
}

i128 i128::operator+(const i128& other) const {
    u64 new_low = low + other.low;
    i64 new_high = high + other.high;
    if (new_low < low) { // Overflow in low part
        new_high++;
    }
    return i128(new_high, new_low);
}

i128 i128::operator-(const i128& other) const {
    u64 new_low = low - other.low;
    i64 new_high = high - other.high;
    if (new_low > low) { // Underflow in low part
        new_high--;
    }
    return i128(new_high, new_low);
}

i128 i128::operator*(const i128& other) const {
    // Convert to unsigned for multiplication, then handle sign
    bool neg_result = (is_negative() != other.is_negative());
    
    // Get absolute values as u128
    u128 a_abs, b_abs;
    if (is_negative()) {
        i128 abs_this = -*this;
        a_abs = u128(static_cast<u64>(abs_this.high), abs_this.low);
    } else {
        a_abs = u128(static_cast<u64>(high), low);
    }
    
    if (other.is_negative()) {
        i128 abs_other = -other;
        b_abs = u128(static_cast<u64>(abs_other.high), abs_other.low);
    } else {
        b_abs = u128(static_cast<u64>(other.high), other.low);
    }
    
    u128 result = a_abs * b_abs;
    
    if (neg_result) {
        // Negate result
        u128 neg_result_u = u128(~result.high, ~result.low) + u128(1ULL);
        return i128(-static_cast<i64>(neg_result_u.high), neg_result_u.low);
    } else {
        return i128(static_cast<i64>(result.high), result.low);
    }
}

i128 i128::operator/(const i128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }
    
    // Handle simple cases
    if (other.high == 0 && other.low == 1) {
        return *this;
    }
    if (*this == other) {
        return i128(1);
    }
    if (is_zero()) {
        return i128(0);
    }
    
    // Get absolute values
    bool neg_result = (is_negative() != other.is_negative());
    u128 dividend_abs, divisor_abs;
    
    if (is_negative()) {
        i128 abs_this = -*this;
        dividend_abs = u128(static_cast<u64>(abs_this.high), abs_this.low);
    } else {
        dividend_abs = u128(static_cast<u64>(high), low);
    }
    
    if (other.is_negative()) {
        i128 abs_other = -other;
        divisor_abs = u128(static_cast<u64>(abs_other.high), abs_other.low);
    } else {
        divisor_abs = u128(static_cast<u64>(other.high), other.low);
    }
    
    u128 quotient = dividend_abs / divisor_abs;
    
    if (neg_result) {
        // Negate quotient
        u128 neg_quotient = u128(~quotient.high, ~quotient.low) + u128(1ULL);
        return i128(-static_cast<i64>(neg_quotient.high), neg_quotient.low);
    } else {
        return i128(static_cast<i64>(quotient.high), quotient.low);
    }
}

i128 i128::operator%(const i128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Modulo by zero");
    }
    
    // Get absolute values
    bool neg_result = is_negative();
    u128 dividend_abs, divisor_abs;
    
    if (is_negative()) {
        i128 abs_this = -*this;
        dividend_abs = u128(static_cast<u64>(abs_this.high), abs_this.low);
    } else {
        dividend_abs = u128(static_cast<u64>(high), low);
    }
    
    if (other.is_negative()) {
        i128 abs_other = -other;
        divisor_abs = u128(static_cast<u64>(abs_other.high), abs_other.low);
    } else {
        divisor_abs = u128(static_cast<u64>(other.high), other.low);
    }
    
    u128 remainder = dividend_abs % divisor_abs;
    
    if (neg_result && !remainder.is_zero()) {
        // Negate remainder
        u128 neg_remainder = u128(~remainder.high, ~remainder.low) + u128(1ULL);
        return i128(-static_cast<i64>(neg_remainder.high), neg_remainder.low);
    } else {
        return i128(static_cast<i64>(remainder.high), remainder.low);
    }
}

i128 i128::operator-() const {
    if (is_zero()) {
        return i128(0);
    }
    i128 result(~high, ~low);
    return result + i128(1);
}

i128& i128::operator+=(const i128& other) {
    *this = *this + other;
    return *this;
}

i128& i128::operator-=(const i128& other) {
    *this = *this - other;
    return *this;
}

i128& i128::operator*=(const i128& other) {
    *this = *this * other;
    return *this;
}

i128& i128::operator/=(const i128& other) {
    *this = *this / other;
    return *this;
}

i128& i128::operator%=(const i128& other) {
    *this = *this % other;
    return *this;
}

i128& i128::operator++() {
    *this = *this + i128(1);
    return *this;
}

i128 i128::operator++(int) {
    i128 temp(*this);
    ++(*this);
    return temp;
}

i128& i128::operator--() {
    *this = *this - i128(1);
    return *this;
}

i128 i128::operator--(int) {
    i128 temp(*this);
    --(*this);
    return temp;
}

i128 i128::operator&(const i128& other) const {
    return i128(high & other.high, low & other.low);
}

i128 i128::operator|(const i128& other) const {
    return i128(high | other.high, low | other.low);
}

i128 i128::operator^(const i128& other) const {
    return i128(high ^ other.high, low ^ other.low);
}

i128 i128::operator~() const {
    return i128(~high, ~low);
}

i128 i128::operator<<(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return i128(0);
    
    if (shift < 64) {
        u64 new_low = low << shift;
        i64 new_high = (high << shift) | static_cast<i64>(low >> (64 - shift));
        return i128(new_high, new_low);
    } else {
        i64 new_high = static_cast<i64>(low << (shift - 64));
        return i128(new_high, 0);
    }
}

i128 i128::operator>>(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return is_negative() ? i128(-1, 0xFFFFFFFFFFFFFFFFULL) : i128(0);
    
    if (shift < 64) {
        u64 new_low = (low >> shift) | (static_cast<u64>(high) << (64 - shift));
        i64 new_high = high >> shift;
        return i128(new_high, new_low);
    } else {
        u64 new_low = static_cast<u64>(high >> (shift - 64));
        if (is_negative()) {
            new_low |= 0xFFFFFFFFFFFFFFFFULL << (128 - shift);
        }
        return i128(is_negative() ? -1 : 0, new_low);
    }
}

i128& i128::operator&=(const i128& other) {
    *this = *this & other;
    return *this;
}

i128& i128::operator|=(const i128& other) {
    *this = *this | other;
    return *this;
}

i128& i128::operator^=(const i128& other) {
    *this = *this ^ other;
    return *this;
}

i128& i128::operator<<=(int shift) {
    *this = *this << shift;
    return *this;
}

i128& i128::operator>>=(int shift) {
    *this = *this >> shift;
    return *this;
}

bool i128::operator==(const i128& other) const {
    return high == other.high && low == other.low;
}

bool i128::operator!=(const i128& other) const {
    return !(*this == other);
}

bool i128::operator<(const i128& other) const {
    if (high != other.high) {
        return high < other.high;
    }
    return low < other.low;
}

bool i128::operator>(const i128& other) const {
    return other < *this;
}

bool i128::operator<=(const i128& other) const {
    return !(*this > other);
}

bool i128::operator>=(const i128& other) const {
    return !(*this < other);
}

std::string i128::to_string() const {
    if (is_zero()) return "0";
    
    i128 temp = is_negative() ? -*this : *this;
    std::string result;
    
    while (!temp.is_zero()) {
        i128 remainder = temp % i128(10);
        result = char('0' + static_cast<int>(remainder.low)) + result;
        temp = temp / i128(10);
    }
    
    if (is_negative()) result = "-" + result;
    return result;
}

std::ostream& operator<<(std::ostream& os, const i128& val) {
    return os << val.to_string();
}

std::istream& operator>>(std::istream& is, i128& val) {
    std::string str;
    is >> str;
    val = i128(str);
    return is;
}

// ===== u128 CLASS IMPLEMENTATIONS =====

u128::u128(const std::string& str) : high(0), low(0) {
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            *this = *this * u128(10ULL) + u128(static_cast<u64>(c - '0'));
        }
    }
}

u128::operator uint64_t() const {
    return low;
}

u128::operator double() const {
    double result = static_cast<double>(low);
    result += static_cast<double>(high) * 18446744073709551616.0; // 2^64
    return result;
}

u128 u128::operator+(const u128& other) const {
    u64 new_low = low + other.low;
    u64 new_high = high + other.high;
    if (new_low < low) { // Overflow in low part
        new_high++;
    }
    return u128(new_high, new_low);
}

u128 u128::operator-(const u128& other) const {
    u64 new_low = low - other.low;
    u64 new_high = high - other.high;
    if (new_low > low) { // Underflow in low part
        new_high--;
    }
    return u128(new_high, new_low);
}

u128 u128::operator*(const u128& other) const {
    // Use 64x64 -> 128 bit multiplication
    // (a.high * 2^64 + a.low) * (b.high * 2^64 + b.low)
    // = a.high * b.high * 2^128 + (a.high * b.low + a.low * b.high) * 2^64 + a.low * b.low
    
    // We can ignore a.high * b.high * 2^128 as it overflows 128 bits
    
    // Multiply low parts: a.low * b.low -> 128 bits
    u64 a_low_low = low & 0xFFFFFFFFULL;
    u64 a_low_high = low >> 32;
    u64 b_low_low = other.low & 0xFFFFFFFFULL;
    u64 b_low_high = other.low >> 32;
    
    u64 p0 = a_low_low * b_low_low;
    u64 p1 = a_low_low * b_low_high + a_low_high * b_low_low;
    u64 p2 = a_low_high * b_low_high;
    
    u64 carry = p1 >> 32;
    u64 new_low = p0 + (p1 << 32);
    if (new_low < p0) carry++;
    
    u64 mid = p2 + carry;
    carry = mid >> 32;
    new_low += (mid << 32);
    if (new_low < (mid << 32)) carry++;
    
    // Add cross terms: a.high * b.low + a.low * b.high
    u64 cross1 = high * other.low;
    u64 cross2 = low * other.high;
    
    u64 new_high = cross1 + cross2 + carry;
    if (new_high < cross1) {
        // Overflow in high part - this means result > 2^128, but we'll cap it
    }
    
    // Add low part of cross terms to new_low
    u64 cross_low = (cross1 & 0xFFFFFFFFULL) + (cross2 & 0xFFFFFFFFULL);
    new_low += cross_low;
    if (new_low < cross_low) {
        new_high++;
    }
    
    return u128(new_high, new_low);
}

u128 u128::operator/(const u128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }
    
    if (other.high == 0 && other.low == 1) {
        return *this;
    }
    if (*this == other) {
        return u128(1ULL);
    }
    if (is_zero()) {
        return u128(0ULL);
    }
    
    if (*this < other) {
        return u128(0ULL);
    }
    
    // Binary long division
    u128 quotient(0ULL);
    u128 remainder(0ULL);
    
    for (int i = 127; i >= 0; --i) {
        remainder = remainder << 1;
        u128 shifted = *this >> i;
        if (shifted.low & 1) {
            remainder.low |= 1;
        }
        
        if (remainder >= other) {
            remainder = remainder - other;
            if (i >= 64) {
                quotient.high |= 1ULL << (i - 64);
            } else {
                quotient.low |= 1ULL << i;
            }
        }
    }
    
    return quotient;
}

u128 u128::operator%(const u128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Modulo by zero");
    }
    
    if (*this < other) {
        return *this;
    }
    
    // Binary long division to get remainder
    u128 remainder(0ULL);
    
    for (int i = 127; i >= 0; --i) {
        remainder = remainder << 1;
        u128 shifted = *this >> i;
        if (shifted.low & 1) {
            remainder.low |= 1;
        }
        
        if (remainder >= other) {
            remainder = remainder - other;
        }
    }
    
    return remainder;
}

u128& u128::operator+=(const u128& other) {
    *this = *this + other;
    return *this;
}

u128& u128::operator-=(const u128& other) {
    *this = *this - other;
    return *this;
}

u128& u128::operator*=(const u128& other) {
    *this = *this * other;
    return *this;
}

u128& u128::operator/=(const u128& other) {
    *this = *this / other;
    return *this;
}

u128& u128::operator%=(const u128& other) {
    *this = *this % other;
    return *this;
}

u128& u128::operator++() {
    *this = *this + u128(1ULL);
    return *this;
}

u128 u128::operator++(int) {
    u128 temp(*this);
    ++(*this);
    return temp;
}

u128& u128::operator--() {
    *this = *this - u128(1ULL);
    return *this;
}

u128 u128::operator--(int) {
    u128 temp(*this);
    --(*this);
    return temp;
}

u128 u128::operator&(const u128& other) const {
    return u128(high & other.high, low & other.low);
}

u128 u128::operator|(const u128& other) const {
    return u128(high | other.high, low | other.low);
}

u128 u128::operator^(const u128& other) const {
    return u128(high ^ other.high, low ^ other.low);
}

u128 u128::operator~() const {
    return u128(~high, ~low);
}

u128 u128::operator<<(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return u128(0ULL);
    
    if (shift < 64) {
        u64 new_low = low << shift;
        u64 new_high = (high << shift) | (low >> (64 - shift));
        return u128(new_high, new_low);
    } else {
        u64 new_high = low << (shift - 64);
        return u128(new_high, 0);
    }
}

u128 u128::operator>>(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return u128(0ULL);
    
    if (shift < 64) {
        u64 new_low = (low >> shift) | (high << (64 - shift));
        u64 new_high = high >> shift;
        return u128(new_high, new_low);
    } else {
        u64 new_low = high >> (shift - 64);
        return u128(0, new_low);
    }
}

u128& u128::operator&=(const u128& other) {
    *this = *this & other;
    return *this;
}

u128& u128::operator|=(const u128& other) {
    *this = *this | other;
    return *this;
}

u128& u128::operator^=(const u128& other) {
    *this = *this ^ other;
    return *this;
}

u128& u128::operator<<=(int shift) {
    *this = *this << shift;
    return *this;
}

u128& u128::operator>>=(int shift) {
    *this = *this >> shift;
    return *this;
}

bool u128::operator==(const u128& other) const {
    return high == other.high && low == other.low;
}

bool u128::operator!=(const u128& other) const {
    return !(*this == other);
}

bool u128::operator<(const u128& other) const {
    if (high != other.high) {
        return high < other.high;
    }
    return low < other.low;
}

bool u128::operator>(const u128& other) const {
    return other < *this;
}

bool u128::operator<=(const u128& other) const {
    return !(*this > other);
}

bool u128::operator>=(const u128& other) const {
    return !(*this < other);
}

std::string u128::to_string() const {
    if (is_zero()) return "0";
    
    u128 temp = *this;
    std::string result;
    
    while (!temp.is_zero()) {
        u128 remainder = temp % u128(10ULL);
        result = char('0' + static_cast<int>(remainder.low)) + result;
        temp = temp / u128(10ULL);
    }
    
    return result;
}

std::ostream& operator<<(std::ostream& os, const u128& val) {
    return os << val.to_string();
}

std::istream& operator>>(std::istream& is, u128& val) {
    std::string str;
    is >> str;
    val = u128(str);
    return is;
}

// Thread-local scope manager stack definition
thread_local std::vector<std::shared_ptr<ScopeManager>> scope_stack;

// ===== TEMPLATE FUNCTION IMPLEMENTATIONS =====

// Functional programming utilities
template<typename Container, typename Predicate>
std::decay_t<Container> filter(Container&& container, Predicate&& predicate) {
	auto result = std::decay_t<Container>{};
	std::copy_if(container.begin(), container.end(), std::back_inserter(result), std::forward<Predicate>(predicate));
	return result;
}

template<typename Container, typename Transform>
std::decay_t<Container> map(Container&& container, Transform&& transform) {
	auto result = std::decay_t<Container>{};
	std::transform(container.begin(), container.end(), std::back_inserter(result), std::forward<Transform>(transform));
	return result;
}

template<typename Container, typename Reducer>
typename std::decay_t<Container>::value_type reduce(Container&& container, Reducer&& reducer) {
	using ValueType = typename std::decay_t<Container>::value_type;
	return std::accumulate(container.begin(), container.end(), ValueType{}, std::forward<Reducer>(reducer));
}

template<typename Container, typename Transform>
Container& map_pipe(Container& container, Transform&& transform) {
	std::transform(container.begin(), container.end(), container.begin(), std::forward<Transform>(transform));
	return container;
}

// Pattern matching utilities
template<typename T, typename... Cases>
auto match(T&& value, Cases&&... cases) {
	return ([&](auto&& val) { return (cases(val) || ...); })(std::forward<T>(value));
}

template<typename T>
bool match_range(T value, T start, T end, bool inclusive) {
	return inclusive ? (value >= start && value <= end) : (value >= start && value < end);
}

template<typename T>
bool match_wildcard(T&& value) {
	return true;
}

template<typename T, typename U>
bool match_value(T&& value, U&& pattern) {
	return value == pattern;
}

// Destructuring utilities
template<typename Container>
typename std::decay_t<Container>::value_type destructure_array_element(Container&& container, size_t index) {
	return container[index];
}

template<typename Container>
std::vector<typename std::decay_t<Container>::value_type> destructure_array_rest(Container&& container,
                                                                                 size_t start_index) {
	using ValueType = typename std::decay_t<Container>::value_type;
	return std::vector<ValueType>(container.begin() + start_index, container.end());
}

template<typename Struct, typename Member>
auto destructure_struct_field(Struct&& obj, Member Struct::*field) {
	return obj.*field;
}

// Compound destructuring class implementations
template<typename Container>
CompoundDestructure<Container>::CompoundDestructure(Container& src) : source(src) {}

template<typename Container>
template<size_t N>
auto CompoundDestructure<Container>::take_first() {
	std::array<typename Container::value_type, N> result;
	std::copy_n(source.begin(), N, result.begin());
	return result;
}

template<typename Container>
template<size_t N>
auto CompoundDestructure<Container>::take_last() {
	std::array<typename Container::value_type, N> result;
	std::copy_n(source.end() - N, N, result.begin());
	return result;
}

template<typename Container>
auto CompoundDestructure<Container>::take_middle(size_t skip_first, size_t skip_last) {
	using ValueType = typename Container::value_type;
	auto start = source.begin() + skip_first;
	auto end = source.end() - skip_last;
	return std::vector<ValueType>(start, end);
}

template<typename Container>
CompoundDestructure<std::decay_t<Container>> compound_destructure(Container& container) {
	return CompoundDestructure<std::decay_t<Container>>(container);
}

// ===== NON-TEMPLATE FUNCTION IMPLEMENTATIONS =====

// Range pattern matching for common types
bool match_range_int(int value, int start, int end, bool inclusive) {
	return inclusive ? (value >= start && value <= end) : (value >= start && value < end);
}

bool match_range_float(float value, float start, float end, bool inclusive) {
	return inclusive ? (value >= start && value <= end) : (value >= start && value < end);
}

bool match_range_double(double value, double start, double end, bool inclusive) {
	return inclusive ? (value >= start && value <= end) : (value >= start && value < end);
}

// ===== EXPLICIT TEMPLATE INSTANTIATIONS FOR COMMON TYPES =====

// Functional operators for std::vector<int>
template std::vector<int> filter(std::vector<int>&& container, std::function<bool(int)>&& predicate);
template std::vector<int> filter(const std::vector<int>& container, std::function<bool(int)>&& predicate);
template std::vector<int> map(std::vector<int>&& container, std::function<int(int)>&& transform);
template std::vector<int> map(const std::vector<int>& container, std::function<int(int)>&& transform);
template int reduce(std::vector<int>&& container, std::function<int(int, int)>&& reducer);
template int reduce(const std::vector<int>& container, std::function<int(int, int)>&& reducer);
template std::vector<int>& map_pipe(std::vector<int>& container, std::function<int(int)>&& transform);

// Functional operators for std::vector<float>
template std::vector<float> filter(std::vector<float>&& container, std::function<bool(float)>&& predicate);
template std::vector<float> filter(const std::vector<float>& container, std::function<bool(float)>&& predicate);
template std::vector<float> map(std::vector<float>&& container, std::function<float(float)>&& transform);
template std::vector<float> map(const std::vector<float>& container, std::function<float(float)>&& transform);
template float reduce(std::vector<float>&& container, std::function<float(float, float)>&& reducer);
template float reduce(const std::vector<float>& container, std::function<float(float, float)>&& reducer);
template std::vector<float>& map_pipe(std::vector<float>& container, std::function<float(float)>&& transform);

// Destructuring for std::vector<int>
template int destructure_array_element(std::vector<int>&& container, size_t index);
template int destructure_array_element(const std::vector<int>& container, size_t index);
template std::vector<int> destructure_array_rest(std::vector<int>&& container, size_t start_index);
template std::vector<int> destructure_array_rest(const std::vector<int>& container, size_t start_index);
template CompoundDestructure<std::vector<int>> compound_destructure(std::vector<int>& container);

// Destructuring for std::vector<float>
template float destructure_array_element(std::vector<float>&& container, size_t index);
template float destructure_array_element(const std::vector<float>& container, size_t index);
template std::vector<float> destructure_array_rest(std::vector<float>&& container, size_t start_index);
template std::vector<float> destructure_array_rest(const std::vector<float>& container, size_t start_index);
template CompoundDestructure<std::vector<float>> compound_destructure(std::vector<float>& container);

// Pattern matching for common types
template bool match_wildcard(int&&);
template bool match_wildcard(const int&);
template bool match_wildcard(float&&);
template bool match_wildcard(const float&);
template bool match_wildcard(double&&);
template bool match_wildcard(const double&);

template bool match_value(int&& value, int&& pattern);
template bool match_value(const int& value, const int& pattern);
template bool match_value(float&& value, float&& pattern);
template bool match_value(const float& value, const float& pattern);
template bool match_value(double&& value, double&& pattern);
template bool match_value(const double& value, const double& pattern);

} // namespace Runtime
} // namespace ArgonLang
