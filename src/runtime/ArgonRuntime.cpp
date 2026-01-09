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

I128::I128(const std::string& str) : high(0), low(0) {
    bool negative = false;
    size_t start = 0;
    if (!str.empty() && str[0] == '-') {
        negative = true;
        start = 1;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            *this = *this * I128(10) + I128(static_cast<I64>(str[i] - '0'));
        }
    }
    
    if (negative) {
        *this = -*this;
    }
}

I128::operator int64_t() const {
    return static_cast<int64_t>(low);
}

I128::operator double() const {
    double result = static_cast<double>(low);
    result += static_cast<double>(high) * 18446744073709551616.0; // 2^64
    return result;
}

I128 I128::operator+(const I128& other) const {
    U64 new_low = low + other.low;
    I64 new_high = high + other.high;
    if (new_low < low) { // Overflow in low part
        new_high++;
    }
    return I128(new_high, new_low);
}

I128 I128::operator-(const I128& other) const {
    U64 new_low = low - other.low;
    I64 new_high = high - other.high;
    if (new_low > low) { // Underflow in low part
        new_high--;
    }
    return I128(new_high, new_low);
}

I128 I128::operator*(const I128& other) const {
    // Convert to unsigned for multiplication, then handle sign
    bool neg_result = (is_negative() != other.is_negative());
    
    // Get absolute values as u128
    U128 a_abs, b_abs;
    if (is_negative()) {
        I128 abs_this = -*this;
        a_abs = U128(static_cast<U64>(abs_this.high), abs_this.low);
    } else {
        a_abs = U128(static_cast<U64>(high), low);
    }
    
    if (other.is_negative()) {
        I128 abs_other = -other;
        b_abs = U128(static_cast<U64>(abs_other.high), abs_other.low);
    } else {
        b_abs = U128(static_cast<U64>(other.high), other.low);
    }
    
    U128 result = a_abs * b_abs;
    
    if (neg_result) {
        // Negate result
        U128 neg_result_u = U128(~result.high, ~result.low) + U128(1ULL);
        return I128(-static_cast<I64>(neg_result_u.high), neg_result_u.low);
    } else {
        return I128(static_cast<I64>(result.high), result.low);
    }
}

I128 I128::operator/(const I128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }
    
    // Handle simple cases
    if (other.high == 0 && other.low == 1) {
        return *this;
    }
    if (*this == other) {
        return I128(1);
    }
    if (is_zero()) {
        return I128(0);
    }
    
    // Get absolute values
    bool neg_result = (is_negative() != other.is_negative());
    U128 dividend_abs, divisor_abs;
    
    if (is_negative()) {
        I128 abs_this = -*this;
        dividend_abs = U128(static_cast<U64>(abs_this.high), abs_this.low);
    } else {
        dividend_abs = U128(static_cast<U64>(high), low);
    }
    
    if (other.is_negative()) {
        I128 abs_other = -other;
        divisor_abs = U128(static_cast<U64>(abs_other.high), abs_other.low);
    } else {
        divisor_abs = U128(static_cast<U64>(other.high), other.low);
    }
    
    U128 quotient = dividend_abs / divisor_abs;
    
    if (neg_result) {
        // Negate quotient
        U128 neg_quotient = U128(~quotient.high, ~quotient.low) + U128(1ULL);
        return I128(-static_cast<I64>(neg_quotient.high), neg_quotient.low);
    } else {
        return I128(static_cast<I64>(quotient.high), quotient.low);
    }
}

