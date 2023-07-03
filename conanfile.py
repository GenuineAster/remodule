from conan import ConanFile
from conan.tools.cmake import CMake
from conans.tools import load
import re

def get_version():
    try:
        content = load("CMakeLists.txt")
        version = re.search(r"set\(REMODULE_VERSION (.*)\)", content).group(1)
        return version.strip()
    except Exception as e:
        print(e)
        return None

class Recipe(ConanFile):
    name = "remodule"
    version = get_version()
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualRunEnv"

    options = {"shared": [True, False], "fPIC": [True, False]}

    default_options = {
        "catch2/*:with_main": True,
        "shared": False,
        "fPIC": True
    }

    scm = {
         "type": "git",
         "url": "auto",
         "revision": "auto"
    }

    exports_sources = ["source/*", "include/*", "cmake/*", "CMakeLists.txt", "test/*"]

    def configure(self):
        if self.options["shared"] and self.settings.os == "Linux":
            self.options["*:fPIC"] = True

    def layout(self):
        self.folders.generators = "conan"
        self.folders.build = "build"

    def requirements(self):
        # Testing only dependencies below
        self.requires("catch2/2.13.9")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE.md", dst="licenses", src=self.source_folder)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["remodule"]
