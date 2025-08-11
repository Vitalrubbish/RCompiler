#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <unordered_map>
#include <cctype>

inline long long StringToInt(const std::string& literal) {
    int32_t base = 10;
    uint32_t start_index = 0;
    if (literal.length() >= 2 && literal[0] == '0') {
        char second_char = literal[1];
        if (second_char == 'b') {
            base = 2;
            start_index = 2;
        } else if (second_char == 'o') {
            base = 8;
            start_index = 2;
        } else if (second_char == 'x') {
            base = 16;
            start_index = 2;
        }
    }
    uint32_t end_index = literal.length();
    uint32_t suffix_pos = literal.find_first_of("ui", start_index);
    if (suffix_pos != std::string::npos) {
        end_index = suffix_pos;
    }
    long long result = 0;
    for (uint32_t i = start_index; i < end_index; i++) {
        char c = literal[i];
        if (c == '_') {
            continue;
        }
        int digit_value = 0;
        char lower_c = static_cast<char>(std::tolower(c));
        if (lower_c >= '0' && lower_c <= '9') {
            digit_value = lower_c - '0';
        } else if (lower_c >= 'a' && lower_c <= 'f') {
            digit_value = 10 + (lower_c - 'a');
        }
        result = result * base + digit_value;
    }
    return result;
}


// Helper functions for starts_with/ends_with
inline bool starts_with(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() &&
           str.compare(0, prefix.size(), prefix) == 0;
}

inline bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Unescape Rust string literals
inline std::string unescape_rust_string(const std::string& input) {
    std::string result;
    result.reserve(input.length());

    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '\\') {
            if (i + 1 >= input.length()) {
                result += '\\';
                continue;
            }

            static const std::unordered_map<char, char> escapes = {
                {'n', '\n'}, {'r', '\r'}, {'t', '\t'},
                {'\\', '\\'}, {'\'', '\''}, {'\"', '\"'},
                {'0', '\0'}
            };

            char next = input[i+1];
            if (escapes.count(next)) {
                result += escapes.at(next);
                i++;
            } else if (next == 'x' && i + 3 < input.length() &&
                      isxdigit(input[i+2]) && isxdigit(input[i+3])) {
                char hex[3] = {input[i+2], input[i+3], '\0'};
                result += static_cast<char>(strtol(hex, nullptr, 16));
                i += 3;
            } else {
                result += input[i];
            }
        } else {
            result += input[i];
        }
    }
    return result;
}

// For string literals (returns std::string)
inline std::string rust_str_to_cpp(const std::string& token) {
    if (token.empty()) return "";

    // Check for string literal types
    if (starts_with(token, "b\"") || starts_with(token, "c\"") ||
        starts_with(token, "cr") || token[0] == '\"') {
        // Remove prefix if exists
        size_t start = (token[0] != '\"') ? 1 : 0;
        if (token.length() < start + 2 || token[start] != '\"' || token.back() != '\"') {
            return token;
        }
        std::string content = token.substr(start + 1, token.length() - start - 2);
        return unescape_rust_string(content);
    }
    else if (starts_with(token, "r") && (token[1] == '#' || token[1] == '\"')) {
        // Raw string literal
        size_t start_hash = token.find('"') + 1;
        size_t end_hash = token.rfind('"') - 1;

        if (token.size() > 2 && token[1] == '#') {
            size_t hash_count = 1;
            while (token[start_hash-2-hash_count] == '#') {
                hash_count++;
            }
            return token.substr(start_hash, end_hash - start_hash + 1);
        }
        return token.substr(2, token.length() - 3);
    }

    return token; // Not a string literal
}

// For char literals (returns char)
inline char rust_char_to_cpp(const std::string& token, char default_char = '\0') {
    if (token.empty()) return default_char;

    // Check for char literal types
    if (token[0] == '\'' && token.back() == '\'') {
        if (token.length() < 3) return default_char;
        std::string content = token.substr(1, token.length() - 2);
        std::string unescaped = unescape_rust_string(content);
        return unescaped.empty() ? default_char : unescaped[0];
    }
    else if (starts_with(token, "b\'")) {
        if (token.length() < 4) return default_char;
        std::string content = token.substr(2, token.length() - 3);
        std::string unescaped = unescape_rust_string(content);
        return unescaped.empty() ? default_char : static_cast<char>(unescaped[0]);
    }

    return default_char; // Not a char literal
}
#endif //UTIL_H
