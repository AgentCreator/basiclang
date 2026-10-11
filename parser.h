//
// Created by Oleksander Krainiak on 12.09.2026.
//

#ifndef BASICLANG_PARSER_H
#define BASICLANG_PARSER_H

#include <set>
#include <unordered_set>

#include "ast.h"
#include "bytecode.h"
#include "lexer.h"
#include "logger.h"

namespace Parser {
    class Parser {
        Lexer::Lexer &m_Lexer;
        Lexer::Token m_Current;
        Lexer::Token m_Peek;

        std::unordered_map<std::string_view, uint64_t> labels;
        std::unordered_map<std::string_view, uint64_t> vars;

        [[nodiscard]] Lexer::Token Current() const;

        [[nodiscard]] Lexer::Token Peek() const;

        /// advances the lexer and yields the current token.
        Lexer::Token Advance();

        /// advances if the current token is of the given type, returns false otherwise
        bool MatchCurrentToken(Lexer::TokenType);

        void AssertCurrentToken(Lexer::TokenType t) const;


        /// advances if the peek token is of the given type, returns false otherwise
        bool MatchPeekToken(Lexer::TokenType);

    public:

        vm::Vm vm{};

        void prefixParse();

        void parseInfix();

        ast::Precedence peekPrecedence() const;

        void runInstructionFromTokenType(Lexer::TokenType t);

        void parseExpression(ast::Precedence precedence = ast::Precedence::lowest);

        uint64_t AddLabel(std::string_view name);
        uint64_t AddVar(std::string_view name);

        void parseGoto();

        void parseAssign();

        void parseLabel();

        void parseStatement();

        explicit Parser(Lexer::Lexer
            &lexer) : m_Lexer(lexer) {
            m_Current = m_Lexer.Next();
            m_Peek = m_Lexer.Next();
        }
    };
}

#endif //BASICLANG_PARSER_H
