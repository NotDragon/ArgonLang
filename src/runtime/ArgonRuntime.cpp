#include "runtime/ArgonRuntime.h"

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <vector>

namespace ArgonLang {
namespace Runtime {

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
