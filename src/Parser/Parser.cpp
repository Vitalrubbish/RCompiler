#include "../../include/Parser/Parser.h"

#include <random>

#include "../../Error/error.h"
#include "../../util/Util.h"

void Parser::ConsumeString(const std::string& str) {
    if (tokens[parseIndex].token == str) {
        parseIndex++;
    } else {
        throw ParseError("Cannot Match " + str, tokens[parseIndex].pos);
    }
}

/****************  Items  ****************/
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
    uint32_t start = parseIndex;
    VisItemNode* node = nullptr;
    try {
        node = ParseFunction();
        return node;
    } catch (std::exception&) {
        delete node;
        node = nullptr;
        parseIndex = start;
    }

    try {
        if (tokens[parseIndex].type == TokenType::Const) {
            node = ParseConstantItem();
            return node;
        }
        if (tokens[parseIndex].type == TokenType::Struct) {
            node = ParseStruct();
            return node;
        }
        if (tokens[parseIndex].type == TokenType::Enum) {
            node = ParseEnumeration();
            return node;
        }
        if (tokens[parseIndex].type == TokenType::Impl) {
            node = ParseImplementation();
            return node;
        }
        return nullptr;
    } catch (std::exception&) {
        delete node;
        throw;
    }
    // TODO Parse Other Items
}

FunctionNode* Parser::ParseFunction() {
    Position pos = tokens[parseIndex].pos;
    FunctionParametersNode* function_parameters_node = nullptr;
    TypeNode* type_node = nullptr;
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

        ConsumeString("(");

        if (tokens[parseIndex].type != TokenType::RParen) {
            function_parameters_node = ParseFunctionParameters();
        }

        ConsumeString(")");

        if (tokens[parseIndex].type == TokenType::RArrow) {
            type_node = ParseFunctionReturnType();
        }

        // TODO ParseWhereClause

        if (tokens[parseIndex].type != TokenType::Semicolon) {
            block_expression_node = ParseBlockExpression();
        } else {
            ConsumeString(";");
        }
        return new FunctionNode(pos, is_const, identifier, function_parameters_node,
            type_node, block_expression_node);
    } catch (std::exception&) {
        delete type_node;
        delete function_parameters_node;
        delete block_expression_node;
        throw;
    }
}

TypeNode *Parser::ParseFunctionReturnType() {
    TypeNode* type = nullptr;
    try {
        ConsumeString("->");
        type = ParseType();
        return type;
    } catch (std::exception&) {
        delete type;
        throw;
    }
}


FunctionParametersNode* Parser::ParseFunctionParameters() {
    Position pos = tokens[parseIndex].pos;
    std::vector<FunctionParamNode*> function_param_nodes;
    FunctionParamNode* tmp = nullptr;
    try {
        tmp = ParseFunctionParam();
        function_param_nodes.emplace_back(tmp);
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            tmp = ParseFunctionParam();
            function_param_nodes.emplace_back(tmp);
            if (tokens[parseIndex].type == TokenType::RParen) {
                break;
            }
        }
        return new FunctionParametersNode(pos, function_param_nodes);
    } catch (std::exception&) {
        delete tmp;
        for (auto& it: function_param_nodes) {
            delete it;
        }
        throw;
    }
}

FunctionParamNode *Parser::ParseFunctionParam() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    FunctionParamPatternNode* function_param_pattern_node = nullptr;
    TypeNode* type_node = nullptr;
    if (tokens[parseIndex].type == TokenType::DotDotDot) {
        ConsumeString("...");
        return new FunctionParamNode(pos, function_param_pattern_node, type_node, true);
    }
    try {
        type_node = ParseType();
        return new FunctionParamNode(pos, function_param_pattern_node, type_node, false);
    } catch (std::exception&) {
        parseIndex = start;
    }
    try {
        function_param_pattern_node = ParseFunctionParamPattern();
        return new FunctionParamNode(pos, function_param_pattern_node, type_node, false);
    } catch (std::exception&) {
        throw ParseError("Parse Error: Failed to Match FunctionParam", pos);
    }
}

FunctionParamPatternNode *Parser::ParseFunctionParamPattern() {
    Position pos = tokens[parseIndex].pos;
    PatternNoTopAltNode* pattern_no_top_alt_node = nullptr;
    TypeNode* type_node = nullptr;
    try {
        pattern_no_top_alt_node = ParsePatternNoTopAlt();
        ConsumeString(":");
        if (tokens[parseIndex].type == TokenType::DotDotDot) {
            ConsumeString("...");
            return new FunctionParamPatternNode(pos, pattern_no_top_alt_node, type_node, true);
        }
        type_node = ParseType();
        return new FunctionParamPatternNode(pos, pattern_no_top_alt_node, type_node, true);
    } catch (std::exception&) {
        delete pattern_no_top_alt_node;
        delete type_node;
        throw;
    }
}

StructNode* Parser::ParseStruct() {
    Position pos = tokens[parseIndex].pos;
    std::vector<StructFieldNode*> struct_field_nodes;
    StructFieldNode* struct_field_node = nullptr;
    try {
        ConsumeString("struct");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex].token;
        parseIndex++;
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
            return new StructNode(pos, identifier, struct_field_nodes);
        }
        ConsumeString("{");
        struct_field_node = ParseStructFieldNode();
        struct_field_nodes.emplace_back(struct_field_node);
        struct_field_node = nullptr;
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
            struct_field_node = ParseStructFieldNode();
            struct_field_nodes.emplace_back(struct_field_node);
            struct_field_node = nullptr;
        }
        ConsumeString("}");
        return new StructNode(pos, identifier, struct_field_nodes);
    } catch (std::exception&) {
        for (auto& it: struct_field_nodes) {
            delete it;
        }
        delete struct_field_node;
        throw;
    }
}

