#ifndef CODE_H
#define CODE_H

#include <vector>

#include "bytecode.hpp"
#include "value.hpp"

namespace runtime {

class Code {
private:
    ssize_t num_variables;
public:
    Code(std::vector<std::unique_ptr<Bytecode>> && bytecodes, ssize_t variables) : num_variables(variables), bytecodes(std::move(bytecodes)) {}
    std::vector<std::unique_ptr<Bytecode>> bytecodes;
    Value run();
    void print();
};

}

#endif