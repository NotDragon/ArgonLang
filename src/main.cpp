#include <fstream>
#include <iostream>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"

int main(int argc, char** argv) {
	if(argc < 3) {
		std::cerr << "Expected 2 arguments";
		return 1;
	}

    std::ifstream file(argv[1]);

	std::string str((std::istreambuf_iterator<char>(file)),
							 std::istreambuf_iterator<char>());

	auto tokens = ArgonLang::tokenize(str);
    ArgonLang::Parser parser(tokens);
	ArgonLang::Result<std::unique_ptr<ArgonLang::ProgramNode>> program = parser.parse();

	if(program.hasError()) {
		std::cerr << "Parsing error occurred \n\t" << program.getErrorMsg();
		std::cerr << "\nAt: \n";
		std::string space;
		while(!program.getStackTrace().empty()) {
			std::string line = " ";
			int lineCounter = 0;
			for(char i : str) {
				if(i == '\n') {
					lineCounter++;
					continue;
				}

				if(i == '\t') continue;

				if(lineCounter == program.getTrace().position.line - 1)	{
					line += i;
				}
			}

			space += " ";
			std::cerr << space << "L " << "Line: " <<  program.getTrace().position.line << " Column: " << program.getTrace().position.column
			<< line
			<< program.getTrace().text << " (" << ArgonLang::ASTNodeTypeToString(program.getTrace().type) << ") \n";
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