StructFieldNode* Parser::ParseStructFieldNode() {
    Position pos = tokens[parseIndex].pos;
    TypeNode* type_node = nullptr;
    try {
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        ConsumeString(":");
        type_node = ParseType();
        return new StructFieldNode(pos, identifier, type_node);
    } catch (std::exception&) {
        delete type_node;
        throw;
    }
}

EnumerationNode *Parser::ParseEnumeration() {
    Position pos = tokens[parseIndex].pos;
    std::vector<EnumVariantNode*> enum_variant_nodes;
    EnumVariantNode* enum_variant_node;
    try {
        ConsumeString("enum");
        if (tokens[parseIndex].type != TokenType::Identifier) {
            throw ParseError("Parse Error: Identifier Not Found", tokens[parseIndex].pos);
        }
        std::string identifier = tokens[parseIndex++].token;
        ConsumeString("{");
        if (tokens[parseIndex].type == TokenType::RBrace) {
            ConsumeString("}");
            return new EnumerationNode(pos, identifier, enum_variant_nodes);
        }
        enum_variant_node = ParseEnumVariant();
        enum_variant_nodes.emplace_back(enum_variant_node);
        enum_variant_node = nullptr;
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
            enum_variant_node = ParseEnumVariant();
            enum_variant_nodes.emplace_back(enum_variant_node);
            enum_variant_node = nullptr;
        }
        ConsumeString("}");
        return new EnumerationNode(pos, identifier, enum_variant_nodes);
    } catch (std::exception&) {
        for (auto& it: enum_variant_nodes) {
            delete it;
        }
        delete enum_variant_node;
        throw;
    }
}

EnumVariantNode *Parser::ParseEnumVariant() {
    Position pos = tokens[parseIndex].pos;
    EnumVariantStructNode* enum_variant_struct_node = nullptr;
    EnumVariantDiscriminantNode* enum_variant_discriminant_node = nullptr;
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
        return new EnumVariantNode(pos, identifier, enum_variant_struct_node,
            enum_variant_discriminant_node);
    } catch (std::exception&) {
        delete enum_variant_struct_node;
        delete enum_variant_discriminant_node;
        throw;
    }
}

EnumVariantStructNode *Parser::ParseEnumVariantStruct() {
    Position pos = tokens[parseIndex].pos;
    std::vector<StructFieldNode*> struct_field_nodes;
    StructFieldNode* struct_field_node = nullptr;
    try {
        ConsumeString("{");
        struct_field_node = ParseStructFieldNode();
        struct_field_nodes.emplace_back(struct_field_node);
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            struct_field_node = ParseStructFieldNode();
            struct_field_nodes.emplace_back(struct_field_node);
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
        }
        ConsumeString("}");
        return new EnumVariantStructNode(pos, struct_field_nodes);
    } catch (std::exception&) {
        delete struct_field_node;
        for (auto& it: struct_field_nodes) {
            delete it;
        }
        throw;
    }
}

EnumVariantDiscriminantNode *Parser::ParseEnumVariantDiscriminant() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expression_node = nullptr;
    try {
        ConsumeString("=");
        expression_node = ParseExpression();
        return new EnumVariantDiscriminantNode(pos, expression_node);
    } catch (std::exception&) {
        delete expression_node;
        throw;
    }
}

ConstantItemNode* Parser::ParseConstantItem() {
    Position pos = tokens[parseIndex].pos;
    TypeNode* type_node = nullptr;
    ExpressionNode* expression_node = nullptr;
    try {
        bool is_underscore = false;
        std::string identifier;
        ConsumeString("const");
        if (tokens[parseIndex].type == TokenType::Underscore) {
            is_underscore = true;
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
        return new ConstantItemNode(pos, identifier, is_underscore, type_node, expression_node);
    } catch (std::exception&) {
        delete type_node;
        delete expression_node;
        throw;
    }
}

AssociatedItemNode* Parser::ParseAssociatedItem() {
    Position pos = tokens[parseIndex].pos;
    ConstantItemNode* constant_item_node = nullptr;
    FunctionNode* function_node = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::Const) {
            constant_item_node = ParseConstantItem();
        } else {
            function_node = ParseFunction();
        }
        return new AssociatedItemNode(pos, constant_item_node, function_node);
    } catch (std::exception&) {
        delete constant_item_node;
        delete function_node;
        throw;
    }
}

ImplementationNode* Parser::ParseImplementation() {
    ImplementationNode* ret = nullptr;
    try {
        ret = ParseInherentImpl();
        return ret;
    } catch (std::exception&) {
        delete ret;
        throw;
    }

    // TODO Parse TraitImpl
}

