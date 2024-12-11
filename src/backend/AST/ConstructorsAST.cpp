#include "backend/AST.h"
#include <utility>

ArgonLang::UnionTypeNode::UnionTypeNode(std::vector<std::unique_ptr<TypeNode>> types): types(std::move(types)) {}
ArgonLang::StringLiteralNode::StringLiteralNode(std::string  val) : value(std::move(val)) {}
ArgonLang::IntegralLiteralNode::IntegralLiteralNode(__int128 val, PrimitiveType type) : type(type) {
	switch (type) {
		case PrimitiveType::INT8:   value.i8 = static_cast<int8_t>(val); break;
		case PrimitiveType::INT16:  value.i16 = static_cast<int16_t>(val); break;
		case PrimitiveType::INT32:  value.i32 = static_cast<int32_t>(val); break;
		case PrimitiveType::INT64:  value.i64 = static_cast<int64_t>(val); break;
		case PrimitiveType::INT128: value.i128 = static_cast<__int128>(val); break;
		default: throw std::runtime_error("Invalid type for IntegralLiteralNode");
	}
}

ArgonLang::FloatLiteralNode::FloatLiteralNode(long double val, PrimitiveType type) : type(type) {
	switch (type) {
		case PrimitiveType::FLOAT32:  value.f32 = static_cast<float>(val); break;
		case PrimitiveType::FLOAT64:  value.f64 = static_cast<double>(val); break;
		case PrimitiveType::FLOAT128: value.f128 = static_cast<long double>(val); break;
		default: throw std::runtime_error("Invalid type for FloatLiteralNode");
	}
}
ArgonLang::BooleanLiteralNode::BooleanLiteralNode(bool val) : value(val) {}
ArgonLang::IdentifierNode::IdentifierNode(std::string  val) : identifier(std::move(val)) {}
ArgonLang::BinaryExpressionNode::BinaryExpressionNode(std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs)
		: left(std::move(lhs)), right(std::move(rhs)), op(std::move(operatorSymbol)) {}
ArgonLang::UnaryExpressionNode::UnaryExpressionNode(
		Token operatorSymbol, std::unique_ptr<ExpressionNode> operandNode): op(std::move(operatorSymbol)), operand(std::move(operandNode)) {}

ArgonLang::ProgramNode::ProgramNode(std::vector<std::unique_ptr<ASTNode>> stmts): nodes(std::move(stmts)) {}

ArgonLang::NullExpressionNode::NullExpressionNode() = default;

ArgonLang::FunctionCallExpressionNode::FunctionCallExpressionNode(std::unique_ptr<ExpressionNode> func,
																  std::vector<std::unique_ptr<ExpressionNode>> args): arguments(std::move(args)), function(std::move(func)) {}

ArgonLang::ToExpressionNode::ToExpressionNode(std::unique_ptr<ExpressionNode> lowerBound, std::unique_ptr<ExpressionNode> upperBound, bool isInclusive): lowerBound(std::move(lowerBound)), upperBound(std::move(upperBound)), isInclusive(isInclusive) {}

ArgonLang::LambdaExpressionNode::LambdaExpressionNode(std::unordered_map<std::string, Variable> params,
													  std::unique_ptr<StatementNode> bd): parameters(std::move(params)), body(std::move(bd)) {}

ArgonLang::ComparisonExpressionNode::ComparisonExpressionNode(std::unique_ptr<ExpressionNode> lhs,
															  ArgonLang::Token  operatorSymbol,
															  std::unique_ptr<ExpressionNode> rhs): left(std::move(lhs)),
																									right(std::move(rhs)), op(std::move(operatorSymbol)) {}

ArgonLang::AssignmentExpressionNode::AssignmentExpressionNode(std::unique_ptr<ExpressionNode> lhs,
															  ArgonLang::Token operatorSymbol,
															  std::unique_ptr<ExpressionNode> rhs): left(std::move(lhs)),
																									right(std::move(rhs)), op(std::move(operatorSymbol))  {}

ArgonLang::IndexExpressionNode::IndexExpressionNode(std::unique_ptr<ExpressionNode> arr,
													std::unique_ptr<ExpressionNode> i): array(std::move(arr)), index(std::move(i)) {}

