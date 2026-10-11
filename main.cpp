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
    constexpr std::string_view source = "-10";

    Lexer::Lexer l = source;
    Parser::Parser p{l};

    p.parseExpression();
    p.vm.AddInstruction({
        .Type = vm::ByteCodeType::Out,
        .LoadedData = std::make_unique<vm::StringData>("res: %d.\n")
    });
    p.vm.Run();
}
