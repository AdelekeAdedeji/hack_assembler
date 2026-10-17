cmake_minimum_required(VERSION 3.20)

project(hack_assembler C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hack_assembler
    src/hack_assembler.c
    src/hack_preprocessor.c
    src/utils.c
)

target_include_directories(hack_assembler PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(hack_assembler PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -g
)

add_custom_target(run
    COMMAND $<TARGET_FILE:hack_assembler>
            ${PROJECT_SOURCE_DIR}/file.asm
    DEPENDS hack_assembler
)

add_custom_target(valgrind
    COMMAND valgrind
        --leak-check=full
        --show-leak-kinds=all
        $<TARGET_FILE:hack_assembler>
        ${PROJECT_SOURCE_DIR}/file.asm
    DEPENDS hack_assembler
)   

