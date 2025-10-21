#include <string>

#include "src/unfucker.cpp"

static void printUsage(const std::string &executable)
{
    fmt::println("Either pass a source file, or --all to fix all files in project\n"
                 "To replace the existing files pass --replace"
                 "\t{}",
                 executable);
    fmt::println("path/to/compile_commands.json [--verbose] [--dump-nodes] [--replace] [--skip-headers] "
                 "[--stop-on-fail] [--all] [--reformat] [path/to/heretical.cpp]\n"
                 "For some autos you will need to use --reformat, because we need to regenerate parts of the code\n"
                 "To create a compilation database run cmake with '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON' on the project "
                 "you're going to fix");
}

std::filesystem::path getCompileCommandsPath()
{
    const std::filesystem::path cwd = std::filesystem::current_path();

    if (std::filesystem::path p{cwd / "compile_commands.json"}; std::filesystem::exists(p)) {
        return p;
    }

    if (std::filesystem::path p{cwd / "build" / "compile_commands.json"}; std::filesystem::exists(p)) {
        return p;
    }

    // First found in build subdirs
    if (std::filesystem::path buildDir{cwd / "build"}; std::filesystem::exists(buildDir)) {
        for (const auto &entry : std::filesystem::directory_iterator{buildDir}) {
            if (entry.is_directory()) {
                if (std::filesystem::path p{entry.path() / "compile_commands.json"}; std::filesystem::exists(p)) {
                    return p;
                }
            }
        }
    }
    return {};
}

int main(int argc, char *argv[])
{
    std::filesystem::path compileDbPath;
    std::vector<std::string> sourceFiles;

    Unfuckifier fixer;
    bool all = false;
    bool failFast = false;
    for (int argNum = 1; argNum < argc; argNum++) {
        const std::string arg = argv[argNum];

        if (arg == "--all") {
            all = true;
            continue;
        }
        if (arg == "--replace") {
            fixer.replaceFile = true;
            continue;
        }
        if (arg == "--verbose") {
            fixer.verbose = true;
            continue;
        }
        if (arg == "--dump-nodes") {
            fixer.dumpNodes = true;
            continue;
        }
        if (arg == "--skip-headers") {
            fixer.skipHeaders = true;
            continue;
        }
        if (arg == "--reformat") {
            fixer.reformat = true;
            continue;
        }
        if (arg == "--fail-fast") {
            failFast = true;
            continue;
        }

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }

        if (arg == "-cc" || arg == "--compile-commands") {
            if (argNum + 1 >= argc) {
                fmt::println("--compile-commands requires an argument");
                return 1;
            }
            compileDbPath = std::filesystem::path{argv[++argNum]};
            continue;
        }
        if (arg == "-b" || arg == "--build-directory") {
            if (argNum + 1 >= argc) {
                fmt::println("--build-directory requires an argument");
                return 1;
            }
            compileDbPath = std::filesystem::path{argv[++argNum]} / "compile_commands.json";
            continue;
        }

        sourceFiles.emplace_back(arg);
    }

    if (compileDbPath.empty() || !std::filesystem::exists(compileDbPath)) {
        compileDbPath = getCompileCommandsPath();
        if (compileDbPath.empty()) {
            log::error("Could not find a valid compilation database");
            return 1;
        }
    }
    log::info("Using compiled database at '{}'", compileDbPath.string());

    compileDbPath.remove_filename();
    int posixSucks = chdir(compileDbPath.c_str());
    if (posixSucks) {
        std::cerr << "failed to chdir to " << compileDbPath << ", noone cares" << std::endl;
    }

    if (!fixer.parseCompilationDatabase(compileDbPath.string())) {
        log::error("Failed to parse compilation database '{}'", compileDbPath.string());
        return 1;
    }

    if (all) {
        const std::vector<std::string> files = fixer.allAvailableFiles();
        const size_t totalFiles = files.size();
        for (size_t i = 0; i < totalFiles; i++) {
            if (!fixer.process(files[i])) {
                if (failFast) {
                    log::critical("Failed to process '{}", files[i]);
                    return 1;
                }
                log::warning("Failed to process '{}'", files[i]);
            }
        }
    } else {
        for (const std::string &file : sourceFiles) {
            if (file.empty() || !std::filesystem::exists(file)) {
                log::warning("Source file '{}' does not exist", file);
                if (failFast) {
                    return 1;
                }
                continue;
            }

            if (!fixer.process(std::filesystem::absolute(file))) {
                log::critical("Failed to process '{}'", file);
                if (failFast) {
                    return 1;
                }
                continue;
            }
        }
    }
    return 0;
}
