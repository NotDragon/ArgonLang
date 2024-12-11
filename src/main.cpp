#include <fstream>
#include <iostream>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"

int main(int argc, char** argv) {
	if(argc < 3) return -1;

    std::string str;
    std::ifstream file(argv[1]);
    while (!file.eof()) {
        std::string t;
        std::getline(file, t);
        str += t;
    }

	auto tokens = ArgonLang::tokenize(str);
    ArgonLang::Parser parser(tokens);
    std::unique_ptr<ArgonLang::ASTNode> program = parser.parse();

	std::ofstream dotFile(argv[2]);
	dotFile << "digraph AST {\n";
	int nodeId = 0;
	program->toDot(dotFile, nodeId);
	dotFile << "}\n";

	std::cout << "\nDOT file generated: ast.dot\n";

	return 0;
}