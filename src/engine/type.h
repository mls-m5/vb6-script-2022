#pragma once

#include <memory>
#include <string>
#include <variant>

class ClassInstance;
class ClassType;

using SingleT = float;
using DoubleT = double;
using ByteT = uint8_t;
using IntegerT = int;
using LongT = long;
using ClassT = std::shared_ptr<ClassInstance>;

struct StructT {
    std::unique_ptr<ClassInstance> ptr;

    StructT() = delete;

    StructT(ClassType *classType);

    StructT(StructT &&) = default;
    StructT &operator=(StructT &&) = default;

    StructT(const StructT &other);
    StructT &operator=(const StructT &other);

    auto get() {
        return ptr.get();
    }

    auto get() const {
        return ptr.get();
    }

    auto operator->() {
        return ptr.get();
    }
};

struct Type {
    enum TypeName {
        Integer,
        Long,
        Byte,
        Single,
        Double,
        String,
        Struct,
        Class,
    };

    TypeName type = Integer;

    ClassType *classType = nullptr;
};

using TypeVariantT = std::variant<IntegerT,
                                  LongT,
                                  ByteT,
                                  float,
                                  double,
                                  std::string,
                                  StructT,
                                  std::shared_ptr<ClassInstance>>;
