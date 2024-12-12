#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

namespace ArgonLang {
    class Token {
    public:
        enum Type {
            Identifier = 0,

			KeywordDef,
			KeywordMut,
			KeywordTo,
			KeywordLazy,
			KeywordParallel,
            KeywordIf,
            KeywordElse,
			KeywordMatch,
            KeywordBreak,
            KeywordContinue,
            KeywordWhile,
            KeywordPass,
            KeywordRepeat,
            KeywordFor,
			KeywordReturn,
			KeywordFunc,
			KeywordClass,
			KeywordImpl,
			KeywordUsing,
			KeywordUnion,
			KeywordEval,
			KeywordType,
			KeywordYield,
			KeywordAwait,
			KeywordWhen,
			KeywordStruct,

            IntegralLiteral,
            FloatLiteral,
            StringLiteral,
            BooleanLiteral,
			PrimitiveType,

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
			FilterAssign,
			MapAssign,
			ReduceAssign,
			AccumulateAssign,
			PipeAssign,

            Equal,
            NotEqual,
            Less,
            Greater,
            LessEqual,
            GreaterEqual,

            LogicalAnd,
            LogicalOr,
            LogicalNot,

			FilterRange,
			MapRange,
			ReduceRange,
			AccumulateRange,
			Ownership,
			Pipe,

            BitwiseAnd,
            BitwiseOr,
            BitwiseXor,
            BitwiseNot,
            LeftShift,
            RightShift,
            BitwiseAndAssign,
            BitwiseOrAssign,
            BitwiseXorAssign,
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

        explicit Token(Type t, std::string val = "") : type(t), value(std::move(val)) {}
        static std::string getTypeAsString(Token::Type type);
    };

    std::vector<Token> tokenize(const std::string& input);
    extern const std::unordered_map<std::string, Token::Type> keywords;
}
#endif // TOKENIZER_H