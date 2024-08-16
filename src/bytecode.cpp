#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

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
    lexer.tokens->print();
    cout << endl;

    /* Parse an expression */
    Parser parser = Parser(lexer.tokens);
    shared_ptr<ast::Expression> expr = parser.parse_expression();
    expr->print();
    cout << endl;

    return 0;
}