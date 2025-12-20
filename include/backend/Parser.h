#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Error/Result.h"
#include "Error/ErrorFormatter.h"
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
		int main_counter = 0;
		std::string current_class_name;
		bool is_match = false;

	public:
		explicit Parser(const std::vector<Token>& tokens);

		int get_main_counter() const;
		Token peek() const;
		Token peek(int offset) const;
		bool eos() const;
		void synchronize();

		bool is_lambda_expression();
		bool is_single_parameter_lambda();
		Result<Token> advance();
		Result<Token> expect(Token::Type type, const std::string& errorMessage);

		Result<std::unique_ptr<ProgramNode>> parse();
		Result<std::unique_ptr<ASTNode>> parse_statement();
		Result<std::unique_ptr<ASTNode>> parse_expression();

		Result<std::unique_ptr<FunctionArgument>> parse_function_argument();
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> parse_constructor_argument();

		// Helper methods for parsing specific types of expressions
		Result<std::unique_ptr<ASTNode>> parse_primary();
		Result<std::unique_ptr<ASTNode>> parse_additive_expression();
		Result<std::unique_ptr<ASTNode>> parse_multiplicative_expression();
		Result<std::unique_ptr<ASTNode>> parse_logical_or_expression();
		Result<std::unique_ptr<ASTNode>> parse_logical_and_expression();
		Result<std::unique_ptr<ASTNode>> parse_equality_expression();
		Result<std::unique_ptr<ASTNode>> parse_relational_expression();
		Result<std::unique_ptr<ASTNode>> parse_bitwise_expression();
		Result<std::unique_ptr<ASTNode>> parse_shift_expression();
		Result<std::unique_ptr<ASTNode>> parse_ternary_expression();
		Result<std::unique_ptr<ASTNode>> parse_assignment_expression();
		Result<std::unique_ptr<ASTNode>> parse_pipe_expression();
		Result<std::unique_ptr<ASTNode>> parse_parallel_expression();
		Result<std::unique_ptr<ASTNode>> parse_struct_expression();
		Result<std::unique_ptr<ASTNode>> parse_match_expression();
		
		// Pattern parsing methods
		Result<std::unique_ptr<PatternNode>> parse_pattern();
		Result<std::unique_ptr<PatternNode>> parse_wildcard_pattern();
		Result<std::unique_ptr<PatternNode>> parse_literal_pattern();
		Result<std::unique_ptr<PatternNode>> parse_identifier_pattern();
		Result<std::unique_ptr<PatternNode>> parse_array_pattern();
		Result<std::unique_ptr<PatternNode>> parse_struct_pattern();
		Result<std::unique_ptr<PatternNode>> parse_constructor_pattern();
		Result<std::unique_ptr<PatternNode>> parse_type_pattern();
		Result<std::unique_ptr<PatternNode>> parse_range_pattern();
		
		Result<std::unique_ptr<ASTNode>> parse_to_expression();
		Result<std::unique_ptr<ASTNode>> parse_deref_expression();
		Result<std::unique_ptr<ASTNode>> parse_ownership_expression();
		Result<std::unique_ptr<ASTNode>> parse_reference_expression();
		Result<std::unique_ptr<ASTNode>> parse_increment_expression();
		Result<std::unique_ptr<ASTNode>> parse_post_increment_expression();
		Result<std::unique_ptr<ASTNode>> parse_await_expression();
		Result<std::unique_ptr<ASTNode>> parse_iterator_expression();
		Result<std::unique_ptr<ASTNode>> parse_unary_minus_expression();
		Result<std::unique_ptr<ASTNode>> parse_unary_plus_expression();
		Result<std::unique_ptr<ASTNode>> parse_logical_not_expression();
		Result<std::unique_ptr<ASTNode>> parse_bitwise_not_expression();
		Result<std::unique_ptr<ASTNode>> parse_filter_expression();
		Result<std::unique_ptr<ASTNode>> parse_map_expression();
		Result<std::unique_ptr<ASTNode>> parse_reduce_expression();
		Result<std::unique_ptr<ASTNode>> parse_range_expression();
		Result<std::unique_ptr<ASTNode>> parse_function_call_expression();
		Result<std::unique_ptr<ASTNode>> parse_index_expression();
		Result<std::unique_ptr<ASTNode>> parse_advanced_array_expression(std::unique_ptr<ExpressionNode> array_expr);
		Result<std::unique_ptr<ASTNode>> parse_nested_array_literal();
		Result<std::unique_ptr<ASTNode>> parse_member_access_expression();
		Result<std::unique_ptr<ASTNode>> parse_lambda_expression();

		// Methods for parsing statements
		Result<std::unique_ptr<ASTNode>> parse_variable_declaration();
		Result<std::unique_ptr<ASTNode>> parse_function_declaration();
		Result<std::unique_ptr<ASTNode>> parse_if_statement();
		Result<std::unique_ptr<ASTNode>> parse_for_statement();
		Result<std::unique_ptr<ASTNode>> parse_while_statement();
		Result<std::unique_ptr<ASTNode>> parse_return_statement();
		Result<std::unique_ptr<ASTNode>> parse_break_statement();
		Result<std::unique_ptr<ASTNode>> parse_continue_statement();
		Result<std::unique_ptr<ASTNode>> parse_type_alias();
		Result<std::unique_ptr<ASTNode>> parse_class_declaration();
		Result<std::unique_ptr<ASTNode>> parse_block();
		Result<std::unique_ptr<ASTNode>> parse_impl_statement();
		Result<std::unique_ptr<ASTNode>> parse_yield_statement();
		Result<std::unique_ptr<ASTNode>> parse_constructor_statement();
		Result<std::unique_ptr<ASTNode>> parse_enum_declaration();
		Result<std::unique_ptr<ASTNode>> parse_union_declaration();
		Result<std::unique_ptr<ASTNode>> parse_constraint_declaration();
		Result<std::unique_ptr<GenericParameter>> parse_generic_parameter();
		Result<std::unique_ptr<ASTNode>> parse_module_declaration();
		Result<std::unique_ptr<ASTNode>> parse_import_statement();

		// Methods for parsing types
		Result<std::unique_ptr<TypeNode>> parse_type();
		Result<std::unique_ptr<TypeNode>> parse_identifier_type();
		Result<std::unique_ptr<TypeNode>> parse_generic_type();
		Result<std::unique_ptr<TypeNode>> parse_generic_type_with_base(std::unique_ptr<TypeNode> base);
		Result<std::unique_ptr<TypeNode>> parse_array_type();
		Result<std::unique_ptr<TypeNode>> parse_function_type(Token::Position pos);
		Result<std::unique_ptr<TypeNode>> parse_variadic_type(Token::Position pos);
		Result<std::unique_ptr<TypeNode>> parse_sum_type();
		Result<std::unique_ptr<TypeNode>> parse_intersection_type();
		Result<std::unique_ptr<TypeNode>> parse_prefixed_type();
	};
}

#endif // PARSER_H
