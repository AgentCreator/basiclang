//
// Created by Oleksander Krainiak on 31.08.2026.
//

#ifndef BASICLANG_LEXER_H
#define BASICLANG_LEXER_H

#include <format>
#include <vector>

namespace Lexer::Labels {
    struct Label {
        const char *position;
        const char *continuation;

        [[nodiscard]] std::string_view Name() const {
            return {position, continuation};
        }
    };



}

template<>
struct std::formatter<Lexer::Labels::Label> {
    static constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    static auto format(const Lexer::Labels::Label &l, std::format_context &ctx) {
        return std::format_to(ctx.out(), "Label({})", l.Name());
    }
};

namespace Lexer {
    enum class TokenType: unsigned char {
        // operators
        plus,
        minus,
        asterisk,
        equals,
        lPar,
        rPar,
        percent,
        pipe,
        amp,
        cmp, // <=> operator, on `a <=> b` is equivalent to sgn(a-b).
        lShift,
        rShift,
        bitNot,
        caret,

        // keywords
        In,
        Out,
        If,
        Goto,

        // strings
        str, // just a string with no interpolations
        interpolation, // the string just before the interpolation


        // misc
        id,
        number,
        colon,
        format, // the :: operator, used in out strings and in strings.
        newline,
        label,
    };

    struct Token {
        TokenType Type;
        std::string_view Literal;
        size_t Line;

        std::string Description() {
            return std::format("Token<{}>({})", static_cast<int>(Type), Literal);
        }
    };

    class Lexer {
        std::string_view m_Source;
        size_t m_Line = 1;
        int m_Parenthesis = 0; // for balancing parenthesis'. Used for multiline stuff.
        int m_Interpolations = 0; // for making sure you can start a string there.

        [[nodiscard]] Token makeToken(TokenType t, std::string_view s) const;

    public:
        // ReSharper disable once CppNonExplicitConvertingConstructor
        Lexer(std::string_view source) : m_Source(source) {
        }

        [[nodiscard]] std::optional<Token> Peek(int *lineBatch = nullptr) const;

        std::optional<Token> Next();
    };
}


#endif //BASICLANG_LEXER_H
