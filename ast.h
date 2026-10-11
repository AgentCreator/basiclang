//
// Created by Oleksander Krainiak on 12.09.2026.
//

#ifndef BASICLANG_AST_H
#define BASICLANG_AST_H
#include <string_view>
#include <utility>

namespace ast {
    enum class Precedence: unsigned char {
        lowest,
        cmp,
        add,
        mul,
        prefix,
    };
}


#endif //BASICLANG_AST_H
