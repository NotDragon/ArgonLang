default:
	g++ main.cpp lib/*.cpp -o build/out.exe -Wall -Wno-missing-braces -I ./include -L ./lib/ -std=c++2a
	./build/out.exe

format:
	clang-format -style=file -i *.cpp

generate_tree: 
	dot -Tpng build/ast.dot -o build/ast.png