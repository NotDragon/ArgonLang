#include <fstream>
#include <iostream>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"

int main(int argc, char** argv) {
	if(argc < 3) {
		std::cerr << "Expected 2 arguments";
		return 1;
	}

    std::string str;
    std::ifstream file(argv[1]);
    while (!file.eof()) {
        std::string t;
        std::getline(file, t);
        str += t;
    }

	auto tokens = ArgonLang::tokenize(str);
    ArgonLang::Parser parser(tokens);
	ArgonLang::Result<std::unique_ptr<ArgonLang::ProgramNode>> program = parser.parse();

	if(program.hasError()) {
		std::cerr << "Parsing error occurred \n\t" << program.getErrorMsg();
		std::cerr << "\nTrace: \n";
		std::string space;
		while(!program.getStackTrace().empty()) {
			space += " ";
			std::cerr << space << "-> " << program.getTrace().text << " (" << ArgonLang::ASTNodeTypeToString(program.getTrace().type) << ")\n";
			program.popTrace();
		}

		return 1;
	}

	if(parser.getMainCounter() == 0) {
		std::cerr << "The main function was not declared";
		return 1;
	}

	if(parser.getMainCounter() > 1) {
		std::cerr << "Multiple definition of the main function";
		return 1;
	}

	std::ofstream dotFile(argv[2]);
	dotFile << "digraph AST {\n";
	int nodeId = 0;
	program.getValue()->toDot(dotFile, nodeId);
	dotFile << "}\n";

	std::cout << "\nDOT file generated: ast.dot\n";

	return 0;
}