#include "backend/AST.h"
#include <utility>

using namespace ArgonLang;
ASTNode::ASTNode(Token::Position position) : position(position) {}
ExpressionNode::ExpressionNode(Token::Position position) : ASTNode(position) {}
StatementNode::StatementNode(Token::Position position): ASTNode(position) {}
TypeNode::TypeNode(Token::Position position): ASTNode(position) {}

ArgonLang::SumTypeNode::SumTypeNode(Token::Position position, std::vector<std::unique_ptr<TypeNode>> types): types(std::move(types)), TypeNode(position) {}
ArgonLang::StringLiteralNode::StringLiteralNode(Token::Position position, std::string  val) : value(std::move(val)),
																							  ExpressionNode(position) {}
ArgonLang::CharLiteralNode::CharLiteralNode(Token::Position position, char val) : value(val), ExpressionNode(position) {}

ArgonLang::IntegralLiteralNode::IntegralLiteralNode(Token::Position position, __int128 val, PrimitiveType type) : type(type), ExpressionNode(position) {
	switch (type) {
		case PrimitiveType::INT8:   value.i8 = static_cast<int8_t>(val); break;
		case PrimitiveType::INT16:  value.i16 = static_cast<int16_t>(val); break;
		case PrimitiveType::INT32:  value.i32 = static_cast<int32_t>(val); break;
		case PrimitiveType::INT64:  value.i64 = static_cast<int64_t>(val); break;
		case PrimitiveType::INT128: value.i128 = static_cast<__int128>(val); break;
		default: throw std::runtime_error("Invalid type for IntegralLiteralNode");
	}
}

ArgonLang::FloatLiteralNode::FloatLiteralNode(Token::Position position, long double val, PrimitiveType type) : type(type), ExpressionNode(position) {
	switch (type) {
		case PrimitiveType::FLOAT32:  value.f32 = static_cast<float>(val); break;
		case PrimitiveType::FLOAT64:  value.f64 = static_cast<double>(val); break;
		case PrimitiveType::FLOAT128: value.f128 = static_cast<long double>(val); break;
		default: throw std::runtime_error("Invalid type for FloatLiteralNode");
	}
}
ArgonLang::BooleanLiteralNode::BooleanLiteralNode(Token::Position position, bool val) : value(val), ExpressionNode(position) {}
ArgonLang::IdentifierNode::IdentifierNode(Token::Position position, std::string  val) : identifier(std::move(val)), ExpressionNode(position) {}
ArgonLang::BinaryExpressionNode::BinaryExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs)
		: left(std::move(lhs)), right(std::move(rhs)), op(std::move(operatorSymbol)), ExpressionNode(position) {}
ArgonLang::UnaryExpressionNode::UnaryExpressionNode(Token::Position position, 
		Token operatorSymbol, std::unique_ptr<ExpressionNode> operandNode): op(std::move(operatorSymbol)), operand(std::move(operandNode)), ExpressionNode(position) {}

ArgonLang::ProgramNode::ProgramNode(Token::Position position, std::vector<std::unique_ptr<ASTNode>> stmts): nodes(std::move(stmts)), StatementNode(position) {}

ArgonLang::NullExpressionNode::NullExpressionNode(Token::Position position): ExpressionNode(position) {}

ArgonLang::FunctionCallExpressionNode::FunctionCallExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> func,
																  std::vector<std::unique_ptr<ExpressionNode>> args): arguments(std::move(args)), function(std::move(func)), ExpressionNode(position) {}

ArgonLang::ToExpressionNode::ToExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lowerBound, std::unique_ptr<ExpressionNode> upperBound, bool isInclusive): lowerBound(std::move(lowerBound)), upperBound(std::move(upperBound)), isInclusive(isInclusive), ExpressionNode(position) {}

ArgonLang::LambdaExpressionNode::LambdaExpressionNode(Token::Position position, std::vector<std::unique_ptr<FunctionArgument>> params,
													  std::unique_ptr<ASTNode> bd): parameters(std::move(params)), body(std::move(bd)), ExpressionNode(position) {}

ArgonLang::ComparisonExpressionNode::ComparisonExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs,
															  ArgonLang::Token  operatorSymbol,
															  std::unique_ptr<ExpressionNode> rhs): left(std::move(lhs)),
																									right(std::move(rhs)), op(std::move(operatorSymbol)), ExpressionNode(position) {}

ArgonLang::AssignmentExpressionNode::AssignmentExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs,
															  ArgonLang::Token operatorSymbol,
															  std::unique_ptr<ExpressionNode> rhs): left(std::move(lhs)),
																									right(std::move(rhs)), op(std::move(operatorSymbol)) , ExpressionNode(position) {}

ArgonLang::IndexExpressionNode::IndexExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> arr,
													std::unique_ptr<ExpressionNode> i): array(std::move(arr)), index(std::move(i)), ExpressionNode(position) {}

