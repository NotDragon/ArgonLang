#ifndef AST_H
#define AST_H

#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include "Tokenizer.h"

#define DEBUG
namespace ArgonLang
{
	// Forward declarations
	class PatternNode;

	enum PrimitiveType {
        INT8 = 0,
        INT16,
        INT32,
        INT64,
        INT128,
        FLOAT32,
        FLOAT64,
        FLOAT128
	};

	enum class MemberVisibility {
		PUB,
		PRI,
		PRO
	};

	enum class [[nodiscard]] ASTNodeType {
		StringLiteral,
		CharLiteral,
		IntegralLiteral,
		FloatLiteral,
		BooleanLiteral,
		Identifier,
		BinaryExpression,
		UnaryExpression,
		NullExpression,
		FunctionCallExpression,
		ToExpression,
		LambdaExpression,
		ComparisonExpression,
		AssignmentExpression,
		IndexExpression,
		SliceExpression,
		MultipleIndexExpression,
		MatchExpression,
		MatchBranch,
		
		// Pattern types
		WildcardPattern,
		LiteralPattern,
		IdentifierPattern,
		ArrayPattern,
		StructPattern,
		ConstructorPattern,
		TypePattern,
		RangePattern,
		TernaryExpression,
		ParallelExpression,
		StructField,
		StructExpression,
		RangeExpression,
		MemberAccessExpression,

		Program,

		ReturnStatement,
		VariableDeclaration,
		IfStatement,
		ForStatement,
		UnionDeclaration,
		EnumDeclaration,
		ConstraintDeclaration,
		ModuleDeclaration,
		ImportStatement,
		YieldStatement,
		WhileStatement,
		BreakStatement,
		ContinueStatement,
		Block,
		TypeAlias,
		FunctionArgument,
		ClassDeclaration,
		FunctionDeclaration,
		FunctionDefinition,
		ConstructorStatement,
		ImplStatement,
		GenericParameter,

		IntersectionType,
		PrefixedType,
		GenericType,
		SumType,
		IdentifierType,
		FunctionType,
		ArrayType,
		VariadicType
	};

	enum class [[nodiscard]] ASTNodeGroup {
		Expression,
		Statement,
		Type
	};

	std::string primitive_type_to_string(PrimitiveType type);
	std::string ast_node_type_to_string(ASTNodeType type);
	PrimitiveType determine_integer_type(const std::string& value);
	PrimitiveType determine_float_type(const std::string& value);

    class ASTNode {
    public:
        virtual ~ASTNode() = default;
		Token::Position position;

		explicit ASTNode(Token::Position pos);
		virtual ASTNodeType get_node_type() const = 0;
		virtual ASTNodeGroup get_node_group() const = 0;

    #ifdef DEBUG
        virtual void print() const = 0;
        virtual void to_dot(std::ostream& os, int& nodeId) const = 0;
    #endif
    };

    class ExpressionNode : public ASTNode {
	public:
		explicit ExpressionNode(Token::Position position);
		ASTNodeGroup get_node_group() const override;
	};

    class StatementNode : public ASTNode {
	public:
		explicit StatementNode(Token::Position position);
		ASTNodeGroup get_node_group() const override;
	};

    class TypeNode : public ASTNode {
	public:
		explicit TypeNode(Token::Position position);
		ASTNodeGroup get_node_group() const override;
	};

    class StringLiteralNode : public ExpressionNode {
    public:
        std::string value;

