#include "backend/AST.h"

#include <utility>

using namespace ArgonLang;
ASTNode::ASTNode(Token::Position position) : position(position) {}
ExpressionNode::ExpressionNode(Token::Position position) : ASTNode(position) {}
StatementNode::StatementNode(Token::Position position) : ASTNode(position) {}
TypeNode::TypeNode(Token::Position position) : ASTNode(position) {}

ArgonLang::SumTypeNode::SumTypeNode(Token::Position position, std::vector<std::unique_ptr<TypeNode>> types)
    : types(std::move(types)), TypeNode(position) {}
ArgonLang::StringLiteralNode::StringLiteralNode(Token::Position position, std::string val)
    : value(std::move(val)), ExpressionNode(position) {}
ArgonLang::CharLiteralNode::CharLiteralNode(Token::Position position, char val)
    : value(val), ExpressionNode(position) {}

ArgonLang::IntegralLiteralNode::IntegralLiteralNode(Token::Position position, std::string val, PrimitiveType type)
    : type(type), value(std::move(val)), ExpressionNode(position) {
}

ArgonLang::FloatLiteralNode::FloatLiteralNode(Token::Position position, std::string val, PrimitiveType type)
    : type(type), value(std::move(val)), ExpressionNode(position) {
}
ArgonLang::BooleanLiteralNode::BooleanLiteralNode(Token::Position position, bool val)
    : value(val), ExpressionNode(position) {}
ArgonLang::IdentifierNode::IdentifierNode(Token::Position position, std::string val)
    : identifier(std::move(val)), ExpressionNode(position) {}
ArgonLang::BinaryExpressionNode::BinaryExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs,
                                                      Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs)
    : left(std::move(lhs)), right(std::move(rhs)), op(std::move(operatorSymbol)), ExpressionNode(position) {}
ArgonLang::UnaryExpressionNode::UnaryExpressionNode(Token::Position position, Token operatorSymbol,
                                                    std::unique_ptr<ExpressionNode> operandNode)
    : op(std::move(operatorSymbol)), operand(std::move(operandNode)), ExpressionNode(position) {}

ArgonLang::UnaryPostExpressionNode::UnaryPostExpressionNode(Token::Position position, Token operatorSymbol,
                                                    std::unique_ptr<ExpressionNode> operandNode)
    : op(std::move(operatorSymbol)), operand(std::move(operandNode)), ExpressionNode(position) {}

ArgonLang::ProgramNode::ProgramNode(Token::Position position, std::vector<std::unique_ptr<ASTNode>> stmts)
    : nodes(std::move(stmts)), StatementNode(position) {}

ArgonLang::NullExpressionNode::NullExpressionNode(Token::Position position) : ExpressionNode(position) {}

ArgonLang::FunctionCallExpressionNode::FunctionCallExpressionNode(
    Token::Position position, std::unique_ptr<ExpressionNode> func, std::vector<std::unique_ptr<ExpressionNode>> args,
    std::vector<std::unique_ptr<TypeNode>> genericTypeArgs)
    : arguments(std::move(args)), function(std::move(func)), genericTypeArgs(std::move(genericTypeArgs)),
      ExpressionNode(position) {}

ArgonLang::ToExpressionNode::ToExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lowerBound,
                                              std::unique_ptr<ExpressionNode> upperBound, bool isInclusive)
    : lowerBound(std::move(lowerBound)), upperBound(std::move(upperBound)), isInclusive(isInclusive),
      ExpressionNode(position) {}

ArgonLang::LambdaExpressionNode::LambdaExpressionNode(Token::Position position,
                                                      std::vector<std::unique_ptr<FunctionArgument>> params,
                                                      std::unique_ptr<ASTNode> bd)
    : parameters(std::move(params)), body(std::move(bd)), ExpressionNode(position) {}

ArgonLang::ComparisonExpressionNode::ComparisonExpressionNode(Token::Position position,
                                                              std::unique_ptr<ExpressionNode> lhs,
                                                              ArgonLang::Token operatorSymbol,
                                                              std::unique_ptr<ExpressionNode> rhs)
    : left(std::move(lhs)), right(std::move(rhs)), op(std::move(operatorSymbol)), ExpressionNode(position) {}

