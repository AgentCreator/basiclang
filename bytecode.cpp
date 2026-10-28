//
// Created by Oleksander Krainiak on 12.09.2026.
//

#include "bytecode.h"

#include <ranges>
#include <string>


void vm::Vm::AddInstruction(ByteCodeInstruction i) {
    m_Code.emplace_back(std::move(i));
}

void vm::Vm::AddInstruction(ByteCodeType i) {
    m_Code.push_back(ByteCodeInstruction{
        .Type = i,
        .LoadedData = nullptr,
    });
}

void vm::Vm::AddLabel(uint64_t name) {
    m_Labels[name] = m_Code.size();
}

void vm::Vm::Run() const {
    std::unordered_map<uint64_t, BigInt> mem;
    std::vector<BigInt> stack;
    stack.reserve(128);
    auto GetBack = [&stack] -> BigInt {
        BigInt val = std::move(stack.back());
        stack.pop_back();
        return val;
    };


    for (int idx = 0; idx < m_Code.size(); idx++) {


        const auto &instruction = m_Code[idx];
        switch (instruction.Type) {
            case ByteCodeType::LoadConst: {
                const auto& [val] = static_cast<const BigIntData &>(*instruction.LoadedData);
                stack.emplace_back(val);
            }
            break;
            case ByteCodeType::LoadMem: {
                auto& [val] = static_cast<const UintData &>(*instruction.LoadedData);
                // std::cout << "Loaded " << mem[val] << " from " << val << '\n';
                stack.emplace_back(mem[val]);
            }
            break;
            case ByteCodeType::SaveToMem: {
                auto& [val] = static_cast<const UintData &>(*instruction.LoadedData);
                auto q = GetBack();
                mem.insert_or_assign(val, std::move(q));
            }
            break;
            case ByteCodeType::Goto: {
                auto& [val] = static_cast<const UintData &>(*instruction.LoadedData);
                idx = m_Labels.at(val)-1;
                // std::cout << "Went to " << val << '\n';

            }
            break;
            case ByteCodeType::IfNotSoGoTo: {
                auto& [ so ] = static_cast<const UintData &>(*instruction.LoadedData);
                auto condition = GetBack();

                if (condition) idx = m_Labels.at(so)-1; // if

            }
            break;
            case ByteCodeType::Out: {
                const auto [s] = static_cast<const StringData &>(*instruction.LoadedData);
                RunOutput(stack, s);
            }
            break;
            case ByteCodeType::In: {
                auto [s] = static_cast<const StringData &>(*instruction.LoadedData);
                RunInput(stack, s);
            }
            break;
            case ByteCodeType::Add: {
                auto b = GetBack();
                auto a = GetBack();


                stack.push_back(a + b);
            }
            break;
            case ByteCodeType::Minus: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a - b);
            }
            break;
            case ByteCodeType::Multiply: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a * b);
            }
            break;
            case ByteCodeType::Modulo: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a % b);
            }
            break;
            case ByteCodeType::Bitor: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a | b);
            }
            break;
            case ByteCodeType::Bitand: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a & b);
            }
            break;
            case ByteCodeType::Cmp: {
                auto b = GetBack();
                auto a = GetBack();
                BigInt c = a-b;
                std::string debug_str3 = std::to_string(c.sign());

                stack.push_back(c.sign());
            }
            break;
            case ByteCodeType::Lshift: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a << static_cast<long long>(b));
            }
            break;
            case ByteCodeType::Rshift: {
                auto b = GetBack();
                auto a = GetBack();
               stack.push_back(a >> static_cast<long long>(b));
            }
            break;
            case ByteCodeType::Bitnot: {
                auto a = GetBack();

                stack.push_back(~a);
            }
            break;
            case ByteCodeType::Bitxor: {
                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a ^ b);
            }
            break;
            case ByteCodeType::Div: {

                auto b = GetBack();
                auto a = GetBack();

                stack.push_back(a / b);
            }
            break;
        }
    }
}

void vm::Vm::RunOutput(std::vector<BigInt> &stack, std::string_view format) {
    auto GetBack = [&stack] -> BigInt {
        BigInt val = std::move(stack.back());
        stack.pop_back();
        return val;
    };
    std::string result;
    bool isSpecial = false;
    bool isEscaped = false;
    for (const auto ch : format) {
        if (ch == '%' && isSpecial) {
            result += '%';
            isSpecial = false;
        } else if (ch == '%' && !isSpecial) {
            isSpecial = true;
        } else if (ch == 's' && isSpecial) {
            auto n = GetBack();
            std::vector<unsigned char> bytes;
            boost::multiprecision::export_bits(n, std::back_inserter(bytes), 8, true);

            std::string s{bytes.cbegin(), bytes.cend()};
            result.append(s);
            isSpecial = false;
        } else if (ch == 'd' && isSpecial) {
            auto n = GetBack();
            result.append(n.str());
            isSpecial = false;
        } else if (ch == '\\') {
            isEscaped = true;
        } else if (isEscaped && ch == 'n') {
            result += '\n';
            isEscaped = false;
        } else if (isEscaped && ch == '"') {
            result += '"';
            isEscaped = false;
        } else if (isEscaped && ch == '\\') {
            result += '\\';
            isEscaped = false;
        } else if (isEscaped && ch == 't') {
            result += '\t';
            isEscaped = false;
        } else {
            result += ch;
        }
    }
    std::print("{}", result);
}

void vm::Vm::RunInput(std::vector<BigInt> &stack, std::string_view format) {
    for (auto word : std::views::split(format, ' ')) {
        std::string_view s{word.begin(), word.end()};
        if (s == "%s") {
            std::string q;
            std::cin >> q;
            BigInt w;
            boost::multiprecision::import_bits(w, q.begin(), q.end(), 8);

            stack.push_back(w);
        } else if (s == "%d") {
            BigInt w;
            std::cin >> w;
            stack.push_back(w);
        } else if (s == "%f") {
            std::string q;
            std::getline(std::cin, q);
            BigInt w;
            boost::multiprecision::import_bits(w, q.begin(), q.end(), 8);

            stack.push_back(w);
        }
    }
}
