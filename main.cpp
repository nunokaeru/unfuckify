#include <string>
#include <filesystem>

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

int main(int argc, char *argv[])
{
    std::filesystem::path compileDbPath;
    std::filesystem::path sourceFile;

    Unfuckifier fixer;
    bool all = false;
    bool stopOnFail = false;
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
        if (arg == "--stop-on-fail") {
            stopOnFail = true;
            continue;
        }

        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }

        if (arg == "--cc" || arg == "--compile-commands") {
            if (argNum + 1 >= argc) {
                fmt::println("--compile-commands requires an argument");
                return 1;
            }
            compileDbPath = std::filesystem::path{argv[++argNum]};
            continue;
        }

        sourceFile = std::filesystem::path{arg};
    }

    if (compileDbPath.empty() || !std::filesystem::exists(compileDbPath)) {
        const std::filesystem::path cwd = std::filesystem::current_path();
        if (std::filesystem::path p{cwd / "compile_commands.json"}; std::filesystem::exists(p)) {
            compileDbPath = p;
        }

        if (std::filesystem::path p{cwd / "build" / "compile_commands.json"}; std::filesystem::exists(p)) {
            compileDbPath = p;
        }

        log::info("Using compiled database at '{}'", compileDbPath.string());
    }
    compileDbPath.remove_filename();
    int posixSucks = chdir(compileDbPath.c_str());
    if (posixSucks) {
        std::cerr << "failed to chdir to " << compileDbPath << ", noone cares" << std::endl;
    }

    if (!fixer.parseCompilationDatabase(compileDbPath.string())) {
        fmt::println("Failed to parse compilation database '{}'", compileDbPath.string());
        return 1;
    }

    if (all) {
        const std::vector<std::string> files = fixer.allAvailableFiles();
        const size_t totalFiles = files.size();
        for (size_t i = 0; i < totalFiles; i++) {
            log::info("Processing file {}/{}", i + 1, totalFiles);
            if (!fixer.process(files[i])) {
                if (stopOnFail) {
                    fmt::println("Failed to process '{}", files[i]);
                    return 1;
                }
                log::warning("Failed to process '{}'", files[i]);
            }
        }
    } else {
        if (sourceFile.empty() || !std::filesystem::exists(sourceFile)) {
            log::error("Source file '{}' does not exist", sourceFile.string());
            return 1;
        }

        sourceFile = std::filesystem::absolute(sourceFile);

        if (!fixer.process(sourceFile.string())) {
            fmt::println("Failed to process '{}'", sourceFile.string());
            return 1;
        }
    }
    return 0;
}
