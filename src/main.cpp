#include <fstream>
#include <iostream>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/AnalysisVisitor.h"
#include "frontend/CodeGenerationVisitor.h"

int main(int argc, char** argv) {
	if(argc < 4) {
		std::cerr << "Expected 3 arguments";
		return 1;
	}

    std::ifstream file(argv[1]);

	std::string str((std::istreambuf_iterator<char>(file)),
							 std::istreambuf_iterator<char>());

	auto tokenizeResult = ArgonLang::tokenize(str);
	if (tokenizeResult.hasError()) {
		std::cerr << "Tokenization failed: " << tokenizeResult.errorMsg << "\n";
		std::cerr << "At: " << tokenizeResult.errorPosition.line << ":" << tokenizeResult.errorPosition.column << "\n";
		return 1;
	}

    ArgonLang::Parser parser(tokenizeResult.tokens);
	ArgonLang::Result<std::unique_ptr<ArgonLang::ProgramNode>> program = parser.parse();

	bool verbose = true;

	if(program.hasError()) {
		std::cerr << "Parsing error occurred:\n\t" << program.getErrorMsg() << "\n";
		
		// Get initial trace information safely
		if (auto currentTrace = program.tryGetTrace()) {
			std::cerr << "At: " << currentTrace->position.line << ":" << currentTrace->position.column << "\n";
		}

		// Create a copy of the stack trace to avoid modifying the original
		auto traceStack = program.getStackTrace();
		size_t columnError = 0;
		std::string space;
		
		while(!traceStack.empty()) {
			if(!verbose && traceStack.size() > 1) {
				traceStack.pop();
				continue;
			}

			auto trace = traceStack.top();
			std::string line = " ";
			int lineCounter = 0;
			bool whiteSpace = true;
			
			for(size_t i = 0; i < str.size(); i++) {
				if(str[i] == '\n') {
					lineCounter++;
					continue;
				}

				if(lineCounter == static_cast<int>(trace.position.line) - 1) {
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

			std::cerr << space << "L" << line
			<< " (" << ArgonLang::ASTNodeTypeToString(trace.type)
			<< " Line: " << trace.position.line << " Column: " << trace.position.column << ")\n";
			columnError = trace.position.column;
			traceStack.pop();
		}

		std::cerr << space;
		for(size_t i = 0; i < columnError + 1; i++)
			std::cerr << " ";
		std::cerr << "^ " << program.getErrorNote() << "\n";

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
	ArgonLang::CodeGenerationVisitor codeGenerator;
	analysis.visit(*program.getValue());
	ArgonLang::Result<std::string> codeResult = codeGenerator.visit(*program.getValue());

	if(codeResult.hasError()) {
		std::cerr << codeResult.getErrorMsg() << "\n";
		return 1;
	}

	std::string code = codeResult.getValue();

	std::ofstream codeFile(argv[3]);
	codeFile << code;

	std::ofstream dotFile(argv[2]);
	dotFile << "digraph AST {\n";
	int nodeId = 0;
	program.getValue()->toDot(dotFile, nodeId);
	dotFile << "}\n";

	std::cout << "\nDOT file generated: ast.dot\n";

	return 0;
}