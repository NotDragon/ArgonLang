#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include <vector>
#include <memory>
#include "Tokenizer.h"
namespace ArgonLang {
	class Parser {
	private:
    	const std::vector<Token>& tokens;
    	size_t current = 0;

	public:
		explicit Parser(const std::vector<Token>& tokens);

		const Token& peek() const;
		bool eos() const;
		const Token& advance();
		Token expect(Token::Type type, const std::string& errorMessage);
		
		std::unique_ptr<ASTNode> parse();
		std::unique_ptr<ASTNode> parseStatement();
		std::unique_ptr<ASTNode> parseExpression();

		// Helper methods for parsing specific types of expressions
		std::unique_ptr<ASTNode> parsePrimary();
		std::unique_ptr<ASTNode> parseAdditiveExpression();
		std::unique_ptr<ASTNode> parseMultiplicativeExpression();
		std::unique_ptr<ASTNode> parseLogicalOrExpression();
		std::unique_ptr<ASTNode> parseLogicalAndExpression();
		std::unique_ptr<ASTNode> parseEqualityExpression();
		std::unique_ptr<ASTNode> parseRelationalExpression();
		std::unique_ptr<ASTNode> parseBitwiseExpression();
		std::unique_ptr<ASTNode> parseShiftExpression();
		std::unique_ptr<ASTNode> parseAssignmentExpression();
		std::unique_ptr<ASTNode> parseParallelExpression();
		std::unique_ptr<ASTNode> parseAwaitExpression();
		std::unique_ptr<ASTNode> parseLazyExpression();
		std::unique_ptr<ASTNode> parseStructExpression();
		std::unique_ptr<ASTNode> parseMatchExpression();
		std::unique_ptr<ASTNode> parseToExpression();
		std::unique_ptr<ASTNode> parseDerefExpression();
		std::unique_ptr<ASTNode> parseReferenceExpression();
		std::unique_ptr<ASTNode> parseIncrementExpression();
		std::unique_ptr<ASTNode> parseUnaryMinusExpression();
		std::unique_ptr<ASTNode> parseUnaryPlusExpression();
		std::unique_ptr<ASTNode> parseLogicalNotExpression();
		std::unique_ptr<ASTNode> parseBitwiseNotExpression();
		std::unique_ptr<ASTNode> parseFilterExpression();
		std::unique_ptr<ASTNode> parseMapExpression();
		std::unique_ptr<ASTNode> parseReduceExpression();
		std::unique_ptr<ASTNode> parseRangeExpression();
		std::unique_ptr<ASTNode> parseFunctionCallExpression();
		std::unique_ptr<ASTNode> parseIndexingExpression();
		std::unique_ptr<ASTNode> parseMemberAccessExpression();

		// Methods for parsing statements
		std::unique_ptr<ASTNode> parseVariableDeclaration();
		std::unique_ptr<ASTNode> parseFunctionDeclaration();
		std::unique_ptr<ASTNode> parseFunctionDefinition();
		std::unique_ptr<ASTNode> parseIfStatement();
		std::unique_ptr<ASTNode> parseForStatement();
		std::unique_ptr<ASTNode> parseWhileStatement();
		std::unique_ptr<ASTNode> parseReturnStatement();
		std::unique_ptr<ASTNode> parseBreakStatement();
		std::unique_ptr<ASTNode> parseContinueStatement();
		std::unique_ptr<ASTNode> parseWhenStatement();
		std::unique_ptr<ASTNode> parseTypeAlias();
		std::unique_ptr<ASTNode> parseClassDeclaration();
		std::unique_ptr<ASTNode> parseBlock();
		std::unique_ptr<ASTNode> parseImplStatement();
		std::unique_ptr<ASTNode> parseYieldStatement();

		// Methods for parsing types
		std::unique_ptr<TypeNode> parseType();
		std::unique_ptr<TypeNode> parseGenericType();
		std::unique_ptr<TypeNode> parseUnionType();
		std::unique_ptr<TypeNode> parseArrayType();
	};
}

#endif // PARSER_H
