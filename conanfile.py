import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain
from conan.tools.cmake import cmake_layout

required_conan_version = ">=1.55.0"
class PrometheusCpp(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps"

    def build_requirements(self):
        if self.settings.os != "Windows":
           self.test_requires("benchmark/1.7.1")
        self.test_requires("gtest/1.12.1")

    def requirements(self):
        self.requires("civetweb/1.15")
        self.requires("libcurl/7.86.0")
        self.requires("zlib/1.2.13")

    def configure(self):
        self.options['civetweb'].with_ssl = False

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["USE_THIRDPARTY_LIBRARIES"] = False
        #tc.cache_variables["ENABLE_PULL"] = False
        #tc.cache_variables["ENABLE_PUSH"] = False
        #tc.variables["MYVAR"] = "MYVAR_VALUE"
        #tc.preprocessor_definitions["MYDEFINE"] = "MYDEF_VALUE"
        #tc.user_presets_path = os.path.join(self.source_folder, '..')
        tc.generate()

    def layout(self):
        # BUILD_FOLDER_SUFFIX={
        #     "None": "",
        #     "Address": "_asan",
        #     "Thread": "_tsan",
        #     "Memory": "_msan",
        #     "UndefinedBehavior": "_ubsan",
        # }
        # build_folder = "_build" + BUILD_FOLDER_SUFFIX[str(self.settings.compiler.sanitizer)]
        # cmake_layout(self, build_folder=build_folder)
        cmake_layout(self, build_folder="_build")

