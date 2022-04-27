#pragma once

#include <memory>
#include <string>
#include <variant>

using IntegerT = int;
using LongT = long;

struct Type {
    enum TypeName {
        String,
        Single,
        Double,
        Integer,
        Long,
        Class,
        Struct,
    };

    TypeName type = Integer;
};

class ClassInstance;

using TypeVariantT = std::variant<std::string,
                                  float,
                                  double,
                                  IntegerT,
                                  LongT,
                                  std::shared_ptr<ClassInstance>,
                                  std::unique_ptr<ClassInstance>>;
