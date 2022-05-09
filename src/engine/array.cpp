#include "array.h"
#include "classinstance.h"
#include "value.h"

ArrayT::ArrayT() = default;
ArrayT::ArrayT(const ArrayT &) = default;
ArrayT &ArrayT::operator=(const ArrayT &) = default;
ArrayT::ArrayT(ArrayT &&) = default;
ArrayT &ArrayT::operator=(ArrayT &&) = default;

ArrayT::~ArrayT() = default;

ArrayT::ArrayT(Type type, int upperBound, int lowerBound) {
    values.resize(upperBound - lowerBound, Value::create(type));
    lowerBound = lowerBound;
}

Value &ArrayT::at(int index) {
    return values.at(index - lowerBound);
}

const Value &ArrayT::at(int index) const {
    return values.at(index);
}

int ArrayT::ubound() const {
    return values.size() + lowerBound;
}

int ArrayT::lbound() const {
    return lowerBound;
}

void ArrayT::redim(int ubound) {
    values.resize(ubound - lbound(), Value::create(type));
}
