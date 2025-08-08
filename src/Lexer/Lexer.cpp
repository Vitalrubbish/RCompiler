#include <vector>
#include "../../error/Error.h"
#include "../../include/Lexer/Lexer.h"

Lexer::Lexer() {
    letter_rules.emplace_back(TokenType::As, std::regex("as"));
    letter_rules.emplace_back(TokenType::Break, std::regex("break"));
    letter_rules.emplace_back(TokenType::Const, std::regex("const"));
    letter_rules.emplace_back(TokenType::Continue, std::regex("continue"));
    letter_rules.emplace_back(TokenType::Crate, std::regex("crate"));
    letter_rules.emplace_back(TokenType::Else, std::regex("else"));
    letter_rules.emplace_back(TokenType::Enum, std::regex("enum"));
    letter_rules.emplace_back(TokenType::False, std::regex("false"));
    letter_rules.emplace_back(TokenType::Fn, std::regex("fn"));
    letter_rules.emplace_back(TokenType::For, std::regex("for"));
    letter_rules.emplace_back(TokenType::If, std::regex("if"));
    letter_rules.emplace_back(TokenType::Impl, std::regex("impl"));
    letter_rules.emplace_back(TokenType::In, std::regex("in"));
    letter_rules.emplace_back(TokenType::Let, std::regex("let"));
    letter_rules.emplace_back(TokenType::Loop, std::regex("loop"));
    letter_rules.emplace_back(TokenType::Match, std::regex("match"));
    letter_rules.emplace_back(TokenType::Mod, std::regex("mod"));
    letter_rules.emplace_back(TokenType::Move, std::regex("move"));
    letter_rules.emplace_back(TokenType::Mut, std::regex("mut"));
    letter_rules.emplace_back(TokenType::Ref, std::regex("ref"));
    letter_rules.emplace_back(TokenType::Return, std::regex("return"));
    letter_rules.emplace_back(TokenType::Static, std::regex("static"));
    letter_rules.emplace_back(TokenType::Struct, std::regex("struct"));
    letter_rules.emplace_back(TokenType::Super, std::regex("super"));
    letter_rules.emplace_back(TokenType::Trait, std::regex("trait"));
    letter_rules.emplace_back(TokenType::True, std::regex("true"));
    letter_rules.emplace_back(TokenType::Type, std::regex("type"));
    letter_rules.emplace_back(TokenType::Unsafe, std::regex("unsafe"));
    letter_rules.emplace_back(TokenType::Use, std::regex("use"));
    letter_rules.emplace_back(TokenType::Where, std::regex("where"));
    letter_rules.emplace_back(TokenType::While, std::regex("while"));
    letter_rules.emplace_back(TokenType::Dyn, std::regex("dyn"));
    letter_rules.emplace_back(TokenType::Abstract, std::regex("abstract"));
    letter_rules.emplace_back(TokenType::Become, std::regex("become"));
    letter_rules.emplace_back(TokenType::Box, std::regex("box"));
    letter_rules.emplace_back(TokenType::Do, std::regex("do"));
    letter_rules.emplace_back(TokenType::Final, std::regex("final"));
    letter_rules.emplace_back(TokenType::Macro, std::regex("macro"));
    letter_rules.emplace_back(TokenType::Override, std::regex("override"));
    letter_rules.emplace_back(TokenType::Priv, std::regex("priv"));
    letter_rules.emplace_back(TokenType::Typeof, std::regex("typeof"));
    letter_rules.emplace_back(TokenType::Unsized, std::regex("unsized"));
    letter_rules.emplace_back(TokenType::Virtual, std::regex("virtual"));
    letter_rules.emplace_back(TokenType::Yield, std::regex("yield"));
    letter_rules.emplace_back(TokenType::Try, std::regex("try"));
    // Match Keywords

    letter_rules.emplace_back(TokenType::Identifier, std::regex("[a-zA-Z][_a-zA-Z0-9]*"));
    // Match Identifier

    non_letter_rules.emplace_back(TokenType::DotDotDot, std::regex(R"(\.\.\.)"));
    non_letter_rules.emplace_back(TokenType::DotDotEq, std::regex(R"(\.\.=)"));
    non_letter_rules.emplace_back(TokenType::SLEq, std::regex(R"(<<=)"));
    non_letter_rules.emplace_back(TokenType::SREq, std::regex(R"(>>=)"));
    non_letter_rules.emplace_back(TokenType::LEq, std::regex(R"(<=)"));
    non_letter_rules.emplace_back(TokenType::EqEq, std::regex(R"(==)"));
    non_letter_rules.emplace_back(TokenType::NEq, std::regex(R"(!=)"));
    non_letter_rules.emplace_back(TokenType::GEq, std::regex(R"(>=)"));
    non_letter_rules.emplace_back(TokenType::AndAnd, std::regex(R"(&&)"));
    non_letter_rules.emplace_back(TokenType::OrOr, std::regex(R"(\|\|)"));
    non_letter_rules.emplace_back(TokenType::SL, std::regex(R"(<<)"));
    non_letter_rules.emplace_back(TokenType::SR, std::regex(R"(>>)"));
    non_letter_rules.emplace_back(TokenType::PlusEq, std::regex(R"(\+=)"));
    non_letter_rules.emplace_back(TokenType::MinusEq, std::regex(R"(-=)"));
    non_letter_rules.emplace_back(TokenType::MulEq, std::regex(R"(\*=)"));
    non_letter_rules.emplace_back(TokenType::DivEq, std::regex(R"(/=)"));
    non_letter_rules.emplace_back(TokenType::ModEq, std::regex(R"(%=)"));
    non_letter_rules.emplace_back(TokenType::XorEq, std::regex(R"(\^=)"));
    non_letter_rules.emplace_back(TokenType::AndEq, std::regex(R"(&=)"));
    non_letter_rules.emplace_back(TokenType::OrEq, std::regex(R"(\|=)"));
    non_letter_rules.emplace_back(TokenType::DotDot, std::regex(R"(\.\.)"));
    non_letter_rules.emplace_back(TokenType::ColonColon, std::regex(R"(::)"));
    non_letter_rules.emplace_back(TokenType::RArrow, std::regex(R"(->)"));
    non_letter_rules.emplace_back(TokenType::LArrow, std::regex(R"(<-)"));
    non_letter_rules.emplace_back(TokenType::FatArrow, std::regex(R"(=>)"));
    non_letter_rules.emplace_back(TokenType::Eq, std::regex(R"(=)"));
    non_letter_rules.emplace_back(TokenType::Lt, std::regex(R"(<)"));
    non_letter_rules.emplace_back(TokenType::Gt, std::regex(R"(>)"));
    non_letter_rules.emplace_back(TokenType::Not, std::regex(R"(!)"));
    non_letter_rules.emplace_back(TokenType::Tilde, std::regex(R"(~)"));
    non_letter_rules.emplace_back(TokenType::Plus, std::regex(R"(\+)"));
    non_letter_rules.emplace_back(TokenType::Minus, std::regex(R"(-)"));
    non_letter_rules.emplace_back(TokenType::Mul, std::regex(R"(\*)"));
    non_letter_rules.emplace_back(TokenType::Div, std::regex(R"(/)"));
    non_letter_rules.emplace_back(TokenType::MOD, std::regex(R"(%)"));
    non_letter_rules.emplace_back(TokenType::Xor, std::regex(R"(\^)"));
    non_letter_rules.emplace_back(TokenType::And, std::regex(R"(&)"));
    non_letter_rules.emplace_back(TokenType::Or, std::regex(R"(\|)"));
    non_letter_rules.emplace_back(TokenType::At, std::regex(R"(@)"));
    non_letter_rules.emplace_back(TokenType::Dot, std::regex(R"(\.)"));
    non_letter_rules.emplace_back(TokenType::Comma, std::regex(R"(,)"));
    non_letter_rules.emplace_back(TokenType::Semicolon, std::regex(R"(;)"));
    non_letter_rules.emplace_back(TokenType::Colon, std::regex(R"(:)"));
    non_letter_rules.emplace_back(TokenType::Pound, std::regex(R"(#)"));
    non_letter_rules.emplace_back(TokenType::Dollar, std::regex(R"(\$)"));
    non_letter_rules.emplace_back(TokenType::Question, std::regex(R"(\?)"));
    non_letter_rules.emplace_back(TokenType::Underscore, std::regex(R"(_)"));
    non_letter_rules.emplace_back(TokenType::LBrace, std::regex(R"(\{)"));
    non_letter_rules.emplace_back(TokenType::RBrace, std::regex(R"(\})"));
    non_letter_rules.emplace_back(TokenType::LBracket, std::regex(R"(\[)"));
    non_letter_rules.emplace_back(TokenType::RBracket, std::regex(R"(\])"));
    non_letter_rules.emplace_back(TokenType::LParen, std::regex(R"(\()"));
    non_letter_rules.emplace_back(TokenType::RParen, std::regex(R"(\))"));
    // Match Punctuation

    non_letter_rules.emplace_back(TokenType::IntegerLiteral, std::regex(R"(((0b[_01]*[01][_01]*)|(0o[_0-7]*[0-7][_0-7]*)|(0x[_0-9a-fA-F]*[0-9a-fA-F][_0-9a-fA-F]*)|([0-9][_0-9]*))(u32|i32|usize|isize)?)"));
    // Match Integer Literal

    non_letter_rules.emplace_back(TokenType::ReservedIntegerLiteral, std::regex(R"(((0b[_0-9]*[0-9][_0-9]*)|(0o[_0-9]*[0-9][_0-9]*)|(0x[_0-9a-fA-F]*[0-9a-fA-F][_0-9a-fA-F]*)|([0-9][_0-9]*))(u32|i32|usize|isize)?)"));
    // Match Reserved Integer Literal

    non_letter_rules.emplace_back(TokenType::CharLiteral, std::regex(R"('([^'\\\n\r\t]|\\[nrt'"\\0]|\\x[0-7][0-9a-fA-F])')"));
    // Match Char Literal

    letter_rules.emplace_back(TokenType::ByteLiteral, std::regex(R"(b'([^'\\\n\r\t]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2})')"));
    // Match Byte Literal

    string_rules.emplace_back(TokenType::StringLiteral, std::regex(R"("([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*")"));
    // Match String Literal

    string_rules.emplace_back(TokenType::RawStringLiteral, std::regex(R"(r(#*)\"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*\"\1)"));
    // Match Raw-String Literal

    string_rules.emplace_back(TokenType::ByteStringLiteral, std::regex(R"(b"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*")"));
    // Match Byte-String Literal

    string_rules.emplace_back(TokenType::RawByteStringLiteral, std::regex(R"(br(#*)\"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*\"\1)"));
    // Match Raw-Byte-String Literal

    string_rules.emplace_back(TokenType::CStringLiteral, std::regex(R"(c"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*")"));
    // Match C-String Literal

    string_rules.emplace_back(TokenType::CStringLiteral, std::regex(R"(cr(#*)\"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*\"\1)"));
    // Match C-String Literal

    non_letter_rules.emplace_back(TokenType::WhiteSpace, std::regex("[ \r\t\n]+"));
    // Match WhiteSpace
}



Token Lexer::GetNextToken(std::string& str) const {
    uint32_t bestMatchLength = 0;
    TokenType bestMatchType = TokenType::None;
    uint32_t len = 0;
    if (str.substr(0, 2) == "//") {
        uint32_t cur = 0;
        while (str[cur] != '\n' && str[cur] != '\r') {
            cur++;
        }
        bestMatchLength = cur;
        bestMatchType = TokenType::LineComment;
        std::string token = str.substr(0, bestMatchLength);
        str = str.substr(bestMatchLength, str.size() - bestMatchLength);
        return Token{token, bestMatchType};
    }
    if (str.substr(0, 2) == "/*") {
        uint32_t cur = 2, cnt = 1;
        while (cnt > 0) {
            if (str.substr(cur, 2) == "/*") {
                cnt++;
            } else if (str.substr(cur, 2) == "*/") {
                cnt--;
            }
            cur++;
        }
        bestMatchLength = cur + 2;
        bestMatchType = TokenType::BlockComment;
        std::string token = str.substr(0, bestMatchLength);
        str = str.substr(bestMatchLength, str.size() - bestMatchLength);
        return Token{token, bestMatchType};
    }
    if (str.substr(0, 2) == "*/") {
        throw SyntaxError("Comments do not match");
    } // Judge whether it is a comment.

    for (const auto& it: string_rules) {
        std::smatch match;
        if (std::regex_search(str, match, it.second) && match.position(0) == 0) {
            len = match.length(0);
            if (len > bestMatchLength) {
                bestMatchLength = len;
                bestMatchType = it.first;
            }
        }
    }
    if (bestMatchLength > 0) {
        std::string token = str.substr(0, bestMatchLength);
        str = str.substr(bestMatchLength, str.size() - bestMatchLength);
        return Token{token, bestMatchType};
    } // Judge String

    std::string match_str = str.substr(0, 64);
    if (str[0] >= 'a' && str[0] <= 'z' || str[0] >= 'A' && str[0] <= 'Z') {
        for (const auto& it: letter_rules) {
            std::smatch match;
            if (std::regex_search(match_str, match, it.second) && match.position(0) == 0) {
                len = match.length(0);
                if (len > bestMatchLength) {
                    bestMatchLength = len;
                    bestMatchType = it.first;
                }
            }
        }
    } else {
        for (const auto& it: non_letter_rules) {
            std::smatch match;
            if (std::regex_search(match_str, match, it.second) && match.position(0) == 0) {
                len = match.length(0);
                if (len > bestMatchLength) {
                    bestMatchLength = len;
                    bestMatchType = it.first;
                }
            }
        }
    } // Judge Others
    std::string token = str.substr(0, bestMatchLength);
    str = str.substr(bestMatchLength, str.size() - bestMatchLength);
    return Token{token, bestMatchType};
}