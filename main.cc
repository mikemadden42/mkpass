#include <charconv>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <system_error>

// https://stackoverflow.com/questions/47977829/generate-a-random-string-in-c11
// g++ -std=c++17 -Wall -Wextra -pedantic -o mkpass main.cc

constexpr size_t min_length = 12;
constexpr size_t max_length = 1024;

std::string random_string(size_t size) {
    static constexpr std::string_view alphabet =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);

    std::string str;
    str.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        str += alphabet[dist(rd)];
    }

    return str;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " length (" << min_length << "-"
                  << max_length << ")" << std::endl;
        exit(1);
    }

    const std::string_view arg(argv[1]);
    const char *const arg_end = arg.data() + arg.size();

    size_t password_length = 0;
    const auto [ptr, ec] =
        std::from_chars(arg.data(), arg_end, password_length);

    if (ec == std::errc::invalid_argument || ptr != arg_end) {
        std::cerr << "Length must be a whole number: " << arg << std::endl;
        exit(1);
    }
    if (ec == std::errc::result_out_of_range || password_length < min_length ||
        password_length > max_length) {
        std::cerr << "Length must be between " << min_length << " and "
                  << max_length << ": " << arg << std::endl;
        exit(1);
    }

    std::cout << random_string(password_length) << std::endl;
}
