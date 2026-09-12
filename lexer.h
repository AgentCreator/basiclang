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


    ///
    /// searches for labels in a source string.
    /// labels in our language are quite simple.
    /// they follow these rules:
    /// 1. they shall be the first thing on a line
    /// 2. they shall be followed by a colon
    /// example:
    /// ```
    /// a = 0
    /// loop: a += 1
    /// out a
    /// if(a < 5) goto loop
    /// ```
    /// output:
    /// 1
    /// 2
    /// 3
    /// 4
    /// they are searched for separately because its annoying to jump to places in a single pass interpreter.
    std::vector<Label> Search(std::string_view source);
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
        slash,
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
        int64_t m_Parenthesis = 0; // for balancing parenthesis'. Used for multiline stuff.
        int64_t m_Interpolations = 0; // for making sure you can start a string there.
        std::vector<Labels::Label> m_Labels{};

        [[nodiscard]] Token makeToken(TokenType t, std::string_view s) const;

    public:
        explicit Lexer(std::string_view source) : m_Source(source), m_Labels(Labels::Search(source)) {
        }

        [[nodiscard]] constexpr std::optional<Token> Peek(int *lineBatch = nullptr) const;

        std::optional<Token> Next();
    };
}


#endif //BASICLANG_LEXER_H
