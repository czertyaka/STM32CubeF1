import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.errors import ConanInvalidConfiguration


class cmsisRecipe(ConanFile):
    name = "cmsis"
    version = "5.1"

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "Drivers/CMSIS/*"

    def validate(self):
        if self.settings.os != "baremetal":
            raise ConanInvalidConfiguration(
                "CMSIS could not be built for profile with operating system"
            )
        elif not self.settings.os.vendor.board:
            raise ConanInvalidConfiguration("CMSIS requires a board to be specified")
        elif not str(self.settings.os.vendor.board).lower().startswith("stm32f1"):
            raise ConanInvalidConfiguration("This recipe is for STM32F1xx boards")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BOARD"] = str(self.settings.os.vendor.board).upper()
        tc.variables["CMAKE_C_COMPILER_WORKS"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["startup"].libs = ["cmsis"]
        self.cpp_info.components["startup"].includedirs = [
            os.path.join(self.package_folder, "include")
        ]
        self.cpp_info.components["startup"].set_property("cmake_target_name", "startup")

        self.cpp_info.components["linker_script"].libs = []
        self.cpp_info.components["linker_script"].set_property(
            "cmake_target_name", "linker_script"
        )
        self.cpp_info.components["linker_script"].includedirs = [
            os.path.join(self.package_folder, "linker")
        ]
