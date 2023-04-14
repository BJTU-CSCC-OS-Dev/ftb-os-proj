cmake_minimum_required(VERSION 3.18)
project(ftb_kernel C)

file(GLOB_RECURSE kerCSrc ${PROJECT_SOURCE_DIR}/csrc/*.c)
add_executable(kernel ${kerCSrc})

target_include_directories(kernel PUBLIC csrc)
target_compile_options(
	kernel
	PUBLIC -nostdlib
)

