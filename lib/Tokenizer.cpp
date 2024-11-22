#include <sstream>
#include "Tokenizer.h"

const std::unordered_map<std::string, Token::Type> keywords = {
    { "def", Token::Token::KeywordDef },
    { "int", Token::Token::KeywordDef },
    { "float", Token::Token::KeywordDef },
    { "string", Token::Token::KeywordDef },
    { "any", Token::Token::KeywordDef },
    { "object", Token::Token::KeywordDef },
    { "list", Token::Token::KeywordDef },

    { "if", Token::Token::KeywordIf },
    { "else", Token::Token::KeywordElse },
    { "while", Token::Token::KeywordWhile },
    { "dowhile", Token::Token::KeywordDoWhile },
    { "continue", Token::Token::KeywordContinue },
    { "break", Token::Token::KeywordBreak },
    { "repeat", Token::Token::KeywordRepeat },
    { "for", Token::Token::KeywordFor },
    
    { "func", Token::Token::KeywordFunc },
    { "class", Token::Token::KeywordClass },
    { "return", Token::Token::KeywordReturn },

    { "when", Token::Token::KeywordWhen },
    { "pass", Token::Token::KeywordPass },
};

const std::vector<Token> tokenize(const std::string& input) {
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
                tokens.emplace_back(Token::DecimalLiteral, numLiteral);
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
            } else {
                tokens.emplace_back(Token::Identifier, identifier);
            }

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
            } else {
                tokens.emplace_back(Token::LeftShift);
                i += 2;
            }
        } else if (c == '>' && input[i + 1] == '>') {
            if (input[i + 2] == '=') {
                tokens.emplace_back(Token::RightShiftAssign);
                i += 3;
            } else {
                tokens.emplace_back(Token::RightShift);
                i += 2;
            }
        } else if (c == '<' && input[i + 1] == '=') {
            tokens.emplace_back(Token::LessEqual);
            i += 2;
        } else if (c == '>' && input[i + 1] == '=') {
            tokens.emplace_back(Token::GreaterEqual);
            i += 2;
        } else if (c == '&' && input[i + 1] == '=') {
            tokens.emplace_back(Token::AndAssign);
            i += 2;
        } else if (c == '|' && input[i + 1] == '=') {
            tokens.emplace_back(Token::OrAssign);
            i += 2;
        } else if (c == '^' && input[i + 1] == '=') {
            tokens.emplace_back(Token::XorAssign);
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
                case '&': tokens.emplace_back(Token::BitwiseAnd); break;
                case '|': tokens.emplace_back(Token::BitwiseOr); break;
                case '^': tokens.emplace_back(Token::BitwiseXor); break;
                case '~': tokens.emplace_back(Token::BitwiseNot); break;
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

std::string Token::get_type_as_string() {
    switch (type) {
            case Token::Identifier: return "Identifier";
            
            case Token::KeywordDef: return "Token::KeywordDef";
            case Token::KeywordIf: return "Token::KeywordIf";
            case Token::KeywordElse: return "Token::KeywordElse";
            case Token::KeywordBreak: return "Token::KeywordBreak";
            case Token::KeywordContinue: return "Token::KeywordContinue";
            case Token::KeywordWhile: return "Token::KeywordWhile";
            case Token::KeywordDoWhile: return "Token::KeywordDoWhile";
            case Token::KeywordWhen: return "Token::KeywordWhen";
            case Token::KeywordPass: return "Token::KeywordPass";
            case Token::KeywordRepeat: return "Token::KeywordRepeat";
            case Token::KeywordFor: return "Token::KeywordFor";
            case Token::KeywordReturn: return "Token::KeywordReturn";
            case Token::KeywordFunc: return "Token::KeywordFunc";
            case Token::KeywordClass: return "Token::KeywordClass";

            case Token::IntegralLiteral: return "IntegralLiteral";
            case Token::DecimalLiteral: return "DecimalLiteral";
            case Token::StringLiteral: return "StringLiteral";

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
            case Token::AndAssign: return "AndAssign";
            case Token::OrAssign: return "OrAssign";
            case Token::XorAssign: return "XorAssign";
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

            case Token::End: return "End";

            default: return "Unknown";
        }
}