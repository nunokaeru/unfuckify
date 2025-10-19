#include <iostream>
#include <string>
#include <format>
#include <string_view>

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
