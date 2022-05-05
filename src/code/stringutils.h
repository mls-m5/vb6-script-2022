#pragma once

#include <algorithm>
#include <string>

// https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
bool iequals(std::string_view a, std::string_view b) {
    return std::equal(
        a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) {
            return tolower(a) == tolower(b);
        });
}
