#ifndef TOKENTYPE_H
#define TOKENTYPE_H
enum class TokenType {
    LineComment,
    BlockComment,
    WhiteSpace,

    As, Break, Const, Continue, Crate, Else, Enum, False,
    Fn, For, If, Impl, In, Let, Loop, Match, Mod, Move,
    Mut, Ref, Return, Self, SELF, Static, Struct,
    Super, Trait, True, Type, Unsafe, Use, Where, While,
    Dyn, Abstract, Become, Box, Do, Final, Macro, Override,
    Priv, Typeof, Unsized, Virtual, Yield, Try,
    Identifier,
    CharLiteral,
    ByteLiteral,
    StringLiteral,
    RawStringLiteral,
    ByteStringLiteral,
    RawByteStringLiteral,
    CStringLiteral,
    RawCStringLiteral,
    IntegerLiteral,
    ReservedIntegerLiteral,
    FloatLiteral,
    DotDotDot, DotDotEq, SLEq, SREq, LEq, EqEq, NEq, GEq, AndAnd,
    OrOr, SL, SR, PlusEq, MinusEq, MulEq, DivEq, ModEq, XorEq,
    AndEq, OrEq, DotDot, ColonColon, RArrow, LArrow, FatArrow, Eq,
    Lt, Gt, Not, Tilde, Plus, Minus, Mul, Div, MOD, Xor, And, Or,
    At, Dot, Comma, Semicolon, Colon, Pound, Dollar, Question,
    Underscore, LBrace, RBrace, LBracket, RBracket, LParen, RParen,
    None
};
#endif //TOKENTYPE_H
