#include "backend/Tokenizer.h"

#include "Error/ErrorFormatter.h"

#include <sstream>

namespace ArgonLang {
const std::unordered_map<std::string, Token::Type> keywords = {
    {"def", Token::KeywordDef},
    {"const", Token::KeywordDef},
    {"mut", Token::KeywordMut},

    {"i8", Token::PrimitiveType},
    {"i16", Token::PrimitiveType},
    {"i32", Token::PrimitiveType},
    {"i64", Token::PrimitiveType},
    {"i128", Token::PrimitiveType},
    {"u8", Token::PrimitiveType},
    {"u16", Token::PrimitiveType},
    {"u32", Token::PrimitiveType},
    {"u64", Token::PrimitiveType},
    {"u128", Token::PrimitiveType},
    {"f16", Token::PrimitiveType},
    {"f32", Token::PrimitiveType},
    {"f64", Token::PrimitiveType},
    {"f128", Token::PrimitiveType},
    {"bool", Token::PrimitiveType},
    {"str", Token::PrimitiveType},
    {"chr", Token::PrimitiveType},

    {"to", Token::KeywordTo},
    {"yield", Token::KeywordYield},

    {"if", Token::KeywordIf},
    {"else", Token::KeywordElse},
    {"while", Token::KeywordWhile},
    {"dowhile", Token::KeywordWhile},
    {"continue", Token::KeywordContinue},
    {"break", Token::KeywordBreak},
    {"for", Token::KeywordFor},

    {"func", Token::KeywordFunc},
    {"class", Token::KeywordClass},
    {"return", Token::KeywordReturn},
    {"impl", Token::KeywordImpl},
    {"struct", Token::KeywordStruct},
    {"pub", Token::KeywordPub},
    {"pri", Token::KeywordPri},
    {"pro", Token::KeywordPro},
    {"constructor", Token::KeywordConstructor},

    {"using", Token::KeywordUsing},
    {"union", Token::KeywordUnion},

    {"enum", Token::KeywordEnum},
    {"typeconst", Token::KeywordTypeconst},
    {"super", Token::KeywordSuper},
    {"inter", Token::KeywordInter},
    {"is", Token::KeywordIs},
    {"throw", Token::KeywordThrow},
    {"throws", Token::KeywordThrows},
    {"try", Token::KeywordTry},
    {"catch", Token::KeywordCatch},

    {"true", Token::BooleanLiteral},
    {"false", Token::BooleanLiteral},
    {"vec", Token::Identifier},
    {"list", Token::Identifier},
    {"range", Token::Identifier},
    {"ref", Token::Identifier},
    {"weak", Token::Identifier},
    {"null", Token::Identifier},
    {"await", Token::KeywordAwait},
    {"module", Token::KeywordModule},
    {"import", Token::KeywordImport},
    {"where", Token::KeywordWhere},
    {"constraint", Token::KeywordConstraint},

    {"par", Token::KeywordPar},

};
}

