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

int32_t testStructDestructuring(){auto __destructure_temp = {.x = a,.y = 20};auto x = __destructure_temp.x;auto y = __destructure_temp.y;return 0;}int32_t main(){testStructDestructuring();}