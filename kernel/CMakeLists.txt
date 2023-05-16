cmake_minimum_required(VERSION 3.18)
project(ftb_kernel C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)

file(GLOB_RECURSE kerCSrc ${PROJECT_SOURCE_DIR}/csrc/*.c)
add_executable(kernel ${kerCSrc})

target_include_directories(kernel PUBLIC csrc)
target_compile_options(
	kernel
	PUBLIC -nostdlib -Wno-incompatible-library-redeclaration -Wall
)