I128 I128::operator%(const I128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Modulo by zero");
    }
    
    // Get absolute values
    bool neg_result = is_negative();
    U128 dividend_abs, divisor_abs;
    
    if (is_negative()) {
        I128 abs_this = -*this;
        dividend_abs = U128(static_cast<U64>(abs_this.high), abs_this.low);
    } else {
        dividend_abs = U128(static_cast<U64>(high), low);
    }
    
    if (other.is_negative()) {
        I128 abs_other = -other;
        divisor_abs = U128(static_cast<U64>(abs_other.high), abs_other.low);
    } else {
        divisor_abs = U128(static_cast<U64>(other.high), other.low);
    }
    
    U128 remainder = dividend_abs % divisor_abs;
    
    if (neg_result && !remainder.is_zero()) {
        // Negate remainder
        U128 neg_remainder = U128(~remainder.high, ~remainder.low) + U128(1ULL);
        return I128(-static_cast<I64>(neg_remainder.high), neg_remainder.low);
    } else {
        return I128(static_cast<I64>(remainder.high), remainder.low);
    }
}

I128 I128::operator-() const {
    if (is_zero()) {
        return I128(0);
    }
    I128 result(~high, ~low);
    return result + I128(1);
}

I128& I128::operator+=(const I128& other) {
    *this = *this + other;
    return *this;
}

I128& I128::operator-=(const I128& other) {
    *this = *this - other;
    return *this;
}

I128& I128::operator*=(const I128& other) {
    *this = *this * other;
    return *this;
}

I128& I128::operator/=(const I128& other) {
    *this = *this / other;
    return *this;
}

I128& I128::operator%=(const I128& other) {
    *this = *this % other;
    return *this;
}

I128& I128::operator++() {
    *this = *this + I128(1);
    return *this;
}

I128 I128::operator++(int) {
    I128 temp(*this);
    ++(*this);
    return temp;
}

I128& I128::operator--() {
    *this = *this - I128(1);
    return *this;
}

I128 I128::operator--(int) {
    I128 temp(*this);
    --(*this);
    return temp;
}

I128 I128::operator&(const I128& other) const {
    return I128(high & other.high, low & other.low);
}

I128 I128::operator|(const I128& other) const {
    return I128(high | other.high, low | other.low);
}

I128 I128::operator^(const I128& other) const {
    return I128(high ^ other.high, low ^ other.low);
}

I128 I128::operator~() const {
    return I128(~high, ~low);
}

I128 I128::operator<<(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return I128(0);
    
    if (shift < 64) {
        U64 new_low = low << shift;
        I64 new_high = (high << shift) | static_cast<I64>(low >> (64 - shift));
        return I128(new_high, new_low);
    } else {
        I64 new_high = static_cast<I64>(low << (shift - 64));
        return I128(new_high, 0);
    }
}

I128 I128::operator>>(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return is_negative() ? I128(-1, 0xFFFFFFFFFFFFFFFFULL) : I128(0);
    
    if (shift < 64) {
        U64 new_low = (low >> shift) | (static_cast<U64>(high) << (64 - shift));
        I64 new_high = high >> shift;
        return I128(new_high, new_low);
    } else {
        U64 new_low = static_cast<U64>(high >> (shift - 64));
        if (is_negative()) {
            new_low |= 0xFFFFFFFFFFFFFFFFULL << (128 - shift);
        }
        return I128(is_negative() ? -1 : 0, new_low);
    }
}

I128& I128::operator&=(const I128& other) {
    *this = *this & other;
    return *this;
}

I128& I128::operator|=(const I128& other) {
    *this = *this | other;
    return *this;
}

I128& I128::operator^=(const I128& other) {
    *this = *this ^ other;
    return *this;
}

I128& I128::operator<<=(int shift) {
    *this = *this << shift;
    return *this;
}

I128& I128::operator>>=(int shift) {
    *this = *this >> shift;
    return *this;
}

bool I128::operator==(const I128& other) const {
    return high == other.high && low == other.low;
}

bool I128::operator!=(const I128& other) const {
    return !(*this == other);
}

bool I128::operator<(const I128& other) const {
    if (high != other.high) {
        return high < other.high;
    }
    return low < other.low;
}

bool I128::operator>(const I128& other) const {
    return other < *this;
}

bool I128::operator<=(const I128& other) const {
    return !(*this > other);
}

bool I128::operator>=(const I128& other) const {
    return !(*this < other);
}

