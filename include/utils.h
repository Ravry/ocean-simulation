#pragma once
#include <iostream>
#include <string_view>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <print>
#include <format>
#include <complex>
#include <cmath>
#include <numbers>

namespace {
    constexpr const char* COLOR_RESET   = "\033[0m";
    constexpr const char* COLOR_RED     = "\033[31m";
    constexpr const char* COLOR_YELLOW  = "\033[33m";
    constexpr const char* COLOR_GREEN   = "\033[32m";

    #define CONSOLE_HINT_INFO "INFO"
    #define CONSOLE_HINT_WARNING "WARNING"
    #define CONSOLE_HINT_ERROR "ERROR"

    template <typename... Args>
    void log_impl(const char* color, const char* file, int line, const char* hint, std::format_string<Args...> fmt, Args&&... args) {
        std::print("[{}:{}][{}{}{}] ", std::filesystem::path(file).filename().string(), line, color, hint, COLOR_RESET);
        std::print(fmt, std::forward<Args>(args)...);
        std::print("\n");
        std::fflush(stdout);
    }

    #define out(fmt, ...) log_impl(COLOR_GREEN, __FILE__, __LINE__, CONSOLE_HINT_INFO, fmt __VA_OPT__(,) __VA_ARGS__)
    #define out_warn(fmt, ...) log_impl(COLOR_YELLOW, __FILE__, __LINE__, CONSOLE_HINT_WARNING,  fmt __VA_OPT__(,) __VA_ARGS__)
    #define out_error(fmt, ...) log_impl(COLOR_RED, __FILE__, __LINE__, CONSOLE_HINT_ERROR,  fmt __VA_OPT__(,) __VA_ARGS__)
}

namespace Utils {
    static void read_file_content(std::string_view file_name, std::string& file_content) {
        std::ifstream file(file_name.data());
        if (!file.is_open()) {
            std::cerr << "failed to open file " << file_name << "\n";
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file_content = buffer.str();
    }

    static std::string_view get_file_name(std::string_view path) {
        return std::filesystem::path(path).filename().string();
    }

    static std::complex<double> complex_exp(double frequency, double time) {
        constexpr std::complex<double> i(0, 1);
        std::complex<double> result = exp(2.f * std::numbers::pi * frequency * i * time);
        return result;
    }
}

namespace Engine::Time {
    class Timer {
    public:
        static double delta_time;
    };
}