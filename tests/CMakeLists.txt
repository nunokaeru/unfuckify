Include(FetchContent)

message(STATUS "CMake Fetching Content - Catch2")
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(tests test_executable.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ProjectName)

catch_discover_tests(tests)
