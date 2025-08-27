#include <iostream>
#include <string>
#include "Error.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantic/ASTNode.h"
#include "Semantic/ASTVisitor.h"
#include "Semantic/ConstEvaluator.h"
#include "Semantic/SemanticChecker.h"
#include "Semantic/SymbolCollector.h"
#include "Semantic/SymbolManager.h"


Lexer lexer;
std::vector<Token> tokens;
std::shared_ptr<ASTNode> root = nullptr;
ScopeManager scope_manager;
SymbolCollector *symbol_collector = new SymbolCollector{scope_manager};
ConstEvaluator *const_evaluator = new ConstEvaluator{scope_manager};
SemanticChecker *semantic_checker = new SemanticChecker{scope_manager};
SymbolManager *symbol_manager = new SymbolManager{scope_manager};

int main() {
    // freopen("../stdin.txt", "r", stdin);
    // freopen("../testcases_official/semantic-1/return2/return2.rx", "r", stdin);
    // freopen("../testcases/Semantic/in03.rx", "r", stdin);
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string text((std::istreambuf_iterator(std::cin)),
                     std::istreambuf_iterator<char>());
    if (text.empty() || text.back() != '\n') {
        text += '\n';
    }
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

        root->accept(symbol_collector);
        root->accept(const_evaluator);
        root->accept(symbol_manager);
        root->accept(semantic_checker);
        std::cout << 0 << '\r';
    } catch (std::exception &error) {
        std::cout << error.what() << '\n';
        std::cout << -1 << '\r';
    }
}
