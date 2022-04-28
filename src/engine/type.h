#pragma once

#include <memory>
#include <string>
#include <variant>

class ClassInstance;

using SingleT = float;
using DoubleT = double;
using IntegerT = int;
using LongT = long;
using ClassT = std::shared_ptr<ClassInstance>;

struct StructT : std::unique_ptr<ClassInstance> {
    StructT() = default;
    StructT(StructT &&) = default;
    StructT &operator=(StructT &&) = default;

    StructT(const StructT &other);
    StructT &operator=(const StructT &other);
};

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

using TypeVariantT = std::variant<std::string,
                                  float,
                                  double,
                                  IntegerT,
                                  LongT,
                                  std::shared_ptr<ClassInstance>,
                                  std::unique_ptr<ClassInstance>>;
