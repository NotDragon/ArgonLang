#include "backend/AST.h"
#include <utility>

ArgonLang::SumTypeNode::SumTypeNode(std::vector<std::unique_ptr<TypeNode>> types): types(std::move(types)) {}
ArgonLang::StringLiteralNode::StringLiteralNode(std::string  val) : value(std::move(val)) {}
ArgonLang::CharLiteralNode::CharLiteralNode(char val) : value(val) {}

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

ArgonLang::LambdaExpressionNode::LambdaExpressionNode(std::vector<std::unique_ptr<FunctionArgument>> params,
													  std::unique_ptr<ASTNode> bd): parameters(std::move(params)), body(std::move(bd)) {}

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
									std::unique_ptr<ASTNode> body): pattern(std::move(pattern)), condition(std::move(condition)), body(std::move(body)) {}

ArgonLang::MatchExpressionNode::MatchExpressionNode(std::unique_ptr<ExpressionNode> value,
													std::vector<std::unique_ptr<MatchBranch>> branches): value(std::move(value)), branches(std::move(branches)) {}


ArgonLang::TernaryExpressionNode::TernaryExpressionNode(std::unique_ptr<ExpressionNode> condition,
														std::unique_ptr<ExpressionNode> trueBranch,
														std::unique_ptr<ExpressionNode> falseBranch): condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)) {}

ArgonLang::ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ExpressionNode> returnExpression, bool isSuper): returnExpression(std::move(returnExpression)), isSuper(isSuper) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(bool isConst, std::unique_ptr<TypeNode> type,
															std::unique_ptr<ExpressionNode> value, std::string name): isConst(isConst), type(std::move(type)), value(std::move(value)), name(std::move(name)) {}

ArgonLang::TypeAliasNode::TypeAliasNode(std::string aliasName, std::unique_ptr<TypeNode> targetType): aliasName(std::move(aliasName)), targetType(std::move(targetType)) {}

ArgonLang::IfStatementNode::IfStatementNode(
		std::unique_ptr<ExpressionNode> condition,
		std::unique_ptr<ASTNode> body,
		std::unique_ptr<StatementNode> elseBranch
) : condition(std::move(condition)),
	body(std::move(body)),
	elseBranch(std::move(elseBranch)) {}

ArgonLang::BlockNode::BlockNode(std::vector<std::unique_ptr<ASTNode>> body): body(std::move(body)) {}

ArgonLang::ForStatementNode::ForStatementNode(
		std::string variableName,
		std::unique_ptr<ExpressionNode> iterator,
		std::unique_ptr<ASTNode> body,
		std::unique_ptr<TypeNode> variableType
) : iterator(std::move(iterator)),
	variableType(std::move(variableType)),
	variableName(std::move(variableName)),
	body(std::move(body)) {}

ArgonLang::UnionDeclarationNode::UnionDeclarationNode(
		std::string unionName,
		std::vector<std::unique_ptr<TypeNode>> types
) : unionName(std::move(unionName)), types(std::move(types)) {}

ArgonLang::IdentifierTypeNode::IdentifierTypeNode(std::string typeName)
		: typeName(std::move(typeName)) {}

ArgonLang::RangeExpressionNode::RangeExpressionNode(std::vector<std::unique_ptr<ExpressionNode>> range): range(std::move(range)) {}

ArgonLang::GenericTypeNode::GenericTypeNode(std::unique_ptr<TypeNode> base, std::vector<std::unique_ptr<TypeNode>> params) : base(std::move(base)), params(std::move(params)) {}

ArgonLang::YieldStatementNode::YieldStatementNode(std::unique_ptr<ExpressionNode> expressionNode): expressionNode(std::move(expressionNode)) {}

ArgonLang::ParallelExpressionNode::ParallelExpressionNode(std::unique_ptr<ASTNode> statementNode): statementNode(std::move(statementNode)) {}

ArgonLang::AwaitExpressionNode::AwaitExpressionNode(std::unique_ptr<ExpressionNode> statementNode): statementNode(std::move(statementNode)) {}


ArgonLang::LazyExpressionNode::LazyExpressionNode(std::unique_ptr<ExpressionNode> statementNode): statementNode(std::move(statementNode)) {}

ArgonLang::WhileStatementNode::WhileStatementNode(bool isDoWhile, std::unique_ptr<ExpressionNode> condition,
												  std::unique_ptr<ASTNode> body,
												  std::unique_ptr<StatementNode> elseBranch): condition(std::move(condition)), body(std::move(body)), elseBranch(std::move(elseBranch)), isDoWhile(isDoWhile) {}

ArgonLang::StructExpressionNode::StructExpressionNode(
		std::vector<StructField> fields): fields(std::move(fields)) {}

ArgonLang::StructField::StructField(std::string name, std::unique_ptr<TypeNode> type,
									std::unique_ptr<ExpressionNode> value): name(std::move(name)), type(std::move(type)), value(std::move(value)) {}

ArgonLang::FunctionArgument::FunctionArgument(std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value,
											  std::string name): type(std::move(type)), value(std::move(value)), name(std::move(name)) {}
ArgonLang::FunctionArgument::FunctionArgument(): type(), value(), name() {}


ArgonLang::FunctionDeclarationNode::FunctionDeclarationNode(std::unique_ptr<TypeNode> returnType,
															std::vector<std::unique_ptr<FunctionArgument>> args,
															std::unique_ptr<ASTNode> body, std::unique_ptr<ExpressionNode> name): returnType(std::move(returnType)), args(std::move(args)), body(std::move(body)), name(std::move(name)) {}

ArgonLang::FunctionDefinitionNode::FunctionDefinitionNode(std::unique_ptr<TypeNode> returnType,
														  std::vector<std::unique_ptr<FunctionArgument>> args, std::unique_ptr<ExpressionNode> name): returnType(std::move(returnType)), args(std::move(args)), name(std::move(name)) { }
ArgonLang::ImplStatementNode::ImplStatementNode(std::string className, std::unique_ptr<StatementNode> body,
												ArgonLang::MemberVisibility visibility): className(std::move(className)), body(std::move(body)), visibility(visibility) { }

ArgonLang::ConstructorStatementNode::ConstructorArgument::ConstructorArgument(std::string name, std::string initializes,
																			  std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value): name(std::move(name)), initializes(std::move(initializes)), type(std::move(type)), value(std::move(value)) { }

ArgonLang::ConstructorStatementNode::ConstructorStatementNode(std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args,
															  std::unique_ptr<ASTNode> body): args(std::move(args)), body(std::move(body)) { }

ArgonLang::ClassDeclarationNode::ClassMember::ClassMember(std::unique_ptr<StatementNode> declaration,
														  ArgonLang::MemberVisibility visibility): declaration(std::move(declaration)), visibility(visibility) { }

ArgonLang::ClassDeclarationNode::ClassDeclarationNode(std::string className, std::vector<ClassMember> body): className(std::move(className)), body(std::move(body)) { }

ArgonLang::MemberAccessExpressionNode::MemberAccessExpressionNode(std::unique_ptr<ExpressionNode> leftExpression, Token accessType, std::unique_ptr<ExpressionNode> member)
: parent(std::move(leftExpression)), member(std::move(member)), accessType(std::move(accessType)) {}

ArgonLang::IntersectionTypeNode::IntersectionTypeNode(std::vector<std::unique_ptr<TypeNode>> types): types(std::move(types)) { }

ArgonLang::PrefixedTypeNode::PrefixedTypeNode(std::unique_ptr<TypeNode> type,
											  ArgonLang::PrefixedTypeNode::Prefix prefix): type(std::move(type)), prefix(prefix) { }