ArgonLang::AssignmentExpressionNode::AssignmentExpressionNode(Token::Position position,
                                                              std::unique_ptr<ExpressionNode> lhs,
                                                              ArgonLang::Token operatorSymbol,
                                                              std::unique_ptr<ExpressionNode> rhs)
    : left(std::move(lhs)), right(std::move(rhs)), op(std::move(operatorSymbol)), ExpressionNode(position) {}

ArgonLang::IndexExpressionNode::IndexExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> arr,
                                                    std::unique_ptr<ExpressionNode> i)
    : array(std::move(arr)), index(std::move(i)), ExpressionNode(position) {}

ArgonLang::SliceExpressionNode::SliceExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> arr,
                                                    std::unique_ptr<ExpressionNode> startIdx,
                                                    std::unique_ptr<ExpressionNode> endIdx, bool inclusive)
    : array(std::move(arr)), start(std::move(startIdx)), end(std::move(endIdx)), isInclusive(inclusive),
      ExpressionNode(position) {}

ArgonLang::MultipleIndexExpressionNode::MultipleIndexExpressionNode(Token::Position position,
                                                                    std::unique_ptr<ExpressionNode> arr,
                                                                    std::vector<std::unique_ptr<ExpressionNode>> idxs)
    : array(std::move(arr)), indices(std::move(idxs)), ExpressionNode(position) {}

ArgonLang::MatchBranch::MatchBranch(Token::Position position, std::unique_ptr<PatternNode> pattern,
                                    std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ASTNode> body)
    : pattern(std::move(pattern)), condition(std::move(condition)), body(std::move(body)), position(position) {}

ArgonLang::MatchExpressionNode::MatchExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> value,
                                                    std::vector<std::unique_ptr<MatchBranch>> branches)
    : value(std::move(value)), branches(std::move(branches)), ExpressionNode(position) {}

ArgonLang::TernaryExpressionNode::TernaryExpressionNode(Token::Position position,
                                                        std::unique_ptr<ExpressionNode> condition,
                                                        std::unique_ptr<ExpressionNode> trueBranch,
                                                        std::unique_ptr<ExpressionNode> falseBranch)
    : condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)),
      ExpressionNode(position) {}

ArgonLang::ReturnStatementNode::ReturnStatementNode(Token::Position position,
                                                    std::unique_ptr<ExpressionNode> returnExpression, bool isSuper)
    : returnExpression(std::move(returnExpression)), isSuper(isSuper), StatementNode(position) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(Token::Position position, bool isConst,
                                                            std::unique_ptr<TypeNode> type,
                                                            std::unique_ptr<ExpressionNode> value, std::string name)
    : isConst(isConst), type(std::move(type)), value(std::move(value)), name(std::move(name)), pattern(nullptr),
      compoundPatterns(), StatementNode(position) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(Token::Position position, bool isConst,
                                                            std::unique_ptr<TypeNode> type,
                                                            std::unique_ptr<ExpressionNode> value,
                                                            std::unique_ptr<PatternNode> pattern)
    : isConst(isConst), type(std::move(type)), value(std::move(value)), name(""), pattern(std::move(pattern)),
      compoundPatterns(), StatementNode(position) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(Token::Position position, bool isConst,
                                                            std::unique_ptr<TypeNode> type,
                                                            std::unique_ptr<ExpressionNode> value,
                                                            std::vector<std::unique_ptr<PatternNode>> compoundPatterns)
    : isConst(isConst), type(std::move(type)), value(std::move(value)), name(""), pattern(nullptr),
      compoundPatterns(std::move(compoundPatterns)), StatementNode(position) {}

ArgonLang::TypeAliasNode::TypeAliasNode(Token::Position position, std::string aliasName,
                                        std::unique_ptr<TypeNode> targetType)
    : aliasName(std::move(aliasName)), targetType(std::move(targetType)), StatementNode(position) {}