std::string I128::to_string() const {
    if (is_zero()) return "0";
    
    I128 temp = is_negative() ? -*this : *this;
    std::string result;
    
    while (!temp.is_zero()) {
        I128 remainder = temp % I128(10);
        result = char('0' + static_cast<int>(remainder.low)) + result;
        temp = temp / I128(10);
    }
    
    if (is_negative()) result = "-" + result;
    return result;
}

std::ostream& operator<<(std::ostream& os, const I128& val) {
    return os << val.to_string();
}

std::istream& operator>>(std::istream& is, I128& val) {
    std::string str;
    is >> str;
    val = I128(str);
    return is;
}

// ===== u128 CLASS IMPLEMENTATIONS =====

U128::U128(const std::string& str) : high(0), low(0) {
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            *this = *this * U128(10ULL) + U128(static_cast<U64>(c - '0'));
        }
    }
}

U128::operator uint64_t() const {
    return low;
}

U128::operator double() const {
    double result = static_cast<double>(low);
    result += static_cast<double>(high) * 18446744073709551616.0; // 2^64
    return result;
}

U128 U128::operator+(const U128& other) const {
    U64 new_low = low + other.low;
    U64 new_high = high + other.high;
    if (new_low < low) { // Overflow in low part
        new_high++;
    }
    return U128(new_high, new_low);
}

U128 U128::operator-(const U128& other) const {
    U64 new_low = low - other.low;
    U64 new_high = high - other.high;
    if (new_low > low) { // Underflow in low part
        new_high--;
    }
    return U128(new_high, new_low);
}

U128 U128::operator*(const U128& other) const {
    // Use 64x64 -> 128 bit multiplication
    // (a.high * 2^64 + a.low) * (b.high * 2^64 + b.low)
    // = a.high * b.high * 2^128 + (a.high * b.low + a.low * b.high) * 2^64 + a.low * b.low
    
    // We can ignore a.high * b.high * 2^128 as it overflows 128 bits
    
    // Multiply low parts: a.low * b.low -> 128 bits
    U64 a_low_low = low & 0xFFFFFFFFULL;
    U64 a_low_high = low >> 32;
    U64 b_low_low = other.low & 0xFFFFFFFFULL;
    U64 b_low_high = other.low >> 32;
    
    U64 p0 = a_low_low * b_low_low;
    U64 p1 = a_low_low * b_low_high + a_low_high * b_low_low;
    U64 p2 = a_low_high * b_low_high;
    
    U64 carry = p1 >> 32;
    U64 new_low = p0 + (p1 << 32);
    if (new_low < p0) carry++;
    
    U64 mid = p2 + carry;
    carry = mid >> 32;
    new_low += (mid << 32);
    if (new_low < (mid << 32)) carry++;
    
    // Add cross terms: a.high * b.low + a.low * b.high
    U64 cross1 = high * other.low;
    U64 cross2 = low * other.high;
    
    U64 new_high = cross1 + cross2 + carry;
    if (new_high < cross1) {
        // Overflow in high part - this means result > 2^128, but we'll cap it
    }
    
    // Add low part of cross terms to new_low
    U64 cross_low = (cross1 & 0xFFFFFFFFULL) + (cross2 & 0xFFFFFFFFULL);
    new_low += cross_low;
    if (new_low < cross_low) {
        new_high++;
    }
    
    return U128(new_high, new_low);
}

U128 U128::operator/(const U128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }
    
    if (other.high == 0 && other.low == 1) {
        return *this;
    }
    if (*this == other) {
        return U128(1ULL);
    }
    if (is_zero()) {
        return U128(0ULL);
    }
    
    if (*this < other) {
        return U128(0ULL);
    }
    
    // Binary long division
    U128 quotient(0ULL);
    U128 remainder(0ULL);
    
    for (int i = 127; i >= 0; --i) {
        remainder = remainder << 1;
        U128 shifted = *this >> i;
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

U128 U128::operator%(const U128& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Modulo by zero");
    }
    
    if (*this < other) {
        return *this;
    }
    
    // Binary long division to get remainder
    U128 remainder(0ULL);
    
    for (int i = 127; i >= 0; --i) {
        remainder = remainder << 1;
        U128 shifted = *this >> i;
        if (shifted.low & 1) {
            remainder.low |= 1;
        }
        
        if (remainder >= other) {
            remainder = remainder - other;
        }
    }
    
    return remainder;
}

