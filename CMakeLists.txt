
cmake_minimum_required(VERSION 3.28)

set(target unfuckify)
set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD C++23)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

project(${target} VERSION 1.0 LANGUAGES C CXX)

find_package(Clang REQUIRED)
include_directories(${CLANG_INCLUDE_DIRS})
add_executable(${target} src/main.cpp)

file(GLOB sources "src/*.cpp")
file(GLOB includes "src/*.h")
add_library(libunfuckify STATIC ${includes} ${sources})
target_link_libraries(${target} PRIVATE ${LIBCLANG_LIBRARIES} libclang libunfuckify)


include(CTest) # calls enable_testing()
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
install(TARGETS ${target})
