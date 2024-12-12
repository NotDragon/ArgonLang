#include <sstream>
#include "backend/Tokenizer.h"

namespace ArgonLang {
    const std::unordered_map<std::string, Token::Type> keywords = {
			{ "def", Token::KeywordDef },
			{ "const", Token::KeywordDef },
			{ "mut", Token::KeywordMut },

			{ "i8", Token::PrimitiveType },
			{ "i16", Token::PrimitiveType },
			{ "i32", Token::PrimitiveType },
			{ "i64", Token::PrimitiveType },
			{ "i128", Token::PrimitiveType },
			{ "u8", Token::PrimitiveType },
			{ "u16", Token::PrimitiveType },
			{ "u32", Token::PrimitiveType },
			{ "u64", Token::PrimitiveType },
			{ "u128", Token::PrimitiveType },
			{ "f32", Token::PrimitiveType },
			{ "f64", Token::PrimitiveType },
			{ "f128", Token::PrimitiveType },
			{ "bool", Token::PrimitiveType },
			{ "str", Token::PrimitiveType },
			{ "chr", Token::PrimitiveType },

			{ "to", Token::KeywordTo },
			{ "parallel", Token::KeywordParallel },
			{ "lazy", Token::KeywordLazy },
			{ "await", Token::KeywordAwait },
			{ "yield", Token::KeywordYield },

			{ "if", Token::KeywordIf },
			{ "else", Token::KeywordElse },
			{ "match", Token::KeywordMatch },
			{ "while", Token::KeywordWhile },
			{ "dowhile", Token::KeywordWhile },
			{ "continue", Token::KeywordContinue },
			{ "break", Token::KeywordBreak },
			{ "repeat", Token::KeywordRepeat },
			{ "for", Token::KeywordFor },

			{ "func", Token::KeywordFunc },
			{ "class", Token::KeywordClass },
			{ "return", Token::KeywordReturn },
			{ "impl", Token::KeywordImpl },
			{ "struct", Token::KeywordStruct },

			{ "using", Token::KeywordUsing },
			{ "type", Token::KeywordType },
			{ "union", Token::KeywordUnion },

			{ "eval", Token::KeywordEval },
    };
}
std::vector<ArgonLang::Token> ArgonLang::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t length = input.size();
    size_t i = 0;

    while (i < length) {
        char c = input[i];

        if (std::isspace(c)) {
			i++;
			continue;
        }

        if (std::isdigit(c)) {
			size_t start = i;
			bool isDecimal = false;
			while (i < length && (std::isdigit(input[i]) || input[i] == '.')) {
				if (input[i] == '.') {
					if (isDecimal) {
					throw std::runtime_error("Invalid numeric literal: multiple decimal points");
					}
					isDecimal = true;
				}
				i++;
			}

			std::string numLiteral = input.substr(start, i - start);

			if (isDecimal) {
				tokens.emplace_back(Token::FloatLiteral, numLiteral);
			} else {
				tokens.emplace_back(Token::IntegralLiteral, numLiteral);
			}
			continue;
        } else if (std::isalpha(c) || c == '_') {
			size_t start = i;
			while (i < length && (std::isalnum(input[i]) || input[i] == '_' || input[i] == '-')) {
				i++;
			}
			std::string identifier = input.substr(start, i - start);
			auto it = keywords.find(identifier);
			if (it != keywords.end()) {
				tokens.emplace_back(it->second, it->first);
				continue;
			}
			tokens.emplace_back(Token::Identifier, identifier);
        } else if (c == '=' && input[i + 1] == '=') {
			tokens.emplace_back(Token::Equal);
			i += 2;
        } else if (c == '!' && input[i + 1] == '=') {
			tokens.emplace_back(Token::NotEqual);
			i += 2;
        } else if (c == '+' && input[i + 1] == '+') {
			tokens.emplace_back(Token::Increment);
			i += 2;
        } else if (c == '-' && input[i + 1] == '-') {
			tokens.emplace_back(Token::Decrement);
			i += 2;
        } else if (c == '+' && input[i + 1] == '=') {
			tokens.emplace_back(Token::PlusAssign);
			i += 2;
        } else if (c == '-' && input[i + 1] == '=') {
			tokens.emplace_back(Token::MinusAssign);
			i += 2;
        } else if (c == '*' && input[i + 1] == '=') {
			tokens.emplace_back(Token::MultiplyAssign);
			i += 2;
        } else if (c == '/' && input[i + 1] == '=') {
			tokens.emplace_back(Token::DivideAssign);
			i += 2;
        } else if (c == '%' && input[i + 1] == '=') {
			tokens.emplace_back(Token::ModuloAssign);
			i += 2;
        } else if (c == '&' && input[i + 1] == '&') {
			tokens.emplace_back(Token::LogicalAnd);
			i += 2;
        } else if (c == '|' && input[i + 1] == '|') {
			tokens.emplace_back(Token::LogicalOr);
			i += 2;
        } else if (c == '<' && input[i + 1] == '<') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::LeftShiftAssign);
				i += 3;
				continue;
			}

			tokens.emplace_back(Token::LeftShift);
			i += 2;
        } else if (c == '>' && input[i + 1] == '>') {
			if (input[i + 2] == '=') {
				tokens.emplace_back(Token::RightShiftAssign);
				i += 3;
				continue;
			}

			tokens.emplace_back(Token::RightShift);
			i += 2;
        } else if (c == '<' && input[i + 1] == '=') {
			tokens.emplace_back(Token::LessEqual);
			i += 2;
        } else if (c == '>' && input[i + 1] == '=') {
			tokens.emplace_back(Token::GreaterEqual);
			i += 2;
        } else if (c == '&' && input[i + 1] == '=') {
			tokens.emplace_back(Token::MapAssign);
			i += 2;
        } else if (c == '|' && input[i + 1] == '=') {
			tokens.emplace_back(Token::FilterAssign);
			i += 2;
		}  else if (c == '^' && input[i + 1] == '=') {
			tokens.emplace_back(Token::ReduceAssign);
			i += 2;
		}  else if (c == '|' && input[i + 1] == '>') {
			if(input[i + 2] == '=') {
				tokens.emplace_back(Token::PipeAssign);
				i += 3;
				continue;
			}
			tokens.emplace_back(Token::Pipe);
			i += 2;
		} else if (c == '^' && input[i + 1] == '^') {
			if(input[i + 2] == '=') {
				tokens.emplace_back(Token::AccumulateAssign);
				i += 3;
				continue;
			}
			tokens.emplace_back(Token::AccumulateRange);
			i += 2;
		} else if (c == '-' && input[i + 1] == '>') {
			tokens.emplace_back(Token::Arrow);
			i += 2;
        } else if (c == ':' && input[i + 1] == ':') {
			tokens.emplace_back(Token::DoubleColon);
			i += 2;
		} else if (c == '#' && input[i + 1] == '#') {
			tokens.emplace_back(Token::DoubleHash);
			i += 2;
		} else if (c == '*' && input[i + 1] == '&') {
			if(input[i + 2] == '=') {
				tokens.emplace_back(Token::BitwiseAndAssign);
				i += 3;
				continue;
			}
			tokens.emplace_back(Token::BitwiseAnd);
			i += 2;
		} else if (c == '*' && input[i + 1] == '|') {
			if(input[i + 2] == '=') {
				tokens.emplace_back(Token::BitwiseOrAssign);
				i += 3;
				continue;
			}
			tokens.emplace_back(Token::BitwiseOr);
			i += 2;
		} else if (c == '*' && input[i + 1] == '^') {
			if(input[i + 2] == '=') {
				tokens.emplace_back(Token::BitwiseXorAssign);
				i += 3;
				continue;
			}
			tokens.emplace_back(Token::BitwiseXor);
			i += 2;
		}  else if (c == '*' && input[i + 1] == '~') {
			tokens.emplace_back(Token::BitwiseNot);
			i += 2;
		} else {
			 switch (c) {
				case '+': tokens.emplace_back(Token::Plus); break;
				case '-': tokens.emplace_back(Token::Minus); break;
				case '*': tokens.emplace_back(Token::Multiply); break;
				case '/': tokens.emplace_back(Token::Divide); break;
				case '%': tokens.emplace_back(Token::Modulo); break;
				case '=': tokens.emplace_back(Token::Assign); break;
				case '<': tokens.emplace_back(Token::Less); break;
				case '>': tokens.emplace_back(Token::Greater); break;
				case '!': tokens.emplace_back(Token::LogicalNot); break;
				case '&': tokens.emplace_back(Token::MapRange); break;
				case '|': tokens.emplace_back(Token::FilterRange); break;
				case '^': tokens.emplace_back(Token::ReduceRange); break;
				case '~': tokens.emplace_back(Token::Ownership); break;
				case '(': tokens.emplace_back(Token::LeftParen); break;
				case ')': tokens.emplace_back(Token::RightParen); break;
				case '{': tokens.emplace_back(Token::LeftBrace); break;
				case '}': tokens.emplace_back(Token::RightBrace); break;
				case '[': tokens.emplace_back(Token::LeftBracket); break;
				case ']': tokens.emplace_back(Token::RightBracket); break;
				case ';': tokens.emplace_back(Token::Semicolon); break;
				case ':': tokens.emplace_back(Token::Colon); break;
				case ',': tokens.emplace_back(Token::Comma); break;
				case '.': tokens.emplace_back(Token::Dot); break;
				case '?': tokens.emplace_back(Token::QuestionMark); break;
				case '#': tokens.emplace_back(Token::Hash); break;
				default: throw std::runtime_error(std::string("Unexpected character: ") + c);
			}
			i++;
        }
    }

    tokens.emplace_back(Token::End);
    return tokens;
}