        explicit StringLiteralNode(Token::Position position, std::string val);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class CharLiteralNode : public ExpressionNode {
	public:
		char value;

		explicit CharLiteralNode(Token::Position position, char val);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

    class IntegralLiteralNode : public ExpressionNode {
    public:
        PrimitiveType type;
        union {
            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;
            __int128 i128;
        } value;

        explicit IntegralLiteralNode(Token::Position position, __int128 val, PrimitiveType type);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class FloatLiteralNode : public ExpressionNode {
    public:
        PrimitiveType type;
        union {
            float f32;
            double f64;
            long double f128;
        } value;

        explicit FloatLiteralNode(Token::Position position, long double val, PrimitiveType type);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class BooleanLiteralNode : public ExpressionNode {
    public:
        bool value;

        explicit BooleanLiteralNode(Token::Position position, bool val);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class IdentifierNode : public ExpressionNode {
    public:
        std::string identifier;

        explicit IdentifierNode(Token::Position position, std::string val);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class BinaryExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> left;
        Token op;
        std::unique_ptr<ExpressionNode> right;

        explicit BinaryExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class UnaryExpressionNode : public ExpressionNode {
    public:
        Token op;
        std::unique_ptr<ExpressionNode> operand;

        explicit UnaryExpressionNode(Token::Position position, Token operatorSymbol, std::unique_ptr<ExpressionNode> operandNode);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class NullExpressionNode : public ExpressionNode {
    public:
        explicit NullExpressionNode(Token::Position position);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class FunctionCallExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> function;
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        std::vector<std::unique_ptr<TypeNode>> genericTypeArgs; // For func<Type1, Type2>(args)

        explicit FunctionCallExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> func, std::vector<std::unique_ptr<ExpressionNode>> args, std::vector<std::unique_ptr<TypeNode>> genericTypeArgs = {});

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };


	class MemberAccessExpressionNode : public ExpressionNode {
	public:
		std::unique_ptr<ExpressionNode> parent;
		Token accessType;
		std::unique_ptr<ExpressionNode> member;

		explicit MemberAccessExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> leftExpression, Token accessType, std::unique_ptr<ExpressionNode> member);

		ASTNodeType get_node_type() const override;

#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream &os, int &nodeId) const override;
#endif
	};

    class ToExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> lowerBound;
        std::unique_ptr<ExpressionNode> upperBound;
        bool isInclusive;

        explicit ToExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lowerBound, std::unique_ptr<ExpressionNode> upperBound, bool isInclusive);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class FunctionArgument {
	public:
		std::unique_ptr<TypeNode> type;
		std::unique_ptr<ExpressionNode> value;
		std::string name;
		Token::Position position;

		explicit FunctionArgument(Token::Position position, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value, std::string name);


		ASTNodeType getNodeType() const;
#ifdef DEBUG
		void print() const;
		void toDot(std::ostream& os, int& nodeId) const;
#endif

	};

    class LambdaExpressionNode : public ExpressionNode {
    public:
		std::vector<std::unique_ptr<FunctionArgument>> parameters;
        std::unique_ptr<ASTNode> body;

        explicit LambdaExpressionNode(Token::Position position, std::vector<std::unique_ptr<FunctionArgument>> params, std::unique_ptr<ASTNode> bd);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ComparisonExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> left;
        ArgonLang::Token op;
        std::unique_ptr<ExpressionNode> right;

        explicit ComparisonExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs, ArgonLang::Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class AssignmentExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> left;
        ArgonLang::Token op;
        std::unique_ptr<ExpressionNode> right;

        explicit AssignmentExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> lhs, ArgonLang::Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class IndexExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> array;
        std::unique_ptr<ExpressionNode> index;

        explicit IndexExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> arr, std::unique_ptr<ExpressionNode> i);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class SliceExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> array;
        std::unique_ptr<ExpressionNode> start;
        std::unique_ptr<ExpressionNode> end;
        bool isInclusive;

        explicit SliceExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> arr,
                                   std::unique_ptr<ExpressionNode> startIdx, std::unique_ptr<ExpressionNode> endIdx, bool inclusive = true);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class MultipleIndexExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> array;
        std::vector<std::unique_ptr<ExpressionNode>> indices;

        explicit MultipleIndexExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> arr,
                                           std::vector<std::unique_ptr<ExpressionNode>> idxs);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class MatchBranch {
    public:
        std::unique_ptr<PatternNode> pattern;
        std::unique_ptr<ExpressionNode> condition; // guard condition
        std::unique_ptr<ASTNode> body;
		Token::Position position;

        explicit MatchBranch(Token::Position position, std::unique_ptr<PatternNode> pattern, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ASTNode> body);

		ASTNodeType getNodeType() const;
#ifdef DEBUG
		void print() const;
		void toDot(std::ostream& os, int& nodeId) const;
#endif
    };


    class MatchExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> value;
        std::vector<std::unique_ptr<MatchBranch>> branches;

        explicit MatchExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> value, std::vector<std::unique_ptr<MatchBranch>> branches);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    // Pattern nodes
    class PatternNode : public ExpressionNode {
    public:
        explicit PatternNode(Token::Position position) : ExpressionNode(position) {}
        virtual ~PatternNode() = default;
    };