ArgonLang::TokenizeResult ArgonLang::tokenize(const std::string& input) {
	std::vector<Token> tokens;
	size_t length = input.size();
	size_t i = 0;

	size_t currentLine = 1;
	size_t currentColumn = 1;

	while (i < length) {
		char c = input[i];
		if (c == '\n') {
			currentLine++;
			currentColumn = 1;
			i++;

			while (input[i] == ' ' || input[i] == '\t') {
				i++;
			}

			continue;
		}

		if (c == '\t') {
			currentColumn += 4;
			i++;
			continue;
		}

		if (std::isspace(c)) {
			currentColumn++;
			i++;
			continue;
		}

		if (c == '\"' || c == '\'') {
			char quoteType = c;
			size_t start = i;
			i++;
			std::string stringLiteral;
			while (i < length && input[i] != quoteType) {
				if (input[i] == '\\') {
					i++;
					if (i >= length) {
						return TokenizeResult(ErrorFormatter::formatTokenizerError(
						                          "Unterminated escape sequence in string", currentLine, currentColumn),
						                      {currentLine, currentColumn});
					}
					switch (input[i]) {
					case 'n':
						stringLiteral += '\n';
						break;
					case 't':
						stringLiteral += '\t';
						break;
					case '\\':
						stringLiteral += '\\';
						break;
					case '\"':
						stringLiteral += '\"';
						break;
					case '\'':
						stringLiteral += '\'';
						break;
					case 'r':
						stringLiteral += '\r';
						break;
					case '0':
						stringLiteral += '\0';
						break;
					default:
						stringLiteral += input[i];
						break;
					}
				} else {
					stringLiteral += input[i];
				}
				i++;
			}
			if (i >= length || input[i] != quoteType) {
				return TokenizeResult(
				    ErrorFormatter::formatTokenizerError("Unterminated string literal", currentLine, currentColumn),
				    {currentLine, currentColumn});
			}
			i++;

			if (quoteType == '\"')
				tokens.emplace_back(Token::StringLiteral, stringLiteral, currentLine, currentColumn);
			else if (stringLiteral.size() != 1) {
				return TokenizeResult(ErrorFormatter::formatTokenizerError("Multiple characters in char literal",
				                                                           currentLine, currentColumn),
				                      {currentLine, currentColumn});
			} else
				tokens.emplace_back(Token::CharLiteral, stringLiteral, currentLine, currentColumn);

			currentColumn += (i - start);
			continue;
		}

		if (std::isdigit(c)) {
			size_t start = i;
			bool isDecimal = false;
			bool isHex = false;
			bool isBinary = false;
			bool isOctal = false;
			
			// Check for hex (0x), binary (0b), octal (0o) prefixes
			if (c == '0' && i + 1 < length) {
				if (input[i + 1] == 'x' || input[i + 1] == 'X') {
					isHex = true;
					i += 2; // Skip '0x'
				} else if (input[i + 1] == 'b' || input[i + 1] == 'B') {
					isBinary = true;
					i += 2; // Skip '0b'
				} else if (input[i + 1] == 'o' || input[i + 1] == 'O') {
					isOctal = true;
					i += 2; // Skip '0o'
				}
			}
			
			// Parse the number based on its format
			if (isHex) {
				while (i < length && (std::isxdigit(input[i]) || input[i] == '`')) {
					i++;
				}
			} else if (isBinary) {
				while (i < length && (input[i] == '0' || input[i] == '1' || input[i] == '`')) {
					i++;
				}
			} else if (isOctal) {
				while (i < length && (input[i] >= '0' && input[i] <= '7' || input[i] == '`')) {
					i++;
				}
			} else {
				// Decimal or float
				while (i < length && (std::isdigit(input[i]) || input[i] == '.' || input[i] == '`' || input[i] == 'e' || input[i] == 'E')) {
					if (input[i] == '.') {
						if (isDecimal) {
							return TokenizeResult(
							    ErrorFormatter::formatTokenizerError("Invalid numeric literal: multiple decimal points",
							                                         currentLine, currentColumn),
							                      {currentLine, currentColumn});
						}
						isDecimal = true;
					} else if (input[i] == 'e' || input[i] == 'E') {
						// Scientific notation
						isDecimal = true;
						i++;
						if (i < length && (input[i] == '+' || input[i] == '-')) {
							i++; // Skip sign
						}
						continue;
					}
					i++;
				}
			}

			// Check for type suffixes
			size_t numEnd = i;
			std::string suffix;
			if (!isDecimal) {
				// Integer type suffixes: i128, u128, i64, u64, i32, u32, i16, u16, i8, u8
				if (i + 4 <= length) {
					std::string possibleSuffix = input.substr(i, 4);
					if (possibleSuffix == "i128" || possibleSuffix == "u128") {
						suffix = possibleSuffix;
						i += 4;
					}
				}
				if (suffix.empty() && i + 3 <= length) {
					std::string possibleSuffix = input.substr(i, 3);
					if (possibleSuffix == "i64" || possibleSuffix == "u64" || 
					    possibleSuffix == "i32" || possibleSuffix == "u32" ||
					    possibleSuffix == "i16" || possibleSuffix == "u16") {
						suffix = possibleSuffix;
						i += 3;
					}
				}
				if (suffix.empty() && i + 2 <= length) {
					std::string possibleSuffix = input.substr(i, 2);
					if (possibleSuffix == "i8" || possibleSuffix == "u8") {
						suffix = possibleSuffix;
						i += 2;
					}
				}
			} else {
				// Float type suffixes: f128, f64, f32, f16
				if (i + 4 <= length) {
					std::string possibleSuffix = input.substr(i, 4);
					if (possibleSuffix == "f128") {
						suffix = possibleSuffix;
						i += 4;
					}
				}
				if (suffix.empty() && i + 3 <= length) {
					std::string possibleSuffix = input.substr(i, 3);
					if (possibleSuffix == "f64" || possibleSuffix == "f32" || possibleSuffix == "f16") {
						suffix = possibleSuffix;
						i += 3;
					}
				}
			}

			std::string numLiteral = input.substr(start, i - start);

			std::erase(numLiteral, '`');

			if (isDecimal) {
				tokens.emplace_back(Token::FloatLiteral, numLiteral, currentLine, currentColumn);
			} else {
				tokens.emplace_back(Token::IntegralLiteral, numLiteral, currentLine, currentColumn);
			}

			currentColumn += (i - start);
			continue;
		}
		if (std::isalpha(c) || c == '_') {
			size_t start = i;
			while (i < length && (std::isalnum(input[i]) || input[i] == '_')) {
				i++;
				currentColumn++;
			}
			std::string identifier = input.substr(start, i - start);
			auto it = keywords.find(identifier);
			if (it != keywords.end()) {
				tokens.emplace_back(it->second, it->first, currentLine, currentColumn);
				continue;
			}
			tokens.emplace_back(Token::Identifier, identifier, currentLine, currentColumn);
			continue;
		} else if (c == '/' && input[i + 1] == '/') {
			// Skip single-line comments
			while (i < length && input[i] != '\n') {
				i++;
				currentColumn++;
			}
			continue;
		} else if (c == '/' && input[i + 1] == '*') {
			// Skip multi-line comments
			i += 2;
			currentColumn += 2;
			while (i < length - 1) {
				if (input[i] == '*' && input[i + 1] == '/') {
					i += 2;
					currentColumn += 2;
					break;
				}
				if (input[i] == '\n') {
					currentLine++;
					currentColumn = 1;
				} else {
					currentColumn++;
				}
				i++;
			}
			continue;
		} else if (c == '=' && input[i + 1] == '=') {
			tokens.emplace_back(Token::Equal, "==", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '!' && input[i + 1] == '=') {
			tokens.emplace_back(Token::NotEqual, "!=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '+' && input[i + 1] == '+') {
			tokens.emplace_back(Token::Increment, "++", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '-' && input[i + 1] == '-') {
			tokens.emplace_back(Token::Decrement, "--", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '+' && input[i + 1] == '=') {
			tokens.emplace_back(Token::PlusAssign, "+=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '-' && input[i + 1] == '=') {
			tokens.emplace_back(Token::MinusAssign, "-=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '=') {
			tokens.emplace_back(Token::MultiplyAssign, "*=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '/' && input[i + 1] == '=') {
			tokens.emplace_back(Token::DivideAssign, "/=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '%' && input[i + 1] == '=') {
			tokens.emplace_back(Token::ModuloAssign, "%=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '&' && input[i + 1] == '&') {
			tokens.emplace_back(Token::LogicalAnd, "&&", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '|' && input[i + 1] == '|') {
			if (input[i + 2] == '>') {
				if (input[i + 3] == '=') {
					tokens.emplace_back(Token::MapPipeAssign, "||>=", currentLine, currentColumn);
					i += 4;
					currentColumn += 4;
					continue;
				}
				tokens.emplace_back(Token::MapPipe, "||>", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}
			tokens.emplace_back(Token::LogicalOr, "||", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '<') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::LeftShiftAssign, "*<=", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}

			tokens.emplace_back(Token::LeftShift, "*<", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '>') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::RightShiftAssign, "*>=", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}

			tokens.emplace_back(Token::RightShift, "*>", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '<' && input[i + 1] == '=') {
			tokens.emplace_back(Token::LessEqual, "<=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '>' && input[i + 1] == '=') {
			tokens.emplace_back(Token::GreaterEqual, ">=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '&' && input[i + 1] == '=') {
			tokens.emplace_back(Token::MapAssign, "&=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '?' && input[i + 1] == '?') {
			tokens.emplace_back(Token::DoubleQuestionMark, "??", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '?' && input[i + 1] == '=') {
			tokens.emplace_back(Token::ReduceAssign, "?=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '|' && input[i + 1] == '=') {
			tokens.emplace_back(Token::FilterAssign, "|=", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '|' && input[i + 1] == '|' && input[i + 2] == '>') {
			if (input[i + 3] == '=') {
				tokens.emplace_back(Token::MapPipeAssign, "||>=", currentLine, currentColumn);
				i += 4;
				currentColumn += 4;
				continue;
			}
			tokens.emplace_back(Token::MapPipe, "||>", currentLine, currentColumn);
			i += 3;
			currentColumn += 3;
		} else if (c == '|' && input[i + 1] == '>') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::PipeAssign, "|>=", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}
			tokens.emplace_back(Token::Pipe, "|>", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '=' && input[i + 1] == '>') {
			tokens.emplace_back(Token::MatchArrow, "=>", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '-' && input[i + 1] == '>') {
			tokens.emplace_back(Token::Arrow, "->", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == ':' && input[i + 1] == ':') {
			tokens.emplace_back(Token::DoubleColon, "::", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '#' && input[i + 1] == '#') {
			tokens.emplace_back(Token::DoubleHash, "##", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '&') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::BitwiseAndAssign, "*&=", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}
			tokens.emplace_back(Token::BitwiseAnd, "*&", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '|') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::BitwiseOrAssign, "*|=", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}
			tokens.emplace_back(Token::BitwiseOr, "*|", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '^') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::BitwiseXorAssign, "*^=", currentLine, currentColumn);
				i += 3;
				currentColumn += 3;
				continue;
			}
			tokens.emplace_back(Token::BitwiseXor, "*^", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else if (c == '*' && input[i + 1] == '~') {
			tokens.emplace_back(Token::BitwiseNot, "*~", currentLine, currentColumn);
			i += 2;
			currentColumn += 2;
		} else {
			switch (c) {
			case '+':
				tokens.emplace_back(Token::Plus, "+", currentLine, currentColumn);
				break;
			case '-':
				tokens.emplace_back(Token::Minus, "-", currentLine, currentColumn);
				break;
			case '*':
				tokens.emplace_back(Token::Multiply, "*", currentLine, currentColumn);
				break;
			case '/':
				tokens.emplace_back(Token::Divide, "/", currentLine, currentColumn);
				break;
			case '%':
				tokens.emplace_back(Token::Modulo, "%", currentLine, currentColumn);
				break;
			case '=':
				tokens.emplace_back(Token::Assign, "=", currentLine, currentColumn);
				break;
			case '<':
				tokens.emplace_back(Token::Less, "<", currentLine, currentColumn);
				break;
			case '>':
				tokens.emplace_back(Token::Greater, ">", currentLine, currentColumn);
				break;
			case '!':
				tokens.emplace_back(Token::LogicalNot, "!", currentLine, currentColumn);
				break;
			case '&':
				tokens.emplace_back(Token::MapRange, "&", currentLine, currentColumn);
				break;
			case '|':
				tokens.emplace_back(Token::FilterRange, "|", currentLine, currentColumn);
				break;
			case '~':
				tokens.emplace_back(Token::Ownership, "~", currentLine, currentColumn);
				break;
			case '(':
				tokens.emplace_back(Token::LeftParen, "(", currentLine, currentColumn);
				break;
			case ')':
				tokens.emplace_back(Token::RightParen, ")", currentLine, currentColumn);
				break;
			case '{':
				tokens.emplace_back(Token::LeftBrace, "{", currentLine, currentColumn);
				break;
			case '}':
				tokens.emplace_back(Token::RightBrace, "}", currentLine, currentColumn);
				break;
			case '[':
				tokens.emplace_back(Token::LeftBracket, "[", currentLine, currentColumn);
				break;
			case ']':
				tokens.emplace_back(Token::RightBracket, "]", currentLine, currentColumn);
				break;
			case ';':
				tokens.emplace_back(Token::Semicolon, ";", currentLine, currentColumn);
				break;
			case ':':
				tokens.emplace_back(Token::Colon, ":", currentLine, currentColumn);
				break;
			case ',':
				tokens.emplace_back(Token::Comma, ",", currentLine, currentColumn);
				break;
			case '.':
				// Check for ellipsis (...)
				if (i + 2 < input.size() && input[i + 1] == '.' && input[i + 2] == '.') {
					tokens.emplace_back(Token::Ellipsis, "...", currentLine, currentColumn);
					i += 2; // Skip the next two dots
					currentColumn += 2;
				} else {
					tokens.emplace_back(Token::Dot, ".", currentLine, currentColumn);
				}
				break;
			case '?':
				tokens.emplace_back(Token::ReduceRange, "?", currentLine, currentColumn);
				break;
			case '#':
				tokens.emplace_back(Token::Hash, "#", currentLine, currentColumn);
				break;
			case '$':
				tokens.emplace_back(Token::Dollar, "$", currentLine, currentColumn);
				break;
			default:
				return TokenizeResult(ErrorFormatter::formatTokenizerError("Unexpected character: " + std::string(1, c),
				                                                           currentLine, currentColumn),
				                      {currentLine, currentColumn});
			}
			i++;
			currentColumn++;
		}
	}

	tokens.emplace_back(Token::End, "END", currentLine, currentColumn);
	return TokenizeResult(std::move(tokens));
}

std::string ArgonLang::Token::getTypeAsString(Token::Type type) {
	switch (type) {
	case Token::Identifier:
		return "Identifier";
	case Token::KeywordDef:
		return "KeywordDef";
	case Token::KeywordIf:
		return "KeywordIf";
	case Token::KeywordElse:
		return "KeywordElse";
	case Token::KeywordBreak:
		return "KeywordBreak";
	case Token::KeywordContinue:
		return "KeywordContinue";
	case Token::KeywordWhile:
		return "KeywordWhile";
	case Token::KeywordFor:
		return "KeywordFor";
	case Token::KeywordReturn:
		return "KeywordReturn";
	case Token::KeywordFunc:
		return "KeywordFunc";
	case Token::KeywordClass:
		return "KeywordClass";
	case Token::KeywordMut:
		return "KeywordMut";
	case Token::KeywordTo:
		return "KeywordTo";
	case Token::KeywordPar:
		return "KeywordPar";
	case Token::KeywordImpl:
		return "KeywordImpl";
	case Token::KeywordUsing:
		return "KeywordUsing";
	case Token::KeywordUnion:
		return "KeywordUnion";
	case Token::KeywordYield:
		return "KeywordYield";
	case Token::KeywordStruct:
		return "KeywordStruct";
	case Token::KeywordConstructor:
		return "KeywordConstructor";
	case Token::KeywordEnum:
		return "KeywordEnum";
	case Token::KeywordTypeconst:
		return "KeywordTypeconst";
	case Token::KeywordInter:
		return "KeywordInter";
	case Token::KeywordIs:
		return "KeywordIs";
	case Token::KeywordThrow:
		return "KeywordThrow";
	case Token::KeywordThrows:
		return "KeywordThrows";
	case Token::KeywordTry:
		return "KeywordTry";
	case Token::KeywordCatch:
		return "KeywordCatch";
	case Token::KeywordAwait:
		return "KeywordAwait";
	case Token::KeywordModule:
		return "KeywordModule";
	case Token::KeywordImport:
		return "KeywordImport";
	case Token::KeywordWhere:
		return "KeywordWhere";

	case Token::IntegralLiteral:
		return "IntegralLiteral";
	case Token::FloatLiteral:
		return "FloatLiteral";
	case Token::StringLiteral:
		return "StringLiteral";
	case Token::BooleanLiteral:
		return "BooleanLiteral";

	case Token::PrimitiveType:
		return "PrimitiveType";

	case Token::Plus:
		return "Plus";
	case Token::Minus:
		return "Minus";
	case Token::Multiply:
		return "Multiply";
	case Token::Divide:
		return "Divide";
	case Token::Modulo:
		return "Modulo";
	case Token::Increment:
		return "Increment";
	case Token::Decrement:
		return "Decrement";
	case Token::Assign:
		return "Assign";
	case Token::PlusAssign:
		return "PlusAssign";
	case Token::MinusAssign:
		return "MinusAssign";
	case Token::MultiplyAssign:
		return "MultiplyAssign";
	case Token::DivideAssign:
		return "DivideAssign";
	case Token::ModuloAssign:
		return "ModuloAssign";

	case Token::Equal:
		return "Equal";
	case Token::NotEqual:
		return "NotEqual";
	case Token::Less:
		return "Less";
	case Token::Greater:
		return "Greater";
	case Token::LessEqual:
		return "LessEqual";
	case Token::GreaterEqual:
		return "GreaterEqual";

	case Token::LogicalAnd:
		return "LogicalAnd";
	case Token::LogicalOr:
		return "LogicalOr";
	case Token::LogicalNot:
		return "LogicalNot";

	case Token::BitwiseAnd:
		return "BitwiseAnd";
	case Token::BitwiseOr:
		return "BitwiseOr";
	case Token::BitwiseXor:
		return "BitwiseXor";
	case Token::BitwiseNot:
		return "BitwiseNot";
	case Token::LeftShift:
		return "LeftShift";
	case Token::RightShift:
		return "RightShift";
	case Token::BitwiseAndAssign:
		return "BitwiseAndAssign";
	case Token::BitwiseOrAssign:
		return "BitwiseOrAssign";
	case Token::BitwiseXorAssign:
		return "BitwiseXorAssign";
	case Token::LeftShiftAssign:
		return "LeftShiftAssign";
	case Token::RightShiftAssign:
		return "RightShiftAssign";

	case Token::LeftParen:
		return "LeftParen";
	case Token::RightParen:
		return "RightParen";
	case Token::LeftBrace:
		return "LeftBrace";
	case Token::RightBrace:
		return "RightBrace";
	case Token::LeftBracket:
		return "LeftBracket";
	case Token::RightBracket:
		return "RightBracket";
	case Token::Semicolon:
		return "Semicolon";
	case Token::Colon:
		return "Colon";
	case Token::DoubleColon:
		return "DoubleColon";
	case Token::Comma:
		return "Comma";
	case Token::Dot:
		return "Dot";
	case Token::Arrow:
		return "Arrow";
	case Token::MatchArrow:
		return "MatchArrow";

	case Token::Hash:
		return "Hash";
	case Token::DoubleHash:
		return "DoubleHash";
	case Token::ToEqual:
		return "ToEqual";
	case Token::Dollar:
		return "Dollar";
	case Token::Ellipsis:
		return "Ellipsis";

	case Token::FilterAssign:
		return "FilterAssign";
	case Token::MapAssign:
		return "MapAssign";
	case Token::FilterRange:
		return "FilterRange";
	case Token::MapRange:
		return "MapRange";
	case Token::ReduceRange:
		return "ReduceRange";
	case Token::Ownership:
		return "Ownership";
	case Token::ReduceAssign:
		return "ReduceAssign";
	case Token::PipeAssign:
		return "PipeAssign";
	case Token::Pipe:
		return "Pipe";

	case Token::End:
		return "End";

	case Token::KeywordPub:
		return "KeywordPub";
	case Token::KeywordPri:
		return "KeywordPri";
	case Token::KeywordPro:
		return "KeywordPro";
	case Token::MapPipeAssign:
		return "MapPipeAssign";
	case Token::MapPipe:
		return "MapPipe";
	case Token::KeywordSuper:
		return "KeywordSuper";
	case Token::CharLiteral:
		return "CharLiteral";
	default: ;
	}

	return "";
}

ArgonLang::Token::Token(Type t, std::string val, size_t line, size_t column)
    : type(t), value(std::move(val)), position({line, column}) {}