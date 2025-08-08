#include <iostream>
#include <string>
#include <chrono>
#include "include/Lexer/Lexer.h"

Lexer lexer;
std::vector<Token> tokens;

int main() {
    auto start = std::chrono::steady_clock::now();
    freopen("../testcases/Lexer/in10.rs", "r", stdin);
    // freopen("../output.txt", "w", stdout);
    std::string text, line;
    while(std::getline(std::cin, line)) {
        text += line;
    }
    text += '\r';
    try {
        std::string current_text = text;
        uint32_t rowIndex = 1;
        while (!current_text.empty()) {
            Token current_token = lexer.GetNextToken(current_text);
            if (current_token.type != TokenType::WhiteSpace &&
                current_token.type != TokenType::BlockComment &&
                current_token.type != TokenType::LineComment) {
                current_token.putPosValue(Position{rowIndex});
                tokens.push_back(current_token);
                std::cout << current_token.pos.GetRow() << " : " << current_token.token << '\n';
            }
            for (auto it: current_token.token) {
                if (it == '\r') {
                    rowIndex++;
                }
            }
        } // Lexer



    } catch (std::exception& error) {
        std::cout << error.what() << '\n';
    }
    auto end = std::chrono::steady_clock::now();
    auto dur = end - start;
    std::cout << "Run time : " << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() << " ms \n";
}