std::string ArgonLang::Token::getTypeAsString(Token::Type type) {

    switch (type) {
		case Token::Identifier: return "Identifier";
		case Token::KeywordDef: return "KeywordDef";
        case Token::KeywordIf: return "KeywordIf";
        case Token::KeywordElse: return "KeywordElse";
        case Token::KeywordBreak: return "KeywordBreak";
        case Token::KeywordContinue: return "KeywordContinue";
        case Token::KeywordWhile: return "KeywordWhile";
        case Token::KeywordWhen: return "KeywordWhen";
        case Token::KeywordPass: return "KeywordPass";
        case Token::KeywordRepeat: return "KeywordRepeat";
        case Token::KeywordFor: return "KeywordFor";
        case Token::KeywordReturn: return "KeywordReturn";
        case Token::KeywordFunc: return "KeywordFunc";
        case Token::KeywordClass: return "KeywordClass";
		case Token::KeywordMut: return "KeywordMut";
		case Token::KeywordTo: return "KeywordTo";
		case Token::KeywordParallel: return "KeywordParallel";
		case Token::KeywordMatch: return "KeywordMatch";
		case Token::KeywordImpl: return "KeywordImpl";
		case Token::KeywordUsing: return "KeywordUsing";
		case Token::KeywordUnion: return "KeywordUnion";
		case Token::KeywordEval: return "KeywordEval";
		case Token::KeywordType: return "KeywordType";
		case Token::KeywordLazy: return "KeywordLazy";
		case Token::KeywordYield: return "KeywordYield";
		case Token::KeywordAwait: return "KeywordAwait";
		case Token::KeywordStruct: return "KeywordStruct";

        case Token::IntegralLiteral: return "IntegralLiteral";
        case Token::FloatLiteral: return "FloatLiteral";
        case Token::StringLiteral: return "StringLiteral";
		case Token::BooleanLiteral: return "BooleanLiteral";

		case Token::PrimitiveType: return "PrimitiveType";

        case Token::Plus: return "Plus";
        case Token::Minus: return "Minus";
        case Token::Multiply: return "Multiply";
        case Token::Divide: return "Divide";
        case Token::Modulo: return "Modulo";
        case Token::Increment: return "Increment";
        case Token::Decrement: return "Decrement";
        case Token::Assign: return "Assign";
        case Token::PlusAssign: return "PlusAssign";
        case Token::MinusAssign: return "MinusAssign";
        case Token::MultiplyAssign: return "MultiplyAssign";
        case Token::DivideAssign: return "DivideAssign";
        case Token::ModuloAssign: return "ModuloAssign";

        case Token::Equal: return "Equal";
        case Token::NotEqual: return "NotEqual";
        case Token::Less: return "Less";
        case Token::Greater: return "Greater";
        case Token::LessEqual: return "LessEqual";
        case Token::GreaterEqual: return "GreaterEqual";

        case Token::LogicalAnd: return "LogicalAnd";
        case Token::LogicalOr: return "LogicalOr";
        case Token::LogicalNot: return "LogicalNot";

        case Token::BitwiseAnd: return "BitwiseAnd";
        case Token::BitwiseOr: return "BitwiseOr";
        case Token::BitwiseXor: return "BitwiseXor";
        case Token::BitwiseNot: return "BitwiseNot";
        case Token::LeftShift: return "LeftShift";
        case Token::RightShift: return "RightShift";
        case Token::BitwiseAndAssign: return "BitwiseAndAssign";
        case Token::BitwiseOrAssign: return "BitwiseOrAssign";
        case Token::BitwiseXorAssign: return "BitwiseXorAssign";
        case Token::LeftShiftAssign: return "LeftShiftAssign";
        case Token::RightShiftAssign: return "RightShiftAssign";

        case Token::LeftParen: return "LeftParen";
        case Token::RightParen: return "RightParen";
        case Token::LeftBrace: return "LeftBrace";
        case Token::RightBrace: return "RightBrace";
        case Token::LeftBracket: return "LeftBracket";
        case Token::RightBracket: return "RightBracket";
        case Token::Semicolon: return "Semicolon";
        case Token::Colon: return "Colon";
        case Token::DoubleColon: return "DoubleColon";
        case Token::Comma: return "Comma";
        case Token::Dot: return "Dot";
        case Token::Arrow: return "Arrow";
        case Token::QuestionMark: return "QuestionMark";

        case Token::Hash: return "Hash";
        case Token::DoubleHash: return "DoubleHash";

		case Token::FilterAssign: return "FilterAssign";
		case Token::MapAssign: return "MapAssign";
		case Token::FilterRange: return "FilterRange";
		case Token::MapRange: return "MapRange";
		case Token::ReduceRange: return "ReduceRange";
		case Token::Ownership: return "Ownership";
		case Token::ReduceAssign: return "ReduceAssign";
		case Token::AccumulateAssign: return "AccumulateAssign";
		case Token::PipeAssign: return "PipeAssign";
		case Token::AccumulateRange: return "AccumulateRange";
		case Token::Pipe: return "Pipe";

		case Token::End: return "End";

	}
}