ArgonLang::MatchBranch::MatchBranch(Token::Position position, std::unique_ptr<ExpressionNode> pattern, std::unique_ptr<ExpressionNode> condition,
									std::unique_ptr<ASTNode> body): pattern(std::move(pattern)), condition(std::move(condition)), body(std::move(body)), position(position) {}

ArgonLang::MatchExpressionNode::MatchExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> value,
													std::vector<std::unique_ptr<MatchBranch>> branches): value(std::move(value)), branches(std::move(branches)), ExpressionNode(position) {}


ArgonLang::TernaryExpressionNode::TernaryExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> condition,
														std::unique_ptr<ExpressionNode> trueBranch,
														std::unique_ptr<ExpressionNode> falseBranch): condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)), ExpressionNode(position) {}

ArgonLang::ReturnStatementNode::ReturnStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> returnExpression, bool isSuper): returnExpression(std::move(returnExpression)), isSuper(isSuper), StatementNode(position) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(Token::Position position, bool isConst, std::unique_ptr<TypeNode> type,
															std::unique_ptr<ExpressionNode> value, std::string name): isConst(isConst), type(std::move(type)), value(std::move(value)), name(std::move(name)), StatementNode(position) {}

ArgonLang::TypeAliasNode::TypeAliasNode(Token::Position position, std::string aliasName, std::unique_ptr<TypeNode> targetType): aliasName(std::move(aliasName)), targetType(std::move(targetType)), StatementNode(position) {}

ArgonLang::IfStatementNode::IfStatementNode(Token::Position position, 
		std::unique_ptr<ExpressionNode> condition,
		std::unique_ptr<ASTNode> body,
		std::unique_ptr<StatementNode> elseBranch
) : condition(std::move(condition)),
	body(std::move(body)),
	elseBranch(std::move(elseBranch)), StatementNode(position) {}

ArgonLang::BlockNode::BlockNode(Token::Position position, std::vector<std::unique_ptr<ASTNode>> body): body(std::move(body)), StatementNode(position) {}

ArgonLang::ForStatementNode::ForStatementNode(Token::Position position, 
		std::string variableName,
		std::unique_ptr<ExpressionNode> iterator,
		std::unique_ptr<ASTNode> body,
		std::unique_ptr<TypeNode> variableType
) : iterator(std::move(iterator)),
	variableType(std::move(variableType)),
	variableName(std::move(variableName)),
	body(std::move(body)), StatementNode(position) {}

ArgonLang::UnionDeclarationNode::UnionDeclarationNode(Token::Position position, 
		std::string unionName,
		std::vector<std::unique_ptr<TypeNode>> types
) : unionName(std::move(unionName)), types(std::move(types)), StatementNode(position) {}

ArgonLang::IdentifierTypeNode::IdentifierTypeNode(Token::Position position, std::string typeName)
		: typeName(std::move(typeName)), TypeNode(position) {}

ArgonLang::RangeExpressionNode::RangeExpressionNode(Token::Position position, std::vector<std::unique_ptr<ExpressionNode>> range): range(std::move(range)), ExpressionNode(position) {}

ArgonLang::GenericTypeNode::GenericTypeNode(Token::Position position, std::unique_ptr<TypeNode> base, std::vector<std::unique_ptr<TypeNode>> params) : base(std::move(base)), params(std::move(params)), TypeNode(position) {}

ArgonLang::YieldStatementNode::YieldStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> expressionNode): expressionNode(std::move(expressionNode)), StatementNode(position) {}

ArgonLang::ParallelExpressionNode::ParallelExpressionNode(Token::Position position, std::unique_ptr<ASTNode> statementNode): statementNode(std::move(statementNode)), ExpressionNode(position) {}

ArgonLang::WhileStatementNode::WhileStatementNode(Token::Position position, bool isDoWhile, std::unique_ptr<ExpressionNode> condition,
												  std::unique_ptr<ASTNode> body,
												  std::unique_ptr<StatementNode> elseBranch): condition(std::move(condition)), body(std::move(body)), elseBranch(std::move(elseBranch)), isDoWhile(isDoWhile), StatementNode(position) {}

ArgonLang::StructExpressionNode::StructExpressionNode(Token::Position position, 
		std::vector<StructField> fields): fields(std::move(fields)), ExpressionNode(position) {}

ArgonLang::StructField::StructField(Token::Position position, std::string name, std::unique_ptr<TypeNode> type,
									std::unique_ptr<ExpressionNode> value): name(std::move(name)), type(std::move(type)), value(std::move(value)), position(position) {}

ArgonLang::FunctionArgument::FunctionArgument(Token::Position position, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value,
											  std::string name): type(std::move(type)), value(std::move(value)), name(std::move(name)), position(position) {}
ArgonLang::FunctionArgument::FunctionArgument(Token::Position position): type(), value(), name(), position(position) {}


ArgonLang::FunctionDeclarationNode::FunctionDeclarationNode(Token::Position position, std::unique_ptr<TypeNode> returnType,
															std::vector<std::unique_ptr<FunctionArgument>> args,
															std::unique_ptr<ASTNode> body, std::unique_ptr<ExpressionNode> name): returnType(std::move(returnType)), args(std::move(args)), body(std::move(body)), name(std::move(name)), StatementNode(position) {}

