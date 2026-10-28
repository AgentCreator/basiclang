#include <fstream>
#include <sstream>

#include "logger.h"
#include "lexer.h"
#include "parser.h"


int main(int argc, char** argv) {
    Log::GlobalLogLevel = Log::Level::Error;



    const char* name = argv[1];
    if (argc < 2) {
        // name = "/Users/sashko/mystuff/progfrog/cp/basiclang/maxNumDemo.basiclang";
        std::println("{} <file_name>", argv[0]);
        return -1;
    }
    std::ifstream file{name};


    std::ostringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();
    // std::println("{}", source);

    Lexer::Lexer l{source};

    // auto clone = l;
    // Lexer::Token t = clone.Next();
    // while (t.Type != Lexer::TokenType::eof) {
    //     std::println("{}", t.Description());
    //     t = clone.Next();
    // }

    Parser::Parser p{l};

    p.parseProgram();
    p.vm.Run();
}
