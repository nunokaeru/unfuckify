# unfuckify

Fixes readability of C++ source code by resolving `auto` and replacing it with
the real type.

## Why

`auto` is the worst abomination to have happened to C++.

Source code is written once and read many times, so spend the two seconds it
takes to write out the type of the variable. Then, when people just want to read
the code they'll have the type right fucking there. Extra points for thinking about people
that aren't using an LSP or an IDE.

## Requirements

- [Cmake](https://cmake.org/download/) higher than `v3.28`;
- [Ninja](https://ninja-build.org/);
- [LLVM](https://llvm.org/);
- [Clang](https://clang.llvm.org/);

### Ubuntu 24.04:

```
sudo apt update
sudo apt install cmake ninja-build gcc g++ clang-20 llvm-20 libclang-20-dev clang-tools-20 libedit-dev zlib1g-dev libzstd-dev libcurl4-openssl-dev
```

## Building and running the project

Three presets are provided, one for windows `windows-clang` (doesn't work) and two for `linux` and `linux-clang`.

- List the presets available with `cmake --list-presets`;
- Run the project configure step with `cmake --preset <preset>`;
- Build the project `cmake --build --preset <preset>`;
- Run the main executable `./build/<preset>/bin/unfuckify` (Add the file extension `.exe` on Windows).
- Testing `ctest --preset <preset>`

## Usage

To use just run pass `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to cmake when
building the project you want to fix, and then run `unfuckify path/to/buildfolder/compile_commands.json` and either the name of a file or
`--all` to fix all files in the project.

By default it will write the fixed source to `foo.cpp-fixed`, pass `--replace`
to replace the existing file.

This uses libclang to parse and resolve the types, so if your project builds
with clang this should work as well.

## Example

In a random CMake based project:

- `mkdir build && cd build`
- `CC="clang" CXX="clang++" cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -GNinja ..`
- `ninja` (to make sure generated files are available)
- `unfuckify --replace compile_commands.json ../src/main.cpp`

## Why the offensive name

Like all great projects this was written in a moment of frustration and anger.

Official upstream suggestions for alternative names, if you want to package
this for a distro that doesn't allow swearing:

- Antonyms of autonomy (courtesy of wordhippo.com):
  - Heteronomy
  - Bondage
  - Subservience
  - Subjugation
  - Other BDSM terms, apparently
- Auto Annihilator
- Auto-b-Gone
- auto-was-a-mistake-please-help
- N1984, The Name Should Have Given It Away
- no-n1984

## Known issues

`auto` is very magic and hides a _lot_ of complexity, so trying to resolve the
actual types is hard.

If libclang is unable to give a proper type for something (like lambdas), we can't replace it.

There are some issues with libclang's handling of tokens vs. cursors, e. g. it
will give us the fully qualified type (including const, \*, &, etc.), but the
extent we get only covers `auto` itself.

We should handle the cases here properly now, but no guarantees that I have catched all cases. So make sure you review the code it changes.

Cases it fails on:

- Some extremely magic function pointer stuff. I don't really know myself what
  the people who wrote it are trying to do, so I don't blame clang.

## TODO

- Handle structured binding (the auto there hides std::pairs, or sometimes QPairs).
- Chop off the 'const' in std::functions.
- Automatically find `compile_commands.json`
- Integrate with cmake presets
- Create `.pre-commit` workflow
- Windows preset to work (what do I need to install)
- Mac preset to work (what do I need to install)
- Should I install clang lib via FetchCmake?
- Automate build process to not depend on installing linux packages (possible?)
- Clang devlib to be statically linked, so the binary can just be shipped standalone

## Docker

- Create docker image: `docker build --tag nunokaeru/unfuckify:dev .`
- Create docker container `docker run -d --tty --name unf nunokaeru/unfuckify:dev`
  - [pre-commit](#pre-commit) will set `volume` and `workdir`, you need to set them yourself if you want to use the container manually
  - [pre-commit](https://pre-commit.com/#docker) will automatically mount the repository with `--volume $PWD:/src:rw,Z` and set the directory with `--workdir /src`
  - To test, change to a different repository in n adjacent directory and run `uvx pre-commit try-repo ..\unfuckify\ unfuckify-dev --verbose --all-files`. The `nunokaeru/unfuckify:dev` docker image must be built manually for testing purposes.
- Execute a command in the docker container: `docker exec unf unfuckify --help`
- Stop and Delete the docker container `docker stop unf && docker rm unf`

# Releases

#### Github

- Tag a certain commit `git tag v0.0.X`
- Push the tags to remote `git push origin --tags`

## To Docker

- Tag docker image (promote to latest or current tag): `docker image tag nunokaeru/unfuckify:dev nunokaeru/unfuckify:latest`
- Publish docker image: `docker push nunokaeru/unfuckify:latest`