    class WildcardPatternNode : public PatternNode {
    public:
        explicit WildcardPatternNode(Token::Position position);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class LiteralPatternNode : public PatternNode {
    public:
        std::unique_ptr<ExpressionNode> literal;

        explicit LiteralPatternNode(Token::Position position, std::unique_ptr<ExpressionNode> literal);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class IdentifierPatternNode : public PatternNode {
    public:
        std::string name;

        explicit IdentifierPatternNode(Token::Position position, std::string name);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ArrayPatternNode : public PatternNode {
    public:
        std::vector<std::unique_ptr<PatternNode>> elements;
        std::unique_ptr<PatternNode> rest; // for [a, b, ...rest] patterns

        explicit ArrayPatternNode(Token::Position position, std::vector<std::unique_ptr<PatternNode>> elements, std::unique_ptr<PatternNode> rest = nullptr);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class StructPatternNode : public PatternNode {
    public:
        std::vector<std::pair<std::string, std::unique_ptr<PatternNode>>> fields;

        explicit StructPatternNode(Token::Position position, std::vector<std::pair<std::string, std::unique_ptr<PatternNode>>> fields);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ConstructorPatternNode : public PatternNode {
    public:
        std::string constructorName;
        std::vector<std::unique_ptr<PatternNode>> arguments;

        explicit ConstructorPatternNode(Token::Position position, std::string constructorName, std::vector<std::unique_ptr<PatternNode>> arguments);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class TypePatternNode : public PatternNode {
    public:
        std::unique_ptr<TypeNode> type;

        explicit TypePatternNode(Token::Position position, std::unique_ptr<TypeNode> type);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class RangePatternNode : public PatternNode {
    public:
        std::unique_ptr<ExpressionNode> start;
        std::unique_ptr<ExpressionNode> end;
        bool isInclusive;

        explicit RangePatternNode(Token::Position position, std::unique_ptr<ExpressionNode> start, std::unique_ptr<ExpressionNode> end, bool isInclusive);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class TernaryExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ExpressionNode> trueBranch;
        std::unique_ptr<ExpressionNode> falseBranch;

        explicit TernaryExpressionNode(Token::Position position, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ExpressionNode> trueBranch, std::unique_ptr<ExpressionNode> falseBranch);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ParallelExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ASTNode> statementNode;

        explicit ParallelExpressionNode(Token::Position position, std::unique_ptr<ASTNode> statementNode);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class StructField {
	public:
		std::string name;
		std::unique_ptr<TypeNode> type;
		std::unique_ptr<ExpressionNode> value;
		Token::Position position;
		explicit StructField(Token::Position position, std::string name, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value);

		ASTNodeType getNodeType() const;
#ifdef DEBUG
		void print() const;
		void toDot(std::ostream& os, int& nodeId) const;
#endif
	};

    class StructExpressionNode : public ExpressionNode {
    public:
        std::vector<StructField> fields;

        explicit StructExpressionNode(Token::Position position, std::vector<StructField> fields);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class RangeExpressionNode : public ExpressionNode {
    public:
        std::vector<std::unique_ptr<ExpressionNode>> range;

        explicit RangeExpressionNode(Token::Position position, std::vector<std::unique_ptr<ExpressionNode>> range);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };


    class ProgramNode : public StatementNode {
    public:
        std::vector<std::unique_ptr<ASTNode>> nodes;

        explicit ProgramNode(Token::Position position, std::vector<std::unique_ptr<ASTNode>> stmts);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ReturnStatementNode : public StatementNode {
    public:
        std::unique_ptr<ExpressionNode> returnExpression;
		bool isSuper;

		explicit ReturnStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> returnExpression, bool isSuper);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
         };

	class GenericParameter {
	public:
		std::string name;
		std::unique_ptr<TypeNode> constraint; // nullptr for unconstrained parameters
		Token::Position position;
		explicit GenericParameter(Token::Position position, std::string name, std::unique_ptr<TypeNode> constraint = nullptr);

		static ASTNodeType getNodeType();
		static ASTNodeGroup getNodeGroup();
	#ifdef DEBUG
		void print() const;
		void toDot(std::ostream& os, int& nodeId) const;
	#endif
	};

     class VariableDeclarationNode : public StatementNode {
    public:
        bool isConst;
        std::unique_ptr<TypeNode> type;
        std::unique_ptr<ExpressionNode> value;
        std::string name; // For simple declarations
        std::unique_ptr<PatternNode> pattern; // For single pattern destructuring
        std::vector<std::unique_ptr<PatternNode>> compoundPatterns; // For compound destructuring like [first], rest

        // Constructor for simple declarations
        explicit VariableDeclarationNode(Token::Position position, bool isConst, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value, std::string name);

        // Constructor for single pattern destructuring
        explicit VariableDeclarationNode(Token::Position position, bool isConst, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value, std::unique_ptr<PatternNode> pattern);

        // Constructor for compound destructuring
        explicit VariableDeclarationNode(Token::Position position, bool isConst, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value, std::vector<std::unique_ptr<PatternNode>> compoundPatterns);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class FunctionDeclarationNode: public StatementNode {
	public:
		std::unique_ptr<TypeNode> returnType;
		std::vector<std::unique_ptr<FunctionArgument>> args;
		std::unique_ptr<ASTNode> body;
		std::unique_ptr<ExpressionNode> name;
		std::vector<std::unique_ptr<GenericParameter>> genericParams;

		explicit FunctionDeclarationNode(Token::Position position, std::unique_ptr<TypeNode> returnType, std::vector<std::unique_ptr<FunctionArgument>> args, std::unique_ptr<ASTNode> body, std::unique_ptr<ExpressionNode> name, std::vector<std::unique_ptr<GenericParameter>> genericParams = {});

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class FunctionDefinitionNode: public StatementNode {
	public:
		std::unique_ptr<TypeNode> returnType;
		std::vector<std::unique_ptr<FunctionArgument>> args;
		std::unique_ptr<ExpressionNode> name;
		std::vector<std::unique_ptr<GenericParameter>> genericParams;

		explicit FunctionDefinitionNode(Token::Position position, std::unique_ptr<TypeNode> returnType, std::vector<std::unique_ptr<FunctionArgument>> args, std::unique_ptr<ExpressionNode> name, std::vector<std::unique_ptr<GenericParameter>> genericParams = {});

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

    class IfStatementNode : public StatementNode {
    public:
        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ASTNode> body;
        std::unique_ptr<StatementNode> elseBranch;

        explicit IfStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ASTNode> body, std::unique_ptr<StatementNode> elseBranch);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ForStatementNode : public StatementNode {
    public:
        std::string variableName;
		std::unique_ptr<TypeNode> variableType;
        std::unique_ptr<ExpressionNode> iterator;
        std::unique_ptr<ASTNode> body;

        explicit ForStatementNode(Token::Position position, std::string variableName, std::unique_ptr<ExpressionNode> iterator, std::unique_ptr<ASTNode> body, std::unique_ptr<TypeNode> variableType);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

        class UnionDeclarationNode : public StatementNode {
    public:
        std::string unionName;
        std::vector<std::unique_ptr<TypeNode>> types;

        explicit UnionDeclarationNode(Token::Position position, std::string unionName, std::vector<std::unique_ptr<TypeNode>> types);

        ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class EnumDeclarationNode : public StatementNode {
	public:
		struct EnumVariant {
			std::string name;
			std::vector<std::unique_ptr<TypeNode>> fields;
			Token::Position position;

			EnumVariant(Token::Position pos, std::string name, std::vector<std::unique_ptr<TypeNode>> fields)
				: position(pos), name(std::move(name)), fields(std::move(fields)) {}
		};

		std::string enumName;
		std::vector<EnumVariant> variants;
		bool isUnion; // true for "enum union", false for plain "enum"

		explicit EnumDeclarationNode(Token::Position position, std::string enumName, std::vector<EnumVariant> variants, bool isUnion = false);

		ASTNodeType get_node_type() const override;
	#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
	#endif
	};


	class ConstraintDeclarationNode : public StatementNode {
	public:
		std::string constraintName;
		std::vector<std::unique_ptr<GenericParameter>> genericParams;
		std::unique_ptr<ExpressionNode> constraintExpression;

		explicit ConstraintDeclarationNode(Token::Position position, std::string constraintName,
			std::vector<std::unique_ptr<GenericParameter>> genericParams,
			std::unique_ptr<ExpressionNode> constraintExpression);

		ASTNodeType get_node_type() const override;
	#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
	#endif
	};

	class ModuleDeclarationNode : public StatementNode {
	public:
		std::string moduleName;
		std::vector<std::unique_ptr<StatementNode>> body;
		std::vector<std::string> exports;

		explicit ModuleDeclarationNode(Token::Position position, std::string moduleName,
			std::vector<std::unique_ptr<StatementNode>> body,
			std::vector<std::string> exports = {});

		ASTNodeType get_node_type() const override;
	#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
	#endif
	};

	class ImportStatementNode : public StatementNode {
	public:
		std::string moduleName;
		std::vector<std::string> importedItems; // empty means import all
		std::string alias; // for "import as" syntax

		explicit ImportStatementNode(Token::Position position, std::string moduleName,
			std::vector<std::string> importedItems = {},
			std::string alias = "");

		ASTNodeType get_node_type() const override;
	#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
	#endif
	};

	class ImplStatementNode : public StatementNode {
	public:
		std::string className;
		std::unique_ptr<StatementNode> body;
		MemberVisibility visibility;

		explicit ImplStatementNode(Token::Position position, std::string className, std::unique_ptr<StatementNode> body, MemberVisibility visibility);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class ConstructorStatementNode : public StatementNode{
	public:
		class ConstructorArgument {
		public:
			std::string name;
			std::string initializes;
			std::unique_ptr<TypeNode> type;
			std::unique_ptr<ExpressionNode> value;
			Token::Position position;

			explicit ConstructorArgument(Token::Position position, std::string name, std::string initializes, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value);
#ifdef DEBUG
			void print() const;
			void toDot(std::ostream& os, int& nodeId) const;
#endif
		};

		std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;
		std::unique_ptr<ASTNode> body;

		explicit ConstructorStatementNode(Token::Position position, std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args, std::unique_ptr<ASTNode> body);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};


	class ClassDeclarationNode : public StatementNode {
	public:
		class ClassMember {
		public:
			std::unique_ptr<StatementNode> declaration;
			MemberVisibility visibility;
			Token::Position position;

			explicit ClassMember(Token::Position position, std::unique_ptr<StatementNode> declaration, MemberVisibility visibility);
#ifdef DEBUG
			void print() const;
			void toDot(std::ostream& os, int& nodeId) const;
#endif
		};

		std::string className;
		std::vector<ClassMember> body;
		std::vector<std::unique_ptr<GenericParameter>> genericParams;

		explicit ClassDeclarationNode(Token::Position position, std::string className, std::vector<ClassMember> body, std::vector<std::unique_ptr<GenericParameter>> genericParams = {});
		ASTNodeType get_node_type() const override;

#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};


	class YieldStatementNode : public StatementNode {
    public:
        std::unique_ptr<ExpressionNode> expressionNode;

        explicit YieldStatementNode(Token::Position position, std::unique_ptr<ExpressionNode> expressionNode);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class WhileStatementNode : public StatementNode {
	public:
		bool isDoWhile;
		std::unique_ptr<ExpressionNode> condition;
		std::unique_ptr<ASTNode> body;
		std::unique_ptr<StatementNode> elseBranch;
		explicit WhileStatementNode(Token::Position position, bool isDoWhile, std::unique_ptr<ExpressionNode> condition,
									std::unique_ptr<ASTNode> body,
									std::unique_ptr<StatementNode> elseBranch);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class BreakStatementNode : public StatementNode {
	public:
		ASTNodeType get_node_type() const override;
		explicit BreakStatementNode(Token::Position position);
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class ContinueStatementNode : public StatementNode {
	public:
		ASTNodeType get_node_type() const override;
		explicit ContinueStatementNode(Token::Position position);
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class TypeAliasNode : public StatementNode {
	public:
		std::string aliasName;
		std::unique_ptr<TypeNode> targetType;

		explicit TypeAliasNode(Token::Position position, std::string aliasName, std::unique_ptr<TypeNode> targetType);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class BlockNode : public StatementNode {
	public:
		std::vector<std::unique_ptr<ASTNode>> body;

		explicit BlockNode(Token::Position position, std::vector<std::unique_ptr<ASTNode>> body);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class IdentifierTypeNode : public TypeNode { // i32
	public:
		std::string typeName;

		explicit IdentifierTypeNode(Token::Position position, std::string typeName);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class GenericTypeNode : public TypeNode { // vec<i32>
	public:
		std::unique_ptr<TypeNode> base;
		std::vector<std::unique_ptr<TypeNode>> params;
		explicit GenericTypeNode(Token::Position position, std::unique_ptr<TypeNode> base, std::vector<std::unique_ptr<TypeNode>> params);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

    class SumTypeNode : public TypeNode { // i32 | f64
    public:
        std::vector<std::unique_ptr<TypeNode>> types;

        explicit SumTypeNode(Token::Position position, std::vector<std::unique_ptr<TypeNode>> types);

		ASTNodeType get_node_type() const override;
    #ifdef DEBUG
        void print() const override;
        void to_dot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class IntersectionTypeNode : public TypeNode { // i32 & Printable
	public:
		std::vector<std::unique_ptr<TypeNode>> types;

		explicit IntersectionTypeNode(Token::Position position, std::vector<std::unique_ptr<TypeNode>> types);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class PrefixedTypeNode : public TypeNode { // *i32
	public:
		enum Prefix {
			Pointer,        // *T - raw pointer
			Owned,          // ~T - unique_ptr
			SharedRef,      // ref<T> - shared_ptr
			WeakRef,        // weak<T> - weak_ptr
			ImmutableRef,   // &T - const reference
			MutableRef      // &&T - mutable reference
		} prefix;

		std::unique_ptr<TypeNode> type;

		explicit PrefixedTypeNode(Token::Position position, std::unique_ptr<TypeNode> type, Prefix prefix);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class FunctionTypeNode : public TypeNode { // func(i32, i32) i32
	public:
		std::vector<std::unique_ptr<TypeNode>> parameterTypes;
		std::unique_ptr<TypeNode> returnType; // nullptr for void/no return
		bool isClosure; // true for func i32 (closure), false for func(i32) i32

		explicit FunctionTypeNode(Token::Position position,
			std::vector<std::unique_ptr<TypeNode>> paramTypes,
			std::unique_ptr<TypeNode> retType = nullptr,
			bool closure = false);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class ArrayTypeNode : public TypeNode { // i32[10]
	public:
		std::unique_ptr<TypeNode> elementType;
		std::unique_ptr<ExpressionNode> size; // nullptr for dynamic arrays

		explicit ArrayTypeNode(Token::Position position,
			std::unique_ptr<TypeNode> elemType,
			std::unique_ptr<ExpressionNode> arraySize = nullptr);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class VariadicTypeNode : public TypeNode { // ...args: vec<any>
	public:
		std::unique_ptr<TypeNode> baseType;

		explicit VariadicTypeNode(Token::Position position, std::unique_ptr<TypeNode> type);

		ASTNodeType get_node_type() const override;
#ifdef DEBUG
		void print() const override;
		void to_dot(std::ostream& os, int& nodeId) const override;
#endif
	};

}


#endif // AST_H