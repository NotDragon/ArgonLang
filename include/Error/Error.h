#ifndef LANG_CMAKE_ERROR_H
#define LANG_CMAKE_ERROR_H
#include <string>

namespace ArgonLang {
	class Error {
	public:
		std::string errorMsg;
		enum Type {
			UNEXPECTED_TOKEN = 0
		} errorType;

		Error(std::string errorMsg, Type type);
	};
}

#endif //LANG_CMAKE_ERROR_H
