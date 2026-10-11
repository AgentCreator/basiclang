//
// Created by Oleksander Krainiak on 12.09.2026.
//

#include "parser.h"

namespace Parser {
    static Log::Logger logger{"Parser"};

    Lexer::Token Parser::Current() const {
        if (!m_Current) logger.Log<Log::Level::Error>("Empty Source");
        return *m_Current;
    }

    Lexer::Token Parser::Peek() const {
        if (!m_Peek) logger.Log<Log::Level::Error>("Empty Source");
        return *m_Peek;
    }

    Lexer::Token Parser::Advance() {
        m_Current = m_Peek;
        m_Peek = m_Lexer.Next();
        return Current();
    }

    bool Parser::MatchCurrentToken(Lexer::TokenType t) {
        if (auto cur = Current(); cur.Type == t) {
            Advance();
            return true;
        }
        return false;
    }

    bool Parser::MatchPeekToken(Lexer::TokenType t) {
        if (auto peek = Current(); peek.Type == t) {
            Advance();
            return true;
        }

        return false;
    }
}

// std::vector<vm::ByteCodeInstruction> Parser::Parser::prefixParse(Lexer::TokenType t) {
//     switch (t) {
//         case Lexer::TokenType::id:
//             return {vm::ByteCodeInstruction{vm::ByteCodeType::LoadMem, }};
//     }
// }