ArgonLang::FunctionDefinitionNode::FunctionDefinitionNode(Token::Position position, std::unique_ptr<TypeNode> returnType,
														  std::vector<std::unique_ptr<FunctionArgument>> args, std::unique_ptr<ExpressionNode> name): returnType(std::move(returnType)), args(std::move(args)), name(std::move(name)), StatementNode(position) {}
ArgonLang::ImplStatementNode::ImplStatementNode(Token::Position position, std::string className, std::unique_ptr<StatementNode> body,
												ArgonLang::MemberVisibility visibility): className(std::move(className)), body(std::move(body)), visibility(visibility), StatementNode(position) {}

ArgonLang::ConstructorStatementNode::ConstructorArgument::ConstructorArgument(Token::Position position, std::string name, std::string initializes,
																			  std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value): name(std::move(name)), initializes(std::move(initializes)), type(std::move(type)), value(std::move(value)), position(position) {}

ArgonLang::ConstructorStatementNode::ConstructorStatementNode(Token::Position position, std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args,
															  std::unique_ptr<ASTNode> body): args(std::move(args)), body(std::move(body)), StatementNode(position) {}

ArgonLang::ClassDeclarationNode::ClassMember::ClassMember(Token::Position position, std::unique_ptr<StatementNode> declaration,
														  ArgonLang::MemberVisibility visibility): declaration(std::move(declaration)), visibility(visibility), position(position) {}

ArgonLang::ClassDeclarationNode::ClassDeclarationNode(Token::Position position, std::string className, std::vector<ClassMember> body): className(std::move(className)), body(std::move(body)), StatementNode(position) {}

ArgonLang::MemberAccessExpressionNode::MemberAccessExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> leftExpression, Token accessType, std::unique_ptr<ExpressionNode> member)
: parent(std::move(leftExpression)), member(std::move(member)), accessType(std::move(accessType)), ExpressionNode(position) {}

ArgonLang::IntersectionTypeNode::IntersectionTypeNode(Token::Position position, std::vector<std::unique_ptr<TypeNode>> types): types(std::move(types)), TypeNode(position) {}

ArgonLang::PrefixedTypeNode::PrefixedTypeNode(Token::Position position, std::unique_ptr<TypeNode> type,
											  ArgonLang::PrefixedTypeNode::Prefix prefix): type(std::move(type)), prefix(prefix), TypeNode(position) {}

ArgonLang::BreakStatementNode::BreakStatementNode(Token::Position position): StatementNode(position) {}
ArgonLang::ContinueStatementNode::ContinueStatementNode(Token::Position position): StatementNode(position) {}

// New AST node constructors
ArgonLang::EnumDeclarationNode::EnumDeclarationNode(Token::Position position, std::string enumName, std::vector<EnumVariant> variants, bool isUnion)
	: enumName(std::move(enumName)), variants(std::move(variants)), isUnion(isUnion), StatementNode(position) {}

ArgonLang::TraitDeclarationNode::TraitDeclarationNode(Token::Position position, std::string traitName, 
	std::vector<std::unique_ptr<TypeNode>> genericParams,
	std::vector<std::unique_ptr<StatementNode>> methods,
	std::unique_ptr<ExpressionNode> constraint)
	: traitName(std::move(traitName)), genericParams(std::move(genericParams)), 
	  methods(std::move(methods)), constraint(std::move(constraint)), StatementNode(position) {}

ArgonLang::ModuleDeclarationNode::ModuleDeclarationNode(Token::Position position, std::string moduleName, 
	std::vector<std::unique_ptr<StatementNode>> body, std::vector<std::string> exports)
	: moduleName(std::move(moduleName)), body(std::move(body)), exports(std::move(exports)), StatementNode(position) {}

ArgonLang::ImportStatementNode::ImportStatementNode(Token::Position position, std::string moduleName,
	std::vector<std::string> importedItems, std::string alias)
	: moduleName(std::move(moduleName)), importedItems(std::move(importedItems)), alias(std::move(alias)), StatementNode(position) {}

ArgonLang::FunctionTypeNode::FunctionTypeNode(Token::Position position, 
	std::vector<std::unique_ptr<TypeNode>> paramTypes, 
	std::unique_ptr<TypeNode> retType,
	bool closure)
	: parameterTypes(std::move(paramTypes)), returnType(std::move(retType)), isClosure(closure), TypeNode(position) {}

ArgonLang::ArrayTypeNode::ArrayTypeNode(Token::Position position, 
	std::unique_ptr<TypeNode> elemType, 
	std::unique_ptr<ExpressionNode> arraySize)
	: elementType(std::move(elemType)), size(std::move(arraySize)), TypeNode(position) {}



ArgonLang::VariadicTypeNode::VariadicTypeNode(Token::Position position, std::unique_ptr<TypeNode> type)
	: baseType(std::move(type)), TypeNode(position) {}

