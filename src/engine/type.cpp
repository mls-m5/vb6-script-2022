#include "type.h"
#include "classinstance.h"

StructT::StructT(const StructT &other)
    : unique_ptr<ClassInstance>{other->clone()} {}

StructT &StructT::operator=(const StructT &other) {
    this->unique_ptr<ClassInstance>::operator=(other->clone());
    return *this;
}
