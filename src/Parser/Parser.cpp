#include "../../include/Parser/Parser.h"
#include "../../Error/error.h"

void Parser::ConsumeString(const std::string& str) {
    if (tokens[parseIndex].token == str) {
        parseIndex++;
    } else {
        throw ParseError("Cannot Match " + str, tokens[parseIndex].pos);
    }
}

CrateNode* Parser::ParseCrate() {
    Position pos = tokens[parseIndex].pos;
    std::vector<VisItemNode*> items;
    try {
        while (parseIndex != tokens.size()) {
            auto* tmp = ParseVisItem();
            items.emplace_back(tmp);
        }
        return new CrateNode(pos, items);
    } catch (std::exception&) {
        for (const auto& it: items) {
            delete it;
        }
        throw;
    }
}

VisItemNode* Parser::ParseVisItem() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    ASTNode* node = nullptr;
    try {
        node = ParseFunction();
        return new VisItemNode(pos, node);
    } catch (std::exception&) {
        parseIndex = start;
        throw;
    }
    // TODO Parse Other Items
}

FunctionNode* Parser::ParseFunction() {
    Position pos = tokens[parseIndex].pos;
    std::vector<GenericParamNode*> generic_param_nodes;
    FunctionParametersNode* function_parameters_node = nullptr;
    TypeNode* type_node = nullptr;
    std::vector<WhereClauseItemNode*> where_clause_item_nodes;
    BlockExpressionNode* block_expression_node = nullptr;
    try {
        bool is_const = false;
        if (tokens[parseIndex].type == TokenType::Const) {
            is_const = true;
            parseIndex++;
        }
        ConsumeString("fn");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex].token;
        parseIndex++;

        // TODO ParseGenericParams

        ConsumeString("(");

        // TODO ParseFunctionParameters

        ConsumeString(")");

        // TODO ParseFunctionReturnType

        // TODO ParseWhereClause
        if (tokens[parseIndex].type != TokenType::Semicolon) {
            block_expression_node = ParseBlockExpression();
        } else {
            ConsumeString(";");
        }
        return new FunctionNode(pos, is_const, identifier, generic_param_nodes,
            function_parameters_node, type_node, where_clause_item_nodes, block_expression_node);
    } catch (std::exception&) {
        throw;
    }
}

BlockExpressionNode* Parser::ParseBlockExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        StatementsNode* node = nullptr;
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
             node = ParseStatements();
        }
        ConsumeString("}");
        return new BlockExpressionNode(pos, node);
    } catch (std::exception&) {
        throw;
    }
}

StatementsNode *Parser::ParseStatements() {
    Position pos = tokens[parseIndex].pos;
    std::vector<StatementNode*> statement_nodes;
    ExpressionNode* expression_node = nullptr;
    while (tokens[parseIndex].type != TokenType::RBrace) {
        StatementNode* tmp;
        try {
            tmp = ParseStatement();
            statement_nodes.emplace_back(tmp);
            continue;
        } catch (std::exception&) {}
        try {
            expression_node = ParseExpressionWithoutBlock();
            break;
        } catch (std::exception&) {
            for (auto& it: statement_nodes) {
                delete it;
            }
            throw;
        }
    }
    return new StatementsNode(pos, statement_nodes, expression_node);
}

StatementNode* Parser::ParseStatement() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
            return new EmptyStatementNode(pos);
        }
        if (tokens[parseIndex].type == TokenType::Let) {

        }
    } catch (std::exception&) {

    }
}

ExpressionNode* Parser::ParseExpressionWithoutBlock() {

}
