cmake_minimum_required(VERSION 3.15)
project(CMSISTestPackage C)

find_package(cmsis REQUIRED COMPONENTS startup linker_script)

add_executable(main main.c)
target_link_libraries(main startup)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LINKER_SCRIPT_PATH ${cmsis_linker_script_INCLUDE_DIRS_DEBUG})
else()
    set(LINKER_SCRIPT_PATH ${cmsis_linker_script_INCLUDE_DIRS_RELEASE})
endif()
target_link_options(main PRIVATE -nostdlib "-T${LINKER_SCRIPT_PATH}/STM32F103XB_FLASH.ld")

add_custom_command(
    TARGET main
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
    ARGS -O binary main main.bin
)
