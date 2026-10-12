//
// Created by Oleksander Krainiak on 12.09.2026.
//

#ifndef BASICLANG_BYTECODE_H
#define BASICLANG_BYTECODE_H

#define MAX_STACK_SIZE 1<<15
#include <deque>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>

namespace vm {
    using BigInt = boost::multiprecision::cpp_int;

    enum class ByteCodeType {
        // memory
        LoadConst,
        LoadMem,
        SaveToMem,
        // keywords
        Goto,
        IfNotSoGoTo,
        Out,
        In,
        // operators
        Add,
        Minus,
        Multiply,
        Modulo,
        Div,
        Bitor,
        Bitand,
        Cmp,
        Lshift,
        Rshift,
        Bitnot,
        Bitxor,
    };

    struct BytecodeData {
        virtual ~BytecodeData() = default;
    };
    struct StringData: BytecodeData {
        std::string s;

        StringData() = default;

        StringData(const char * str): s(str) {}
        StringData(std::string_view str): s(str) {}
    };
    struct BigIntData: BytecodeData {
        BigInt s;
        BigIntData(BigInt q): s(q) {}
    };
    struct UintData: BytecodeData {
        uint64_t s;
        UintData(uint64_t q): s(q) {}
    };


    struct ByteCodeInstruction {
        ByteCodeType Type;
        std::unique_ptr<BytecodeData> LoadedData;
        //
        // ByteCodeInstruction(ByteCodeType Type, std::unique_ptr<BytecodeData> LoadedData)
        // : Type(Type), LoadedData(std::move(LoadedData)) {}
        //
        // ByteCodeInstruction(ByteCodeInstruction&& other) noexcept
        //     : Type(other.Type), LoadedData(std::move(other.LoadedData)) {}
        //
        // ByteCodeInstruction(const ByteCodeInstruction&) = delete;
        // ByteCodeInstruction& operator=(const ByteCodeInstruction&) = delete;
    };


    class Vm {

        std::unordered_map<uint64_t, uint64_t> m_Labels{};


    public:
        std::vector<ByteCodeInstruction> m_Code{};

        void AddInstruction(ByteCodeInstruction);
        void AddInstruction(ByteCodeType);
        void AddLabel(uint64_t name);
        void Run() const;

        static void RunOutput(std::vector<BigInt>& stack, std::string_view format);

        static void RunInput(std::vector<BigInt>& stack, std::string_view format);
    };

}

#endif //BASICLANG_BYTECODE_H
