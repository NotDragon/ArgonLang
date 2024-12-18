//
// Created by User on 13/12/2024.
//

#ifndef LANG_CMAKE_RESULT_H
#define LANG_CMAKE_RESULT_H
#include <memory>
#include <type_traits>

namespace ArgonLang {

	template<typename T>
	class Result {
		T value;
		std::string errorMsg;

		template <typename U>
		struct is_unique_ptr : std::false_type {};

		template <typename U>
		struct is_unique_ptr<std::unique_ptr<U>> : std::true_type {};

	public:
		bool isNull() const {
			if constexpr (is_unique_ptr<T>::value)
				return value == nullptr;
			return false;
		}

		bool hasError() const {
			return !errorMsg.empty();
		}

		std::string getErrorMsg() const {
			return errorMsg;
		}

		T& getValue() {
			if(hasError()) {
				throw std::runtime_error("Attempting to access value when there is an error: " + errorMsg);
			}

			if (isNull()) {
				throw std::runtime_error("Attempting to access value when there is no value.");
			}
			return value;
		}

		T&& moveValue() {
			if(hasError()) {
				throw std::runtime_error("Attempting to move value when there is an error: " + errorMsg);
			}
			if (isNull()) {
				throw std::runtime_error("Attempting to move value when there is no value.");
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
		Result(std::string errorMsg, T value): value(value), errorMsg(std::move(errorMsg)) { }
	};
}

#endif //LANG_CMAKE_RESULT_H
