import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain
from conan.tools.cmake import cmake_layout

class PrometheusCpp(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps"

    def build_requirements(self):
        self.build_requires("benchmark/1.7.1")
        self.build_requires("civetweb/1.15")        
        self.build_requires("libcurl/7.86.0")        
        self.build_requires("gtest/1.12.1")        
        self.build_requires("zlib/1.2.13")        

    def generate(self):
        #self.folders.build_folder = self.generators_folder
        print(vars(self.layouts.build))
        print(self.package_folder)
        print(self.export_folder)
        print(self.source_folder)
        print(self.generators_folder)
        print(self.build_folder)
        print('----')
        tc = CMakeToolchain(self)
        tc.cache_variables["USE_THIRDPARTY_LIBRARIES"] = False
        #tc.variables["MYVAR"] = "MYVAR_VALUE"
        #tc.preprocessor_definitions["MYDEFINE"] = "MYDEF_VALUE"
        #tc.user_presets_path = os.path.join(self.source_folder, '..')
        tc.generate()

    #def layout(self):
    #    self.build = self.generators_folder

    def layout(self):
        cmake_layout(self)
