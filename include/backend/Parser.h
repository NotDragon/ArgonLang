#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Error/Result.h"
#include <vector>
#include <memory>
#include "backend/Tokenizer.h"

namespace ArgonLang {
	template <typename Target, typename Source>
	std::unique_ptr<Target> dynamic_unique_cast(std::unique_ptr<Source> source) {
		if (auto* result = dynamic_cast<Target*>(source.get())) {
			source.release();
			return std::unique_ptr<Target>(result);
		}
		throw std::runtime_error("Invalid cast");
	}

	template <typename Target, typename Source>
	std::unique_ptr<Target> dynamic_unique_cast(std::unique_ptr<Source&> source) {
		if (auto* result = dynamic_cast<Target*>(source)) {
			source.release();
			return std::unique_ptr<Target>(result);
		}
		throw std::runtime_error("Invalid cast");
	}

	class Parser {
	private:
    	const std::vector<Token>& tokens;
    	size_t current = 0;
		int mainCounter = 0;

	public:
		explicit Parser(const std::vector<Token>& tokens);

		int getMainCounter() const;
		Token peek() const;
		bool eos() const;
		void synchronize();

		bool isLambdaExpression();
		Result<Token> advance();
		Result<Token> expect(Token::Type type, const std::string& errorMessage);

		Result<std::unique_ptr<ProgramNode>> parse();
		Result<std::unique_ptr<ASTNode>> parseStatement();
		Result<std::unique_ptr<ASTNode>> parseExpression();

		Result<std::unique_ptr<FunctionArgument>> parseFunctionArgument();
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> parseConstructorArgument();

		// Helper methods for parsing specific types of expressions
		Result<std::unique_ptr<ASTNode>> parsePrimary();
		Result<std::unique_ptr<ASTNode>> parseAdditiveExpression();
		Result<std::unique_ptr<ASTNode>> parseMultiplicativeExpression();
		Result<std::unique_ptr<ASTNode>> parseLogicalOrExpression();
		Result<std::unique_ptr<ASTNode>> parseLogicalAndExpression();
		Result<std::unique_ptr<ASTNode>> parseEqualityExpression();
		Result<std::unique_ptr<ASTNode>> parseRelationalExpression();
		Result<std::unique_ptr<ASTNode>> parseBitwiseExpression();
		Result<std::unique_ptr<ASTNode>> parseShiftExpression();
		Result<std::unique_ptr<ASTNode>> parseAssignmentExpression();
		Result<std::unique_ptr<ASTNode>> parsePipeExpression();
		Result<std::unique_ptr<ASTNode>> parseParallelExpression();
		Result<std::unique_ptr<ASTNode>> parseStructExpression();
		Result<std::unique_ptr<ASTNode>> parseMatchExpression();
		Result<std::unique_ptr<ASTNode>> parseToExpression();
		Result<std::unique_ptr<ASTNode>> parseDerefExpression();
		Result<std::unique_ptr<ASTNode>> parseReferenceExpression();
		Result<std::unique_ptr<ASTNode>> parseIncrementExpression();
		Result<std::unique_ptr<ASTNode>> parseUnaryMinusExpression();
		Result<std::unique_ptr<ASTNode>> parseUnaryPlusExpression();
		Result<std::unique_ptr<ASTNode>> parseLogicalNotExpression();
		Result<std::unique_ptr<ASTNode>> parseBitwiseNotExpression();
		Result<std::unique_ptr<ASTNode>> parseFilterExpression();
		Result<std::unique_ptr<ASTNode>> parseMapExpression();
		Result<std::unique_ptr<ASTNode>> parseReduceExpression();
		Result<std::unique_ptr<ASTNode>> parseRangeExpression();
		Result<std::unique_ptr<ASTNode>> parseFunctionCallExpression();
		Result<std::unique_ptr<ASTNode>> parseIndexExpression();
		Result<std::unique_ptr<ASTNode>> parseMemberAccessExpression();
		Result<std::unique_ptr<ASTNode>> parseLambdaExpression();

		// Methods for parsing statements
		Result<std::unique_ptr<ASTNode>> parseVariableDeclaration();
		Result<std::unique_ptr<ASTNode>> parseFunctionDeclaration();
		Result<std::unique_ptr<ASTNode>> parseIfStatement();
		Result<std::unique_ptr<ASTNode>> parseForStatement();
		Result<std::unique_ptr<ASTNode>> parseWhileStatement();
		Result<std::unique_ptr<ASTNode>> parseReturnStatement();
		Result<std::unique_ptr<ASTNode>> parseBreakStatement();
		Result<std::unique_ptr<ASTNode>> parseContinueStatement();
		Result<std::unique_ptr<ASTNode>> parseTypeAlias();
		Result<std::unique_ptr<ASTNode>> parseClassDeclaration();
		Result<std::unique_ptr<ASTNode>> parseBlock();
		Result<std::unique_ptr<ASTNode>> parseImplStatement();
		Result<std::unique_ptr<ASTNode>> parseYieldStatement();
		Result<std::unique_ptr<ASTNode>> parseConstructorStatement();
		Result<std::unique_ptr<ASTNode>> parseEnumDeclaration();
		Result<std::unique_ptr<ASTNode>> parseTraitDeclaration();
		Result<std::unique_ptr<ASTNode>> parseModuleDeclaration();
		Result<std::unique_ptr<ASTNode>> parseImportStatement();

		// Methods for parsing types
		Result<std::unique_ptr<TypeNode>> parseType();
		Result<std::unique_ptr<TypeNode>> parseIdentifierType();
		Result<std::unique_ptr<TypeNode>> parseGenericType();
		Result<std::unique_ptr<TypeNode>> parseSumType();
		Result<std::unique_ptr<TypeNode>> parseIntersectionType();
		Result<std::unique_ptr<TypeNode>> parsePrefixedType();
	};
}

#endif // PARSER_H
