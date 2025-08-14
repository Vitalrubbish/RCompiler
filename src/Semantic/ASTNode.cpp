#include "Semantic/ASTNode.h"

/****************  Items  ****************/
CrateNode::~CrateNode() {
    for (auto &it: items_) {
        delete it;
    }
}

FunctionNode::~FunctionNode() {
    delete function_parameters_;
    delete type_;
    /*for (auto& it: where_clause_items_) {
        delete it;
    }*/
    delete block_expression_;
}

FunctionParametersNode::~FunctionParametersNode() {
    for (auto &it: function_params_) {
        delete it;
    }
}

FunctionParamNode::~FunctionParamNode() {
    // delete function_param_pattern_;
    delete type_;
}

FunctionParamPatternNode::~FunctionParamPatternNode() {
    delete pattern_no_top_alt_;
    delete type_;
}

StructNode::~StructNode() {
    for (auto &it: struct_field_nodes_) {
        delete it;
    }
}

StructFieldNode::~StructFieldNode() {
    delete type_node_;
}

StructExpressionNode::~StructExpressionNode() {
    delete path_in_expression_node_;
    delete struct_expr_fields_node_;
    delete struct_base_node_;
}

StructExprFieldsNode::~StructExprFieldsNode() {
    for (auto &it: struct_expr_field_nodes_) {
        delete it;
    }
}

StructExprFieldNode::~StructExprFieldNode() {
    delete expression_node_;
}

StructBaseNode::~StructBaseNode() {
    delete expression_node_;
}

EnumerationNode::~EnumerationNode() {
    for (auto &it: enum_variant_nodes_) {
        delete it;
    }
}

EnumVariantNode::~EnumVariantNode() {
    delete enum_variant_discriminant_node_;
    delete enum_variant_struct_node_;
}

EnumVariantStructNode::~EnumVariantStructNode() {
    for (auto &it: struct_field_nodes_) {
        delete it;
    }
}

EnumVariantDiscriminantNode::~EnumVariantDiscriminantNode() {
    delete expression_node_;
}

ConstantItemNode::~ConstantItemNode() {
    delete type_node_;
    delete expression_node_;
}

AssociatedItemNode::~AssociatedItemNode() {
    delete constant_item_node_;
    delete function_node_;
}

InherentImplNode::~InherentImplNode() {
    delete type_node_;
    for (auto &it: associated_item_nodes_) {
        delete it;
    }
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

InfiniteLoopExpressionNode::~InfiniteLoopExpressionNode() {
    delete block_expression_;
}

PredicateLoopExpressionNode::~PredicateLoopExpressionNode() {
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

MatchArmsNode::~MatchArmsNode() {
    for (auto &it: match_arm_nodes_) {
        delete it;
    }
    for (auto &it: expression_nodes_) {
        delete it;
    }
}

MatchArmNode::~MatchArmNode() {
    delete pattern_node_;
    delete match_arm_guard_;
}

TupleExpressionNode::~TupleExpressionNode() {
    for (auto &it: expressions_) {
        delete it;
    }
}

JumpExpressionNode::~JumpExpressionNode() {
    delete expression_;
}

AssignmentExpressionNode::~AssignmentExpressionNode() {
    delete lhs_;
    delete rhs_;
}

LogicOrExpressionNode::~LogicOrExpressionNode() {
    delete lhs_;
    delete rhs_;
}

LogicAndExpressionNode::~LogicAndExpressionNode() {
    delete lhs_;
    delete rhs_;
}

ComparisonExpressionNode::~ComparisonExpressionNode() {
    delete lhs_;
    delete rhs_;
}

BitwiseOrExpressionNode::~BitwiseOrExpressionNode() {
    delete lhs_;
    delete rhs_;
}

BitwiseXorExpressionNode::~BitwiseXorExpressionNode() {
    delete lhs_;
    delete rhs_;
}

BitwiseAndExpressionNode::~BitwiseAndExpressionNode() {
    delete lhs_;
    delete rhs_;
}

ShiftExpressionNode::~ShiftExpressionNode() {
    delete lhs_;
    delete rhs_;
}

AddMinusExpressionNode::~AddMinusExpressionNode() {
    delete lhs_;
    delete rhs_;
}

MulDivModExpressionNode::~MulDivModExpressionNode() {
    delete lhs_;
    delete rhs_;
}

TypeCastExpressionNode::~TypeCastExpressionNode() {
    delete expression_;
}

UnaryExpressionNode::~UnaryExpressionNode() {
    delete expression_;
}

FunctionCallExpressionNode::~FunctionCallExpressionNode() {
    delete callee_;
    for (auto &it: params_) {
        delete it;
    }
}

ArrayIndexExpressionNode::~ArrayIndexExpressionNode() {
    delete base_;
    delete index_;
}

MemberAccessExpressionNode::~MemberAccessExpressionNode() {
    delete base_;
}

ArrayLiteralNode::~ArrayLiteralNode() {
    for (auto &it: expressions_) {
        delete it;
    }
    delete lhs_;
    delete rhs_;
}

GroupedExpressionNode::~GroupedExpressionNode() {
    delete expression_;
}

PathInExpressionNode::~PathInExpressionNode() {
    for (auto &it: path_indent_segments_) {
        delete it;
    }
}

/****************  Patterns  ****************/
PatternNode::~PatternNode() {
    for (auto &it: pattern_no_top_alts_) {
        delete it;
    }
}

LiteralPatternNode::~LiteralPatternNode() {
    delete expression_;
}

IdentifierPatternNode::~IdentifierPatternNode() {
    delete node_;
}

GroupedPatternNode::~GroupedPatternNode() {
    delete pattern_;
}

SlicePatternNode::~SlicePatternNode() {
    for (auto &it: patterns_) {
        delete it;
    }
}

PathPatternNode::~PathPatternNode() {
    delete expression_;
}

/****************  Support Node for Expression  ****************/

ConditionsNode::~ConditionsNode() {
    delete expression_;
}

StatementsNode::~StatementsNode() {
    for (auto &it: statements_) {
        delete it;
    }
    delete expression_;
}

/****************  Types  ***************/
ParenthesizedTypeNode::~ParenthesizedTypeNode() {
    delete type_;
}

TypePathNode::~TypePathNode() {
    for (auto &it: type_path_segment_nodes_) {
        delete it;
    }
}

TypePathSegmentNode::~TypePathSegmentNode() {
    delete path_indent_segment_node_;
}

TupleTypeNode::~TupleTypeNode() {
    for (auto &it: type_nodes_) {
        delete it;
    }
}

SliceTypeNode::~SliceTypeNode() {
    delete type_;
}

ArrayTypeNode::~ArrayTypeNode() {
    delete type_;
    delete expression_node_;
}
