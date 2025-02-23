#pragma once

#include <string_view>
#include <format>
#include <iostream>
#include <source_location>

namespace Jetstream::Utils {

#if !defined(JET_DIST) && !defined(JET_DEBUG) && !defined(JET_RELEASE)
#define JET_DIST
#endif

// #define JET_LOG(message) Jetstream::Utils::LogInfo(message)
// #define JET_WARN(message) Jetstream::Utils::LogWarning(message)
// #define JET_ERROR(message) Jetstream::Utils::LogError(message)

#ifndef JET_DIST
#define JET_ASSERT(expr, message) ::Jetstream::Utils::Assert(expr, #expr, message)
#else
#define JET_ASSERT(expr, message)
#endif

enum class Color: uint8_t {
    DEFAULT = 39,
    BLUE = 34,
    YELLOW = 33,
    RED = 31,
};

std::string SprintColor(std::string_view message, Color color = Color::DEFAULT);

template<typename... Args>
void Println(std::string_view message, Args&&... args) {
    std::cout << std::vformat(message, std::make_format_args(args...)) << '\n';
}

void Log(std::string_view message, Color color = Color::DEFAULT);

void LogInfo(std::string_view message, const std::source_location& location = std::source_location::current());
void LogWarning(std::string_view message, const std::source_location& location = std::source_location::current());
void LogError(std::string_view message, const std::source_location& location = std::source_location::current());
void LogAssert(std::string_view message, std::string_view expr_message, const std::source_location& location = std::source_location::current());

void Assert(const bool& expr, std::string_view expr_message, std::string_view message, const std::source_location& location = std::source_location::current());

}

using Jetstream::Utils::LogInfo;
using Jetstream::Utils::LogWarning;
using Jetstream::Utils::LogError;


