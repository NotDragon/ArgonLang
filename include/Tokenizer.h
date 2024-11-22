#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <unordered_map>

class Token {
public:
	enum Type {
		Identifier = 0, 
        
        KeywordDef,
        KeywordIf,
        KeywordElse,
        KeywordBreak,
        KeywordContinue,
        KeywordWhile,
        KeywordDoWhile,
        KeywordWhen,
        KeywordPass,
        KeywordRepeat,
        KeywordFor,
        KeywordReturn,
        KeywordFunc,
        KeywordClass,

        IntegralLiteral,
        DecimalLiteral,
        StringLiteral,
        BooleanLiteral,

        Plus,
        Minus,
        Multiply,
        Divide,
        Modulo,
        Increment,
        Decrement,
        Assign,
        PlusAssign,
        MinusAssign,
        MultiplyAssign,
        DivideAssign,
        ModuloAssign,

        Equal,
        NotEqual,
        Less,
        Greater,
        LessEqual,
        GreaterEqual,

        LogicalAnd,
        LogicalOr,
        LogicalNot,

        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        BitwiseNot,
        LeftShift,
        RightShift,
        AndAssign,
        OrAssign,
        XorAssign,
        LeftShiftAssign,
        RightShiftAssign,

        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,
        LeftBracket,
        RightBracket,
        Semicolon,
        Colon,
        DoubleColon,
        Comma,
        Dot,
        Arrow,
        QuestionMark,

        Hash,
        DoubleHash,
		
        End
    };
    Type type;
    std::string value;

    Token(Type t, const std::string& val = "") : type(t), value(val) {}
    std::string get_type_as_string();
};

const std::vector<Token> tokenize(const std::string& input);
extern const std::unordered_map<std::string, Token::Type> keywords;

#endif // TOKENIZER_H