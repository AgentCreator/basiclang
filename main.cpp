#include "logger.h"
#include "lexer.h"
#include "parser.h"


int main() {
    Log::GlobalLogLevel = Log::Level::Info;
    constexpr Log::Logger logger{"General"};
    constexpr std::string_view source = "a = 0\n"
            "loop: a = a + 1\n"
            "if((a <=> 10) + 1) goto loop\n"
            "a";
    // constexpr std::string_view source = "";

    Lexer::Lexer l = source;
    Parser::Parser p{l};

    p.parseProgram();
    p.vm.AddInstruction({
        .Type = vm::ByteCodeType::Out,
        .LoadedData = std::make_unique<vm::StringData>("res: %d.\n")
    });
    p.vm.Run();
}
