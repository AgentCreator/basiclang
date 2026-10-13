#include <fstream>
#include <sstream>

#include "logger.h"
#include "lexer.h"
#include "parser.h"


int main(int argc, char** argv) {
    Log::GlobalLogLevel = Log::Level::Info;

    if (argc < 2) {
        std::println("{} <file_name>", argv[0]);
        return -1;
    }
    std::ifstream file{argv[1]};


    std::ostringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();
    // std::println("{}", source);

    Lexer::Lexer l{source};
    Parser::Parser p{l};

    p.parseProgram();
    p.vm.Run();
}
