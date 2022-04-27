#include "token.h"

bool Token::operator==(std::string_view str) {
    if (str.size() != content.size()) {
        return false;
    }
    for (size_t i = 0; i < str.size(); ++i) {
        if (std::tolower(str.at(i)) != std::tolower(content.at(i))) {
            return false;
        }
    }

    return true;
}
