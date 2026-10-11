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

        Lexer::Token Advance();

        bool MatchCurrentToken(Lexer::TokenType);

        void AssertCurrentToken(Lexer::TokenType t) const;

        void AssertPeekToken(Lexer::TokenType t) const;

        bool MatchPeekToken(Lexer::TokenType);

    public:

        vm::Vm vm{};

        void prefixParse();


        ast::Precedence peekPrecedence() const;

        void runInstructionFromTokenType(Lexer::TokenType t);

        void parseExpression(ast::Precedence precedence = ast::Precedence::lowest);

        uint64_t AddLabel(std::string_view name);
        uint64_t AddVar(std::string_view name);

        void parseGoto();

        void parseAssign();

        void parseLabel();

        void parseIf();

        void parseStatement();

        void parseProgram();

        explicit Parser(Lexer::Lexer
            &lexer) : m_Lexer(lexer) {
            m_Current = m_Lexer.Next();
            m_Peek = m_Lexer.Next();
        }
    };
}

#endif //BASICLANG_PARSER_H
