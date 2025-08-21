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

int32_t testCompoundDestructuring(){auto __compound_temp = {1,2,3,4,5};auto first = __compound_temp[0];auto rest = std::vector<decltype(__compound_temp[0])>(__compound_temp.begin() + 1, __compound_temp.end());auto __compound_temp = {10,20,30,40};auto prefix = std::vector<decltype(__compound_temp[0])>(__compound_temp.begin(), __compound_temp.end() - 1);auto last = __compound_temp[__compound_temp.size() - 1 + 0];return 0;}int32_t main(){return testCompoundDestructuring();;}