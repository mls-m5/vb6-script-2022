#include "functionbody.h"
#include "classinstance.h"

void FunctionBody::call(const FunctionArgumentValues &args,
                        LocalContext &context) const {
    auto local = LocalContext{context.globalContext, localVariables, args};
}
