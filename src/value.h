#pragma once
#include "type.h"
#include <memory>
#include <string>

struct Value {
    std::variant<std::string,
                 float,
                 double,
                 IntegerT,
                 LongT,
                 std::shared_ptr<ClassInstance>,
                 std::unique_ptr<ClassInstance>>
        value;

    // Implement operator =()
};
