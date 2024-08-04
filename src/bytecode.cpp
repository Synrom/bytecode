#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <memory>
#include <string.h>
#include "lexer/lexer.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: program <file_to_read>");
        return 1;
    }

    ifstream file(argv[1]);
    shared_ptr<string> code_string = shared_ptr<string>(new string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));

    /* Lex program and print tokens */
    Lexer lexer(code_string);
    lexer.parse();
    lexer.tokens.print();

    return 0;
}