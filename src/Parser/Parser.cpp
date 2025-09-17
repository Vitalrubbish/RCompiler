#include "Parser/Parser.h"
#include "Error.h"
#include "Util.h"
#include <memory>

void Parser::ConsumeString(const std::string &str) {
    if (parseIndex < tokens.size() && tokens[parseIndex].token == str) {
        parseIndex++;
    } else {
        throw ParseError("Parse Error: Cannot Match :" + str, tokens[parseIndex].pos);
    }
}

/****************  Items  ****************/
std::shared_ptr<CrateNode> Parser::ParseCrate() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<VisItemNode>> items;
    while (parseIndex != tokens.size()) {
        auto tmp = ParseVisItem();
        items.emplace_back(tmp);
    }
    return std::make_shared<CrateNode>(pos, std::move(items));
}

std::shared_ptr<VisItemNode> Parser::ParseVisItem() {
    try {
        if (tokens[parseIndex].type == TokenType::Fn ||
            (parseIndex < tokens.size() - 1 && tokens[parseIndex + 1].type == TokenType::Fn)) {
            return ParseFunction();
        }
        if (tokens[parseIndex].type == TokenType::Const) {
            return ParseConstantItem();
        }
        if (tokens[parseIndex].type == TokenType::Struct) {
            return ParseStruct();
        }
        if (tokens[parseIndex].type == TokenType::Enum) {
            return ParseEnumeration();
        }
        if (tokens[parseIndex].type == TokenType::Impl) {
            return ParseImplementation();
        }
        return ParseTrait();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<FunctionNode> Parser::ParseFunction() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<FunctionParametersNode> function_parameters_node = nullptr;
    std::shared_ptr<TypeNode> type_node = nullptr;
    std::shared_ptr<BlockExpressionNode> block_expression_node = nullptr;
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
        ConsumeString("(");
        if (tokens[parseIndex].type != TokenType::RParen) {
            function_parameters_node = ParseFunctionParameters();
        }
        ConsumeString(")");
        if (tokens[parseIndex].type == TokenType::RArrow) {
            type_node = ParseFunctionReturnType();
        }
        if (tokens[parseIndex].type != TokenType::Semicolon) {
            block_expression_node = ParseBlockExpression();
        } else {
            ConsumeString(";");
        }
        return std::make_shared<FunctionNode>(pos, is_const, identifier, function_parameters_node,
                                type_node, block_expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<TypeNode> Parser::ParseFunctionReturnType() {
    try {
        ConsumeString("->");
        return ParseType();
    } catch (const ParseError &) {
        throw;
    }
}


std::shared_ptr<FunctionParametersNode> Parser::ParseFunctionParameters() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<FunctionParamNode>> function_param_nodes;
    std::shared_ptr<SelfParamNode> self_param_node;
    try {
        if (tokens[parseIndex].type == TokenType::Self || tokens[parseIndex + 1].type == TokenType::Self ||
            tokens[parseIndex + 2].type == TokenType::Self) {
            self_param_node = ParseSelfParamNode();
            if (tokens[parseIndex].type == TokenType::RParen) {
                return std::make_shared<FunctionParametersNode>(pos, self_param_node, function_param_nodes);
            }
        } else {
            function_param_nodes.emplace_back(ParseFunctionParam());
        }
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::RParen) {
                break;
            }
            function_param_nodes.emplace_back(ParseFunctionParam());
        }
        return std::make_shared<FunctionParametersNode>(pos, self_param_node, std::move(function_param_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<FunctionParamNode> Parser::ParseFunctionParam() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PatternNoTopAltNode> function_param_pattern_node = nullptr;
    std::shared_ptr<TypeNode> type_node = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::DotDotDot) {
            ConsumeString("...");
            return std::make_shared<FunctionParamNode>(pos, function_param_pattern_node, type_node, true);
        }
        function_param_pattern_node = ParsePatternNoTopAlt();
        ConsumeString(":");
        type_node = ParseType();
        return std::make_shared<FunctionParamNode>(pos, function_param_pattern_node, type_node, false);
    } catch (const ParseError &) {
        throw ParseError("Parse Error: Failed to Match FunctionParam", pos);
    }
}

std::shared_ptr<SelfParamNode> Parser::ParseSelfParamNode() {
    Position pos = tokens[parseIndex].pos;
    try {
        bool is_mut = false;
        bool have_and = false;
        if (tokens[parseIndex].type == TokenType::And) {
            ConsumeString("&");
            have_and = true;
        }
        if (tokens[parseIndex].type == TokenType::Mut) {
            ConsumeString("mut");
            is_mut = true;
        }
        ConsumeString("self");
        if (have_and || tokens[parseIndex].type != TokenType::Colon) {
            return std::make_shared<ShortHandSelfNode>(pos, have_and, is_mut);
        }
        ConsumeString(":");
        auto type_node = ParseType();
        return std::make_shared<TypedSelfNode>(pos, is_mut, type_node);
    } catch (std::exception&) {
        throw;
    }
}


std::shared_ptr<FunctionParamPatternNode> Parser::ParseFunctionParamPattern() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt_node = nullptr;
    std::shared_ptr<TypeNode> type_node = nullptr;
    try {
        pattern_no_top_alt_node = ParsePatternNoTopAlt();
        ConsumeString(":");
        if (tokens[parseIndex].type == TokenType::DotDotDot) {
            ConsumeString("...");
            return std::make_shared<FunctionParamPatternNode>(pos, pattern_no_top_alt_node, type_node, true);
        }
        type_node = ParseType();
        return std::make_shared<FunctionParamPatternNode>(pos, pattern_no_top_alt_node, type_node, false);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StructNode> Parser::ParseStruct() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<StructFieldNode>> struct_field_nodes;
    try {
        ConsumeString("struct");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex].token;
        parseIndex++;
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
            return std::make_shared<StructNode>(pos, identifier, std::move(struct_field_nodes));
        }
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
            struct_field_nodes.emplace_back(ParseStructFieldNode());
            while (tokens[parseIndex].type == TokenType::Comma) {
                ConsumeString(",");
                if (tokens[parseIndex].type == TokenType::RBrace) {
                    break;
                }
                struct_field_nodes.emplace_back(ParseStructFieldNode());
            }
        }
        ConsumeString("}");
        return std::make_shared<StructNode>(pos, identifier, std::move(struct_field_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StructFieldNode> Parser::ParseStructFieldNode() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        ConsumeString(":");
        auto type_node = ParseType();
        return std::make_shared<StructFieldNode>(pos, identifier, type_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<EnumerationNode> Parser::ParseEnumeration() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<EnumVariantNode>> enum_variant_nodes;
    try {
        ConsumeString("enum");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        ConsumeString("{");
        if (tokens[parseIndex].type == TokenType::RBrace) {
            ConsumeString("}");
            return std::make_shared<EnumerationNode>(pos, identifier, std::move(enum_variant_nodes));
        }
        enum_variant_nodes.emplace_back(ParseEnumVariant());
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
            enum_variant_nodes.emplace_back(ParseEnumVariant());
        }
        ConsumeString("}");
        return std::make_shared<EnumerationNode>(pos, identifier, std::move(enum_variant_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<EnumVariantNode> Parser::ParseEnumVariant() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<EnumVariantStructNode> enum_variant_struct_node = nullptr;
    std::shared_ptr<EnumVariantDiscriminantNode> enum_variant_discriminant_node = nullptr;
    try {
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        if (tokens[parseIndex].type == TokenType::LBrace) {
            enum_variant_struct_node = ParseEnumVariantStruct();
        }
        if (tokens[parseIndex].type == TokenType::Eq) {
            enum_variant_discriminant_node = ParseEnumVariantDiscriminant();
        }
        return std::make_shared<EnumVariantNode>(pos, identifier, enum_variant_struct_node,
                                   enum_variant_discriminant_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<EnumVariantStructNode> Parser::ParseEnumVariantStruct() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<StructFieldNode>> struct_field_nodes;
    try {
        ConsumeString("{");
        struct_field_nodes.emplace_back(ParseStructFieldNode());
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
            struct_field_nodes.emplace_back(ParseStructFieldNode());
        }
        ConsumeString("}");
        return std::make_shared<EnumVariantStructNode>(pos, std::move(struct_field_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<EnumVariantDiscriminantNode> Parser::ParseEnumVariantDiscriminant() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("=");
        auto expression_node = ParseExpression();
        return std::make_shared<EnumVariantDiscriminantNode>(pos, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ConstantItemNode> Parser::ParseConstantItem() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<TypeNode> type_node = nullptr;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    try {
        bool is_underscore = false;
        std::string identifier;
        ConsumeString("const");
        if (tokens[parseIndex].type == TokenType::Underscore) {
            is_underscore = true;
            parseIndex++;
        } else {
            if (tokens[parseIndex].type != TokenType::Identifier) {
                throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
            }
            identifier = tokens[parseIndex++].token;
        }
        ConsumeString(":");
        type_node = ParseType();
        if (tokens[parseIndex].type == TokenType::Eq) {
            ConsumeString("=");
            expression_node = ParseExpression();
        }
        ConsumeString(";");
        return std::make_shared<ConstantItemNode>(pos, identifier, is_underscore, type_node, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<AssociatedItemNode> Parser::ParseAssociatedItem() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<ConstantItemNode> constant_item_node = nullptr;
    std::shared_ptr<FunctionNode> function_node = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::Const) {
            constant_item_node = ParseConstantItem();
        } else {
            function_node = ParseFunction();
        }
        return std::make_shared<AssociatedItemNode>(pos, constant_item_node, function_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ImplementationNode> Parser::ParseImplementation() {
    uint32_t start = parseIndex;
    try {
        return ParseInherentImpl();
    } catch (const ParseError &e) {
        // std::cout << e.what() << '\n';
        parseIndex = start;
    }

    try {
        return ParseTraitImpl();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<InherentImplNode> Parser::ParseInherentImpl() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<AssociatedItemNode>> associated_item_nodes;
    try {
        ConsumeString("impl");
        auto type_node = ParseType();
        ConsumeString("{");
        while (tokens[parseIndex].type != TokenType::RBrace) {
            associated_item_nodes.emplace_back(ParseAssociatedItem());
        }
        ConsumeString("}");
        return std::make_shared<InherentImplNode>(pos, type_node, std::move(associated_item_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<TraitImplNode> Parser::ParseTraitImpl() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<AssociatedItemNode>> associated_item_nodes;
    try {
        ConsumeString("impl");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        ConsumeString("for");
        auto type_node = ParseType();
        ConsumeString("{");
        while (tokens[parseIndex].type != TokenType::RBrace) {
            associated_item_nodes.emplace_back(ParseAssociatedItem());
        }
        ConsumeString("}");
        return std::make_shared<TraitImplNode>(pos, identifier, type_node, std::move(associated_item_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<TraitNode> Parser::ParseTrait() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<AssociatedItemNode>> associated_item_nodes;
    try {
        ConsumeString("trait");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        ConsumeString("{");
        while (tokens[parseIndex].type != TokenType::RBrace) {
            associated_item_nodes.emplace_back(ParseAssociatedItem());
        }
        ConsumeString("}");
        return std::make_shared<TraitNode>(pos, identifier, std::move(associated_item_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

/****************  Expression  ****************/
std::shared_ptr<ExpressionNode> Parser::ParseExpression() {
    uint32_t start = parseIndex;
    try {
        return ParseExpressionWithoutBlock();
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        return ParseExpressionWithBlock();
    } catch (const ParseError &) {
        throw;
    }
}

/****************  Expression With Block  ****************/
std::shared_ptr<ExpressionNode> Parser::ParseExpressionWithBlock() {
    try {
        if (tokens[parseIndex].type == TokenType::Const) {
            return ParseConstBlockExpression();
        }
        if (tokens[parseIndex].type == TokenType::Loop) {
            return ParseInfiniteLoopExpression();
        }
        if (tokens[parseIndex].type == TokenType::While) {
            return ParsePredicateLoopExpression();
        }
        if (tokens[parseIndex].type == TokenType::If) {
            return ParseIfExpression();
        }
        if (tokens[parseIndex].type == TokenType::Match) {
            return ParseMatchExpression();
        }
        return ParseBlockExpression();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<BlockExpressionNode> Parser::ParseBlockExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        std::shared_ptr<StatementsNode> node = nullptr;
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
            node = ParseStatements();
        }
        ConsumeString("}");
        return std::make_shared<BlockExpressionNode>(pos, false, node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<BlockExpressionNode> Parser::ParseConstBlockExpression() {
    Position pos = tokens[parseIndex].pos;
    ConsumeString("const");
    try {
        std::shared_ptr<StatementsNode> node = nullptr;
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
            node = ParseStatements();
        }
        ConsumeString("}");
        return std::make_shared<BlockExpressionNode>(pos, true, node);
    } catch (const ParseError &) {
        throw;
    }
}


std::shared_ptr<InfiniteLoopExpressionNode> Parser::ParseInfiniteLoopExpression() {
    Position pos = tokens[parseIndex].pos;
    ConsumeString("loop");
    try {
        auto node = ParseBlockExpression();
        return std::make_shared<InfiniteLoopExpressionNode>(pos, node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<PredicateLoopExpressionNode> Parser::ParsePredicateLoopExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("while");
        auto conditions = ParseConditions();
        auto node = ParseBlockExpression();
        return std::make_shared<PredicateLoopExpressionNode>(pos, conditions, node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<IfExpressionNode> Parser::ParseIfExpression() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<ConditionsNode> conditions_node = nullptr;
    std::shared_ptr<BlockExpressionNode> true_block_expression_node = nullptr;
    std::shared_ptr<BlockExpressionNode> false_block_expression_node = nullptr;
    std::shared_ptr<IfExpressionNode> if_expression_node = nullptr;
    try {
        ConsumeString("if");
        conditions_node = ParseConditions();
        true_block_expression_node = ParseBlockExpression();
        if (tokens[parseIndex].type == TokenType::Else) {
            ConsumeString("else");
            if (tokens[parseIndex].type == TokenType::If) {
                if_expression_node = ParseIfExpression();
            } else {
                false_block_expression_node = ParseBlockExpression();
            }
        }
        return std::make_shared<IfExpressionNode>(pos, conditions_node, true_block_expression_node,
                                    false_block_expression_node, if_expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<MatchExpressionNode> Parser::ParseMatchExpression() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    std::shared_ptr<MatchArmsNode> match_arms_node = nullptr;
    try {
        ConsumeString("match");
        expression_node = ParseExpression(); // TODO Check whether it is a StructExpression
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
            match_arms_node = ParseMatchArms();
        }
        ConsumeString("}");
        return std::make_shared<MatchExpressionNode>(pos, expression_node, match_arms_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<MatchArmsNode> Parser::ParseMatchArms() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<MatchArmNode>> match_arms_nodes;
    std::vector<std::shared_ptr<ExpressionNode>> expression_nodes;
    try {
        while (tokens[parseIndex].type != TokenType::RBrace) {
            match_arms_nodes.emplace_back(ParseMatchArm());
            ConsumeString("=>");
            expression_nodes.emplace_back(ParseExpression());
            if (std::dynamic_pointer_cast<BlockExpressionNode>(expression_nodes.back()) != nullptr) {
                if (tokens[parseIndex].type == TokenType::Comma) {
                    ConsumeString(",");
                }
            } else {
                if (tokens[parseIndex].type == TokenType::Comma) {
                    ConsumeString(",");
                } else {
                    break;
                }
            }
        }
        return std::make_shared<MatchArmsNode>(pos, std::move(match_arms_nodes), std::move(expression_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<MatchArmNode> Parser::ParseMatchArm() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PatternNode> pattern_node = nullptr;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    try {
        pattern_node = ParsePattern();
        if (tokens[parseIndex].type == TokenType::If) {
            ConsumeString("if");
            expression_node = ParseExpression();
        }
        return std::make_shared<MatchArmNode>(pos, pattern_node, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

/****************  Expression Without Block  ****************/
std::shared_ptr<ExpressionNode> Parser::ParseExpressionWithoutBlock() {
    Position pos = tokens[parseIndex].pos;
    if (tokens[parseIndex].type == TokenType::Continue) {
        ConsumeString("continue");
        return std::make_shared<ContinueExpressionNode>(pos);
    }
    try {
        return ParseJumpExpression();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseTupleExpression() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<ExpressionNode>> expression_nodes;
    try {
        ConsumeString("(");
        while (tokens[parseIndex].token != ")") {
            expression_nodes.emplace_back(ParseExpression());
        }
        ConsumeString(")");
        return std::make_shared<TupleExpressionNode>(pos, std::move(expression_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseJumpExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        TokenType type;
        if (tokens[parseIndex].type == TokenType::Break) {
            type = TokenType::Break;
            parseIndex++;
        } else if (tokens[parseIndex].type == TokenType::Return) {
            type = TokenType::Return;
            parseIndex++;
        } else {
            return ParseAssignmentExpression();
        }

        std::shared_ptr<ExpressionNode> tmp = nullptr;
        try {
            tmp = ParseAssignmentExpression();
        } catch (const ParseError &) {
            tmp = nullptr;
        }

        return std::make_shared<JumpExpressionNode>(pos, type, tmp);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseAssignmentExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseLogicalOrExpression();
        if (tokens[parseIndex].type == TokenType::Eq || tokens[parseIndex].type == TokenType::PlusEq ||
            tokens[parseIndex].type == TokenType::MinusEq || tokens[parseIndex].type == TokenType::MulEq ||
            tokens[parseIndex].type == TokenType::DivEq || tokens[parseIndex].type == TokenType::ModEq ||
            tokens[parseIndex].type == TokenType::AndEq || tokens[parseIndex].type == TokenType::OrEq ||
            tokens[parseIndex].type == TokenType::XorEq || tokens[parseIndex].type == TokenType::SLEq ||
            tokens[parseIndex].type == TokenType::SREq) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            auto rhs_ = ParseAssignmentExpression();
            return std::make_shared<AssignmentExpressionNode>(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseLogicalOrExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseLogicalAndExpression();
        while (tokens[parseIndex].type == TokenType::OrOr) {
            ConsumeString("||");
            auto rhs_ = ParseLogicalAndExpression();
            lhs_ = std::make_shared<LogicOrExpressionNode>(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseLogicalAndExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseComparisonExpression();
        while (tokens[parseIndex].type == TokenType::AndAnd) {
            ConsumeString("&&");
            auto rhs_ = ParseComparisonExpression();
            lhs_ = std::make_shared<LogicAndExpressionNode>(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseComparisonExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseBitwiseOrExpression();
        while (tokens[parseIndex].type == TokenType::Lt || tokens[parseIndex].type == TokenType::Gt
               || tokens[parseIndex].type == TokenType::LEq || tokens[parseIndex].type == TokenType::GEq
               || tokens[parseIndex].type == TokenType::EqEq || tokens[parseIndex].type == TokenType::NEq) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            auto rhs_ = ParseBitwiseOrExpression();
            lhs_ = std::make_shared<ComparisonExpressionNode>(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseBitwiseOrExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseBitwiseXorExpression();
        while (tokens[parseIndex].type == TokenType::Or) {
            parseIndex++;
            auto rhs_ = ParseBitwiseXorExpression();
            lhs_ = std::make_shared<BitwiseOrExpressionNode>(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseBitwiseXorExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseBitwiseAndExpression();
        while (tokens[parseIndex].type == TokenType::Xor) {
            parseIndex++;
            auto rhs_ = ParseBitwiseAndExpression();
            lhs_ = std::make_shared<BitwiseXorExpressionNode>(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseBitwiseAndExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseShiftExpression();
        while (tokens[parseIndex].type == TokenType::And) {
            parseIndex++;
            auto rhs_ = ParseShiftExpression();
            lhs_ = std::make_shared<BitwiseAndExpressionNode>(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseShiftExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseAddMinusExpression();
        while (tokens[parseIndex].type == TokenType::SL || tokens[parseIndex].type == TokenType::SR) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            auto rhs_ = ParseAddMinusExpression();
            lhs_ = std::make_shared<ShiftExpressionNode>(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseAddMinusExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseMulDivModExpression();
        while (tokens[parseIndex].type == TokenType::Plus || tokens[parseIndex].type == TokenType::Minus) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            auto rhs_ = ParseMulDivModExpression();
            lhs_ = std::make_shared<AddMinusExpressionNode>(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseMulDivModExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseTypeCastExpression();
        while (tokens[parseIndex].type == TokenType::Mul || tokens[parseIndex].type == TokenType::Div
               || tokens[parseIndex].type == TokenType::MOD) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            auto rhs_ = ParseTypeCastExpression();
            lhs_ = std::make_shared<MulDivModExpressionNode>(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseTypeCastExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParseUnaryExpression();
        while (tokens[parseIndex].type == TokenType::As) {
            parseIndex++;
            auto rhs_ = ParseType();
            lhs_ = std::make_shared<TypeCastExpressionNode>(pos, rhs_, lhs_);
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseUnaryExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        while (tokens[parseIndex].type == TokenType::Minus ||
            tokens[parseIndex].type == TokenType::Not ||
            tokens[parseIndex].type == TokenType::And ||
            tokens[parseIndex].type == TokenType::AndAnd ||
            tokens[parseIndex].type == TokenType::Mul) {
            TokenType type = tokens[parseIndex++].type;
            if (type == TokenType::And && tokens[parseIndex].type == TokenType::Mut) {
                ConsumeString("mut");
                type = TokenType::AndMut;
            }
            if (type == TokenType::AndAnd && tokens[parseIndex].type == TokenType::Mut) {
                ConsumeString("mut");
                type = TokenType::AndAndMut;
            }
            auto rhs_ = ParseUnaryExpression();
            return std::make_shared<UnaryExpressionNode>(pos, type, rhs_);
        }
        return ParseCallExpression();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseCallExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto lhs_ = ParsePrimaryExpression();
        while (true) {
            if (tokens[parseIndex].type == TokenType::LParen) {
                ConsumeString("(");
                std::vector<std::shared_ptr<ExpressionNode>> params_;
                while (tokens[parseIndex].type != TokenType::RParen) {
                    params_.push_back(ParseExpression());
                    if (tokens[parseIndex].type == TokenType::Comma) {
                        ConsumeString(",");
                    } else if (tokens[parseIndex].type != TokenType::RParen) {
                        throw ParseError("Parse Error: Lack of , to split two params", pos);
                    }
                }
                ConsumeString(")");
                lhs_ = std::make_shared<FunctionCallExpressionNode>(pos, lhs_, std::move(params_));
            } else if (tokens[parseIndex].type == TokenType::LBracket) {
                ConsumeString("[");
                auto expression_node = ParseExpression();
                ConsumeString("]");
                lhs_ = std::make_shared<ArrayIndexExpressionNode>(pos, lhs_, expression_node);
            } else if (tokens[parseIndex].type == TokenType::Dot) {
                ConsumeString(".");
                if (tokens[parseIndex].type == TokenType::IntegerLiteral ||
                    tokens[parseIndex].type == TokenType::Identifier) {
                    lhs_ = std::make_shared<MemberAccessExpressionNode>(pos, lhs_, tokens[parseIndex]);
                    parseIndex++;
                } else {
                    throw ParseError("Parse Error: Expected an identifier or integer after .", pos);
                }
            } else {
                break;
            }
        }
        return lhs_;
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParsePrimaryExpression() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;

    try {
        return ParseLiteral();
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        return ParseStructExpression();
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        if (tokens[parseIndex].type == TokenType::LBrace || tokens[parseIndex].type == TokenType::If ||
            tokens[parseIndex].type == TokenType::Const || tokens[parseIndex].type == TokenType::Loop ||
            tokens[parseIndex].type == TokenType::While) {
            return ParseExpressionWithBlock();
        }
        if (tokens[parseIndex].type == TokenType::Identifier ||
            tokens[parseIndex].type == TokenType::Self ||
            tokens[parseIndex].type == TokenType::SELF) {
            return ParsePathExpression();
        }
        if (tokens[parseIndex].type == TokenType::LParen) {
            ConsumeString("(");
            auto first = ParseExpression();
            if (tokens[parseIndex].type == TokenType::Comma) {
                std::vector<std::shared_ptr<ExpressionNode>> expression_nodes;
                expression_nodes.emplace_back(first);
                while (tokens[parseIndex].type != TokenType::RParen) {
                    expression_nodes.push_back(ParseExpression());
                    if (tokens[parseIndex].type == TokenType::Comma) {
                        ConsumeString(",");
                    } else {
                        break; // Allow trailing comma
                    }
                }
                ConsumeString(")");
                return std::make_shared<TupleExpressionNode>(pos, std::move(expression_nodes));
            }
            ConsumeString(")");
            return std::make_shared<GroupedExpressionNode>(pos, first);
        }
        throw ParseError("Parse Error: Invalid Primary Expression", pos);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StructExpressionNode> Parser::ParseStructExpression() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PathInExpressionNode> path_in_expression_node = nullptr;
    std::shared_ptr<StructExprFieldsNode> struct_field_node = nullptr;
    std::shared_ptr<StructBaseNode> struct_base_node = nullptr;
    try {
        path_in_expression_node = ParsePathInExpression();
        ConsumeString("{");
        if (tokens[parseIndex].type == TokenType::DotDot) {
            struct_base_node = ParseStructBase();
        } else if (tokens[parseIndex].type != TokenType::RBrace) {
            struct_field_node = ParseStructExprFields();
        }
        ConsumeString("}");
        return std::make_shared<StructExpressionNode>(pos, path_in_expression_node,
                                        struct_field_node, struct_base_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StructExprFieldNode> Parser::ParseStructExprField() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<ExpressionNode> expr = nullptr;
    try {
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", pos);
        }
        std::string identifier = tokens[parseIndex].token;
        parseIndex++;
        if (tokens[parseIndex].type == TokenType::Colon) {
            ConsumeString(":");
            expr = ParseExpression();
        }
        return std::make_shared<StructExprFieldNode>(pos, identifier, expr);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StructExprFieldsNode> Parser::ParseStructExprFields() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<StructExprFieldNode>> fields;
    std::shared_ptr<StructBaseNode> base = nullptr;
    try {
        fields.emplace_back(ParseStructExprField());
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::DotDot) {
                base = ParseStructBase();
                break;
            }
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
            fields.emplace_back(ParseStructExprField());
        }
        return std::make_shared<StructExprFieldsNode>(pos, std::move(fields), base);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StructBaseNode> Parser::ParseStructBase() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("..");
        auto expr = ParseExpression();
        return std::make_shared<StructBaseNode>(pos, expr);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionNode> Parser::ParseLiteral() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::IntegerLiteral) {
            std::string token_ = tokens[parseIndex].token;
            bool is_u32 = true, is_i32 = true, is_isize = true, is_usize = true;
            uint32_t len = token_.size();
            if (len >= 3) {
                if (token_.substr(len - 3, 3) == "i32") {
                    is_i32 = true;
                    is_u32 = is_isize = is_usize = false;
                }
                if (token_.substr(len - 3, 3) == "u32") {
                    is_u32 = true;
                    is_i32 = is_isize = is_usize = false;
                }
            }
            if (len >= 5) {
                if (token_.substr(len - 5, 5) == "isize") {
                    is_isize = true;
                    is_i32 = is_u32 = is_usize = false;
                }
                if (token_.substr(len - 5, 5) == "usize") {
                    is_usize = true;
                    is_i32 = is_u32 = is_isize = false;
                }
            }
            return std::make_shared<IntLiteralNode>(pos, StringToInt(tokens[parseIndex++].token),
                is_u32, is_i32, is_usize, is_isize);
        }
        if (tokens[parseIndex].type == TokenType::StringLiteral ||
            tokens[parseIndex].type == TokenType::RawStringLiteral) {
            return std::make_shared<StringLiteralNode>(pos, rust_str_to_cpp(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::CStringLiteral ||
            tokens[parseIndex].type == TokenType::RawCStringLiteral) {
            return std::make_shared<CStringLiteralNode>(pos, rust_str_to_cpp(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::CharLiteral) {
            return std::make_shared<CharLiteralNode>(pos, rust_char_to_cpp(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::True) {
            parseIndex++;
            return std::make_shared<BoolLiteralNode>(pos, true);
        }
        if (tokens[parseIndex].type == TokenType::False) {
            parseIndex++;
            return std::make_shared<BoolLiteralNode>(pos, false);
        }
        if (tokens[parseIndex].type == TokenType::LBracket) {
            ConsumeString("[");
            std::vector<std::shared_ptr<ExpressionNode>> expression_nodes;
            std::shared_ptr<ExpressionNode> lhs = nullptr;
            std::shared_ptr<ExpressionNode> rhs = nullptr;
            if (tokens[parseIndex].type == TokenType::RBracket) {
                ConsumeString("]");
                return std::make_shared<ArrayLiteralNode>(pos, std::move(expression_nodes), lhs, rhs);
            }
            auto tmp = ParseExpression();
            if (tokens[parseIndex].type == TokenType::Semicolon) {
                lhs = tmp;
                ConsumeString(";");
                rhs = ParseExpression();
                ConsumeString("]");
                return std::make_shared<ArrayLiteralNode>(pos, std::move(expression_nodes), lhs, rhs);
            }
            expression_nodes.emplace_back(tmp);
            while (tokens[parseIndex].type == TokenType::Comma) {
                ConsumeString(",");
                if (tokens[parseIndex].type == TokenType::RBracket) {
                    break;
                }
                expression_nodes.push_back(ParseExpression());
            }
            ConsumeString("]");
            return std::make_shared<ArrayLiteralNode>(pos, std::move(expression_nodes), lhs, rhs);
        }
        throw ParseError("Parse Error: Not A Literal", pos);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<PathExpressionNode> Parser::ParsePathExpression() {
    try {
        return ParsePathInExpression();
    } catch (const ParseError &) {
        throw;
    }
}


std::shared_ptr<PathInExpressionNode> Parser::ParsePathInExpression() {
    Position pos = tokens[parseIndex].pos;
    try {
        std::vector<std::shared_ptr<PathIndentSegmentNode>> simple_path_segments;
        if (tokens[parseIndex].type == TokenType::ColonColon) {
            ConsumeString("::");
        }
        simple_path_segments.emplace_back(ParsePathIndentSegment());
        while (tokens[parseIndex].type == TokenType::ColonColon) {
            ConsumeString("::");
            simple_path_segments.emplace_back(ParsePathIndentSegment());
        }
        return std::make_shared<PathInExpressionNode>(pos, std::move(simple_path_segments));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<StatementsNode> Parser::ParseStatements() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<StatementNode>> statement_nodes;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    while (tokens[parseIndex].type != TokenType::RBrace) {
        uint32_t start = parseIndex;
        try {
            statement_nodes.emplace_back(ParseStatement());
            continue;
        } catch (const ParseError &) {
            parseIndex = start;
        }

        try {
            expression_node = ParseExpressionWithoutBlock();
            break;
        } catch (const ParseError &) {
            throw;
        }
    }
    return std::make_shared<StatementsNode>(pos, std::move(statement_nodes), expression_node);
}

std::shared_ptr<ConditionsNode> Parser::ParseConditions() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    std::shared_ptr<ExpressionNode> tmp = nullptr;
    std::shared_ptr<LetChainNode> let_chain_node = nullptr;
    try {
        ConsumeString("(");
        tmp = ParseExpression();
        ConsumeString(")");
        return std::make_shared<ConditionsNode>(pos, tmp, let_chain_node);
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        ConsumeString("(");
        let_chain_node = ParseLetChain();
        ConsumeString(")");
        return std::make_shared<ConditionsNode>(pos, tmp, let_chain_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<LetChainNode> Parser::ParseLetChain() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<LetChainConditionNode>> let_chain_condition_nodes;
    try {
        let_chain_condition_nodes.emplace_back(ParseLetChainCondition());
        while (tokens[parseIndex].type == TokenType::AndAnd) {
            ConsumeString("&&");
            let_chain_condition_nodes.emplace_back(ParseLetChainCondition());
        }
        return std::make_shared<LetChainNode>(pos, std::move(let_chain_condition_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<LetChainConditionNode> Parser::ParseLetChainCondition() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PatternNode> pattern_node = nullptr;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::Let) {
            ConsumeString("let");
            pattern_node = ParsePattern();
            ConsumeString("=");
            expression_node = ParseExpression();
            return std::make_shared<LetChainConditionNode>(pos, pattern_node, expression_node);
        }
        expression_node = ParseExpression();
        return std::make_shared<LetChainConditionNode>(pos, pattern_node, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

/****************  Statement  ****************/
std::shared_ptr<StatementNode> Parser::ParseStatement() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    try {
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
            return std::make_shared<EmptyStatementNode>(pos);
        }
        if (tokens[parseIndex].type == TokenType::Let) {
            return ParseLetStatement();
        }
        return ParseExpressionStatement();
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        return std::make_shared<VisItemStatementNode>(pos, ParseVisItem());
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<LetStatementNode> Parser::ParseLetStatement() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt_node = nullptr;
    std::shared_ptr<TypeNode> type_node = nullptr;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    std::shared_ptr<BlockExpressionNode> block_expression_node = nullptr;
    try {
        ConsumeString("let");
        pattern_no_top_alt_node = ParsePatternNoTopAlt();
        if (tokens[parseIndex].type == TokenType::Colon) {
            ConsumeString(":");
            type_node = ParseType();
        }
        if (tokens[parseIndex].type == TokenType::Eq) {
            ConsumeString("=");
            expression_node = ParseExpression();
            if (tokens[parseIndex].type == TokenType::Else) {
                ConsumeString("else");
                block_expression_node = ParseBlockExpression();
                // TODO Check Whether the expression node is lazyBooleanExpression Or end with a '}'
            }
        }
        ConsumeString(";");
        return std::make_shared<LetStatementNode>(pos, pattern_no_top_alt_node, type_node,
                                    expression_node, block_expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ExpressionStatementNode> Parser::ParseExpressionStatement() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    std::shared_ptr<ExpressionNode> expression_node = nullptr;
    try {
        expression_node = ParseExpressionWithoutBlock();
        ConsumeString(";");
        return std::make_shared<ExpressionStatementNode>(pos, expression_node);
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        bool has_semicolon = false;
        expression_node = ParseExpressionWithBlock();
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
            has_semicolon = true;
        }
        return std::make_shared<ExpressionStatementNode>(pos, expression_node, has_semicolon);
    } catch (const ParseError &) {
        throw ParseError("Parse Error: Failed to match ExpressionStatement", pos);
    }
}


/****************  Pattern  ****************/
std::shared_ptr<PatternNode> Parser::ParsePattern() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<PatternNoTopAltNode>> pattern_no_top_alt_nodes;
    try {
        if (tokens[parseIndex].type == TokenType::Or) {
            ConsumeString("|");
        }
        pattern_no_top_alt_nodes.emplace_back(ParsePatternNoTopAlt());
        while (tokens[parseIndex].type == TokenType::Or) {
            ConsumeString("|");
            pattern_no_top_alt_nodes.emplace_back(ParsePatternNoTopAlt());
        }
        return std::make_shared<PatternNode>(pos, std::move(pattern_no_top_alt_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<PatternNoTopAltNode> Parser::ParsePatternNoTopAlt() {
    try {
        return ParsePatternWithoutRange();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<PatternWithoutRangeNode> Parser::ParsePatternWithoutRange() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;

    try { return ParseLiteralPattern(); } catch (const ParseError &) { parseIndex = start; }
    try { return ParsePathPattern(); } catch (const ParseError &) { parseIndex = start; }
    try { return ParseIdentifierPattern(); } catch (const ParseError &) { parseIndex = start; }

    try {
        ConsumeString("_");
        return std::make_shared<WildcardPatternNode>(pos);
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        ConsumeString("..");
        return std::make_shared<RestPatternNode>(pos);
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        ConsumeString("(");
        auto pattern_node = ParsePattern();
        ConsumeString(")");
        return std::make_shared<GroupedPatternNode>(pos, pattern_node);
    } catch (const ParseError &) {
        parseIndex = start;
    }

    try {
        return ParseSlicePattern();
    } catch (const ParseError &) {
        parseIndex = start;
        throw ParseError("Parse Error: Failed to Parse PatternWithoutRange", pos);
    }
}

std::shared_ptr<LiteralPatternNode> Parser::ParseLiteralPattern() {
    Position pos = tokens[parseIndex].pos;
    try {
        bool have_minus = false;
        if (tokens[parseIndex].type == TokenType::Minus) {
            have_minus = true;
            ConsumeString("-");
        }
        auto expression_node = ParseLiteral();
        return std::make_shared<LiteralPatternNode>(pos, have_minus, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<IdentifierPatternNode> Parser::ParseIdentifierPattern() {
    Position pos = tokens[parseIndex].pos;
    std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt_node = nullptr;
    try {
        bool is_ref = false;
        bool is_mut = false;
        if (tokens[parseIndex].type == TokenType::Ref) {
            ConsumeString("ref");
            is_ref = true;
        }
        if (tokens[parseIndex].type == TokenType::Mut) {
            ConsumeString("mut");
            is_mut = true;
        }
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier is missing", pos);
        }
        std::string identifier = tokens[parseIndex].token;
        parseIndex++;
        if (tokens[parseIndex].type == TokenType::At) {
            ConsumeString("@");
            pattern_no_top_alt_node = ParsePatternNoTopAlt();
        }
        return std::make_shared<IdentifierPatternNode>(pos, is_ref, is_mut, identifier, pattern_no_top_alt_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<SlicePatternNode> Parser::ParseSlicePattern() {
    Position pos = tokens[parseIndex].pos;
    std::vector<std::shared_ptr<PatternNode>> pattern_nodes;
    try {
        ConsumeString("[");
        if (tokens[parseIndex].type != TokenType::RBracket) {
            pattern_nodes.emplace_back(ParsePattern());
            while (tokens[parseIndex].type == TokenType::Comma) {
                ConsumeString(",");
                if (tokens[parseIndex].type == TokenType::RBracket) {
                    break;
                }
                pattern_nodes.emplace_back(ParsePattern());
            }
        }
        ConsumeString("]");
        return std::make_shared<SlicePatternNode>(pos, std::move(pattern_nodes));
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<PathPatternNode> Parser::ParsePathPattern() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto expression_node = ParsePathExpression();
        return std::make_shared<PathPatternNode>(pos, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

/****************  Types  ****************/
std::shared_ptr<TypeNode> Parser::ParseType() {
    try {
        return ParseTypeNoBounds();
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<TypeNoBoundsNode> Parser::ParseTypeNoBounds() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::Identifier) {
            return ParseTypePath();
        }
        if (tokens[parseIndex].type == TokenType::LParen) {
            return ParseUnitType();
        }
        if (tokens[parseIndex].type == TokenType::LBracket) {
            return ParseArrayType();
        }
        return ParseReferenceType();
    } catch (std::exception&) {
        throw ParseError("Parse Error: Failed to Parse TypeNoBounds", pos);
    }
}

std::shared_ptr<ParenthesizedTypeNode> Parser::ParseParenthesizedType() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("(");
        auto tmp = ParseType();
        ConsumeString(")");
        return std::make_shared<ParenthesizedTypeNode>(pos, tmp);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<TypePathNode> Parser::ParseTypePath() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::ColonColon) {
            ConsumeString("::");
        }
        auto tmp = ParseTypePathSegment();
        return std::make_shared<TypePathNode>(pos, tmp);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<TypePathSegmentNode> Parser::ParseTypePathSegment() {
    Position pos = tokens[parseIndex].pos;
    try {
        auto tmp = ParsePathIndentSegment();
        return std::make_shared<TypePathSegmentNode>(pos, tmp);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<UnitTypeNode> Parser::ParseUnitType() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("(");
        ConsumeString(")");
        return std::make_shared<UnitTypeNode>(pos);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ArrayTypeNode> Parser::ParseArrayType() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("[");
        auto type_node = ParseType();
        ConsumeString(";");
        auto expression_node = ParseExpression();
        ConsumeString("]");
        return std::make_shared<ArrayTypeNode>(pos, type_node, expression_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<SliceTypeNode> Parser::ParseSliceType() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("[");
        auto type_node = ParseType();
        ConsumeString("]");
        return std::make_shared<SliceTypeNode>(pos, type_node);
    } catch (const ParseError &) {
        throw;
    }
}

std::shared_ptr<ReferenceTypeNode> Parser::ParseReferenceType() {
    Position pos = tokens[parseIndex].pos;
    try {
        bool is_mut = false;
        ConsumeString("&");
        if (tokens[parseIndex].type == TokenType::Mut) {
            ConsumeString("mut");
            is_mut = true;
        }
        auto type = ParseType();
        return std::make_shared<ReferenceTypeNode>(pos, is_mut, type);
    } catch (std::exception&) {
        throw;
    }
}


/****************  Paths  ****************/
std::shared_ptr<PathIndentSegmentNode> Parser::ParsePathIndentSegment() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::Super || tokens[parseIndex].type == TokenType::Self ||
            tokens[parseIndex].type == TokenType::SELF || tokens[parseIndex].type == TokenType::Crate ||
            tokens[parseIndex].type == TokenType::Identifier) {
            auto node = std::make_shared<PathIndentSegmentNode>(pos, tokens[parseIndex].type, tokens[parseIndex].token);
            parseIndex++;
            return node;
        }
        throw ParseError("Parse Error: Invalid SimplePathSegment", pos);
    } catch (const ParseError &) {
        throw;
    }
}