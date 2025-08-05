#include <iostream>
#include <string>
#include "include/Lexer/Lexer.h"
#include "include/Preprocessor/Preprocessor.h"

Lexer lexer;

int main() {
    freopen("../testcases/Lexer/in04.txt", "r", stdin);
    std::string text, line;
    while(std::getline(std::cin, line)) {
        text += line;
    }
    text += '\r';
    try {
        Preprocessor::RemoveComments(text);
        std::string current_text = text;
        while (!current_text.empty()) {
            Token current_token = lexer.GetNextToken(current_text);
            std::cout << current_token.token << '\n';
        }
    } catch (std::exception& error) {
        std::cout << error.what() << '\n';
    }
}