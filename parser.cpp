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

    void Parser::AssertCurrentToken(Lexer::TokenType t) const {
        if (auto cur = Current(); cur.Type == t) {
            return;
        }
        logger.Log<Log::Level::Error>("Expected a token of {} type, a {} found",
                                      static_cast<int>(t),
                                      static_cast<int>(Current().Type)
        );
    }

    bool Parser::MatchPeekToken(Lexer::TokenType t) {
        if (auto peek = Current(); peek.Type == t) {
            Advance();
            return true;
        }

        return false;
    }

    void Parser::prefixParse(Lexer::TokenType) {
    }

    void Parser::parseExpression() {
    }
}

// void Parser::Parser::prefixParse(Lexer::TokenType t) {
//     switch (t) {
//
//     }
// }

uint64_t Parser::Parser::AddLabel(std::string_view name) {
    if (labels.contains(name)) return labels.at(name);
    return labels[name] = labels.size();
}

uint64_t Parser::Parser::AddVar(std::string_view name) {
    if (vars.contains(name)) return vars.at(name);
    return vars[name] = vars.size();
}

void Parser::Parser::parseGoto() {
    // if (!MatchCurrentToken(Lexer::TokenType::Goto)) {
    //     logger.Log<Log::Level::Error>("Goto ran in wrong place");
    // }
    AssertCurrentToken(Lexer::TokenType::Goto);
    Advance();
    auto tok = Current();
    Advance();
    if (tok.Type != Lexer::TokenType::id && tok.Type != Lexer::TokenType::number) {
        logger.Log<Log::Level::Error>(
            "Goto to a weird label: `{}`. must be either an id or a number", tok.Literal);
    }
    auto labelID = AddLabel(tok.Literal);
    vm.AddInstruction({
        .Type = vm::ByteCodeType::Goto,
        .LoadedData = std::make_unique<vm::UintData>(labelID),
    });
    AssertCurrentToken(Lexer::TokenType::newline);
}

void Parser::Parser::parseAssign() {
    AssertCurrentToken(Lexer::TokenType::id);
    auto name = Current();
    auto nameID = AddVar(name.Literal);
    AssertCurrentToken(Lexer::TokenType::equals);
    Advance();
    parseExpression();
    vm.AddInstruction({
        .Type = vm::ByteCodeType::SaveToMem,
        .LoadedData = std::make_unique<vm::UintData>(nameID),
    });
}

void Parser::Parser::parseLabel() {
    AssertCurrentToken(Lexer::TokenType::label);
    auto name = Current().Literal;
    Advance();
    name.remove_suffix(1);
    auto id = AddLabel(name);
    vm.AddLabel(id);

    parseStatement();
}


void Parser::Parser::parseStatement() {
    using enum Lexer::TokenType;
    switch (Current().Type) {
        case Goto:
            parseGoto();
            break;
        case id:
            if (Peek().Type == equals) {
                parseAssign();
            }
            break;
        case label:
            parseLabel();
            break;
        default:



    }
}
