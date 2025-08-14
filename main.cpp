#include <iostream>
#include <string>
#include "Error.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantic/ASTNode.h"

Lexer lexer;
std::vector<Token> tokens;
ASTNode* root = nullptr;

int main() {
    // freopen("../testcases_official/Semantic/array05.rx", "r", stdin);
    // freopen("../testcases/Parser/in01.rs", "r", stdin);
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
            if (current_token.type == TokenType::ReservedIntegerLiteral) {
                throw LexError("Lex Error: Invalid Integer");
            }
            for (auto it: current_token.token) {
                if (it == '\r') {
                    rowIndex++;
                }
            }
        } // Lexer

        Parser parser(tokens);
        root = parser.ParseCrate(); // Parser


    } catch (std::exception& error) {
        std::cout << error.what() << '\n';
    }
    delete root;
}