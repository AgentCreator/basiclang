//
// Created by Oleksander Krainiak on 12.09.2026.
//

#include "parser.h"

namespace Parser {
    static Log::Logger logger{"Parser"};

    Lexer::Token Parser::Current() const {
        // if (!m_Current) logger.Log<Log::Level::Error>("Empty Source");
        return m_Current;
    }

    Lexer::Token Parser::Peek() const {
        // if (!m_Peek) logger.Log<Log::Level::Error>("Empty Source");
        return m_Peek;
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

    void Parser::AssertPeekToken(Lexer::TokenType t) const {
        if (auto cur = Peek(); cur.Type == t) {
            return;
        }
        logger.Log<Log::Level::Error>("Expected a token of {} type, a {} found",
                                      static_cast<int>(t),
                                      static_cast<int>(Peek().Type)
        );
    }

    bool Parser::MatchPeekToken(Lexer::TokenType t) {
        if (auto peek = Current(); peek.Type == t) {
            Advance();
            return true;
        }

        return false;
    }
}

ast::Precedence precedenceOf(Lexer::Token t) {
    switch (t.Type) {
        case Lexer::TokenType::plus:
        case Lexer::TokenType::minus:
            return ast::Precedence::add;
        case Lexer::TokenType::asterisk:
        case Lexer::TokenType::percent:
        case Lexer::TokenType::slash:
        case Lexer::TokenType::lShift:
        case Lexer::TokenType::rShift:
        case Lexer::TokenType::caret:
        case Lexer::TokenType::pipe:
        case Lexer::TokenType::amp:
            return ast::Precedence::mul;
        case Lexer::TokenType::cmp:
            return ast::Precedence::cmp;
        default: return ast::Precedence::lowest;
    }
}

ast::Precedence Parser::Parser::peekPrecedence() const {
    return precedenceOf(Peek());
}

void Parser::Parser::runInstructionFromTokenType(Lexer::TokenType t) {
    switch (t) {
        case Lexer::TokenType::plus:
            vm.AddInstruction(vm::ByteCodeType::Add);
            break;
        case Lexer::TokenType::minus:
            vm.AddInstruction(vm::ByteCodeType::Minus);
            break;
        case Lexer::TokenType::asterisk:
            vm.AddInstruction(vm::ByteCodeType::Multiply);
            break;
        case Lexer::TokenType::slash:
            vm.AddInstruction(vm::ByteCodeType::Div);
            break;
        case Lexer::TokenType::percent:
            vm.AddInstruction(vm::ByteCodeType::Modulo);
            break;
        case Lexer::TokenType::caret:
            vm.AddInstruction(vm::ByteCodeType::Bitxor);
            break;
        case Lexer::TokenType::pipe:
            vm.AddInstruction(vm::ByteCodeType::Bitor);
            break;
        case Lexer::TokenType::amp:
            vm.AddInstruction(vm::ByteCodeType::Bitand);
            break;
        case Lexer::TokenType::lShift:
            vm.AddInstruction(vm::ByteCodeType::Lshift);
            break;
        case Lexer::TokenType::rShift:
            vm.AddInstruction(vm::ByteCodeType::Rshift);
            break;
        case Lexer::TokenType::cmp:
            vm.AddInstruction(vm::ByteCodeType::Cmp);
            break;
        default:


    }
}

void Parser::Parser::parseExpression(ast::Precedence precedence) {
    prefixParse();
    while (precedence < peekPrecedence()) {
        Advance();
        auto op = Current();
        auto prec = precedenceOf(op);
        Advance();
        parseExpression(prec);
        runInstructionFromTokenType(op.Type);
    }
}

void Parser::Parser::prefixParse() {
    switch (Current().Type) {
        case Lexer::TokenType::id:
            vm.AddInstruction({
                .Type = vm::ByteCodeType::LoadMem,
                .LoadedData = std::make_unique<vm::UintData>(AddVar(Current().Literal)),
            });

            break;
        case Lexer::TokenType::number: {
            vm::BigInt num(Current().Literal);
            vm.AddInstruction({
                .Type = vm::ByteCodeType::LoadConst,
                .LoadedData = std::make_unique<vm::BigIntData>(num),
            });
        }
        break;
        case Lexer::TokenType::bitNot: {
            Advance();
            parseExpression(ast::Precedence::prefix);
            vm.AddInstruction(vm::ByteCodeType::Bitnot);
        }
        break;
        case Lexer::TokenType::minus: {
            Advance();
            parseExpression(ast::Precedence::prefix);
            vm.AddInstruction({
                .Type = vm::ByteCodeType::LoadConst,
                .LoadedData = std::make_unique<vm::BigIntData>(-1),
            });
            vm.AddInstruction(vm::ByteCodeType::Multiply);
        }
        break;
        case Lexer::TokenType::lPar: {
            Advance();
            parseExpression();
            AssertPeekToken(Lexer::TokenType::rPar);
            Advance();
        }
        break;
        default:
            logger.Log<Log::Level::Error>("Unknown Expression");
    }
}


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
    if (tok.Type != Lexer::TokenType::id) {
        logger.Log<Log::Level::Error>(
            "Goto to a weird label: `{}`. must be either an id or a number", tok.Literal);
    }
    auto labelID = AddLabel(tok.Literal);
    vm.AddInstruction({
        .Type = vm::ByteCodeType::Goto,
        .LoadedData = std::make_unique<vm::UintData>(labelID),
    });
}

