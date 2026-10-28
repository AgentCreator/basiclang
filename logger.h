//
// Created by Oleksander Krainiak on 31.08.2026.
//

#ifndef BASICLANG_LOGGER_H
#define BASICLANG_LOGGER_H
#include <iostream>
#include <format>
#include <print>

// its all in a header file because for some reason when I
// tried to isolate the implementation logic to other files
// weird compile errors were happening, I think that may be
// because of the templates.

namespace Log {
    enum class Level {
        Debug, Info, Warning, Error, None
    };

    template<Level level>
    consteval auto NameOf() -> const char * {
        if constexpr (level == Level::Debug) return "DEBUG";
        if constexpr (level == Level::Error) return "ERROR";
        if constexpr (level == Level::Warning) return "WARNING";
        if constexpr (level == Level::Info) return "INFO";

        return "UNKNOWN";
    }

    inline auto GlobalLogLevel = Level::None;

    struct Logger {
        const std::string_view name;

        template<Level Level, typename... Args>
        void Log(std::format_string<Args...> fmt, Args &&... args) const {
            if constexpr (Level == Log::Level::Error) {
                throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
            }

            static_assert(Level != Log::Level::None, "you can't log to None");
            if (Level >= GlobalLogLevel)
                std::println("[{}.{}]: {}", name, NameOf<Level>(),
                             std::format(fmt, std::forward<Args>(args)...));
        }
    };
}

#endif //BASICLANG_LOGGER_H
