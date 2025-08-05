#include <regex>
#include <vector>
#include "../../include/Lexer/Lexer.h"

std::string Keyword[] = {
    "as", "break", "const", "continue", "crate", "else", "enum", "extern",
    "false", "fn", "for", "if", "impl", "in", "let", "loop", "match", "mod",
    "move", "mut", "pub", "ref", "return", "self", "Self", "static", "struct",
    "super", "trait", "true", "type", "unsafe", "use", "where", "while","async",
    "await", "dyn","abstract", "become", "box", "do", "final", "macro", "override",
    "priv","typeof", "unsized", "virtual", "yield", "try"
};

Lexer::Lexer() {
    rules.emplace_back(std::pair(TokenType::As, "as"));
    rules.emplace_back(std::pair(TokenType::Break, "break"));
    rules.emplace_back(std::pair(TokenType::Const, "const"));
    rules.emplace_back(std::pair(TokenType::Continue, "continue"));
    rules.emplace_back(std::pair(TokenType::Crate, "crate"));
    rules.emplace_back(std::pair(TokenType::Else, "else"));
    rules.emplace_back(std::pair(TokenType::Enum, "enum"));
    rules.emplace_back(std::pair(TokenType::Extern, "extern"));
    rules.emplace_back(std::pair(TokenType::False, "false"));
    rules.emplace_back(std::pair(TokenType::Fn, "fn"));
    rules.emplace_back(std::pair(TokenType::For, "for"));
    rules.emplace_back(std::pair(TokenType::If, "if"));
    rules.emplace_back(std::pair(TokenType::Impl, "impl"));
    rules.emplace_back(std::pair(TokenType::In, "in"));
    rules.emplace_back(std::pair(TokenType::Let, "let"));
    rules.emplace_back(std::pair(TokenType::Loop, "loop"));
    rules.emplace_back(std::pair(TokenType::Match, "match"));
    rules.emplace_back(std::pair(TokenType::Mod, "mod"));
    rules.emplace_back(std::pair(TokenType::Move, "move"));
    rules.emplace_back(std::pair(TokenType::Mut, "mut"));
    rules.emplace_back(std::pair(TokenType::Pub, "pub"));
    rules.emplace_back(std::pair(TokenType::Ref, "ref"));
    rules.emplace_back(std::pair(TokenType::Return, "return"));
    rules.emplace_back(std::pair(TokenType::Self, "self"));
    rules.emplace_back(std::pair(TokenType::SELF, "Self"));
    rules.emplace_back(std::pair(TokenType::Static, "static"));
    rules.emplace_back(std::pair(TokenType::Struct, "struct"));
    rules.emplace_back(std::pair(TokenType::Super, "super"));
    rules.emplace_back(std::pair(TokenType::Trait, "trait"));
    rules.emplace_back(std::pair(TokenType::True, "true"));
    rules.emplace_back(std::pair(TokenType::Type, "type"));
    rules.emplace_back(std::pair(TokenType::Unsafe, "unsafe"));
    rules.emplace_back(std::pair(TokenType::Use, "use"));
    rules.emplace_back(std::pair(TokenType::Where, "where"));
    rules.emplace_back(std::pair(TokenType::While, "while"));
    rules.emplace_back(std::pair(TokenType::Async, "async"));
    rules.emplace_back(std::pair(TokenType::Await, "await"));
    rules.emplace_back(std::pair(TokenType::Dyn, "dyn"));
    rules.emplace_back(std::pair(TokenType::Abstract, "abstract"));
    rules.emplace_back(std::pair(TokenType::Become, "become"));
    rules.emplace_back(std::pair(TokenType::Box, "box"));
    rules.emplace_back(std::pair(TokenType::Do, "do"));
    rules.emplace_back(std::pair(TokenType::Final, "final"));
    rules.emplace_back(std::pair(TokenType::Macro, "macro"));
    rules.emplace_back(std::pair(TokenType::Override, "override"));
    rules.emplace_back(std::pair(TokenType::Priv, "priv"));
    rules.emplace_back(std::pair(TokenType::Typeof, "typeof"));
    rules.emplace_back(std::pair(TokenType::Unsized, "unsized"));
    rules.emplace_back(std::pair(TokenType::Virtual, "virtual"));
    rules.emplace_back(std::pair(TokenType::Yield, "yield"));
    rules.emplace_back(std::pair(TokenType::Try, "try"));
    // Match Keywords

    rules.emplace_back(std::pair(TokenType::Identifier, "[a-zA-Z][a-zA-Z0-9]*"));
    // Match Identifier

    rules.emplace_back(std::pair(TokenType::IntegerLiteral, R"(((0b[_01]*[01][_01]*)|(0o[_0-7]*[0-7][_0-7]*)|(0x[_0-9a-fA-F]*[0-9a-fA-F][_0-9a-fA-F]*)|([0-9][_0-9]*))(u8|i8|u16|i16|u32|i32|u64|i64|u128|i128|usize|isize)?)"));
    // Match Integer Identifier

    rules.emplace_back(std::pair(TokenType::DotDotDot, R"(\.\.\.)"));
    rules.emplace_back(std::pair(TokenType::DotDotEq, R"(\.\.=)"));
    rules.emplace_back(std::pair(TokenType::SLEq, R"(<<=)"));
    rules.emplace_back(std::pair(TokenType::SREq, R"(>>=)"));
    rules.emplace_back(std::pair(TokenType::LEq, R"(<=)"));
    rules.emplace_back(std::pair(TokenType::EqEq, R"(==)"));
    rules.emplace_back(std::pair(TokenType::NEq, R"(!=)"));
    rules.emplace_back(std::pair(TokenType::GEq, R"(>=)"));
    rules.emplace_back(std::pair(TokenType::AndAnd, R"(&&)"));
    rules.emplace_back(std::pair(TokenType::OrOr, R"(\|\|)"));
    rules.emplace_back(std::pair(TokenType::SL, R"(<<)"));
    rules.emplace_back(std::pair(TokenType::SR, R"(>>)"));
    rules.emplace_back(std::pair(TokenType::PlusEq, R"(\+=)"));
    rules.emplace_back(std::pair(TokenType::MinusEq, R"(-=)"));
    rules.emplace_back(std::pair(TokenType::MulEq, R"(\*=)"));
    rules.emplace_back(std::pair(TokenType::DivEq, R"(/=)"));
    rules.emplace_back(std::pair(TokenType::ModEq, R"(%=)"));
    rules.emplace_back(std::pair(TokenType::XorEq, R"(\^=)"));
    rules.emplace_back(std::pair(TokenType::AndEq, R"(&=)"));
    rules.emplace_back(std::pair(TokenType::OrEq, R"(\|=)"));
    rules.emplace_back(std::pair(TokenType::DotDot, R"(\.\.)"));
    rules.emplace_back(std::pair(TokenType::ColonColon, R"(::)"));
    rules.emplace_back(std::pair(TokenType::RArrow, R"(->)"));
    rules.emplace_back(std::pair(TokenType::LArrow, R"(<-)"));
    rules.emplace_back(std::pair(TokenType::FatArrow, R"(=>)"));
    rules.emplace_back(std::pair(TokenType::Eq, R"(=)"));
    rules.emplace_back(std::pair(TokenType::Lt, R"(<)"));
    rules.emplace_back(std::pair(TokenType::Gt, R"(>)"));
    rules.emplace_back(std::pair(TokenType::Not, R"(!)"));
    rules.emplace_back(std::pair(TokenType::Tilde, R"(~)"));
    rules.emplace_back(std::pair(TokenType::Plus, R"(\+)"));
    rules.emplace_back(std::pair(TokenType::Minus, R"(-)"));
    rules.emplace_back(std::pair(TokenType::Mul, R"(\*)"));
    rules.emplace_back(std::pair(TokenType::Div, R"(/)"));
    rules.emplace_back(std::pair(TokenType::MOD, R"(%)"));
    rules.emplace_back(std::pair(TokenType::Xor, R"(\^)"));
    rules.emplace_back(std::pair(TokenType::And, R"(&)"));
    rules.emplace_back(std::pair(TokenType::Or, R"(\|)"));
    rules.emplace_back(std::pair(TokenType::At, R"(@)"));
    rules.emplace_back(std::pair(TokenType::Dot, R"(\.)"));
    rules.emplace_back(std::pair(TokenType::Comma, R"(,)"));
    rules.emplace_back(std::pair(TokenType::Semicolon, R"(;)"));
    rules.emplace_back(std::pair(TokenType::Colon, R"(:)"));
    rules.emplace_back(std::pair(TokenType::Pound, R"(#)"));
    rules.emplace_back(std::pair(TokenType::Dollar, R"($)"));
    rules.emplace_back(std::pair(TokenType::Question, R"(\?)"));
    rules.emplace_back(std::pair(TokenType::Underscore, R"(_)"));
    rules.emplace_back(std::pair(TokenType::LBrace, R"(\{)"));
    rules.emplace_back(std::pair(TokenType::RBrace, R"(\})"));
    rules.emplace_back(std::pair(TokenType::LBracket, R"(\[)"));
    rules.emplace_back(std::pair(TokenType::RBracket, R"(\])"));
    rules.emplace_back(std::pair(TokenType::LParen, R"(\()"));
    rules.emplace_back(std::pair(TokenType::RParen, R"(\))"));
    // Match Punctuation

    rules.emplace(std::pair(TokenType::StringLiteral, R"(\"([^\"\\]|\\.)*\")");
    // Match StringLiteral

    rules.emplace_back(std::pair(TokenType::WhiteSpace, "[ \r\t\n]+"));
    // Match WhiteSpace
}

Token Lexer::GetNextToken(std::string& str) const {
    uint32_t bestMatchLength = 0;
    TokenType bestMatchType = TokenType::None;
    uint32_t len = 0;
    for (const auto& it: rules) {
        std::regex pattern(it.second);
        std::smatch match;
        if (std::regex_search(str, match, pattern) && match.position(0) == 0) {
            len = match.length(0);
            if (len > bestMatchLength) {
                bestMatchLength = len;
                bestMatchType = it.first;
            }
        }
    }
    std::string token = str.substr(0, bestMatchLength);
    str = str.substr(bestMatchLength, str.size() - bestMatchLength);
    return Token{token, bestMatchType};
}
