//
// Created by Oleksander Krainiak on 31.08.2026.
//

#include <string_view>
// #include <vector>
#include <ranges>

#include "logger.h"
#include "lexer.h"

#include <cassert>

namespace Lexer {
    constexpr Log::Logger logger{"Lexer"};
}


// namespace Lexer::Labels {
//     std::vector<Label> Search(std::string_view source) {
//         std::vector<Label> result{};
//         for (auto line: source
//                         | std::views::split('\n')
//                         | std::views::transform([](const auto a) {
//                             return std::string_view(a);
//                         })) {
//             while (!line.empty() and std::isspace(line[0])) {
//                 line.remove_prefix(1);
//             }
//             if (line.empty()) continue;
//
//             if (line[0] == '"') continue;
//             const char *position = line.data();
//             while (!line.empty() && !std::isspace(line[0]) && line[0] != ':' && line[0] != '"' && line[0] != '(') {
//                 line.remove_prefix(1);
//             }
//             if (line.empty()) continue;
//             if (line[0] != ':') continue;
//             const char *continuation = line.substr(1).data();
//             Label label{position, continuation};
//             logger.Log<Log::Level::Debug>("Found Label {}", label);
//             result.emplace_back(label);
//         }
//
//         return result;
//     }
// }

std::string_view stripWhitespace(std::string_view s) {
    std::string_view result = s;
    while (!result.empty() && std::isspace(result[0]) && result[0] != '\n') {
        result.remove_prefix(1);
    }
    return result;
}


Lexer::Token Lexer::Lexer::makeToken(TokenType t, std::string_view s) const {
    return Token{.Type = t, .Literal = s, .Line = m_Line};
}


/// @return the length of the number
static constexpr int makeNumber(std::string_view source) {

    int res = 0;
    while (!source.empty() && isdigit(source[0])) {
        res++;
        source.remove_prefix(1);
    }
    return res;
}

/// @return the length of the identifier
static constexpr int makeId(std::string_view source) {
    int res = 0;
    static constexpr std::string_view view{"+-*/&^|()\";:'<>=~%{}!"};
    while (!source.empty() && !view.contains(source[0]) && !std::isspace(source[0])) {
        res++;
        source.remove_prefix(1);
    }
    return res;
}

static constexpr Lexer::Token makeString(std::string_view source) {
    std::string_view result = source;
    assert(result[0] == '"' || result[0] == '}');
    result.remove_prefix(1);
    bool escaped = false;
    while (!result.empty()) {
        if (result[0] == '"' && !escaped) {
            return Lexer::Token{
                .Type = Lexer::TokenType::str,
                .Literal = {source.begin(), result.begin() + 1},
                .Line = 0
            };
        }
        if (result[0] == '{' && !escaped) {
            return Lexer::Token{
                .Type = Lexer::TokenType::interpolation,
                .Literal = {source.begin(), result.begin() + 1},
                .Line = 0
            };
        }
        if (result[0] == '\\' && !escaped) {
            escaped = true;
        }
        result.remove_prefix(1);
    }
    Lexer::logger.Log<Log::Level::Error>("not closed string");
    return {};
}


Lexer::Token Lexer::Lexer::Peek(int *lineBatch) const {
    auto source = m_Source;

tokenize:
    source = stripWhitespace(source);
    if (source.empty()) return Token{.Type = TokenType::eof, .Literal = "", .Line = m_Line};


    switch (const char c = source[0]; c) {
        case '+': return makeToken(TokenType::plus, source.substr(0, 1));
        case '-': return makeToken(TokenType::minus, source.substr(0, 1));
        case '*': return makeToken(TokenType::asterisk, source.substr(0, 1));
        // case '/': return makeToken(TokenType::slash, source.substr(0, 1));
        case '=': return makeToken(TokenType::equals, source.substr(0, 1));
        case '(': return makeToken(TokenType::lPar, source.substr(0, 1));
        case ')': return makeToken(TokenType::rPar, source.substr(0, 1));
        case '%': return makeToken(TokenType::percent, source.substr(0, 1));
        case '/': return makeToken(TokenType::slash, source.substr(0, 1));
        case '|': return makeToken(TokenType::pipe, source.substr(0, 1));
        case '&': return makeToken(TokenType::amp, source.substr(0, 1));
        case ':': return makeToken(TokenType::colon, source.substr(0, 1));
        case '!': return makeToken(TokenType::format, source.substr(0, 1));
        case '^': return makeToken(TokenType::caret, source.substr(0, 1));
        case '\n':
            if (lineBatch) (*lineBatch)++;
            if (m_Parenthesis == 0 && m_Interpolations == 0) return makeToken(TokenType::newline, source.substr(0, 1));
            source.remove_prefix(1);
            source = stripWhitespace(source);

            goto tokenize;
        case '<':
            if (source.starts_with("<<")) return makeToken(TokenType::lShift, source.substr(0, 2));
            if (source.starts_with("<=>")) return makeToken(TokenType::cmp, source.substr(0, 3));

            logger.Log<Log::Level::Error>("Unknown Token at `{}...`", source.substr(0, 10));
            break;
        case '>':
            if (source.starts_with(">>")) return makeToken(TokenType::rShift, source.substr(0, 2));

            logger.Log<Log::Level::Error>("Unknown Token at line {}, `{}...`", m_Line, source.substr(0, 10));
            break;
        case '~': return makeToken(TokenType::bitNot, source.substr(0, 1));
        default: {
            if (std::isdigit(c)) {
                return makeToken(TokenType::number, source.substr(0, makeNumber(source)));
            }
            if (c == '"' || (c == '}' && m_Interpolations != 0)) {
                auto res = makeString(source);
                res.Line = m_Line;
                return res;
            }
            Token token = makeToken(TokenType::id, source.substr(0, makeId(source)));
            if (const auto end = token.Literal.end(); end != nullptr && *end == ':') {
                return makeToken(TokenType::label, source.substr(0, token.Literal.size()+1));
                // source = source.substr(token.Literal.size() + 1);
                // goto tokenize;
            }
            static constexpr std::array keywords{
                std::make_tuple(TokenType::Goto, "goto"),
                std::make_tuple(TokenType::If, "if"),
                std::make_tuple(TokenType::In, "in"),
                std::make_tuple(TokenType::Out, "out"),
            };
            for (const auto &[type, literal]: keywords) {
                if (token.Literal == literal) {
                    token.Type = type;
                    break;
                }
            }
            return token;
        }
    }

    return Token{.Type = TokenType::eof, .Literal = "", .Line = m_Line};
}

Lexer::Token Lexer::Lexer::Next() {
    int lineBatch = 0;
    std::optional<Token> token = Peek(&lineBatch);
    m_Line += lineBatch;
    if (!token) return Token{.Type = TokenType::eof, .Literal = "", .Line = m_Line};
    Token rawToken = *token;
    if (rawToken.Type == TokenType::lPar) m_Parenthesis++;
    if (rawToken.Type == TokenType::rPar) m_Parenthesis--;
    if (rawToken.Type == TokenType::interpolation) m_Interpolations++;
    if (rawToken.Type == TokenType::str && rawToken.Literal[0] == '}') m_Interpolations--;

    m_Source.remove_prefix(rawToken.Literal.end() - m_Source.begin());
    return rawToken;
}
