#pragma once

#include "type.h"
#include <vector>

class Value;

class ArrayT {
    static constexpr int standardLBound = 0;

    std::vector<Value> values;
    int lowerBound = 1;
    Type type;

public:
    ArrayT(Type type,
           int upperBound = standardLBound,
           int lowerBound = standardLBound);

    ArrayT();
    ArrayT(const ArrayT &);
    ArrayT &operator=(const ArrayT &);
    ArrayT(ArrayT &&);
    ArrayT &operator=(ArrayT &&);
    ~ArrayT();

    Value &at(int index);
    const Value &at(int index) const;

    int ubound() const;
    int lbound() const;

    void redim(int ubound);
};
