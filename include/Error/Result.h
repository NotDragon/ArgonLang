//
// Created by User on 13/12/2024.
//

#ifndef LANG_CMAKE_RESULT_H
#define LANG_CMAKE_RESULT_H
#include <memory>

namespace ArgonLang {

	template<typename T>
	class Result {
		T value;
		std::string errorMsg;
	public:
		bool hasValue() const {
			return value != nullptr;
		}

		bool hasError() const {
			return !errorMsg.empty();
		}

		std::string getErrorMsg() const {
			return errorMsg;
		}
		T& getValue() const {
			if (!hasValue()) {
				throw std::runtime_error("Attempting to access value when there is no value.");
			}
			return value;
		}
		T&& moveValue() {
			if(hasError()) {
				throw std::runtime_error("Attempting to extract value when there is an error: " + errorMsg);
			}
			if (!hasValue()) {
				throw std::runtime_error("Attempting to extract value when there is no value.");
			}
			return std::move(value);
		}

		Result& operator=(T newValue) {
			this->value = std::move(newValue);
			return *this;
		}

		Result(T value): value(std::move(value)), errorMsg("") { }
		Result(): value(nullptr), errorMsg("") { }
//		Result(std::unique_ptr<T> value);
		Result(std::string errorMsg): value(nullptr), errorMsg(std::move(errorMsg)) { }
	};
}

#endif //LANG_CMAKE_RESULT_H