ArgonLang::MatchBranch::MatchBranch(std::unique_ptr<ExpressionNode> pattern, std::unique_ptr<ExpressionNode> condition,
									std::unique_ptr<ExpressionNode> body): pattern(std::move(pattern)), condition(std::move(condition)), body(std::move(body)) {}

ArgonLang::MatchExpressionNode::MatchExpressionNode(std::unique_ptr<ExpressionNode> value,
													std::vector<std::unique_ptr<MatchBranch>> branches): value(std::move(value)), branches(std::move(branches)) {}


ArgonLang::TernaryExpressionNode::TernaryExpressionNode(std::unique_ptr<ExpressionNode> condition,
														std::unique_ptr<ExpressionNode> trueBranch,
														std::unique_ptr<ExpressionNode> falseBranch): condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)) {}

ArgonLang::ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ExpressionNode> returnExpression): returnExpression(std::move(returnExpression)) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(bool isConst, std::unique_ptr<TypeNode> type,
															std::unique_ptr<ExpressionNode> value, std::string name): isConst(isConst), type(std::move(type)), value(std::move(value)), name(std::move(name)) {}

ArgonLang::TypeAliasNode::TypeAliasNode(std::string aliasName, std::unique_ptr<TypeNode> targetType): aliasName(std::move(aliasName)), targetType(std::move(targetType)) {}

ArgonLang::IfStatementNode::IfStatementNode(
		std::unique_ptr<ExpressionNode> condition,
		std::unique_ptr<StatementNode> body,
		std::unique_ptr<StatementNode> elseBranch
) : condition(std::move(condition)),
	body(std::move(body)),
	elseBranch(std::move(elseBranch)) {}

ArgonLang::BlockNode::BlockNode(std::vector<std::unique_ptr<ASTNode>> body): body(std::move(body)) {}

ArgonLang::ForStatementNode::ForStatementNode(
		std::string variableName,
		std::unique_ptr<ExpressionNode> iterator,
		std::unique_ptr<StatementNode> body
) : iterator(std::move(iterator)),
	variableName(std::move(variableName)),
	body(std::move(body)) {}

ArgonLang::UnionDeclarationNode::UnionDeclarationNode(
		std::string unionName,
		std::vector<std::unique_ptr<TypeNode>> types
) : unionName(std::move(unionName)), types(std::move(types)) {}

ArgonLang::IdentifierTypeNode::IdentifierTypeNode(std::string typeName)
		: typeName(std::move(typeName)) {}

ArgonLang::RangeExpressionNode::RangeExpressionNode(std::vector<std::unique_ptr<ExpressionNode>> range): range(std::move(range)) {}

ArgonLang::GenericTypeNode::GenericTypeNode(std::string name, std::vector<std::unique_ptr<TypeNode>> params) : name(std::move(name)), params(std::move(params)) {}

ArgonLang::WhenStatementNode::WhenStatementNode(std::string variableName, std::unique_ptr<ExpressionNode> iterator,
												std::unique_ptr<StatementNode> body): variableName(std::move(variableName)), iterator(std::move(iterator)), body(std::move(body)) {}

ArgonLang::YieldStatementNode::YieldStatementNode(std::unique_ptr<ExpressionNode> expressionNode): expressionNode(std::move(expressionNode)) {}

ArgonLang::ParallelExpressionNode::ParallelExpressionNode(std::unique_ptr<ASTNode> statementNode): statementNode(std::move(statementNode)) {}

ArgonLang::AwaitExpressionNode::AwaitExpressionNode(std::unique_ptr<ExpressionNode> statementNode): statementNode(std::move(statementNode)) {}


ArgonLang::LazyExpressionNode::LazyExpressionNode(std::unique_ptr<ExpressionNode> statementNode): statementNode(std::move(statementNode)) {}

ArgonLang::WhileStatementNode::WhileStatementNode(bool isDoWhile, std::unique_ptr<ExpressionNode> condition,
												  std::unique_ptr<StatementNode> body,
												  std::unique_ptr<StatementNode> elseBranch): condition(std::move(condition)), body(std::move(body)), elseBranch(std::move(elseBranch)), isDoWhile(isDoWhile) {}

ArgonLang::StructExpressionNode::StructExpressionNode(
		std::vector<std::tuple<std::string, std::unique_ptr<TypeNode>, std::unique_ptr<ExpressionNode>>> fields): fields(std::move(fields)) {}
