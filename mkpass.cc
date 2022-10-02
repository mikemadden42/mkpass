#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>

// https://stackoverflow.com/questions/47977829/generate-a-random-string-in-c11
// g++ -std=c++17 -Wall -Wextra -pedantic -o mkpass mkpass.cc

std::string random_string(int size) {
    std::string str(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, size);
}

int main(int argc, char *argv[]) {
    size_t password_length;
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " password length" << std::endl;
        exit(1);
    } else {
        password_length = std::strtol(argv[1], nullptr, 10);
        if (password_length == 0L) {
            std::cout << "Cannot convert argument to number: " << argv[1]
                      << std::endl;
            exit(1);
        }
    }

    std::cout << random_string(password_length) << std::endl;
}
