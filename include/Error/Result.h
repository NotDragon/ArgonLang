//
// Created by User on 13/12/2024.
//

#ifndef LANG_CMAKE_RESULT_H
#define LANG_CMAKE_RESULT_H
#include <memory>
#include <type_traits>
#include <stack>
#include <utility>
#include "backend/AST.h"
#include "backend/Tokenizer.h"

namespace ArgonLang {
	struct Trace {
		ASTNodeType type;
		Token::Position position;
		Trace();
		Trace(ASTNodeType type, Token::Position position);
	};

	template<typename T>
	class Result {
		T value;
		std::string errorMsg;
		std::string errorNote;

		template <typename U>
		struct is_unique_ptr : std::false_type {};

		template <typename U>
		struct is_unique_ptr<std::unique_ptr<U>> : std::true_type {};
	protected:
		std::stack<Trace> trace;
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

		std::string getErrorNote() const {
			return errorNote;
		}

		T& getValue() {
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

		void addToTrace(const Trace& trace1) {
			trace.push(trace1);
		}

		std::stack<Trace> getStackTrace() {
			return trace;
		}

		Trace getTrace() {
			return trace.top();
		}

		void popTrace() {
			trace.pop();
		}

		Result(T value): value(std::move(value)), errorMsg("") { }
		Result(): value(nullptr), errorMsg("") { }
		Result(std::string errorMsg, std::string errorNote, T value): value(value), errorMsg(std::move(errorMsg)), errorNote(std::move(errorNote)) { }

//		Result(std::string errorMsg): value(nullptr), errorMsg(std::move(errorMsg)) { }
//
//		Result(std::string errorMsg, std::stack<Trace> trace): value(nullptr), errorMsg(std::move(errorMsg)), trace(std::move(trace)) {}

//		Result(std::string errorMsg, std::stack<Trace> oldTrace, Trace newTrace): value(nullptr), errorMsg(std::move(errorMsg)), trace(std::move(oldTrace)) {
//			trace.push(newTrace);
//		}

		Result(std::string errorMsg, std::string errorNote, Trace newTrace): value(nullptr), errorMsg(std::move(errorMsg)), errorNote(std::move(errorNote)) {
			trace.push(newTrace);
		}

		template<class U>
		Result(Result<U> res, const Trace& newTrace): value(nullptr), errorMsg(std::move(res.getErrorMsg())), errorNote(std::move(res.getErrorNote())) {
			trace = res.getStackTrace();
			trace.push(newTrace);
		}
	};
}

#endif //LANG_CMAKE_RESULT_H
