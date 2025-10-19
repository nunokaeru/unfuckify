#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <filesystem>
#include "../src/unfucker.cpp"

std::filesystem::path getTestDataPath(const std::string &filename)
{
    std::filesystem::path parent = std::filesystem::absolute(__FILE__).parent_path() / "data";
    return std::filesystem::absolute(parent / filename);
}

TEST_CASE("Detect files containing 'auto'")
{
    auto [path, expected] = GENERATE(table<std::filesystem::path, bool>({
        {"lambda.cpp", true},
        {"lambda_fixed.cpp", false},
    }));

    std::filesystem::path absPath = getTestDataPath(path);
    INFO(std::filesystem::exists(absPath));
    REQUIRE(fileContainsAuto(absPath) == expected);
}
