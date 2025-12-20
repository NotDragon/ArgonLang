#include <cstdint>
#include "runtime/ArgonRuntime.h"

int main(){ARGON_SCOPE_BEGIN();auto arr = ArgonLang::Runtime::par([&]() { return vector{1,2,3,4,5}; });}