U128& U128::operator+=(const U128& other) {
    *this = *this + other;
    return *this;
}

U128& U128::operator-=(const U128& other) {
    *this = *this - other;
    return *this;
}

U128& U128::operator*=(const U128& other) {
    *this = *this * other;
    return *this;
}

U128& U128::operator/=(const U128& other) {
    *this = *this / other;
    return *this;
}

U128& U128::operator%=(const U128& other) {
    *this = *this % other;
    return *this;
}

U128& U128::operator++() {
    *this = *this + U128(1ULL);
    return *this;
}

U128 U128::operator++(int) {
    U128 temp(*this);
    ++(*this);
    return temp;
}

U128& U128::operator--() {
    *this = *this - U128(1ULL);
    return *this;
}

U128 U128::operator--(int) {
    U128 temp(*this);
    --(*this);
    return temp;
}

U128 U128::operator&(const U128& other) const {
    return U128(high & other.high, low & other.low);
}

U128 U128::operator|(const U128& other) const {
    return U128(high | other.high, low | other.low);
}

U128 U128::operator^(const U128& other) const {
    return U128(high ^ other.high, low ^ other.low);
}

U128 U128::operator~() const {
    return U128(~high, ~low);
}

U128 U128::operator<<(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return U128(0ULL);
    
    if (shift < 64) {
        U64 new_low = low << shift;
        U64 new_high = (high << shift) | (low >> (64 - shift));
        return U128(new_high, new_low);
    } else {
        U64 new_high = low << (shift - 64);
        return U128(new_high, 0);
    }
}

U128 U128::operator>>(int shift) const {
    if (shift <= 0) return *this;
    if (shift >= 128) return U128(0ULL);
    
    if (shift < 64) {
        U64 new_low = (low >> shift) | (high << (64 - shift));
        U64 new_high = high >> shift;
        return U128(new_high, new_low);
    } else {
        U64 new_low = high >> (shift - 64);
        return U128(0, new_low);
    }
}

U128& U128::operator&=(const U128& other) {
    *this = *this & other;
    return *this;
}

U128& U128::operator|=(const U128& other) {
    *this = *this | other;
    return *this;
}

U128& U128::operator^=(const U128& other) {
    *this = *this ^ other;
    return *this;
}

U128& U128::operator<<=(int shift) {
    *this = *this << shift;
    return *this;
}

U128& U128::operator>>=(int shift) {
    *this = *this >> shift;
    return *this;
}

bool U128::operator==(const U128& other) const {
    return high == other.high && low == other.low;
}

bool U128::operator!=(const U128& other) const {
    return !(*this == other);
}

bool U128::operator<(const U128& other) const {
    if (high != other.high) {
        return high < other.high;
    }
    return low < other.low;
}

bool U128::operator>(const U128& other) const {
    return other < *this;
}

bool U128::operator<=(const U128& other) const {
    return !(*this > other);
}

bool U128::operator>=(const U128& other) const {
    return !(*this < other);
}

std::string U128::to_string() const {
    if (is_zero()) return "0";
    
    U128 temp = *this;
    std::string result;
    
    while (!temp.is_zero()) {
        U128 remainder = temp % U128(10ULL);
        result = char('0' + static_cast<int>(remainder.low)) + result;
        temp = temp / U128(10ULL);
    }
    
    return result;
}

std::ostream& operator<<(std::ostream& os, const U128& val) {
    return os << val.to_string();
}

std::istream& operator>>(std::istream& is, U128& val) {
    std::string str;
    is >> str;
    val = U128(str);
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