InherentImplNode *Parser::ParseInherentImpl() {
    Position pos = tokens[parseIndex].pos;
    TypeNode* type_node = nullptr;
    std::vector<AssociatedItemNode*> associated_item_nodes;
    AssociatedItemNode* associated_item_node = nullptr;
    try {
        ConsumeString("impl");
        type_node = ParseType();
        ConsumeString("{");
        while (tokens[parseIndex].type != TokenType::RBrace) {
            associated_item_node = ParseAssociatedItem();
            associated_item_nodes.emplace_back(associated_item_node);
        }
        ConsumeString("}");
        return new InherentImplNode(pos, type_node, associated_item_nodes);
    } catch (std::exception&) {
        delete type_node;
        delete associated_item_node;
        for (auto& it: associated_item_nodes) {
            delete it;
        }
        throw;
    }
}

/****************  Expression  ****************/
ExpressionNode *Parser::ParseExpression() {
    uint32_t start = parseIndex;
    try {
        auto* ret = ParseExpressionWithBlock();
        return ret;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        auto* ret = ParseExpressionWithoutBlock();
        return ret;
    } catch (std::exception&) {
        throw;
    }
}

/****************  Expression With Block  ****************/
ExpressionNode* Parser::ParseExpressionWithBlock() {
    ExpressionWithBlockNode* node = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::Const) {
            node = ParseConstBlockExpression();
        } else if (tokens[parseIndex].type == TokenType::Loop) {
            node = ParseInfiniteLoopExpression();
        } else if (tokens[parseIndex].type == TokenType::While) {
            node = ParsePredicateLoopExpression();
        } else if (tokens[parseIndex].type == TokenType::If) {
            node = ParseIfExpression();
        } else if (tokens[parseIndex].type == TokenType::Match) {
            node = ParseMatchExpression();
        } else {
            node = ParseBlockExpression();
        }
        return node;
    } catch (std::exception&) {
        delete node;
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
        return new BlockExpressionNode(pos, false, node);
    } catch (std::exception&) {
        throw;
    }
}

BlockExpressionNode* Parser::ParseConstBlockExpression() {
    Position pos = tokens[parseIndex].pos;
    ConsumeString("const");
    try {
        StatementsNode* node = nullptr;
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
            node = ParseStatements();
        }
        ConsumeString("}");
        return new BlockExpressionNode(pos, true, node);
    } catch (std::exception&) {
        throw;
    }
}


InfiniteLoopExpressionNode* Parser::ParseInfiniteLoopExpression() {
    Position pos = tokens[parseIndex].pos;
    ConsumeString("loop");
    try {
        auto* node = ParseBlockExpression();
        return new InfiniteLoopExpressionNode(pos, node);
    } catch (std::exception&) {
        throw;
    }
}

PredicateLoopExpressionNode* Parser::ParsePredicateLoopExpression() {
    Position pos = tokens[parseIndex].pos;
    ConditionsNode* conditions = nullptr;
    try {
        ConsumeString("while");
        conditions = ParseConditions();
        auto* node = ParseBlockExpression();
        return new PredicateLoopExpressionNode(pos, conditions, node);
    } catch (std::exception&) {
        delete conditions;
        throw;
    }
}

IfExpressionNode* Parser::ParseIfExpression() {
    Position pos = tokens[parseIndex].pos;
    ConditionsNode* conditions_node = nullptr;
    BlockExpressionNode* true_block_expression_node = nullptr;
    BlockExpressionNode* false_block_expression_node = nullptr;
    IfExpressionNode* if_expression_node = nullptr;
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
        return new IfExpressionNode(pos, conditions_node, true_block_expression_node,
            false_block_expression_node, if_expression_node);
    } catch (std::exception&) {
        delete conditions_node;
        delete true_block_expression_node;
        delete false_block_expression_node;
        delete if_expression_node;
        throw;
    }
}

MatchExpressionNode* Parser::ParseMatchExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expression_node = nullptr;
    MatchArmsNode* match_arms_node = nullptr;
    try {
        ConsumeString("match");
        expression_node = ParseExpression(); // TODO Check whether it is a StructExpression
        ConsumeString("{");
        if (tokens[parseIndex].type != TokenType::RBrace) {
            match_arms_node = ParseMatchArms();
        }
        ConsumeString("}");
        return new MatchExpressionNode(pos, expression_node, match_arms_node);
    } catch (std::exception&) {
        delete expression_node;
        delete match_arms_node;
        throw;
    }
}

