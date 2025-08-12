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
			KeywordPar,
            KeywordIf,
            KeywordElse,
            KeywordBreak,
            KeywordContinue,
            KeywordWhile,
            KeywordFor,
			KeywordReturn,
			KeywordSuper,
			KeywordFunc,
			KeywordClass,
			KeywordImpl,
			KeywordUsing,
			KeywordUnion,
			KeywordEnum,
			KeywordYield,
			KeywordStruct,
			KeywordPub,
			KeywordPri,
			KeywordPro,
			KeywordConstructor,
			KeywordTypeconst,
			KeywordInter,
			KeywordIs,
			KeywordThrow,
			KeywordThrows,
			KeywordTry,
			KeywordCatch,
			KeywordAwait,
			KeywordModule,
			KeywordImport,
			KeywordTrait,
			KeywordWhere,

            IntegralLiteral,
            FloatLiteral,
            StringLiteral,
			CharLiteral,
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
			MapPipeAssign,

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
			MapPipe,

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
            MatchArrow,
            QuestionMark,

            Hash,
			DoubleHash,
			
			// Additional operators for range expressions
			ToEqual,  // to=
			Dollar,   // $ for iterator syntax

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

    class TokenizeResult {
    public:
        std::vector<Token> tokens;
        std::string errorMsg;
        Token::Position errorPosition;
        
        bool hasError() const { return !errorMsg.empty(); }
        
        TokenizeResult(std::vector<Token> tokens) : tokens(std::move(tokens)), errorPosition{0, 0} {}
        TokenizeResult(std::string error, Token::Position pos) : errorMsg(std::move(error)), errorPosition(pos) {}
    };

    TokenizeResult tokenize(const std::string& input);
    extern const std::unordered_map<std::string, Token::Type> keywords;
}
#endif // TOKENIZER_H