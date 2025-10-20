#include <iostream>
#include <string>
#include <format>
#include <string_view>

//! Should be used for application output
namespace fmt {
template <typename... Args>
inline void print(const std::string_view str, Args &&...args)
{
    std::cout << std::vformat(str, std::make_format_args(args...));
}

template <typename... Args>
inline std::string format(const std::string str, Args &&...args)
{
    return std::vformat(str, std::make_format_args(args...));
}

template <typename... Args>
inline void println(const std::string str, Args &&...args)
{
    const std::string wArgs = std::vformat(str, std::make_format_args(args...));
    fmt::print("{}\n", wArgs);
}
} // namespace fmt

//! Should be used for logging / debugging output
namespace log {

enum Level
{
    Debug,
    Info,
    Warning,
    Critical,
    Error
};

static Level logLevel = Info;

template <typename... Args>
inline void log(const std::string_view str, Args &&...args)
{
    std::cerr << std::vformat(str, std::make_format_args(args...));
}

template <typename... Args>
inline void info(const std::string str, Args &&...args)
{
    if (logLevel > Level::Info) {
        return;
    }
    const std::string wArgs = std::vformat(str, std::make_format_args(args...));
    log::log("\x1b[36m[info] {}\x1b[0m\n", wArgs);
}

template <typename... Args>
inline void warning(const std::string str, Args &&...args)
{
    if (logLevel > Level::Warning) {
        return;
    }
    const std::string wArgs = std::vformat(str, std::make_format_args(args...));
    log::log("\x1b[33m[warn] {}\x1b[0m\n", wArgs);
}

template <typename... Args>
inline void critical(const std::string str, Args &&...args)
{
    if (logLevel > Level::Critical) {
        return;
    }
    const std::string wArgs = std::vformat(str, std::make_format_args(args...));
    log::log("\x1b[31m[crit] }\x1b[0m\n", wArgs);
}

template <typename... Args>
inline void error(const std::string str, Args &&...args)
{
    if (logLevel > Level::Error) {
        return;
    }
    const std::string wArgs = std::vformat(str, std::make_format_args(args...));
    log::log("\x1b[31m[error]{}\x1b[0m\n", wArgs);
}
} // namespace log