ArgonLang::IfStatementNode::IfStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> condition,
                                            std::unique_ptr<ASTNode> body, std::unique_ptr<StatementNode> elseBranch)
    : condition(std::move(condition)), body(std::move(body)), elseBranch(std::move(elseBranch)),
      StatementNode(position) {}

ArgonLang::BlockNode::BlockNode(Token::Position position, std::vector<std::unique_ptr<ASTNode>> body)
    : body(std::move(body)), StatementNode(position) {}

ArgonLang::ForStatementNode::ForStatementNode(Token::Position position, std::string variableName,
                                              std::unique_ptr<ExpressionNode> iterator, std::unique_ptr<ASTNode> body,
                                              std::unique_ptr<TypeNode> variableType)
    : iterator(std::move(iterator)), variableType(std::move(variableType)), variableName(std::move(variableName)),
      body(std::move(body)), StatementNode(position) {}

ArgonLang::UnionDeclarationNode::UnionDeclarationNode(Token::Position position, std::string unionName,
                                                      std::vector<UnionField> fields)
    : unionName(std::move(unionName)), fields(std::move(fields)), StatementNode(position) {}

ArgonLang::IdentifierTypeNode::IdentifierTypeNode(Token::Position position, std::string typeName)
    : typeName(std::move(typeName)), TypeNode(position) {}

ArgonLang::RangeExpressionNode::RangeExpressionNode(Token::Position position,
                                                    std::vector<std::unique_ptr<ExpressionNode>> range)
    : range(std::move(range)), ExpressionNode(position) {}

ArgonLang::GenericTypeNode::GenericTypeNode(Token::Position position, std::unique_ptr<TypeNode> base,
                                            std::vector<std::unique_ptr<TypeNode>> params)
    : base(std::move(base)), params(std::move(params)), TypeNode(position) {}

ArgonLang::YieldStatementNode::YieldStatementNode(Token::Position position,
                                                  std::unique_ptr<ExpressionNode> expressionNode)
    : expressionNode(std::move(expressionNode)), StatementNode(position) {}

ArgonLang::ParallelExpressionNode::ParallelExpressionNode(Token::Position position,
                                                          std::unique_ptr<ASTNode> statementNode)
    : statementNode(std::move(statementNode)), ExpressionNode(position) {}

ArgonLang::TryExpressionNode::TryExpressionNode(Token::Position position,
                                                std::unique_ptr<ExpressionNode> expression)
    : expression(std::move(expression)), ExpressionNode(position) {}

ArgonLang::WhileStatementNode::WhileStatementNode(Token::Position position, bool isDoWhile,
                                                  std::unique_ptr<ExpressionNode> condition,
                                                  std::unique_ptr<ASTNode> body,
                                                  std::unique_ptr<StatementNode> elseBranch)
    : condition(std::move(condition)), body(std::move(body)), elseBranch(std::move(elseBranch)), isDoWhile(isDoWhile),
      StatementNode(position) {}

ArgonLang::StructExpressionNode::StructExpressionNode(Token::Position position, std::vector<StructField> fields, std::string type)
    : fields(std::move(fields)), type(std::move(type)), ExpressionNode(position) {}

ArgonLang::StructField::StructField(Token::Position position, std::string name, std::unique_ptr<TypeNode> type,
                                    std::unique_ptr<ExpressionNode> value)
    : name(std::move(name)), type(std::move(type)), value(std::move(value)), position(position) {}

ArgonLang::FunctionArgument::FunctionArgument(Token::Position position, std::unique_ptr<TypeNode> type,
                                              std::unique_ptr<ExpressionNode> value, std::string name)
    : type(std::move(type)), value(std::move(value)), name(std::move(name)), position(position) {}

