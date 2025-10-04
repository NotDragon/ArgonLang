#include <fstream>
#include <iostream>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/AnalysisVisitor.h"
#include "frontend/CodeGenerationVisitor.h"

int main(int argc, char** argv) {
	std::string filename;
	std::string output_filename = "out.cpp";
	std::string dot_filename;
	bool verbose = false;

	for (int i = 1; i < argc; ++i) {
		if (std::string arg = argv[i]; arg == "-o") {
			if (i + 1 >= argc) {
				std::cerr << "Missing output filename after -o\n";
				return 1;
			}
			output_filename = argv[++i];
		} else if (arg == "-d" || arg == "--dot") {
			if (i + 1 >= argc) {
				std::cerr << "Missing dot filename after " << arg << "\n";
				return 1;
			}
			dot_filename = argv[++i];
		} else if (arg == "-v" || arg == "--verbose") {
			verbose = true;
		} else {
			if (!filename.empty()) {
				std::cerr << "Unexpected argument: " << arg << "\n";
				return 1;
			}
			filename = arg;
		}
	}

    std::ifstream file(filename);

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

	if(!program.has_value()) {
		std::cerr << "Parsing error occurred:\n\t" << program.error().message << "\n";

		// Get initial position information safely
		std::cerr << "At: " << program.error().position.line << ":" << program.error().position.column << "\n";

		// Show any additional notes from the error context
		if (!program.error().context.notes.empty()) {
			std::cerr << "Note: ";
			for (const auto& note : program.error().context.notes) {
				std::cerr << note << " ";
			}
			std::cerr << "\n";
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

	ArgonLang::AnalysisVisitor analysis;
	ArgonLang::CodeGenerationVisitor codeGenerator;
	analysis.visit(*program.value());
	ArgonLang::Result<std::string> codeResult = codeGenerator.visit(*program.value());

	if(!codeResult.has_value()) {
		std::cerr << codeResult.error().message << "\n";
		return 1;
	}

	std::string code = codeResult.value();

	std::ofstream codeFile(output_filename);
	codeFile << code;

	if (!dot_filename.empty()) {
		std::ofstream dotFile(argv[2]);
		dotFile << "digraph AST {\n";
		int nodeId = 0;
		program.value()->toDot(dotFile, nodeId);
		dotFile << "}\n";

		std::cout << "\nDOT file generated: ast.dot\n";
	}

	return 0;
}