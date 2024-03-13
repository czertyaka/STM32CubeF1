cmake_minimum_required(VERSION 3.15)
project(cmsis C ASM)

set(BOARD "STM32F103C8" CACHE STRING "Select a board to build CMSIS for")
set(SUPPORTED_BOARDS
    "STM32F103C8"
)

message(CHECK_START "Checking for board support")
if (${BOARD} IN_LIST SUPPORTED_BOARDS)
    message(CHECK_PASS "done")
else ()
    message(FATAL_ERROR "unknown board: ${BOARD}")
endif ()

set(CMSIS_DIR "${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS")
set(CMSIS_INCLUDE_DIR "${CMSIS_DIR}/Include")

add_library(cmsis STATIC)

if (${BOARD} MATCHES "^STM32F1.+$")

    set(STM32F1XX_DIR "${CMSIS_DIR}/Device/ST/STM32F1xx")
    set(STM32F1XX_SRC_DIR "${STM32F1XX_DIR}/Source")
    set(STM32F1XX_INCLUDE_DIR "${STM32F1XX_DIR}/Include")

    if (${BOARD} STREQUAL "STM32F103C8")
        set(BOARD_TARGET "STM32F103xB")
    endif ()

    target_compile_definitions(cmsis PRIVATE "${BOARD_TARGET}")

    string(TOLOWER "startup_${BOARD_TARGET}.s" STARTUP_SRC)
    target_sources(cmsis PRIVATE
        "${STM32F1XX_SRC_DIR}/Templates/gcc/${STARTUP_SRC}"
        "${STM32F1XX_SRC_DIR}/Templates/system_stm32f1xx.c"
    )
    target_sources(cmsis PUBLIC
        FILE_SET HEADERS BASE_DIRS "${CMSIS_INCLUDE_DIR}"
        FILES "${CMSIS_INCLUDE_DIR}/core_cm3.h"
        FILES "${CMSIS_INCLUDE_DIR}/cmsis_version.h"
        FILES "${CMSIS_INCLUDE_DIR}/cmsis_compiler.h"
        FILES "${CMSIS_INCLUDE_DIR}/cmsis_gcc.h"
    )
    string(TOLOWER "${BOARD_TARGET}.h" BOARD_TARGET_HEADER)
    target_sources(cmsis PUBLIC
        FILE_SET HEADERS BASE_DIRS "${STM32F1XX_INCLUDE_DIR}"
        FILES
            "${STM32F1XX_INCLUDE_DIR}/stm32f1xx.h"
            "${STM32F1XX_INCLUDE_DIR}/system_stm32f1xx.h"
            "${STM32F1XX_INCLUDE_DIR}/${BOARD_TARGET_HEADER}"
    )

    string(TOUPPER "${BOARD_TARGET}_FLASH" LINKER_SCRIPT_FILENAME)
    set(LINKER_SCRIPT "${STM32F1XX_SRC_DIR}/Templates/gcc/linker/${LINKER_SCRIPT_FILENAME}.ld")

else ()

    message(FATAL_ERROR "Board does not match to any known board families")

endif ()

install(TARGETS cmsis FILE_SET HEADERS )
install(FILES ${LINKER_SCRIPT} DESTINATION linker)
