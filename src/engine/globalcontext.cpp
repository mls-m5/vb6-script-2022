#include "globalcontext.h"
#include "classinstance.h"
#include "classtype.h"

GlobalContext::~GlobalContext() = default;

Value GlobalContext::err() const {
    return _err;
}

void GlobalContext::err(Value value) {
    _err = std::move(value);
}

void GlobalContext::end() {
    if (!_disableEnd) {
        std::exit(0);
    }
}

void GlobalContext::disableEnd(bool value) {
    _disableEnd = value;
}

GlobalContext::GlobalContext()
    : nothingType{std::make_unique<ClassType>()} {}
