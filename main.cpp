

#include "logger.h"
#include "lexer.h"
#include "parser.h"

bool trigger = false;

void* operator new(std::size_t size) {

    std::cout << "[ALLOCATION] Heap allocation of " << size << " bytes triggered!\n";

    return std::malloc(size);
}

void operator delete(void* memory) noexcept {
    std::free(memory);
}

int main() {
    // Log::GlobalLogLevel = Log::Level::Info;
    // constexpr Log::Logger logger{"General"};
    // constexpr std::string_view source = "loop: a = 0\n"
    //                                     "a += 1\n"
    //                                     "if((a <=> 10) + 1) goto loop\n"
    //                                     "out \"over\"";
    //
    // Lexer::Lexer l = source;
    // while (auto t = l.Next()) {
    //     std::println("{}", t->Description());
    // }
    vm::Vm program{};
    // program.AddInstruction({
    //     .Type = vm::ByteCodeType::In,
    //     .LoadedData = std::make_unique<vm::StringData>("%d")
    // });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadConst,
        .LoadedData = std::make_unique<vm::BigIntData>(1'000'000)
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::SaveToMem,
        .LoadedData = std::make_unique<vm::UintData>(0), // amount
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadConst,
        .LoadedData = std::make_unique<vm::BigIntData>(0),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::SaveToMem,
        .LoadedData = std::make_unique<vm::UintData>(1), // result
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadConst,
        .LoadedData = std::make_unique<vm::BigIntData>(0),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::SaveToMem,
        .LoadedData = std::make_unique<vm::UintData>(2), // counter
    });
    program.AddLabel(1);

    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(0),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(2),
    });
    program.AddInstruction(vm::ByteCodeType::Cmp);
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadConst,
        .LoadedData = std::make_unique<vm::BigIntData>(1),
    });
    program.AddInstruction(vm::ByteCodeType::Add);
    program.AddInstruction({
        .Type = vm::ByteCodeType::IfSoGoTo,
        .LoadedData = std::make_unique<vm::SoElseData>(3, 2),
    });
    program.AddLabel(2);
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(1),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(2),
    });
    program.AddInstruction(vm::ByteCodeType::Add);
    program.AddInstruction({
        .Type = vm::ByteCodeType::SaveToMem,
        .LoadedData = std::make_unique<vm::UintData>(1),
    });

    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(2),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadConst,
        .LoadedData = std::make_unique<vm::BigIntData>(1),
    });
    program.AddInstruction(vm::ByteCodeType::Add);
    program.AddInstruction({
        .Type = vm::ByteCodeType::SaveToMem,
        .LoadedData = std::make_unique<vm::UintData>(2),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::Goto,
        .LoadedData = std::make_unique<vm::UintData>(1),
    });
    program.AddLabel(3);
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(1),
    });
    program.AddInstruction({
        .Type = vm::ByteCodeType::LoadMem,
        .LoadedData = std::make_unique<vm::UintData>(0),
    });
    program.AddInstruction({
       .Type = vm::ByteCodeType::Out,
       .LoadedData = std::make_unique<vm::StringData>("sum of %d first numbers is %d."),
   });


    trigger = true;
    std::println("starting to run");
    program.Run();
}
