#include <fstream>
#include <iostream>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/AnalysisVisitor.h"

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

	bool verbose = true;

	if(program.hasError()) {
		std::cerr << "Parsing error occurred \n\t" << program.getErrorMsg();
		std::cerr << "\nAt: \n";

		size_t columnError = 0;
		std::string space;
		while(!program.getStackTrace().empty()) {
			if(program.getTrace().type == ArgonLang::ASTNodeType::Program // Don't print global scope
				|| (!verbose && program.getStackTrace().size() > 1)) { // If not verbose, only print the last one
				program.popTrace();
				continue;
			}

			std::string line = " ";
			int lineCounter = 0;
			bool whiteSpace = true;
			for(int i = 0; i < str.size(); i++) {
				if(str[i] == '\n') {
					lineCounter++;
					continue;
				}

				if(lineCounter == program.getTrace().position.line - 1)	{
					if(!whiteSpace) {
						line += str[i];
						continue;
					}

					if(str[i] == ' ' || str[i] == '\t') continue;

					line += str[i];
					whiteSpace = false;
				}
			}

			space += " ";

			std::cerr << space << "L"
			<< line
			<< " (" << ArgonLang::ASTNodeTypeToString(program.getTrace().type)
			<< " Line: " <<  program.getTrace().position.line << " Column: " << program.getTrace().position.column << ")\n";
			columnError = program.getTrace().position.column;
			program.popTrace();
		}

		std::cerr << space;
		for(int i = 0; i < columnError + 1; i++)
			std::cerr << " ";
		std::cerr << "^ " << program.getErrorNote();

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

	ArgonLang::AnalysisVisitor analysis;
	analysis.visit(*program.getValue());

	std::ofstream dotFile(argv[2]);
	dotFile << "digraph AST {\n";
	int nodeId = 0;
	program.getValue()->toDot(dotFile, nodeId);
	dotFile << "}\n";

	std::cout << "\nDOT file generated: ast.dot\n";

	return 0;
}