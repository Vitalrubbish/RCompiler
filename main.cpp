#include <iostream>
#include <string>
#include "Error.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantic/ASTNode.h"
#include "Semantic/ASTVisitor.h"
#include "Semantic/SemanticChecker.h"
#include "Semantic/SymbolCollector.h"

Lexer lexer;
std::vector<Token> tokens;
ASTNode *root = nullptr;
ScopeManager scope_manager;
SymbolCollector* symbol_collector = new SymbolCollector{scope_manager};
SemanticChecker* semantic_checker = new SemanticChecker{scope_manager};

int main() {
    // freopen("../testcases_official/Semantic/array05.rx", "r", stdin);
    freopen("../testcases/Parser/in02.rx", "r", stdin);
    std::string text, line;
    while (std::getline(std::cin, line)) {
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
                // std::cout << current_token.pos.GetRow() << " : " << current_token.token << '\n';
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

        root -> accept(semantic_checker);
        // std::cout << -1;
    } catch (std::exception &error) {
        std::cout << error.what() << '\n';
        // std::cout << 0;
    }
    delete root;
}
