#include <algorithm>
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

int main() { std::cout << random_string(24) << std::endl; }
