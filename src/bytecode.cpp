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
    std::cout << "Lexemes:" << endl;
    lexer.tokens->print();
    std::cout << endl;
    std::cout << endl;

    /* Parse an expression */
    Parser parser = Parser(lexer.tokens);
    shared_ptr<ast::File> file_ast = parser.parse_file();
    std::cout << "AST:" << endl;
    file_ast->print();
    std::cout << endl;

    /* Compile AST to Bytecode */
    BytecodeCompiler compiler;
    file_ast->visit(compiler);
    std::shared_ptr<runtime::Code> code = compiler.code();
    std::cout << "Bytecodes:" << endl;
    code->print();
    std::cout << endl;

    std::cout << "Bytecode of Functions:" << endl;
    for (auto func = compiler.functions.begin(); func != compiler.functions.end(); func++) 
        func->get()->print();
    
    /* TODO: also print bytecode of classes */
    std::cout << "Bytecode of Classes:" << endl;
    for(auto class_struct = compiler.classes.begin(); class_struct != compiler.classes.end(); class_struct++)
        class_struct->get()->print();

    /* execute code */
    runtime::Value result = code->run();
    std::cout << "Ran code with result: " << result.to_string() << endl;

    return 0;
}