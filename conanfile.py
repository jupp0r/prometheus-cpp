import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain
from conan.tools.cmake import cmake_layout

class PrometheusCpp(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps"

    def build_requirements(self):
        if self.settings.os != "Windows":
            self.build_requires("benchmark/1.7.1")
        self.build_requires("civetweb/1.15")        
        self.build_requires("libcurl/7.86.0")        
        self.build_requires("gtest/1.12.1")        
        self.build_requires("zlib/1.2.13")        

    def configure(self):
        self.options['civetweb'].with_ssl = False

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["USE_THIRDPARTY_LIBRARIES"] = False
        #tc.variables["MYVAR"] = "MYVAR_VALUE"
        #tc.preprocessor_definitions["MYDEFINE"] = "MYDEF_VALUE"
        #tc.user_presets_path = os.path.join(self.source_folder, '..')
        tc.generate()

    def layout(self):
        BUILD_FOLDER_SUFFIX={
            "None": "",
            "Address": "_asan",
            "Thread": "_tsan",
            "Memory": "_msan",
            "UndefinedBehavior": "_ubsan",
        }
        build_folder = "_build" + BUILD_FOLDER_SUFFIX[str(self.settings.compiler.sanitizer)]
        cmake_layout(self, build_folder=build_folder)

