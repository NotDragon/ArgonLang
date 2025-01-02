//
// Created by User on 02/01/2025.
//

#ifndef ARGONLANG_TRANSLATIONUNITTOKENIZER_H
#define ARGONLANG_TRANSLATIONUNITTOKENIZER_H
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ArgonLang {
	namespace TranslationUnit {
		class Token {
		public:
			enum Type {
				Identifier = 0,

				UnitToken,

				UnitBinaryExpression,
				UnitAdditiveBinaryExpression,
				UnitMultiplicativeBinaryExpression,
				UnitLogicalOrBinaryExpression,
				UnitLogicalAndBinaryExpression,
				UnitEqualityBinaryExpression,
				UnitRelationalBinaryExpression,
				UnitBitwiseBinaryExpression,
				UnitAssignmentBinaryExpression,

				UnitUnaryExpression,
				UnitParallelUnaryExpression,
				UnitDerefUnaryExpression,
				UnitReferenceUnaryExpression,
				UnitIncrementUnaryExpression,
				UnitDecrementUnaryExpression,
				UnitPlusUnaryExpression,
				UnitMinusUnaryExpression,
				UnitLogicalNotUnaryExpression,
				UnitBitwiseNotUnaryExpression,

				UnitStructExpression,
				UnitMatchExpression,
				UnitToExpression,
				UnitFilterExpression,
				UnitMapExpression,
				UnitReduceExpression,
				UnitFunctionCallExpression,
				UnitRangeExpression,
				UnitIndexExpression,
				UnitMemberAccessExpression,

				UnitVariableDeclaration,
				UnitFunctionDeclaration,
				UnitIfStatement,
				UnitForStatement,
				UnitWhileStatement,
				UnitReturnStatement,
				UnitBreakStatement,
				UnitContinueStatement,
				UnitTypeAlias,
				UnitClassDeclaration,
				UnitBlock,
				UnitImplStatement,
				UnitYieldStatement,
				UnitConstructorStatement,

				UnitType,
				UnitIdentifierType,
				UnitGenericType,
				UnitSumType,
				UnitIntersectionType,
				UnitPrefixedType,

				UnitImport,

				KeywordIf,
				KeywordElse,
				KeywordBreak,
				KeywordContinue,
				KeywordWhile,
				KeywordFor,

				Literal,

				Equal,
				NotEqual,
				Less,
				Greater,
				LessEqual,
				GreaterEqual,
				LogicalAnd,
				LogicalOr,
				LogicalNot,

				LeftParen,
				RightParen,
				LeftBrace,
				RightBrace,
				Comma,
				Dot,
				Arrow,
				MatchArrow,

				Dollar,

				End
			} type;
			std::string value;

			struct Position {
				size_t line;
				size_t column;
			} position;

			explicit Token(Type t, std::string val, size_t line, size_t column);
			explicit Token() = default;

			static std::string getTypeAsString(Token::Type type);
		};
		std::vector<Token> tokenizeTranslationUnit(const std::string& input);

		extern const std::unordered_map<std::string, Token::Type> keywords;
		std::unordered_map<std::string, const std::function<std::string(int)>&> unit;
	}
}

#endif //ARGONLANG_TRANSLATIONUNITTOKENIZER_H