ArgonLang::FunctionDeclarationNode::FunctionDeclarationNode(
    Token::Position position, std::unique_ptr<TypeNode> returnType, std::vector<std::unique_ptr<FunctionArgument>> args,
    std::unique_ptr<ASTNode> body, std::unique_ptr<ExpressionNode> name,
    std::vector<std::unique_ptr<GenericParameter>> genericParams)
    : returnType(std::move(returnType)), args(std::move(args)), body(std::move(body)), name(std::move(name)),
      genericParams(std::move(genericParams)), StatementNode(position) {}

ArgonLang::FunctionDefinitionNode::FunctionDefinitionNode(Token::Position position,
                                                          std::unique_ptr<TypeNode> returnType,
                                                          std::vector<std::unique_ptr<FunctionArgument>> args,
                                                          std::unique_ptr<ExpressionNode> name,
                                                          std::vector<std::unique_ptr<GenericParameter>> genericParams)
    : returnType(std::move(returnType)), args(std::move(args)), name(std::move(name)),
      genericParams(std::move(genericParams)), StatementNode(position) {}
ArgonLang::ImplStatementNode::ImplStatementNode(Token::Position position, std::string className,
                                                std::unique_ptr<StatementNode> body,
                                                ArgonLang::MemberVisibility visibility)
    : className(std::move(className)), body(std::move(body)), visibility(visibility), StatementNode(position) {}

ArgonLang::ConstructorStatementNode::ConstructorArgument::ConstructorArgument(Token::Position position,
                                                                              std::string name, std::string initializes,
                                                                              std::unique_ptr<TypeNode> type,
                                                                              std::unique_ptr<ExpressionNode> value)
    : name(std::move(name)), initializes(std::move(initializes)), type(std::move(type)), value(std::move(value)),
      position(position) {}

ArgonLang::ConstructorStatementNode::ConstructorStatementNode(std::string name,
    Token::Position position, std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args,
    std::unique_ptr<ASTNode> body)
    : args(std::move(args)), body(std::move(body)), StatementNode(position), name(std::move(name)) {}

ArgonLang::ClassDeclarationNode::ClassMember::ClassMember(Token::Position position,
                                                          std::unique_ptr<StatementNode> declaration,
                                                          ArgonLang::MemberVisibility visibility)
    : declaration(std::move(declaration)), visibility(visibility), position(position) {}

ArgonLang::ClassDeclarationNode::ClassDeclarationNode(Token::Position position, std::string className,
                                                      std::vector<ClassMember> body,
                                                      std::vector<std::unique_ptr<GenericParameter>> genericParams,
                                                      std::vector<std::unique_ptr<TypeNode>> baseClasses)
    : className(std::move(className)), body(std::move(body)), genericParams(std::move(genericParams)),
      baseClasses(std::move(baseClasses)), StatementNode(position) {}

ArgonLang::MemberAccessExpressionNode::MemberAccessExpressionNode(Token::Position position,
                                                                  std::unique_ptr<ExpressionNode> leftExpression,
                                                                  Token accessType,
                                                                  std::unique_ptr<ExpressionNode> member)
    : parent(std::move(leftExpression)), member(std::move(member)), accessType(std::move(accessType)),
      ExpressionNode(position) {}

ArgonLang::IntersectionTypeNode::IntersectionTypeNode(Token::Position position,
                                                      std::vector<std::unique_ptr<TypeNode>> types)
    : types(std::move(types)), TypeNode(position) {}

ArgonLang::PrefixedTypeNode::PrefixedTypeNode(Token::Position position, std::unique_ptr<TypeNode> type,
                                              ArgonLang::PrefixedTypeNode::Prefix prefix)
    : type(std::move(type)), prefix(prefix), TypeNode(position) {}

ArgonLang::BreakStatementNode::BreakStatementNode(Token::Position position) : StatementNode(position) {}
ArgonLang::ContinueStatementNode::ContinueStatementNode(Token::Position position) : StatementNode(position) {}

// New AST node constructors
ArgonLang::EnumDeclarationNode::EnumDeclarationNode(Token::Position position, std::string enumName,
                                                    std::vector<EnumVariant> variants,
                                                    std::unique_ptr<TypeNode> constraintType, bool isUnion)
    : enumName(std::move(enumName)), variants(std::move(variants)),
      constraintType(std::move(constraintType)), isUnion(isUnion), StatementNode(position) {}

