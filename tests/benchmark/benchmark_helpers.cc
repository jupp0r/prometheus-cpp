#include <algorithm>
#include <cstdlib>

#include "benchmark_helpers.h"

std::string generateRandomString(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

std::map<std::string, std::string> generateRandomLabels(
    std::size_t numberOfPairs) {
    const auto labelCharacterCount = 10;
    auto labelPairs = std::map<std::string, std::string>{};
    for (int i = 0; i < numberOfPairs; i++) {
        labelPairs.insert({generateRandomString(labelCharacterCount),
                        generateRandomString(labelCharacterCount)});
    }
    return labelPairs;
}
