#include "Error.h"
#include "Lexer/Lexer.h"

Lexer::Lexer() {
    letter_rules.emplace_back(TokenType::As, boost::regex("as"));
    letter_rules.emplace_back(TokenType::Break, boost::regex("break"));
    letter_rules.emplace_back(TokenType::Const, boost::regex("const"));
    letter_rules.emplace_back(TokenType::Continue, boost::regex("continue"));
    letter_rules.emplace_back(TokenType::Crate, boost::regex("crate"));
    letter_rules.emplace_back(TokenType::Else, boost::regex("else"));
    letter_rules.emplace_back(TokenType::Enum, boost::regex("enum"));
    letter_rules.emplace_back(TokenType::False, boost::regex("false"));
    letter_rules.emplace_back(TokenType::Fn, boost::regex("fn"));
    letter_rules.emplace_back(TokenType::For, boost::regex("for"));
    letter_rules.emplace_back(TokenType::If, boost::regex("if"));
    letter_rules.emplace_back(TokenType::Impl, boost::regex("impl"));
    letter_rules.emplace_back(TokenType::In, boost::regex("in"));
    letter_rules.emplace_back(TokenType::Let, boost::regex("let"));
    letter_rules.emplace_back(TokenType::Loop, boost::regex("loop"));
    letter_rules.emplace_back(TokenType::Match, boost::regex("match"));
    letter_rules.emplace_back(TokenType::Mod, boost::regex("mod"));
    letter_rules.emplace_back(TokenType::Move, boost::regex("move"));
    letter_rules.emplace_back(TokenType::Mut, boost::regex("mut"));
    letter_rules.emplace_back(TokenType::Ref, boost::regex("ref"));
    letter_rules.emplace_back(TokenType::Return, boost::regex("return"));
    letter_rules.emplace_back(TokenType::Self, boost::regex("self"));
    letter_rules.emplace_back(TokenType::SELF, boost::regex("Self"));
    letter_rules.emplace_back(TokenType::Struct, boost::regex("struct"));
    letter_rules.emplace_back(TokenType::Super, boost::regex("super"));
    letter_rules.emplace_back(TokenType::Trait, boost::regex("trait"));
    letter_rules.emplace_back(TokenType::True, boost::regex("true"));
    letter_rules.emplace_back(TokenType::Type, boost::regex("type"));
    letter_rules.emplace_back(TokenType::Unsafe, boost::regex("unsafe"));
    letter_rules.emplace_back(TokenType::Use, boost::regex("use"));
    letter_rules.emplace_back(TokenType::Where, boost::regex("where"));
    letter_rules.emplace_back(TokenType::While, boost::regex("while"));
    letter_rules.emplace_back(TokenType::Dyn, boost::regex("dyn"));
    letter_rules.emplace_back(TokenType::Abstract, boost::regex("abstract"));
    letter_rules.emplace_back(TokenType::Become, boost::regex("become"));
    letter_rules.emplace_back(TokenType::Do, boost::regex("do"));
    letter_rules.emplace_back(TokenType::Final, boost::regex("final"));
    letter_rules.emplace_back(TokenType::Override, boost::regex("override"));
    // Match Keywords

    letter_rules.emplace_back(TokenType::Identifier, boost::regex("[a-zA-Z][_a-zA-Z0-9]*"));
    // Match Identifier

    // non_letter_rules.emplace_back(TokenType::DotDotDot, boost::regex(R"(\.\.\.)"));
    // non_letter_rules.emplace_back(TokenType::DotDotEq, boost::regex(R"(\.\.=)"));
    non_letter_rules.emplace_back(TokenType::SLEq, boost::regex(R"(<<=)"));
    non_letter_rules.emplace_back(TokenType::SREq, boost::regex(R"(>>=)"));
    non_letter_rules.emplace_back(TokenType::LEq, boost::regex(R"(<=)"));
    non_letter_rules.emplace_back(TokenType::EqEq, boost::regex(R"(==)"));
    non_letter_rules.emplace_back(TokenType::NEq, boost::regex(R"(!=)"));
    non_letter_rules.emplace_back(TokenType::GEq, boost::regex(R"(>=)"));
    non_letter_rules.emplace_back(TokenType::AndAnd, boost::regex(R"(&&)"));
    non_letter_rules.emplace_back(TokenType::OrOr, boost::regex(R"(\|\|)"));
    non_letter_rules.emplace_back(TokenType::SL, boost::regex(R"(<<)"));
    non_letter_rules.emplace_back(TokenType::SR, boost::regex(R"(>>)"));
    non_letter_rules.emplace_back(TokenType::PlusEq, boost::regex(R"(\+=)"));
    non_letter_rules.emplace_back(TokenType::MinusEq, boost::regex(R"(-=)"));
    non_letter_rules.emplace_back(TokenType::MulEq, boost::regex(R"(\*=)"));
    non_letter_rules.emplace_back(TokenType::DivEq, boost::regex(R"(/=)"));
    non_letter_rules.emplace_back(TokenType::ModEq, boost::regex(R"(%=)"));
    non_letter_rules.emplace_back(TokenType::XorEq, boost::regex(R"(\^=)"));
    non_letter_rules.emplace_back(TokenType::AndEq, boost::regex(R"(&=)"));
    non_letter_rules.emplace_back(TokenType::OrEq, boost::regex(R"(\|=)"));
    non_letter_rules.emplace_back(TokenType::DotDot, boost::regex(R"(\.\.)"));
    non_letter_rules.emplace_back(TokenType::ColonColon, boost::regex(R"(::)"));
    non_letter_rules.emplace_back(TokenType::RArrow, boost::regex(R"(->)"));
    non_letter_rules.emplace_back(TokenType::LArrow, boost::regex(R"(<-)"));
    // non_letter_rules.emplace_back(TokenType::FatArrow, boost::regex(R"(=>)"));
    non_letter_rules.emplace_back(TokenType::Eq, boost::regex(R"(=)"));
    non_letter_rules.emplace_back(TokenType::Lt, boost::regex(R"(<)"));
    non_letter_rules.emplace_back(TokenType::Gt, boost::regex(R"(>)"));
    non_letter_rules.emplace_back(TokenType::Not, boost::regex(R"(!)"));
    non_letter_rules.emplace_back(TokenType::Tilde, boost::regex(R"(~)"));
    non_letter_rules.emplace_back(TokenType::Plus, boost::regex(R"(\+)"));
    non_letter_rules.emplace_back(TokenType::Minus, boost::regex(R"(-)"));
    non_letter_rules.emplace_back(TokenType::Mul, boost::regex(R"(\*)"));
    non_letter_rules.emplace_back(TokenType::Div, boost::regex(R"(/)"));
    non_letter_rules.emplace_back(TokenType::MOD, boost::regex(R"(%)"));
    non_letter_rules.emplace_back(TokenType::Xor, boost::regex(R"(\^)"));
    non_letter_rules.emplace_back(TokenType::And, boost::regex(R"(&)"));
    non_letter_rules.emplace_back(TokenType::Or, boost::regex(R"(\|)"));
    non_letter_rules.emplace_back(TokenType::At, boost::regex(R"(@)"));
    non_letter_rules.emplace_back(TokenType::Dot, boost::regex(R"(\.)"));
    non_letter_rules.emplace_back(TokenType::Comma, boost::regex(R"(,)"));
    non_letter_rules.emplace_back(TokenType::Semicolon, boost::regex(R"(;)"));
    non_letter_rules.emplace_back(TokenType::Colon, boost::regex(R"(:)"));
    non_letter_rules.emplace_back(TokenType::Pound, boost::regex(R"(#)"));
    non_letter_rules.emplace_back(TokenType::Dollar, boost::regex(R"(\$)"));
    non_letter_rules.emplace_back(TokenType::Question, boost::regex(R"(\?)"));
    non_letter_rules.emplace_back(TokenType::Underscore, boost::regex(R"(_)"));
    non_letter_rules.emplace_back(TokenType::LBrace, boost::regex(R"(\{)"));
    non_letter_rules.emplace_back(TokenType::RBrace, boost::regex(R"(\})"));
    non_letter_rules.emplace_back(TokenType::LBracket, boost::regex(R"(\[)"));
    non_letter_rules.emplace_back(TokenType::RBracket, boost::regex(R"(\])"));
    non_letter_rules.emplace_back(TokenType::LParen, boost::regex(R"(\()"));
    non_letter_rules.emplace_back(TokenType::RParen, boost::regex(R"(\))"));
    // Match Punctuation

    non_letter_rules.emplace_back(TokenType::IntegerLiteral,
                                  boost::regex(
                                      R"(((0b[_01]*[01][_01]*)|(0o[_0-7]*[0-7][_0-7]*)|(0x[_0-9a-fA-F]*[0-9a-fA-F][_0-9a-fA-F]*)|([0-9][_0-9]*))(u32|i32|usize|isize)?)"));
    // Match Integer Literal

    non_letter_rules.emplace_back(TokenType::ReservedIntegerLiteral,
                                  boost::regex(
                                      R"(((0b[_0-9]*[0-9][_0-9]*)|(0o[_0-9]*[0-9][_0-9]*)|(0x[_0-9a-fA-F]*[0-9a-fA-F][_0-9a-fA-F]*)|([0-9][_0-9]*))(u32|i32|usize|isize)?)"));
    // Match Reserved Integer Literal

    non_letter_rules.emplace_back(TokenType::CharLiteral,
                                  boost::regex(R"('([^'\\\n\r\t]|\\[nrt'"\\0]|\\x[0-7][0-9a-fA-F])')"));
    // Match Char Literal

    string_rules.emplace_back(TokenType::StringLiteral,
                              boost::regex(R"("([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*")"));
    // Match String Literal

    string_rules.emplace_back(TokenType::RawStringLiteral,
                              boost::regex(R"(r(#*)\"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*\"\1)"));
    // Match Raw-String Literal

    string_rules.emplace_back(TokenType::CStringLiteral,
                              boost::regex(R"(c"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*")"));
    // Match C-String Literal

    string_rules.emplace_back(TokenType::CStringLiteral,
                              boost::regex(R"(cr(#*)\"([^"\\\r]|\\[nrt'"\\0]|\\x[0-9a-fA-F]{2}|\\\r)*\"\1)"));
    // Match C-String Literal

    non_letter_rules.emplace_back(TokenType::WhiteSpace, boost::regex("[ \r\t\n]+"));
    // Match WhiteSpace
}


Token Lexer::GetNextToken(std::string &str) const {
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
        throw LexError("Comments do not match");
    } // Judge whether it is a comment.
    for (const auto &it: string_rules) {
        boost::smatch match;
        if (regex_search(str, match, it.second, boost::match_continuous)) {
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

    if (str[0] >= 'a' && str[0] <= 'z' || str[0] >= 'A' && str[0] <= 'Z') {
        for (const auto &it: letter_rules) {
            boost::smatch match;
            if (regex_search(str, match, it.second, boost::match_continuous)) {
                len = match.length(0);
                if (len > bestMatchLength) {
                    bestMatchLength = len;
                    bestMatchType = it.first;
                }
            }
        }
    } else {
        for (const auto &it: non_letter_rules) {
            boost::smatch match;
            if (boost::regex_search(str, match, it.second, boost::match_continuous)) {
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