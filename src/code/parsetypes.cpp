#include "parsetypes.h"
#include <array>

const Type *parseType(Token::Keyword type) {
    using namespace std::literals;

#define PAIR(x)                                                                \
    std::pair {                                                                \
        Token::x, Type {                                                       \
            Type::x                                                            \
        }                                                                      \
    }

    static constexpr auto names = std::array{
        PAIR(Integer),
        PAIR(Long),
        PAIR(Byte),
        PAIR(Single),
        PAIR(Double),
        PAIR(String),
        std::pair{Token::Boolean, Type{Type::Integer}},
    };

    for (auto &s : names) {
        if (s.first == type) {
            return &s.second;
        }
    }

    return nullptr;
}
