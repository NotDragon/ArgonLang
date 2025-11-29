#include <cstdint>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <memory>
#include <functional>
#include <utility>
#include <iterator>
#include <variant>
#include <vector>
#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <type_traits>
#include "runtime/ArgonRuntime.h"

// Built-in concepts
template<typename T>
concept Number = std::is_arithmetic_v<T>;

template<typename T>
concept Type = true; // Any type

int32_t main(){ARGON_SCOPE_BEGIN();int32_t x = 0;while(x < 10){ARGON_SCOPE_BEGIN();x=x + 1;}return x;}