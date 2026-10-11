//
// Created by Oleksander Krainiak on 12.09.2026.
//

#ifndef BASICLANG_PARSER_H
#define BASICLANG_PARSER_H

#include "ast.h"
#include "bytecode.h"
#include "lexer.h"
#include "logger.h"

namespace Parser {
    class Parser {
        Lexer::Lexer &m_Lexer;
        std::optional<Lexer::Token> m_Current;
        std::optional<Lexer::Token> m_Peek;

        [[nodiscard]] Lexer::Token Current() const;

        [[nodiscard]] Lexer::Token Peek() const;

        /// advances the lexer and yields the current token.
        Lexer::Token Advance();

        /// advances if the current token is of the given type, returns false otherwise
        bool MatchCurrentToken(Lexer::TokenType);

        /// advances if the peek token is of the given type, returns false otherwise
        bool MatchPeekToken(Lexer::TokenType);

    public:
        std::vector<vm::ByteCodeInstruction> prefixParse(Lexer::TokenType);

        std::vector<vm::ByteCodeInstruction> parseExpression();

        explicit Parser(Lexer::Lexer
            &lexer) : m_Lexer(lexer) {
            m_Current = m_Lexer.Next();
            m_Peek = m_Lexer.Next();
        }
    };
}

#endif //BASICLANG_PARSER_H
