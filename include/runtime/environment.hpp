#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector>

#include "value.hpp"

namespace runtime {

class Environment {
public:
    std::vector<Value> stack;
    std::unique_ptr<Environment> child;
    std::weak_ptr<Environment> parent;
    Value pop();
    void push(Value v);
    bool is_empty();
};

}

#endif