#include <fstream>
#include <iostream>
#include "Parser.h"
#include "Tokenizer.h"
#include "PrintVisitor.h"

int main() {
	std::string str;
	std::ifstream file("in.c");

	while (!file.eof()) {
		std::string t;
		std::getline(file, t);
		str += t;
	}

	Parser parser(tokenize(str));
	std::unique_ptr<ASTNode> program = parser.parse();
	PrintVisitor visitor;
	
	visitor.visit(std::unique_ptr<ExpressionNode>(dynamic_cast<ExpressionNode*>(program.get())));

	std::ofstream dotFile("build/ast.dot");
	dotFile << "digraph AST {\n";
	int nodeId = 0;
	program->toDot(dotFile, nodeId);
	dotFile << "}\n";

	std::cout << "\nDOT file generated: ast.dot\n";

	return 0;
}