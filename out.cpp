#include <cstdint>
#include "runtime/ArgonRuntime.h"

enum class Option { Some, None };
str test(Option opt){ARGON_SCOPE_BEGIN();return ([&]() { auto __match_val = opt;if(ArgonLang::Runtime::match_value(__match_val, Option::Some(value)) && (value > 0)) {return "positive some";} else if(ArgonLang::Runtime::match_value(__match_val, Option::Some(value)) && (value < 0)) {return "negative some";} else if(ArgonLang::Runtime::match_value(__match_val, Option::Some(value)) && (value == 0)) {return "zero some";} else if(ArgonLang::Runtime::match_value(__match_val, Option::None())) {return "none";} else if(ArgonLang::Runtime::match_wildcard(__match_val)) {return "unknown";}})();}int main(){ARGON_SCOPE_BEGIN();}