MatchArmsNode* Parser::ParseMatchArms() {
    Position pos = tokens[parseIndex].pos;
    std::vector<MatchArmNode*> match_arms_nodes;
    MatchArmNode* match_arm_node = nullptr;
    std::vector<ExpressionNode*> expression_nodes;
    ExpressionNode* expression_node = nullptr;
    try {
        while (tokens[parseIndex].type != TokenType::RBrace) {
            match_arm_node = ParseMatchArm();
            match_arms_nodes.emplace_back(match_arm_node);
            ConsumeString("=>");
            expression_node = ParseExpression();
            expression_nodes.emplace_back(expression_node);
            if (dynamic_cast<BlockExpressionNode*>(expression_node) != nullptr) {
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
        return new MatchArmsNode(pos, match_arms_nodes, expression_nodes);
    } catch (std::exception&) {
        delete expression_node;
        delete match_arm_node;
        for (auto& it: expression_nodes) {
            delete it;
        }
        for (auto& it: match_arms_nodes) {
            delete it;
        }
        throw;
    }
}

MatchArmNode* Parser::ParseMatchArm() {
    Position pos = tokens[parseIndex].pos;
    PatternNode* pattern_node = nullptr;
    ExpressionNode* expression_node = nullptr;
    try {
        pattern_node = ParsePattern();
        if (tokens[parseIndex].type == TokenType::If) {
            ConsumeString("if");
            expression_node = ParseExpression();
        }
        return new MatchArmNode(pos, pattern_node, expression_node);
    } catch (std::exception&) {
        delete pattern_node;
        delete expression_node;
        throw;
    }
}

/****************  Expression Without Block  ****************/
ExpressionNode* Parser::ParseExpressionWithoutBlock() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expression_node = nullptr;
    if (tokens[parseIndex].type == TokenType::Continue) {
        ConsumeString("continue");
        return new ContinueExpressionNode(pos);
    }
    try {
        expression_node = ParseJumpExpression();
        return expression_node;
    } catch(std::exception&) {
        delete expression_node;
        throw;
    }
}

ExpressionNode *Parser::ParseTupleExpression() {
    Position pos = tokens[parseIndex].pos;
    std::vector<ExpressionNode*> expression_nodes;
    try {
        ConsumeString("(");
        while (tokens[parseIndex].token != ")") {
            auto* tmp = ParseExpression();
            expression_nodes.emplace_back(tmp);
        }
        ConsumeString(")");
        return new TupleExpressionNode(pos, expression_nodes);
    } catch (std::exception&) {
        throw;
    }
}

ExpressionNode* Parser::ParseJumpExpression() {
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
            auto* tmp = ParseAssignmentExpression();
            return tmp;
        }
        auto* tmp = ParseAssignmentExpression();
        return new JumpExpressionNode(pos, type, tmp);
    } catch (std::exception&) {
        throw;
    }
}