void Parser::Parser::parseAssign() {
    AssertCurrentToken(Lexer::TokenType::id);
    auto name = Current();
    auto nameID = AddVar(name.Literal);
    Advance();
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

void Parser::Parser::parseIf() {
    static int counter = 0;
    // these characters specifically because they can't be used in a label, so there are no overlaps
    constexpr char def_name[] = "(^-^)";
    std::string name = def_name;
    name.append(std::to_string(counter++));
    auto label = AddLabel(name);
    AssertCurrentToken(Lexer::TokenType::If);
    Advance();
    AssertCurrentToken(Lexer::TokenType::lPar);
    Advance();
    parseExpression();
    AssertPeekToken(Lexer::TokenType::rPar);
    Advance();
    Advance();


    /*
     * constructing this thingger:
     * if(!<cond>) goto <1>
     * <expr>
     * <1>:
     *
     */

    vm.AddInstruction({
        .Type = vm::ByteCodeType::IfNotSoGoTo,
        .LoadedData = std::make_unique<vm::UintData>(label),
    });
    parseStatement();
    vm.AddLabel(label);
}

void Parser::Parser::parseOut() {
    AssertCurrentToken(Lexer::TokenType::Out);
    Advance();
    std::string format_string{};
    while (Current().Type != Lexer::TokenType::str) {
        format_string += Current().Literal;
        Advance();
        parseExpression();
        Advance();
        if (MatchCurrentToken(Lexer::TokenType::format)) {
            format_string += '%';
            AssertCurrentToken(Lexer::TokenType::id);
            format_string += Current().Literal;
        } else {
            format_string += "%d";
        }
    }
    format_string += Current().Literal;
    vm.AddInstruction({
        .Type = vm::ByteCodeType::Out,
        .LoadedData = std::make_unique<vm::StringData>(format_string)
    });

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
            } else {
                logger.Log<Log::Level::Warning>("parsing something as an expression statement.\n"
                    "this shouldn't happen.");
                parseExpression();
            }
            break;
        case label:
            parseLabel();
            break;

        case If:
            parseIf();
            break;
        case Out:
            parseOut();
            break;

        default:
            logger.Log<Log::Level::Warning>("parsing something as an expression statement.\n"
                "this shouldn't happen.");
            parseExpression();
    }
}

void Parser::Parser::parseProgram() {
    while (Current().Type != Lexer::TokenType::eof) {
        parseStatement();
        if (Peek().Type == Lexer::TokenType::eof) break;
        Advance();
        AssertCurrentToken(Lexer::TokenType::newline);
        Advance();
    }
}
