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
		bool hasValue() const;
		bool hasError() const;

		std::string getErrorMsg() const;
		T& getValue() const;
		T&& moveValue();

		Result& operator=(T newValue);

		Result(T value);
		Result();
//		Result(std::unique_ptr<T> value);
		Result(std::string errorMsg);
	};


	template<typename T>
	bool ArgonLang::Result<T>::hasValue() const {
		return value != nullptr;
	}

	template<typename T>
	bool ArgonLang::Result<T>::hasError() const {
		return !errorMsg.empty();
	}

	template<typename T>
	std::string ArgonLang::Result<T>::getErrorMsg() const {
		return errorMsg;
	}


	template<typename T>
	T& ArgonLang::Result<T>::getValue() const {
		if (!hasValue()) {
			throw std::runtime_error("Attempting to access value when there is no value.");
		}
		return value;
	}

	template<typename T>
	T&& Result<T>::moveValue() {
		if(hasError()) {
			throw std::runtime_error("Attempting to extract value when there is an error: " + errorMsg);
		}
		if (!hasValue()) {
			throw std::runtime_error("Attempting to extract value when there is no value.");
		}
		return std::move(value);
	}

	template<typename T>
	ArgonLang::Result<T>::Result(T value): value(std::move(value)), errorMsg("") { }

	template<typename T>
	ArgonLang::Result<T>::Result(std::string errorMsg): value(nullptr), errorMsg(std::move(errorMsg)) { }

	template<typename T>
	ArgonLang::Result<T>::Result(): value(nullptr), errorMsg("") { }

//template<typename T>
//ArgonLang::Result<T>::Result(std::unique_ptr<T> value): value(std::move(value)), errorMsg("") { }

	template<typename T>
	ArgonLang::Result<T>& ArgonLang::Result<T>::operator=(T newValue) {
		this->value = std::move(newValue);
		return *this;
	}
}

#endif //LANG_CMAKE_RESULT_H
