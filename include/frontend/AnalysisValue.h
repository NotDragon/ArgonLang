//
// Created by User on 31/12/2024.
//

#ifndef ARGONLANG_ANALYSISVALUE_H
#define ARGONLANG_ANALYSISVALUE_H
#include <string>
#include <cstdint>

namespace ArgonLang {

	enum class [[nodiscard]] ValueType {
		Integral,
		Float,
		String,
		Char,
		Bool
	};

	template<typename T>
	class AnalysisValue {
	public:
		T value;

		virtual ValueType getValueType() = 0;
	};

	union Int {
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		__int128 i128;
	};

	union Float {
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		__int128 i128;
	};

	class StringValue : public AnalysisValue<std::string> {
	public:
		ValueType getValueType();
	};

	class CharValue : public AnalysisValue<char> {
	public:
		ValueType getValueType();
	};

	class IntegralValue : public AnalysisValue<Int> {
	public:
		ValueType getValueType();
	};

	class FloatValue : public AnalysisValue<Float> {
	public:
		ValueType getValueType();
	};

	class BooleanValue : public AnalysisValue<bool> {
	public:
		ValueType getValueType();
	};

}
#endif //ARGONLANG_ANALYSISVALUE_H
