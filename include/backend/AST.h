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
		MatchExpression,
		TernaryExpression,
		ParallelExpression,
		AwaitExpression,
		LazyExpression,
		StructExpression,
		RangeExpression,
		Program,
		ReturnStatement,
		VariableDeclaration,
		IfStatement,
		ForStatement,
		UnionDeclaration,
		YieldStatement,
		WhileStatement,
		BreakStatement,
		ContinueStatement,
		Block,
		SumType,
		IdentifierType,
		TypeAlias,
		GenericType,
		StructField,
		MatchBranch,
		FunctionArgument,
		ClassDeclaration,
		FunctionDeclaration,
		FunctionDefinition,
		ConstructorStatement,
		ImplStatement,
		MemberAccessExpression,
		IntersectionType,
		PrefixedType
	};

	enum class [[nodiscard]] ASTNodeGroup {
		Expression,
		Statement,
		Type
	};

	std::string primitiveTypeToString(PrimitiveType type);
	std::string ASTNodeTypeToString(ASTNodeType type);
	PrimitiveType determineIntegerType(const std::string& value);
	PrimitiveType determineFloatType(const std::string& value);

    class ASTNode {
    public:
        virtual ~ASTNode() = default;

		virtual ASTNodeType getNodeType() const = 0;
		virtual ASTNodeGroup getNodeGroup() const = 0;

    #ifdef DEBUG
        virtual void print() const = 0;
        virtual void toDot(std::ostream& os, int& nodeId) const = 0;
    #endif
    };

    class ExpressionNode : public ASTNode {
	public:
		ASTNodeGroup getNodeGroup() const override;
	};

    class StatementNode : public ASTNode {
	public:
		ASTNodeGroup getNodeGroup() const override;
	};

    class TypeNode : public ASTNode {
	public:
		ASTNodeGroup getNodeGroup() const override;
	};

    class StringLiteralNode : public ExpressionNode {
    public:
        std::string value;

        explicit StringLiteralNode(std::string val);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class CharLiteralNode : public ExpressionNode {
	public:
		char value;

		explicit CharLiteralNode(char val);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
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

        explicit IntegralLiteralNode(__int128 val, PrimitiveType type);
		
		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
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

        explicit FloatLiteralNode(long double val, PrimitiveType type);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class BooleanLiteralNode : public ExpressionNode {
    public:
        bool value;

        explicit BooleanLiteralNode(bool val);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class IdentifierNode : public ExpressionNode {
    public:
        std::string identifier;

        explicit IdentifierNode(std::string val);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class BinaryExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> left;
        Token op;
        std::unique_ptr<ExpressionNode> right;

        explicit BinaryExpressionNode(std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class UnaryExpressionNode : public ExpressionNode {
    public:
        Token op;
        std::unique_ptr<ExpressionNode> operand;

        explicit UnaryExpressionNode(Token operatorSymbol, std::unique_ptr<ExpressionNode> operandNode);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class NullExpressionNode : public ExpressionNode {
    public:
        explicit NullExpressionNode();

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class FunctionCallExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> function;
        std::vector<std::unique_ptr<ExpressionNode>> arguments;

        explicit FunctionCallExpressionNode(std::unique_ptr<ExpressionNode> func, std::vector<std::unique_ptr<ExpressionNode>> args);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };


	class MemberAccessExpressionNode : public ExpressionNode {
	public:
		std::unique_ptr<ExpressionNode> parent;
		Token accessType;
		std::unique_ptr<ExpressionNode> member;

		explicit MemberAccessExpressionNode(std::unique_ptr<ExpressionNode> leftExpression, Token accessType, std::unique_ptr<ExpressionNode> member);

		ASTNodeType getNodeType() const override;

#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream &os, int &nodeId) const override;
#endif
	};

    class ToExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> lowerBound;
        std::unique_ptr<ExpressionNode> upperBound;
        bool isInclusive;

        explicit ToExpressionNode(std::unique_ptr<ExpressionNode> lowerBound, std::unique_ptr<ExpressionNode> upperBound, bool isInclusive);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class FunctionArgument {
	public:
		std::unique_ptr<TypeNode> type;
		std::unique_ptr<ExpressionNode> value;
		std::string name;

		explicit FunctionArgument(std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value, std::string name);
		explicit FunctionArgument();

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

        explicit LambdaExpressionNode(std::vector<std::unique_ptr<FunctionArgument>> params, std::unique_ptr<ASTNode> bd);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ComparisonExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> left;
        ArgonLang::Token op;
        std::unique_ptr<ExpressionNode> right;

        explicit ComparisonExpressionNode(std::unique_ptr<ExpressionNode> lhs, ArgonLang::Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class AssignmentExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> left;
        ArgonLang::Token op;
        std::unique_ptr<ExpressionNode> right;

        explicit AssignmentExpressionNode(std::unique_ptr<ExpressionNode> lhs, ArgonLang::Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class IndexExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> array;
        std::unique_ptr<ExpressionNode> index;

        explicit IndexExpressionNode(std::unique_ptr<ExpressionNode> arr, std::unique_ptr<ExpressionNode> i);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class MatchBranch {
    public:
        std::unique_ptr<ExpressionNode> pattern;
        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ASTNode> body;

        explicit MatchBranch(std::unique_ptr<ExpressionNode> pattern, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ASTNode> body);

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

        explicit MatchExpressionNode(std::unique_ptr<ExpressionNode> value, std::vector<std::unique_ptr<MatchBranch>> branches);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class TernaryExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ExpressionNode> trueBranch;
        std::unique_ptr<ExpressionNode> falseBranch;

        explicit TernaryExpressionNode(std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ExpressionNode> trueBranch, std::unique_ptr<ExpressionNode> falseBranch);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ParallelExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ASTNode> statementNode;

        explicit ParallelExpressionNode(std::unique_ptr<ASTNode> statementNode);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class AwaitExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> statementNode;

        explicit AwaitExpressionNode(std::unique_ptr<ExpressionNode> statementNode);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class LazyExpressionNode : public ExpressionNode {
    public:
        std::unique_ptr<ExpressionNode> statementNode;

        explicit LazyExpressionNode(std::unique_ptr<ExpressionNode> statementNode);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class StructField {
	public:
		std::string name;
		std::unique_ptr<TypeNode> type;
		std::unique_ptr<ExpressionNode> value;
		explicit StructField(std::string name, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value);

		ASTNodeType getNodeType() const;
#ifdef DEBUG
		void print() const;
		void toDot(std::ostream& os, int& nodeId) const;
#endif
	};

    class StructExpressionNode : public ExpressionNode {
    public:
        std::vector<StructField> fields;

        explicit StructExpressionNode(std::vector<StructField> fields);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class RangeExpressionNode : public ExpressionNode {
    public:
        std::vector<std::unique_ptr<ExpressionNode>> range;

        explicit RangeExpressionNode(std::vector<std::unique_ptr<ExpressionNode>> range);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };


    class ProgramNode : public StatementNode {
    public:
        std::vector<std::unique_ptr<ASTNode>> nodes;

        explicit ProgramNode(std::vector<std::unique_ptr<ASTNode>> stmts);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ReturnStatementNode : public StatementNode {
    public:
        std::unique_ptr<ExpressionNode> returnExpression;
		bool isSuper;

        explicit ReturnStatementNode(std::unique_ptr<ExpressionNode> returnExpression, bool isSuper);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class VariableDeclarationNode : public StatementNode {
    public:
        bool isConst;
        std::unique_ptr<TypeNode> type;
        std::unique_ptr<ExpressionNode> value;
        std::string name;

        explicit VariableDeclarationNode(bool isConst, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value, std::string name);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class FunctionDeclarationNode: public StatementNode {
	public:
		std::unique_ptr<TypeNode> returnType;
		std::vector<std::unique_ptr<FunctionArgument>> args;
		std::unique_ptr<ASTNode> body;
		std::unique_ptr<ExpressionNode> name;

		explicit FunctionDeclarationNode(std::unique_ptr<TypeNode> returnType, std::vector<std::unique_ptr<FunctionArgument>> args, std::unique_ptr<ASTNode> body, std::unique_ptr<ExpressionNode> name);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class FunctionDefinitionNode: public StatementNode {
	public:
		std::unique_ptr<TypeNode> returnType;
		std::vector<std::unique_ptr<FunctionArgument>> args;
		std::unique_ptr<ExpressionNode> name;

		explicit FunctionDefinitionNode(std::unique_ptr<TypeNode> returnType, std::vector<std::unique_ptr<FunctionArgument>> args, std::unique_ptr<ExpressionNode> name);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

    class IfStatementNode : public StatementNode {
    public:
        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ASTNode> body;
        std::unique_ptr<StatementNode> elseBranch;

        explicit IfStatementNode(std::unique_ptr<ExpressionNode> condition, std::unique_ptr<ASTNode> body, std::unique_ptr<StatementNode> elseBranch);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class ForStatementNode : public StatementNode {
    public:
        std::string variableName;
		std::unique_ptr<TypeNode> variableType;
        std::unique_ptr<ExpressionNode> iterator;
        std::unique_ptr<ASTNode> body;

        explicit ForStatementNode(std::string variableName, std::unique_ptr<ExpressionNode> iterator, std::unique_ptr<ASTNode> body, std::unique_ptr<TypeNode> variableType);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

    class UnionDeclarationNode : public StatementNode {
    public:
        std::string unionName;
        std::vector<std::unique_ptr<TypeNode>> types;

        explicit UnionDeclarationNode(std::string unionName, std::vector<std::unique_ptr<TypeNode>> types);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class ImplStatementNode : public StatementNode {
	public:
		std::string className;
		std::unique_ptr<StatementNode> body;
		MemberVisibility visibility;

		explicit ImplStatementNode(std::string className, std::unique_ptr<StatementNode> body, MemberVisibility visibility);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
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

			explicit ConstructorArgument(std::string name, std::string initializes, std::unique_ptr<TypeNode> type, std::unique_ptr<ExpressionNode> value);
#ifdef DEBUG
			void print() const;
			void toDot(std::ostream& os, int& nodeId) const;
#endif
		};

		std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;
		std::unique_ptr<ASTNode> body;

		explicit ConstructorStatementNode(std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args, std::unique_ptr<ASTNode> body);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};


	class ClassDeclarationNode : public StatementNode {
	public:
		class ClassMember {
		public:
			std::unique_ptr<StatementNode> declaration;
			MemberVisibility visibility;

			explicit ClassMember(std::unique_ptr<StatementNode> declaration, MemberVisibility visibility);
#ifdef DEBUG
			void print() const;
			void toDot(std::ostream& os, int& nodeId) const;
#endif
		};

		std::string className;
		std::vector<ClassMember> body;

		explicit ClassDeclarationNode(std::string className, std::vector<ClassMember> body);
		ASTNodeType getNodeType() const override;

#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};


	class YieldStatementNode : public StatementNode {
    public:
        std::unique_ptr<ExpressionNode> expressionNode;

        explicit YieldStatementNode(std::unique_ptr<ExpressionNode> expressionNode);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class WhileStatementNode : public StatementNode {
	public:
		bool isDoWhile;
		std::unique_ptr<ExpressionNode> condition;
		std::unique_ptr<ASTNode> body;
		std::unique_ptr<StatementNode> elseBranch;
		explicit WhileStatementNode(bool isDoWhile, std::unique_ptr<ExpressionNode> condition,
									std::unique_ptr<ASTNode> body,
									std::unique_ptr<StatementNode> elseBranch);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class BreakStatementNode : public StatementNode {
	public:
		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class ContinueStatementNode : public StatementNode {
	public:
		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class TypeAliasNode : public StatementNode {
	public:
		std::string aliasName;
		std::unique_ptr<TypeNode> targetType;

		explicit TypeAliasNode(std::string aliasName, std::unique_ptr<TypeNode> targetType);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class BlockNode : public StatementNode {
	public:
		std::vector<std::unique_ptr<ASTNode>> body;

		explicit BlockNode(std::vector<std::unique_ptr<ASTNode>> body);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class IdentifierTypeNode : public TypeNode { // i32
	public:
		std::string typeName;

		explicit IdentifierTypeNode(std::string typeName);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class GenericTypeNode : public TypeNode { // vec<i32>
	public:
		std::unique_ptr<TypeNode> base;
		std::vector<std::unique_ptr<TypeNode>> params;
		explicit GenericTypeNode(std::unique_ptr<TypeNode> base, std::vector<std::unique_ptr<TypeNode>> params);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

    class SumTypeNode : public TypeNode { // i32 | f64
    public:
        std::vector<std::unique_ptr<TypeNode>> types;

        explicit SumTypeNode(std::vector<std::unique_ptr<TypeNode>> types);

		ASTNodeType getNodeType() const override;
    #ifdef DEBUG
        void print() const override;
        void toDot(std::ostream& os, int& nodeId) const override;
    #endif
    };

	class IntersectionTypeNode : public TypeNode { // i32 & Printable
	public:
		std::vector<std::unique_ptr<TypeNode>> types;

		explicit IntersectionTypeNode(std::vector<std::unique_ptr<TypeNode>> types);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

	class PrefixedTypeNode : public TypeNode { // *i32
	public:
		enum Prefix {
			Pointer,
			Owned
		} prefix;

		std::unique_ptr<TypeNode> type;

		explicit PrefixedTypeNode(std::unique_ptr<TypeNode> type, Prefix prefix);

		ASTNodeType getNodeType() const override;
#ifdef DEBUG
		void print() const override;
		void toDot(std::ostream& os, int& nodeId) const override;
#endif
	};

}


#endif // AST_H