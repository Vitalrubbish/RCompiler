#include "../../include/ASTNode/ASTNode.h"

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
    delete function_param_pattern_;
    delete type_;
}

/****************  Statement  ****************/
LetStatementNode::~LetStatementNode() {
    delete pattern_no_top_alt_;
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
    delete match_arms_;
}

/****************  Support Node for Expression  ****************/

ConditionsNode::~ConditionsNode() {
    delete expression_;
}

