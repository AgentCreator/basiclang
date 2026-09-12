

#include "logger.h"
#include "lexer.h"

int main() {
    Log::GlobalLogLevel = Log::Level::Info;
    constexpr Log::Logger logger{"General"};
    constexpr char source[] = "out \"Hello {2+2::s}!\"";

    // auto labels = Lexer::Labels::Search(source);
    // for (auto label: labels) {
    //     logger.Log<Log::Level::Info>("{}", label);
    // }
    Lexer::Lexer l{source};
    while (auto t = l.Next()) {
        std::println("{}", t->Description());
    }



}
