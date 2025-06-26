//
// Created by User on 02/01/2025.
//
#include "TranslationUnit/TranslationUnitTokenizer.h"
#include <sstream>

using namespace ArgonLang::TranslationUnit;

const std::unordered_map<std::string, Token::Type> keywords = {
		{ "if", Token::KeywordIf },
		{ "else", Token::KeywordElse },
		{ "while", Token::KeywordWhile },
		{ "continue", Token::KeywordContinue },
		{ "break", Token::KeywordBreak },
		{ "for", Token::KeywordFor },
		{ "End", Token::KeywordEnd },

		{ "Token", Token::UnitToken },
		{ "BinaryExpression", Token::UnitBinaryExpression },
		{ "AdditiveBinaryExpression", Token::UnitAdditiveBinaryExpression },
		{ "MultiplicativeBinaryExpression", Token::UnitMultiplicativeBinaryExpression },
		{ "LogicalOrBinaryExpression", Token::UnitLogicalOrBinaryExpression },
		{ "LogicalAndBinaryExpression", Token::UnitLogicalAndBinaryExpression },
		{ "EqualityBinaryExpression", Token::UnitEqualityBinaryExpression },
		{ "RelationalBinaryExpression", Token::UnitRelationalBinaryExpression },
		{ "BitwiseBinaryExpression", Token::UnitBitwiseBinaryExpression },
		{ "AssignmentBinaryExpression", Token::UnitAssignmentBinaryExpression },

		{ "UnaryExpression", Token::UnitUnaryExpression },
		{ "ParallelUnaryExpression", Token::UnitParallelUnaryExpression },
		{ "DerefUnaryExpression", Token::UnitDerefUnaryExpression },
		{ "ReferenceUnaryExpression", Token::UnitReferenceUnaryExpression },
		{ "IncrementUnaryExpression", Token::UnitIncrementUnaryExpression },
		{ "DecrementUnaryExpression", Token::UnitDecrementUnaryExpression },
		{ "PlusUnaryExpression", Token::UnitPlusUnaryExpression },
		{ "MinusUnaryExpression", Token::UnitMinusUnaryExpression },
		{ "LogicalNotUnaryExpression", Token::UnitLogicalNotUnaryExpression },
		{ "BitwiseNotUnaryExpression", Token::UnitBitwiseNotUnaryExpression },

		{ "StructExpression", Token::UnitStructExpression },
		{ "MatchExpression", Token::UnitMatchExpression },
		{ "ToExpression", Token::UnitToExpression },
		{ "FilterExpression", Token::UnitFilterExpression },
		{ "MapExpression", Token::UnitMapExpression },
		{ "ReduceExpression", Token::UnitReduceExpression },
		{ "FunctionCallExpression", Token::UnitFunctionCallExpression },
		{ "RangeExpression", Token::UnitRangeExpression },
		{ "IndexExpression", Token::UnitIndexExpression },
		{ "MemberAccessExpression", Token::UnitMemberAccessExpression },

		{ "VariableDeclaration", Token::UnitVariableDeclaration },
		{ "FunctionDeclaration", Token::UnitFunctionDeclaration },
		{ "IfStatement", Token::UnitIfStatement },
		{ "ForStatement", Token::UnitForStatement },
		{ "WhileStatement", Token::UnitWhileStatement },
		{ "ReturnStatement", Token::UnitReturnStatement },
		{ "BreakStatement", Token::UnitBreakStatement },
		{ "ContinueStatement", Token::UnitContinueStatement },
		{ "TypeAlias", Token::UnitTypeAlias },
		{ "ClassDeclaration", Token::UnitClassDeclaration },
		{ "Block", Token::UnitBlock },
		{ "ImplStatement", Token::UnitImplStatement },
		{ "YieldStatement", Token::UnitYieldStatement },
		{ "ConstructorStatement", Token::UnitConstructorStatement },

		{ "Type", Token::UnitType },
		{ "IdentifierType", Token::UnitIdentifierType },
		{ "GenericType", Token::UnitGenericType },
		{ "SumType", Token::UnitSumType },
		{ "IntersectionType", Token::UnitIntersectionType },
		{ "PrefixedType", Token::UnitPrefixedType },

		{ "Import", Token::UnitImport },
};

