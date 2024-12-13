#include "Error/Result.h"

template<typename T>
bool ArgonLang::Result<T>::hasValue() {
	return value.has_value();
}

template<typename T>
bool ArgonLang::Result<T>::hasError() {
	return !errorMsg.empty();
}

template<typename T>
std::string ArgonLang::Result<T>::getErrorMsg() {
	if(hasError())
		return errorMsg;
}

template<typename T>
T ArgonLang::Result<T>::getvalue() {
	if(hasValue())
		return value;
}

template<typename T>
ArgonLang::Result<T>::Result(T value): value(value), errorMsg("") { }

template<typename T>
ArgonLang::Result<T>::Result(std::string errorMsg): value(std::nullopt), errorMsg(std::move(errorMsg)) { }