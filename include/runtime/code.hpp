#ifndef CODE_H
#define CODE_H

#include <vector>

#include "bytecode.hpp"
#include "value.hpp"

namespace runtime {

class Code {
public:
    Code(std::vector<std::unique_ptr<Bytecode>> && bytecodes) : bytecodes(std::move(bytecodes)) {}
    std::vector<std::unique_ptr<Bytecode>> bytecodes;
    Value run();
    void print();
};

}

#endif