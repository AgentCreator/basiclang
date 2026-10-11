

#include "logger.h"
#include "lexer.h"
#include "parser.h"


int main() {
    Log::GlobalLogLevel = Log::Level::Info;
    constexpr Log::Logger logger{"General"};
    // constexpr std::string_view source = "loop: a = 0\n"
    //         "a += 1\n"
    //         "if((a <=> 10) + 1) goto loop\n"
    //         "out \"over: {a!d}\"";
    constexpr std::string_view source = "q: goto q\n";

    Lexer::Lexer l = source;
    Parser::Parser p{l};

    p.parseStatement();

}
