//
// Created by User on 13/12/2024.
//

#ifndef LANG_CMAKE_RESULT_H
#define LANG_CMAKE_RESULT_H
#include <memory>
#include <optional>

namespace ArgonLang {

	template<typename T>
	class Result {
		std::optional<T> value;
		std::string errorMsg;
	public:
		bool hasValue();
		bool hasError();

		std::string getErrorMsg();

		T getvalue();

		explicit Result(T value);
		explicit Result(std::string errorMsg);
	};
}

#endif //LANG_CMAKE_RESULT_H