std::vector<Token> ArgonLang::TranslationUnit::tokenizeTranslationUnit(const std::string& input) {
	std::vector<Token> tokens;
	size_t length = input.size();
	size_t i = 0;

	size_t currentLine = 1;
	size_t currentColumn = 1;

	while(i < length) {
		char c = input[i];
		if(c == '\n') {
			currentLine++;
			currentColumn = 1;
			i++;

			while(input[i] == ' ' || input[i] == '\t') {
				i++;
			}

			continue;
		}

		if(c == '\t') {
			currentColumn+=4;
			i++;
			continue;
		}

		if (std::isspace(c)) {
			currentColumn++;
			i++;
			continue;
		}

		if (c == '`') {
			size_t start = i;
			i++;
			std::string literal;
			while (i < length && input[i] != '`') {
				if (input[i] == '\\') {
					i++;
					if (i >= length) throw std::runtime_error("Unterminated escape sequence in literal" + std::to_string(currentLine) + ", column " + std::to_string(currentColumn));

					switch (input[i]) {
						case 'n': literal += '\n'; break;
						case 't': literal += '\t'; break;
						case '\\': literal += '\\'; break;
						case 'r': literal += '\r'; break;
						case '0': literal += '\0'; break;
						case '`': literal += '`'; break;
						default: literal += input[i]; break;
					}
				} else {
					literal += input[i];
				}
				i++;
			}
			if (i >= length) {
				throw std::runtime_error("Unterminated literal" + std::to_string(currentLine) + ", column " + std::to_string(currentColumn));
			}
			i++;

			tokens.emplace_back(Token::Literal, literal, currentLine, currentColumn);

			currentColumn += (i - start);
			continue;
		}  else if (c == '=' && input[i + 1] == '=') {
			tokens.emplace_back(Token::Equal, "==", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '!' && input[i + 1] == '=') {
			tokens.emplace_back(Token::NotEqual, "!=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '>' && input[i + 1] == '=') {
			tokens.emplace_back(Token::GreaterEqual, ">=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '<' && input[i + 1] == '=') {
			tokens.emplace_back(Token::LessEqual, "<=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '&' && input[i + 1] == '&') {
			tokens.emplace_back(Token::LogicalAnd, "&&", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '|' && input[i + 1] == '|') {
			tokens.emplace_back(Token::LogicalOr, "||", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		}  else if (c == '-' && input[i + 1] == '>') {
			tokens.emplace_back(Token::Arrow, "->", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		}  else if (c == '=' && input[i + 1] == '>') {
			tokens.emplace_back(Token::MatchArrow, "=>", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else {
			switch (c) {
				case '<': tokens.emplace_back(Token::Less, "<", currentLine, currentColumn); break;
				case '>': tokens.emplace_back(Token::Greater, ">", currentLine, currentColumn); break;
				case '!': tokens.emplace_back(Token::LogicalNot, "!", currentLine, currentColumn); break;
				case '(': tokens.emplace_back(Token::LeftParen, "(", currentLine, currentColumn); break;
				case ')': tokens.emplace_back(Token::RightParen, ")", currentLine, currentColumn); break;
				case '{': tokens.emplace_back(Token::LeftBrace, "{", currentLine, currentColumn); break;
				case '}': tokens.emplace_back(Token::RightBrace, "}", currentLine, currentColumn); break;
				case ',': tokens.emplace_back(Token::Comma, ",", currentLine, currentColumn); break;
				case '.': tokens.emplace_back(Token::Dot, ".", currentLine, currentColumn); break;
				case '$': tokens.emplace_back(Token::Dollar, "$", currentLine, currentColumn); break;
				default: throw std::runtime_error(std::string("Unexpected character: ") + c);
			}
			i++;
			currentColumn++;
		}
	}

	tokens.emplace_back(Token::End, "END", currentLine, currentColumn);

	return tokens;
}

std::string ArgonLang::TranslationUnit::Token::getTypeAsString(Token::Type type) {
	switch (type) {
		case Token::Identifier: return "Identifier";
		case Token::UnitToken: return "UnitToken";

		case Token::UnitBinaryExpression: return "UnitBinaryExpression";
		case Token::UnitAdditiveBinaryExpression: return "UnitAdditiveBinaryExpression";
		case Token::UnitMultiplicativeBinaryExpression: return "UnitMultiplicativeBinaryExpression";
		case Token::UnitLogicalOrBinaryExpression: return "UnitLogicalOrBinaryExpression";
		case Token::UnitLogicalAndBinaryExpression: return "UnitLogicalAndBinaryExpression";
		case Token::UnitEqualityBinaryExpression: return "UnitEqualityBinaryExpression";
		case Token::UnitRelationalBinaryExpression: return "UnitRelationalBinaryExpression";
		case Token::UnitBitwiseBinaryExpression: return "UnitBitwiseBinaryExpression";
		case Token::UnitAssignmentBinaryExpression: return "UnitAssignmentBinaryExpression";

		case Token::UnitUnaryExpression: return "UnitUnaryExpression";
		case Token::UnitParallelUnaryExpression: return "UnitParallelUnaryExpression";
		case Token::UnitDerefUnaryExpression: return "UnitDerefUnaryExpression";
		case Token::UnitReferenceUnaryExpression: return "UnitReferenceUnaryExpression";
		case Token::UnitIncrementUnaryExpression: return "UnitIncrementUnaryExpression";
		case Token::UnitDecrementUnaryExpression: return "UnitDecrementUnaryExpression";
		case Token::UnitPlusUnaryExpression: return "UnitPlusUnaryExpression";
		case Token::UnitMinusUnaryExpression: return "UnitMinusUnaryExpression";
		case Token::UnitLogicalNotUnaryExpression: return "UnitLogicalNotUnaryExpression";
		case Token::UnitBitwiseNotUnaryExpression: return "UnitBitwiseNotUnaryExpression";

		case Token::UnitStructExpression: return "UnitStructExpression";
		case Token::UnitMatchExpression: return "UnitMatchExpression";
		case Token::UnitToExpression: return "UnitToExpression";
		case Token::UnitFilterExpression: return "UnitFilterExpression";
		case Token::UnitMapExpression: return "UnitMapExpression";
		case Token::UnitReduceExpression: return "UnitReduceExpression";
		case Token::UnitFunctionCallExpression: return "UnitFunctionCallExpression";
		case Token::UnitRangeExpression: return "UnitRangeExpression";
		case Token::UnitIndexExpression: return "UnitIndexExpression";
		case Token::UnitMemberAccessExpression: return "UnitMemberAccessExpression";

		case Token::UnitVariableDeclaration: return "UnitVariableDeclaration";
		case Token::UnitFunctionDeclaration: return "UnitFunctionDeclaration";
		case Token::UnitIfStatement: return "UnitIfStatement";
		case Token::UnitForStatement: return "UnitForStatement";
		case Token::UnitWhileStatement: return "UnitWhileStatement";
		case Token::UnitReturnStatement: return "UnitReturnStatement";
		case Token::UnitBreakStatement: return "UnitBreakStatement";
		case Token::UnitContinueStatement: return "UnitContinueStatement";
		case Token::UnitTypeAlias: return "UnitTypeAlias";
		case Token::UnitClassDeclaration: return "UnitClassDeclaration";
		case Token::UnitBlock: return "UnitBlock";
		case Token::UnitImplStatement: return "UnitImplStatement";
		case Token::UnitYieldStatement: return "UnitYieldStatement";
		case Token::UnitConstructorStatement: return "UnitConstructorStatement";

		case Token::UnitType: return "UnitType";
		case Token::UnitIdentifierType: return "UnitIdentifierType";
		case Token::UnitGenericType: return "UnitGenericType";
		case Token::UnitSumType: return "UnitSumType";
		case Token::UnitIntersectionType: return "UnitIntersectionType";
		case Token::UnitPrefixedType: return "UnitPrefixedType";

		case Token::UnitImport: return "UnitImport";

		case Token::KeywordIf: return "KeywordIf";
		case Token::KeywordElse: return "KeywordElse";
		case Token::KeywordBreak: return "KeywordBreak";
		case Token::KeywordContinue: return "KeywordContinue";
		case Token::KeywordWhile: return "KeywordWhile";
		case Token::KeywordFor: return "KeywordFor";
		case Token::KeywordEnd: return "KeywordEnd";

		case Token::Literal: return "Literal";

		case Token::Equal: return "Equal";
		case Token::NotEqual: return "NotEqual";
		case Token::Less: return "Less";
		case Token::Greater: return "Greater";
		case Token::LessEqual: return "LessEqual";
		case Token::GreaterEqual: return "GreaterEqual";
		case Token::LogicalAnd: return "LogicalAnd";
		case Token::LogicalOr: return "LogicalOr";
		case Token::LogicalNot: return "LogicalNot";

		case Token::LeftParen: return "LeftParen";
		case Token::RightParen: return "RightParen";
		case Token::LeftBrace: return "LeftBrace";
		case Token::RightBrace: return "RightBrace";
		case Token::Comma: return "Comma";
		case Token::Dot: return "Dot";
		case Token::Arrow: return "Arrow";
		case Token::MatchArrow: return "MatchArrow";

		case Token::Dollar: return "Dollar";

		case Token::End: return "End";
	}
}