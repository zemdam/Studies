#include <iostream>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string myString;
    std::cin >> myString;

    char lastChar = ' ';
    size_t lastIndex = 0;
    size_t min = SIZE_MAX;

    for (size_t i = 0; i < myString.size(); i++) {
        if (myString[i] != '*') {
            if (lastChar != ' ' && lastChar != myString[i]) {
                min = std::min(min, i - 1 - lastIndex);
            }
            lastChar = myString[i];
            lastIndex = i;
        }
    }

    if (min == SIZE_MAX) {
        std::cout << 1 << "\n";

        return 0;
    }

    std::cout << myString.size() - min << "\n";

    return 0;
}
