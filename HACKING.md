# Hacking

Here is some wisdom to help you build and test this project as a developer and
potential contributor.

If you plan to contribute, please read the [CONTRIBUTING](CONTRIBUTING.md)
guide.

## Developer mode

Build system targets that are only useful for developers of this project are
hidden if the `REMODULE_DEVELOPER_MODE` option is disabled. Enabling this
option makes tests and other developer targets and options available. Not
enabling this option means that you are a consumer of this project and thus you
have no need for these targets and options.

Developer mode is always set to on in CI workflows.

### Software prerequisites

- [CMake][cmake_download] 3.24.0 or higher
- [Conan][conan] 1.52.0 or higher
- [Clang 14][clang_14] or higher
    - Independently of if you use it for compiling, it's needed for `clang-format` to enforce linting rules.

[conan]: https://conan.io/
[cmake_download]: https://cmake.org/download/
[clang_14]: https://clang.llvm.org/

### Presets

This project makes use of [presets][cmake_presets] to simplify the process of configuring
the project. As a developer, you are recommended to always have the [latest
CMake version][cmake_download] installed to make use of the latest Quality-of-Life
additions.

You have a few options to pass `REMODULE_DEVELOPER_MODE` to the configure
command, but this project prefers to use presets.

Though it is not required, as a developer, you can create a `CMakeUserPresets.json` file at the root of
the project. The `CMakePresets.json` shipped with the project should suffice for most use cases.

`CMakeUserPresets.json` example:
```json
{
  "version": 5,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 24,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "dev",
      "binaryDir": "${sourceDir}/build/dev",
      "inherits": ["dev-mode", "conan", "ci-<os>"],
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "dev",
      "configurePreset": "dev",
      "configuration": "Debug"
    }
  ],
  "testPresets": [
    {
      "name": "dev",
      "configurePreset": "dev",
      "configuration": "Debug",
      "output": {
        "outputOnFailure": true
      }
    }
  ]
}
```

You should replace `<os>` in your newly created presets file with the name of
the operating system you have, which may be `win64` or `unix`. You can see what
these correspond to in the [`CMakePresets.json`](CMakePresets.json) file.

`CMakeUserPresets.json` is also the perfect place in which you can put all
sorts of things that you would otherwise want to pass to the configure command
in the terminal.

[cmake_presets]: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html

### Dependency management

The above preset will make use of the [conan][conan] dependency manager. Download
the dependencies and generate the necessary CMake files by running this command
in the project root:

```sh
conan install . -s build_type=Debug -b missing
```

You must run this once per build type you want to develop with.

Note that if your conan profile does not specify the same compiler used by
CMake, then that could potentially cause issues. See the [conan docs][profile]
on profiles.

[profile]: https://docs.conan.io/en/latest/using_packages/using_profiles.html

### Configure, build and test

If you followed the above instructions, then you can configure, build and test
the project respectively with the following commands from the project root on
any operating system with any build system:

```sh
cmake --preset=dev
cmake --build --preset=dev
ctest --preset=dev
```

If you are using a compatible editor (e.g. VSCode) or IDE (e.g. CLion, VS), you
will also be able to select the above created user presets for automatic
integration.

Please note that both the build and test commands accept a `-j` flag to specify
the number of jobs to use, which should ideally be specified to the number of
threads your CPU has. You may also want to add that to your preset using the
`jobs` property, see the [presets documentation][cmake_presets] for more details.

### Developer mode targets

These are targets you may invoke using the build command from above, with an
additional `-t <target>` flag:

#### `coverage`

Available if `ENABLE_COVERAGE` is enabled. This target processes the output of
the previously run tests when built with coverage configuration. The commands
this target runs can be found in the `COVERAGE_TRACE_COMMAND` and
`COVERAGE_HTML_COMMAND` cache variables. The trace command produces an info
file by default, which can be submitted to services with CI integration. The
HTML command uses the trace command's output to generate a HTML document to
`<binary-dir>/coverage_html` by default.

#### `docs`

Available if `BUILD_MCSS_DOCS` is enabled. Builds to documentation using
Doxygen and m.css. The output will go to `<binary-dir>/docs` by default
(customizable using `DOXYGEN_OUTPUT_DIRECTORY`).

#### `format-check` and `format-fix`

These targets run the clang-format tool on the codebase to check errors and to
fix them respectively. Customization available using the `FORMAT_PATTERNS` and
`FORMAT_COMMAND` cache variables.

#### `spell-check` and `spell-fix`

These targets run the codespell tool on the codebase to check errors and to fix
them respectively. Customization available using the `SPELL_COMMAND` cache
variable.