ArgonLang::GenericParameter::GenericParameter(Token::Position position, std::string name,
                                              std::unique_ptr<TypeNode> constraint)
    : name(std::move(name)), constraint(std::move(constraint)), position(position) {}

ArgonLang::ConstraintDeclarationNode::ConstraintDeclarationNode(
    Token::Position position, std::string constraintName, std::vector<std::unique_ptr<GenericParameter>> genericParams,
    std::unique_ptr<ExpressionNode> constraintExpression)
    : constraintName(std::move(constraintName)), genericParams(std::move(genericParams)),
      constraintExpression(std::move(constraintExpression)), StatementNode(position) {}

ArgonLang::ModuleDeclarationNode::ModuleDeclarationNode(Token::Position position, std::string moduleName,
                                                        std::vector<std::unique_ptr<StatementNode>> body,
                                                        std::vector<std::string> exports)
    : moduleName(std::move(moduleName)), body(std::move(body)), exports(std::move(exports)), StatementNode(position) {}

ArgonLang::ImportStatementNode::ImportStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> moduleName,
                                                    std::vector<std::string> importedItems, std::string alias)
    : StatementNode(position), moduleName(std::move(moduleName)), importedItems(std::move(importedItems)),
      alias(std::move(alias)) {}

ArgonLang::FunctionTypeNode::FunctionTypeNode(Token::Position position,
                                              std::vector<std::unique_ptr<TypeNode>> paramTypes,
                                              std::unique_ptr<TypeNode> retType, bool closure)
    : parameterTypes(std::move(paramTypes)), returnType(std::move(retType)), isClosure(closure), TypeNode(position) {}

ArgonLang::ArrayTypeNode::ArrayTypeNode(Token::Position position, std::unique_ptr<TypeNode> elemType,
                                        std::unique_ptr<ExpressionNode> arraySize)
    : elementType(std::move(elemType)), size(std::move(arraySize)), TypeNode(position) {}

ArgonLang::VariadicTypeNode::VariadicTypeNode(Token::Position position, std::unique_ptr<TypeNode> type)
    : baseType(std::move(type)), TypeNode(position) {}

// Pattern node constructors
ArgonLang::WildcardPatternNode::WildcardPatternNode(Token::Position position) : PatternNode(position) {}

ArgonLang::LiteralPatternNode::LiteralPatternNode(Token::Position position, std::unique_ptr<ExpressionNode> literal)
    : literal(std::move(literal)), PatternNode(position) {}

ArgonLang::IdentifierPatternNode::IdentifierPatternNode(Token::Position position, std::string name)
    : name(std::move(name)), PatternNode(position) {}

ArgonLang::ArrayPatternNode::ArrayPatternNode(Token::Position position,
                                              std::vector<std::unique_ptr<PatternNode>> elements,
                                              std::unique_ptr<PatternNode> rest)
    : elements(std::move(elements)), rest(std::move(rest)), PatternNode(position) {}

ArgonLang::StructPatternNode::StructPatternNode(
    Token::Position position, std::vector<std::pair<std::string, std::unique_ptr<PatternNode>>> fields)
    : fields(std::move(fields)), PatternNode(position) {}

ArgonLang::ConstructorPatternNode::ConstructorPatternNode(Token::Position position, std::string constructorName,
                                                          std::vector<std::unique_ptr<PatternNode>> arguments)
    : constructorName(std::move(constructorName)), arguments(std::move(arguments)), PatternNode(position) {}

ArgonLang::TypePatternNode::TypePatternNode(Token::Position position, std::unique_ptr<TypeNode> type)
    : type(std::move(type)), PatternNode(position) {}

ArgonLang::RangePatternNode::RangePatternNode(Token::Position position, std::unique_ptr<ExpressionNode> start,
                                              std::unique_ptr<ExpressionNode> end, bool isInclusive)
    : start(std::move(start)), end(std::move(end)), isInclusive(isInclusive), PatternNode(position) {}
