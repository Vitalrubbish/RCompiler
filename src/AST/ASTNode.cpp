#include "../../include/AST/ASTNode.h"

/****************  Items  ****************/
CrateNode::~CrateNode() {
    for (auto & it : items_) {
        delete it;
    }
}

VisItemNode::~VisItemNode() {
    delete node_;
}

FunctionNode::~FunctionNode() {
    /*for (auto& it: generic_param_) {
        delete it;
    }*/
    delete function_parameters_;
    delete type_;
    /*for (auto& it: where_clause_items_) {
        delete it;
    }*/
    delete block_expression_;
}

FunctionParametersNode::~FunctionParametersNode() {
    for (auto& it: function_params_) {
        delete it;
    }
}

FunctionParamNode::~FunctionParamNode() {
     // delete function_param_pattern_;
    delete type_;
}

/****************  Statement  ****************/
LetStatementNode::~LetStatementNode() {
    // delete pattern_no_top_alt_;
    delete type_;
    delete expression_;
    delete block_expression_;
}

ExpressionStatementNode::~ExpressionStatementNode() {
    delete expression_;
}

InfiniteLoopExpression::~InfiniteLoopExpression() {
    delete block_expression_;
}

PredicateLoopExpression::~PredicateLoopExpression() {
    delete conditions_;
    delete block_expression_;
}

/****************  Expression  ****************/
BlockExpressionNode::~BlockExpressionNode() {
    delete statements_;
}

IfExpressionNode::~IfExpressionNode() {
    delete conditions_;
    delete true_block_expression_;
    delete false_block_expression_;
    delete if_expression_;
}

MatchExpressionNode::~MatchExpressionNode() {
    delete expression_;
    // delete match_arms_;
}

JumpExpressionNode::~JumpExpressionNode() {
    delete expression_;
}

AssignmentExpressionNode::~AssignmentExpressionNode() {
    delete lhs_;
    delete rhs_;
}

LogicOrExpressionNode::~LogicOrExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

LogicAndExpressionNode::~LogicAndExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

ComparisonExpressionNode::~ComparisonExpressionNode() {
    delete lhs_;
    delete rhs_;
}

BitwiseOrExpressionNode::~BitwiseOrExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

BitwiseXorExpressionNode::~BitwiseXorExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

BitwiseAndExpressionNode::~BitwiseAndExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

ShiftExpressionNode::~ShiftExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

AddMinusExpressionNode::~AddMinusExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

MulDivModExpressionNode::~MulDivModExpressionNode() {
    for (auto& it: expressions_) {
        delete it;
    }
}

TypeCastExpressionNode::~TypeCastExpressionNode() {
    delete expression_;
}

UnaryExpressionNode::~UnaryExpressionNode() {
    delete expression_;
}

MemberCallExpressionNode::~MemberCallExpressionNode() {
    for (auto& it: suffixes_) {
        delete it;
    }
}

GroupedExpressionNode::~GroupedExpressionNode() {
    delete expression_;
}

PathExpressionNode::~PathExpressionNode() {
    for (auto& it: simple_path_segments_) {
        delete it;
    }
}

/****************  Patterns  ****************/
PatternNode::~PatternNode() {
    for (auto& it: pattern_no_top_alts_) {
        // delete it;
    }
}
/****************  Support Node for Expression  ****************/

ConditionsNode::~ConditionsNode() {
    delete expression_;
}

StatementsNode::~StatementsNode() {
    for (auto& it: statements_) {
        delete it;
    }
    delete expression_;
}

/****************  Types  ***************/
ParenthesizedTypeNode::~ParenthesizedTypeNode() {
    delete type_;
}