ExpressionNode* Parser::ParseAssignmentExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseLogicalOrExpression();
        if (tokens[parseIndex].type == TokenType::Eq || tokens[parseIndex].type == TokenType::PlusEq ||
            tokens[parseIndex].type == TokenType::MinusEq || tokens[parseIndex].type == TokenType::MulEq ||
            tokens[parseIndex].type == TokenType::DivEq || tokens[parseIndex].type == TokenType::ModEq ||
            tokens[parseIndex].type == TokenType::AndEq || tokens[parseIndex].type == TokenType::OrEq ||
            tokens[parseIndex].type == TokenType::XorEq || tokens[parseIndex].type == TokenType::SLEq ||
            tokens[parseIndex].type == TokenType::SREq) {
            TokenType type;
            type = tokens[parseIndex].type;
            parseIndex++;
            rhs_ = ParseAssignmentExpression();
            return new AssignmentExpressionNode(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseLogicalOrExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseLogicalAndExpression();
        while (tokens[parseIndex].type == TokenType::OrOr) {
            ConsumeString("||");
            rhs_ = ParseLogicalAndExpression();
            lhs_ = new LogicOrExpressionNode(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseLogicalAndExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseComparisonExpression();
        while (tokens[parseIndex].type == TokenType::AndAnd) {
            ConsumeString("&&");
            rhs_ = ParseComparisonExpression();
            lhs_ = new LogicAndExpressionNode(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseComparisonExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseBitwiseOrExpression();
        while (tokens[parseIndex].type == TokenType::Lt || tokens[parseIndex].type == TokenType::Gt
            || tokens[parseIndex].type == TokenType::LEq || tokens[parseIndex].type == TokenType::GEq
            || tokens[parseIndex].type == TokenType::EqEq || tokens[parseIndex].type == TokenType::NEq) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            rhs_ = ParseBitwiseOrExpression();
            lhs_ = new ComparisonExpressionNode(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseBitwiseOrExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseBitwiseXorExpression();
        while (tokens[parseIndex].type == TokenType::Or) {
            parseIndex++;
            rhs_ = ParseBitwiseXorExpression();
            lhs_ = new BitwiseOrExpressionNode(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseBitwiseXorExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseBitwiseAndExpression();
        while (tokens[parseIndex].type == TokenType::Xor) {
            parseIndex++;
            rhs_ = ParseBitwiseAndExpression();
            lhs_ = new BitwiseXorExpressionNode(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseBitwiseAndExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseShiftExpression();
        while (tokens[parseIndex].type == TokenType::Or) {
            parseIndex++;
            rhs_ = ParseShiftExpression();
            lhs_ = new BitwiseAndExpressionNode(pos, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseShiftExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseAddMinusExpression();
        while (tokens[parseIndex].type == TokenType::SL || tokens[parseIndex].type == TokenType::SR) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            rhs_ = ParseAddMinusExpression();
            lhs_ = new ShiftExpressionNode(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseAddMinusExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseMulDivModExpression();
        while (tokens[parseIndex].type == TokenType::Plus || tokens[parseIndex].type == TokenType::Minus) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            rhs_ = ParseMulDivModExpression();
            lhs_ = new AddMinusExpressionNode(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseMulDivModExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
    try {
        lhs_ = ParseTypeCastExpression();
        while (tokens[parseIndex].type == TokenType::Mul || tokens[parseIndex].type == TokenType::Div
            || tokens[parseIndex].type == TokenType::MOD) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            rhs_ = ParseTypeCastExpression();
            lhs_ = new MulDivModExpressionNode(pos, type, lhs_, rhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseTypeCastExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    TypeNode* rhs_ = nullptr;
    try {
        lhs_ = ParseUnaryExpression();
        while (tokens[parseIndex].type == TokenType::As) {
            parseIndex++;
            rhs_ = ParseType();
            lhs_ = new TypeCastExpressionNode(pos, rhs_, lhs_);
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseUnaryExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* rhs_ = nullptr;
    try {
        while (tokens[parseIndex].type == TokenType::Minus || tokens[parseIndex].type == TokenType::Not) {
            TokenType type = tokens[parseIndex].type;
            parseIndex++;
            rhs_ = ParseUnaryExpression();
            return new UnaryExpressionNode(pos, type, rhs_);
        }
        return ParseCallExpression();
    } catch (std::exception&) {
        delete rhs_;
        throw;
    }
}

ExpressionNode* Parser::ParseCallExpression() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* lhs_ = nullptr;
    std::vector<ExpressionNode*> params_;
    ExpressionNode* expression_node = nullptr;
    try {
        lhs_ = ParsePrimaryExpression();
        while (true) {
            if (tokens[parseIndex].type == TokenType::LParen) {
                ConsumeString("(");
                while (tokens[parseIndex].type != TokenType::RParen) {
                    auto* tmp = ParseExpression();
                    params_.push_back(tmp);
                    if (tokens[parseIndex].type == TokenType::Comma) {
                        ConsumeString(",");
                    } else if (tokens[parseIndex].type != TokenType::RBracket) {
                        throw ParseError("Parse Error: Lack of , to split two params", pos);
                    }
                }
                ConsumeString(")");
                lhs_ = new FunctionCallExpressionNode(pos, lhs_, params_);
            } else if (tokens[parseIndex].type == TokenType::LBracket) {
                ConsumeString("[");
                expression_node = ParseExpression();
                ConsumeString("]");
                lhs_ = new ArrayIndexExpressionNode(pos, lhs_, expression_node);
            } else if (tokens[parseIndex].type == TokenType::Dot) {
                ConsumeString(".");
                if (tokens[parseIndex].type == TokenType::IntegerLiteral ||
                    tokens[parseIndex].type == TokenType::Identifier) {
                    lhs_ = new MemberAccessExpressionNode(pos, lhs_, tokens[parseIndex]);
                    parseIndex++;
                } else {
                    throw ParseError("Parse Error: Expected an identifier or integer after .", pos);
                }
            } else {
                break;
            }
        }
        return lhs_;
    } catch (std::exception&) {
        delete lhs_;
        delete expression_node;
        for (auto& it: params_) {
            delete it;
        }
        throw;
    }
}

ExpressionNode* Parser::ParsePrimaryExpression() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    std::vector<ExpressionNode*> expression_nodes;
    try {
        auto* tmp = ParseLiteral();
        return tmp;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        auto* tmp = ParseStructExpression();
        return tmp;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        if (tokens[parseIndex].type == TokenType::Identifier) {
            return ParsePathExpression();
        }
        if (tokens[parseIndex].type == TokenType::LParen) {
            ConsumeString("(");
            auto* first = ParseExpression();
            if (tokens[parseIndex].type == TokenType::Comma) {
                expression_nodes.emplace_back(first);
                while (tokens[parseIndex].type != TokenType::RParen) {
                    auto* tmp = ParseExpression();
                    expression_nodes.push_back(tmp);
                    if (tokens[parseIndex].type == TokenType::Comma) {
                        ConsumeString(",");
                    } else {
                        ConsumeString(")");
                    }
                }
                return new TupleExpressionNode(pos, expression_nodes);
            }
            ConsumeString(")");
            return new GroupedExpressionNode(pos, first);
        }
        throw ParseError("Parse Error: Invalid Primary Expression", pos);
    } catch (std::exception&) {
        for (auto& it: expression_nodes) {
            delete it;
        }
        throw;
    }
}

StructExpressionNode* Parser::ParseStructExpression() {
    Position pos = tokens[parseIndex].pos;
    PathInExpressionNode* path_in_expression_node = nullptr;
    StructExprFieldsNode* struct_field_node = nullptr;
    StructBaseNode* struct_base_node = nullptr;
    try {
        path_in_expression_node = ParsePathInExpression();
        ConsumeString("{");
        if (tokens[parseIndex].type == TokenType::DotDot) {
            struct_base_node = ParseStructBase();
        } else if (tokens[parseIndex].type != TokenType::RBrace) {
            struct_field_node = ParseStructExprFields();
        }
        ConsumeString("}");
        return new StructExpressionNode(pos, path_in_expression_node,
                struct_field_node, struct_base_node);
    } catch (std::exception&) {
        delete path_in_expression_node;
        delete struct_field_node;
        delete struct_base_node;
        throw;
    }
}

StructExprFieldNode* Parser::ParseStructExprField() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expr = nullptr;
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
        return new StructExprFieldNode(pos, identifier, expr);
    } catch (std::exception&) {
        delete expr;
        throw;
    }
}

StructExprFieldsNode* Parser::ParseStructExprFields() {
    Position pos = tokens[parseIndex].pos;
    std::vector<StructExprFieldNode*> fields;
    StructExprFieldNode* field_node = nullptr;
    StructBaseNode* base = nullptr;
    try {
        field_node = ParseStructExprField();
        fields.emplace_back(field_node);
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::DotDot) {
                base = ParseStructBase();
                break;
            }
            if (tokens[parseIndex].type == TokenType::RBrace) {
                break;
            }
            field_node = ParseStructExprField();
            fields.emplace_back(field_node);
        }
        return new StructExprFieldsNode(pos, fields, base);
    } catch (const ParseError&) {
        for (auto it : fields) {
            delete it;
        }
        delete field_node;
        delete base;
        throw;
    }
}

StructBaseNode* Parser::ParseStructBase() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expr = nullptr;
    try {
        ConsumeString("..");
        expr = ParseExpression();
        return new StructBaseNode(pos, expr);
    } catch (std::exception&) {
        delete expr;
        throw;
    }
}

ExpressionNode *Parser::ParseLiteral() {
    Position pos = tokens[parseIndex].pos;
    std::vector<ExpressionNode*> expression_nodes;
    ExpressionNode* lhs = nullptr;
    ExpressionNode* rhs = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::IntegerLiteral) {
            return new IntLiteralNode(pos, StringToInt(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::StringLiteral ||
            tokens[parseIndex].type == TokenType::RawStringLiteral) {
            return new StringLiteralNode(pos, rust_str_to_cpp(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::CStringLiteral ||
            tokens[parseIndex].type == TokenType::RawCStringLiteral) {
            return new CStringLiteralNode(pos, rust_str_to_cpp(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::CharLiteral) {
            return new CharLiteralNode(pos, rust_char_to_cpp(tokens[parseIndex++].token));
        }
        if (tokens[parseIndex].type == TokenType::True) {
            parseIndex++;
            return new BoolLiteralNode(pos, true);
        }
        if (tokens[parseIndex].type == TokenType::False) {
            parseIndex++;
            return new BoolLiteralNode(pos, false);
        }
        if (tokens[parseIndex].type == TokenType::LBracket) {
            ConsumeString("[");
            if (tokens[parseIndex].type == TokenType::RBracket) {
                ConsumeString("]");
                return new ArrayLiteralNode(pos, expression_nodes, lhs, rhs);
            }
            auto* tmp = ParseExpression();
            if (tokens[parseIndex].type == TokenType::Semicolon) {
                lhs = tmp;
                ConsumeString(";");
                rhs = ParseExpression();
                ConsumeString("]");
                return new ArrayLiteralNode(pos, expression_nodes, lhs, rhs);
            }
            expression_nodes.emplace_back(tmp);
            while (tokens[parseIndex].type == TokenType::Comma) {
                ConsumeString(",");
                tmp = ParseExpression();
                expression_nodes.push_back(tmp);
            }
            ConsumeString("]");
            return new ArrayLiteralNode(pos, expression_nodes, lhs, rhs);
        }
        throw ParseError("Parse Error: Not A Literal", pos);
    } catch (std::exception&) {
        for (auto& it: expression_nodes) {
            delete it;
        }
        delete lhs;
        delete rhs;
        throw;
    }
}

PathExpressionNode *Parser::ParsePathExpression() {
    PathInExpressionNode* path_in_expression_node = nullptr;
    try {
        path_in_expression_node = ParsePathInExpression();
        return path_in_expression_node;
    } catch (std::exception&) {
        delete path_in_expression_node;
        throw;
    }
}


PathInExpressionNode* Parser::ParsePathInExpression() {
    Position pos = tokens[parseIndex].pos;
    PathIndentSegmentNode* tmp = nullptr;
    std::vector<PathIndentSegmentNode*> simple_path_segments;
    try {
        if (tokens[parseIndex].type == TokenType::ColonColon) {
            ConsumeString("::");
        }
        tmp = ParsePathIndentSegment();
        simple_path_segments.emplace_back(tmp);
        while (tokens[parseIndex].type == TokenType::ColonColon) {
            ConsumeString("::");
            tmp = ParsePathIndentSegment();
            simple_path_segments.emplace_back(tmp);
        }
        return new PathInExpressionNode(pos, simple_path_segments);
    } catch (std::exception&) {
        delete tmp;
        for (auto& it: simple_path_segments) {
            delete it;
        }
        throw;
    }
}

StatementsNode *Parser::ParseStatements() {
    Position pos = tokens[parseIndex].pos;
    std::vector<StatementNode*> statement_nodes;
    ExpressionNode* expression_node = nullptr;
    while (tokens[parseIndex].type != TokenType::RBrace) {
        StatementNode* tmp;
        uint32_t start = parseIndex;
        try {
            tmp = ParseStatement();
            statement_nodes.emplace_back(tmp);
            continue;
        } catch (std::exception&) {
            parseIndex = start;
        }

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

ConditionsNode *Parser::ParseConditions() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* tmp = nullptr;
    try {
        tmp = ParseExpression();
        // TODO Parse LetChain
        return new ConditionsNode(pos, tmp);
    } catch (std::exception&) {
        delete tmp;
        throw;
    }
}


/****************  Statement  ****************/
StatementNode* Parser::ParseStatement() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
            return new EmptyStatementNode(pos);
        }
        if (tokens[parseIndex].type == TokenType::Let) {
            auto* node = ParseLetStatement();
            return node;
        }
        auto* node = ParseExpressionStatement();
        return node;
    } catch (std::exception&) {
        throw;
    }
}

LetStatementNode *Parser::ParseLetStatement() {
    Position pos = tokens[parseIndex].pos;
    PatternNoTopAltNode* pattern_no_top_alt_node = nullptr;
    TypeNode* type_node = nullptr;
    ExpressionNode* expression_node = nullptr;
    BlockExpressionNode* block_expression_node = nullptr;
    try {
        ConsumeString("let");
        pattern_no_top_alt_node = ParsePatternNoTopAlt();
        if (tokens[parseIndex].type == TokenType::Colon) {
            ConsumeString(":");
            type_node = ParseType();
        }
        ConsumeString("=");
        expression_node = ParseExpression();
        if (tokens[parseIndex].type == TokenType::Else) {
            ConsumeString("else");
            block_expression_node = ParseBlockExpression();
            // TODO Check Whether the expression node is lazyBooleanExpression Or end with a '}'
        }
        ConsumeString(";");
        return new LetStatementNode(pos, pattern_no_top_alt_node, type_node,
            expression_node, block_expression_node);
    } catch (std::exception&) {
        delete pattern_no_top_alt_node;
        delete type_node;
        delete expression_node;
        delete block_expression_node;
        throw;
    }
}

ExpressionStatementNode *Parser::ParseExpressionStatement() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    ExpressionNode* expression_node = nullptr;
    try {
        expression_node = ParseExpressionWithoutBlock();
        ConsumeString(";");
        return new ExpressionStatementNode(pos, expression_node);
    } catch (std::exception&) {
        delete expression_node;
        expression_node = nullptr;
        parseIndex = start;
    }

    try {
        expression_node = ParseExpressionWithBlock();
        if (tokens[parseIndex].type == TokenType::Semicolon) {
            ConsumeString(";");
        }
        return new ExpressionStatementNode(pos, expression_node);
    } catch (std::exception&) {
        delete expression_node;
        throw ParseError("Parse Error: Failed to match ExpressionStatement", pos);
    }
}



/****************  Pattern  ****************/
PatternNode* Parser::ParsePattern() {
    Position pos = tokens[parseIndex].pos;
    std::vector<PatternNoTopAltNode*> pattern_no_top_alt_nodes;
    PatternNoTopAltNode* tmp = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::Or) {
            ConsumeString("|");
        }
        tmp = ParsePatternNoTopAlt();
        pattern_no_top_alt_nodes.emplace_back(tmp);
        while (tokens[parseIndex].type == TokenType::Or) {
            ConsumeString("|");
            tmp = ParsePatternNoTopAlt();
            pattern_no_top_alt_nodes.emplace_back(tmp);
        }
        return new PatternNode(pos, pattern_no_top_alt_nodes);
    } catch (std::exception&) {
        delete tmp;
        for (auto& it: pattern_no_top_alt_nodes) {
            delete it;
        }
        throw;
    }
}

PatternNoTopAltNode* Parser::ParsePatternNoTopAlt() {
    try {
        PatternNoTopAltNode* ret = ParsePatternWithoutRange();
        return ret;
    } catch (std::exception&) {
        throw;
    }
}

PatternWithoutRangeNode *Parser::ParsePatternWithoutRange() {
    Position pos = tokens[parseIndex].pos;
    uint32_t start = parseIndex;
    PatternWithoutRangeNode* tmp = nullptr;
    PatternNode* pattern_node = nullptr;
    try {
        tmp = ParseLiteralPattern();
        return tmp;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        tmp = ParseIdentifierPattern();
        return tmp;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ConsumeString("_");
        return new WildcardPatternNode(pos);
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ConsumeString("..");
        return new RestPatternNode(pos);
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ConsumeString("(");
        pattern_node = ParsePattern();
        ConsumeString(")");
        return new GroupedPatternNode(pos, pattern_node);
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        tmp = ParseSlicePattern();
        return tmp;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        tmp = ParsePathPattern();
        return tmp;
    } catch (std::exception&) {
        throw ParseError("Parse Error: Failed to Parse PatternWithoutRange", pos);
    }
}

LiteralPatternNode* Parser::ParseLiteralPattern() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expression_node = nullptr;
    try {
        bool have_minus_ = false;
        if (tokens[parseIndex].type == TokenType::Minus) {
            have_minus_ = true;
            ConsumeString("-");
        }
        expression_node = ParseLiteral();
        return new LiteralPatternNode(pos, have_minus_, expression_node);
    } catch (std::exception&) {
        delete expression_node;
        throw;
    }
}

IdentifierPatternNode* Parser::ParseIdentifierPattern() {
    Position pos = tokens[parseIndex].pos;
    PatternNoTopAltNode* pattern_no_top_alt_node = nullptr;
    try {
        bool is_ref_ = false;
        bool is_mut_ = false;
        if (tokens[parseIndex].type == TokenType::Ref) {
            ConsumeString("ref");
            is_ref_ = true;
        }
        if (tokens[parseIndex].type == TokenType::Mut) {
            ConsumeString("mut");
            is_mut_ = true;
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
        return new IdentifierPatternNode(pos, is_ref_, is_mut_, identifier, pattern_no_top_alt_node);
    } catch (std::exception&) {
        delete pattern_no_top_alt_node;
        throw;
    }
}

SlicePatternNode* Parser::ParseSlicePattern() {
    Position pos = tokens[parseIndex].pos;
    std::vector<PatternNode*> pattern_nodes;
    PatternNode* tmp = nullptr;
    try {
        ConsumeString("[");
        tmp = ParsePattern();
        pattern_nodes.emplace_back(tmp);
        while (tokens[parseIndex].type == TokenType::Comma) {
            ConsumeString(",");
            if (tokens[parseIndex].type == TokenType::RBracket) {
                break;
            }
            tmp = ParsePattern();
            pattern_nodes.emplace_back(tmp);
        }
        return new SlicePatternNode(pos, pattern_nodes);
    } catch (std::exception&) {
        delete tmp;
        for (auto& it: pattern_nodes) {
            delete it;
        }
        throw;
    }
}

PathPatternNode* Parser::ParsePathPattern() {
    Position pos = tokens[parseIndex].pos;
    ExpressionNode* expression_node = nullptr;
    try {
        expression_node = ParsePathExpression();
        return new PathPatternNode(pos, expression_node);
    } catch (std::exception&) {
        delete expression_node;
        throw;
    }
}

/****************  Types  ****************/
TypeNode* Parser::ParseType() {
    TypeNode* type_node = nullptr;
    try {
        type_node = ParseTypeNoBounds();
        return type_node;
    } catch (std::exception&) {
        delete type_node;
        throw;
    }
}

TypeNoBoundsNode* Parser::ParseTypeNoBounds() {
    Position pos = tokens[parseIndex].pos;
    TypeNoBoundsNode* ret = nullptr;
    uint32_t start = parseIndex;
    try {
        ret = ParseParenthesizedType();
        return ret;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ret = ParseTypePath();
        return ret;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ret = ParseTupleType();
        return ret;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ret = ParseArrayType();
        return ret;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ret = ParseSliceType();
        return ret;
    } catch (std::exception&) {
        parseIndex = start;
    }

    try {
        ret = ParseInferredType();
        return ret;
    } catch (std::exception&) {
        throw ParseError("Parse Error: Failed to Parse Type", pos);
    }
}

ParenthesizedTypeNode* Parser::ParseParenthesizedType() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("(");
        auto* tmp = ParseType();
        ConsumeString(")");
        return new ParenthesizedTypeNode(pos, tmp);
    } catch (std::exception&) {
        throw;
    }
}

TypePathNode* Parser::ParseTypePath() {
    Position pos = tokens[parseIndex].pos;
    std::vector<TypePathSegmentNode*> type_path_segment_nodes;
    TypePathSegmentNode* tmp = nullptr;
    try {
        if (tokens[parseIndex].type == TokenType::ColonColon) {
            ConsumeString("::");
        }
        tmp = ParseTypePathSegment();
        type_path_segment_nodes.emplace_back(tmp);
        while (tokens[parseIndex].type == TokenType::ColonColon) {
            tmp = ParseTypePathSegment();
            type_path_segment_nodes.emplace_back(tmp);
        }
        return new TypePathNode(pos, type_path_segment_nodes);
    } catch (std::exception&) {
        delete tmp;
        for (auto& it: type_path_segment_nodes) {
            delete it;
        }
        throw;
    }
}

TypePathSegmentNode* Parser::ParseTypePathSegment() {
    Position pos = tokens[parseIndex].pos;
    PathIndentSegmentNode* tmp = nullptr;
    try {
        tmp = ParsePathIndentSegment();
        return new TypePathSegmentNode(pos, tmp);
    } catch (std::exception&) {
        delete tmp;
        throw;
    }
}

TupleTypeNode* Parser::ParseTupleType() {
    Position pos = tokens[parseIndex].pos;
    std::vector<TypeNode*> type_nodes;
    try {
        ConsumeString("(");
        while (tokens[parseIndex].type != TokenType::RParen) {
            auto* tmp = ParseType();
            type_nodes.emplace_back(tmp);
            if (tokens[parseIndex].type == TokenType::Comma) {
                ConsumeString(",");
            } else {
                break;
            }
        }
        ConsumeString(")");
        return new TupleTypeNode(pos, type_nodes);
    } catch (std::exception&) {
        throw;
    }
}

ArrayTypeNode* Parser::ParseArrayType() {
    Position pos = tokens[parseIndex].pos;
    TypeNode* type_node = nullptr;
    ExpressionNode* expression_node = nullptr;
    try {
        ConsumeString("[");
        type_node = ParseType();
        ConsumeString(";");
        expression_node = ParseExpression();
        ConsumeString("]");
        return new ArrayTypeNode(pos, type_node, expression_node);
    } catch (std::exception&) {
        delete type_node;
        delete expression_node;
        throw;
    }
}

SliceTypeNode* Parser::ParseSliceType() {
    Position pos = tokens[parseIndex].pos;
    TypeNode* type_node = nullptr;
    try {
        ConsumeString("[");
        type_node = ParseType();
        ConsumeString("]");
        return new SliceTypeNode(pos, type_node);
    } catch (std::exception&) {
        delete type_node;
        throw;
    }
}

InferredTypeNode* Parser::ParseInferredType() {
    Position pos = tokens[parseIndex].pos;
    try {
        ConsumeString("_");
        return new InferredTypeNode(pos);
    } catch (std::exception&) {
        throw;
    }
}


/****************  Paths  ****************/
PathIndentSegmentNode *Parser::ParsePathIndentSegment() {
    Position pos = tokens[parseIndex].pos;
    try {
        if (tokens[parseIndex].type == TokenType::Super || tokens[parseIndex].type == TokenType::Self ||
            tokens[parseIndex].type == TokenType::SELF || tokens[parseIndex].type == TokenType::Crate ||
            tokens[parseIndex].type == TokenType::Identifier) {
            return new PathIndentSegmentNode(pos, tokens[parseIndex].type, tokens[parseIndex++].token);
        }
        throw ParseError("Parse Error: Invalid SimplePathSegment", pos);
    } catch (std::exception&) {
        throw;
    }
}