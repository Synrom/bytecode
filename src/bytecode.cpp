#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "runtime/compile.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <memory>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: program <file_to_read>\n");
        return 1;
    }

    ifstream file(argv[1]);
    shared_ptr<string> code_string = shared_ptr<string>(new string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));

    /* Lex program and print tokens */
    Lexer lexer(code_string, argv[1]);
    lexer.parse();
    cout << "Lexemes:" << endl;
    lexer.tokens->print();
    cout << endl;
    cout << endl;

    /* Parse an expression */
    Parser parser = Parser(lexer.tokens);
    vector<shared_ptr<ast::Statement>> block = parser.parse_block();
    cout << "AST:" << endl;
    for(auto stmt = block.begin(); stmt != block.end(); stmt++) {
        stmt->get()->print();
    }
    cout << endl;

    /* Compile AST to Bytecode */
    BytecodeCompiler compiler;
    compiler.compile(block);
    runtime::Code code = compiler.code();
    cout << "Bytecodes:" << endl;
    code.print();
    cout << endl;

    /* execute code */
    runtime::Value result = code.run();
    cout << "Ran code with result: " << result.to_string() << endl;

    return 0;
}