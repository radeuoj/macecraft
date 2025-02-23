#include "jet_utils.h"

#include <chrono>
#include <filesystem>

std::string Jetstream::Utils::SprintColor(std::string_view message, Color color) {
    std::stringstream ss;
    
    ss << "\033[" << static_cast<int>(color) << "m" << message << "\033[39m";

    return ss.str();
}

void Jetstream::Utils::Log(std::string_view message, Color color) {

    auto now = std::chrono::current_zone()->to_local(std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()));
    std::string formatted_time = std::format("{:%H:%M:%S}", now);

    // auto file_path = std::filesystem::relative(location.file_name(), std::filesystem::current_path()).string();

    std::stringstream ss;
    
    ss << "[" << formatted_time << "] " << message;

    // switch (color) {
    // case Color::YELLOW:
    //     ss << "WARNING at ";
    //     break;
    // case Color::RED:
    //     ss << "ERROR at ";
    //     break;
    // }
    
    // ss << file_path << ':' << location.line() << ':' << location.column() << ' ' << message;

    std::cout << SprintColor(ss.str(), color) << '\n';
}

void Jetstream::Utils::LogInfo(std::string_view message, const std::source_location& location) {
    auto file_path = std::filesystem::relative(location.file_name(), std::filesystem::current_path()).string();
    Log(std::format("{}:{}:{} {}", file_path, location.line(), location.column(), message), Color::DEFAULT);
}

void Jetstream::Utils::LogWarning(std::string_view message, const std::source_location& location) {
    auto file_path = std::filesystem::relative(location.file_name(), std::filesystem::current_path()).string();
    Log(std::format("WARNING at {}:{}:{} {}", file_path, location.line(), location.column(), message), Color::YELLOW);
}

void Jetstream::Utils::LogError(std::string_view message, const std::source_location& location) {
    auto file_path = std::filesystem::relative(location.file_name(), std::filesystem::current_path()).string();
    Log(std::format("ERROR at {}:{}:{} {}", file_path, location.line(), location.column(), message), Color::RED);
}

void Jetstream::Utils::LogAssert(std::string_view message, std::string_view expr_message, const std::source_location& location) {
    auto file_path = std::filesystem::relative(location.file_name(), std::filesystem::current_path()).string();
    Log(std::format("ASSERTION {} FAILED at {}:{}:{} {}", expr_message, file_path, location.line(), location.column(), message), Color::RED);
}

void Jetstream::Utils::Assert(const bool& expr, std::string_view expr_message, std::string_view message, const std::source_location& location) {
    if (!expr) {
        LogAssert(message, expr_message, location);
        std::abort();
    }
}
