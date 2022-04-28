#include "type.h"
#include "classinstance.h"

StructT::StructT(const StructT &other)
    : ptr{other.ptr->clone()} {}

StructT::StructT(ClassType *classType) {
    ptr = ClassInstance::create(classType);
}

StructT &StructT::operator=(const StructT &other) {
    ptr = other.ptr->clone();
    return *this;
}
