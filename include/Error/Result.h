//
// Created by User on 13/12/2024.
//

#ifndef LANG_CMAKE_RESULT_H
#define LANG_CMAKE_RESULT_H
#include <memory>
#include <type_traits>
#include <stack>
#include <utility>
#include <optional>
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
		std::stack<Trace> trace;

		template <typename U>
		struct is_unique_ptr : std::false_type {};

		template <typename U>
		struct is_unique_ptr<std::unique_ptr<U>> : std::true_type {};

	public:
		// Check if the value is null (only for pointer types)
		bool isNull() const {
			if constexpr (is_unique_ptr<T>::value)
				return value == nullptr;
			return false;
		}

		// Check if this Result contains an error
		bool hasError() const {
			return !errorMsg.empty();
		}

		// Check if this Result is valid (has value and no error)
		bool isValid() const {
			return !hasError() && !isNull();
		}

		// Get error message (safe)
		const std::string& getErrorMsg() const {
			return errorMsg;
		}

		// Get error note (safe)
		const std::string& getErrorNote() const {
			return errorNote;
		}

		// Get value reference (safe - returns optional)
		std::optional<std::reference_wrapper<T>> tryGetValue() {
			if (hasError() || isNull()) {
				return std::nullopt;
			}
			return std::ref(value);
		}

		// Get value reference (unsafe - for backward compatibility)
		T& getValue() {
			if (hasError()) {
				throw std::runtime_error("Attempting to access value when there is an error: " + errorMsg);
			}
			if (isNull()) {
				throw std::runtime_error("Attempting to access value when there is no value.");
			}
			return value;
		}

		// Move value (safe - returns optional)
		std::optional<T> tryMoveValue() {
			if (hasError() || isNull()) {
				return std::nullopt;
			}
			return std::move(value);
		}

		// Move value (unsafe - for backward compatibility)
		T&& moveValue() {
			if (hasError()) {
				throw std::runtime_error("Attempting to move value when there is an error: " + errorMsg);
			}
			if (isNull()) {
				throw std::runtime_error("Attempting to move value when there is no value.");
			}
			return std::move(value);
		}

		// Assignment operator
		Result& operator=(T newValue) {
			this->value = std::move(newValue);
			this->errorMsg.clear();
			this->errorNote.clear();
			// Clear trace on successful assignment
			while (!trace.empty()) {
				trace.pop();
			}
			return *this;
		}

		// Trace management
		void addToTrace(const Trace& newTrace) {
			trace.push(newTrace);
		}

		// Get copy of stack trace (safe)
		std::stack<Trace> getStackTrace() const {
			return trace;
		}

		// Get current trace (safe)
		std::optional<Trace> tryGetTrace() const {
			if (trace.empty()) {
				return std::nullopt;
			}
			return trace.top();
		}

		// Get current trace (unsafe - for backward compatibility)
		Trace getTrace() const {
			if (trace.empty()) {
				throw std::runtime_error("Attempting to access trace when trace stack is empty");
			}
			return trace.top();
		}

		// Pop trace (safe)
		bool popTrace() {
			if (trace.empty()) {
				return false;
			}
			trace.pop();
			return true;
		}

		// Check if trace is empty
		bool hasTrace() const {
			return !trace.empty();
		}

		// Constructors

		// Success constructor
		Result(T value) : value(std::move(value)), errorMsg(""), errorNote("") {}

		// Default constructor (for pointer types)
		Result() requires(is_unique_ptr<T>::value) : value(nullptr), errorMsg(""), errorNote("") {}

		// Default constructor (for non-pointer types) 
		Result() requires(!is_unique_ptr<T>::value) : value{}, errorMsg(""), errorNote("") {}

		// Error constructor with value
		Result(std::string errorMsg, std::string errorNote, T value) 
			: value(std::move(value)), errorMsg(std::move(errorMsg)), errorNote(std::move(errorNote)) {}

		// Error constructor with trace
		Result(std::string errorMsg, std::string errorNote, Trace newTrace) 
			: value{}, errorMsg(std::move(errorMsg)), errorNote(std::move(errorNote)) {
			trace.push(newTrace);
		}

		// Error propagation constructor (fixed)
		template<class U>
		Result(const Result<U>& res, const Trace& newTrace) 
			: value{}, errorMsg(res.getErrorMsg()), errorNote(res.getErrorNote()), trace(res.getStackTrace()) {
			trace.push(newTrace);
		}

		// Copy constructor
		Result(const Result& other) = default;

		// Move constructor
		Result(Result&& other) noexcept = default;

		// Copy assignment
		Result& operator=(const Result& other) = default;

		// Move assignment
		Result& operator=(Result&& other) noexcept = default;
	};
}

#endif //LANG_CMAKE_RESULT_H