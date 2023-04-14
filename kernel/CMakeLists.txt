cmake_minimum_required(VERSION 3.18)
project(ftb_kernel C)

add_executable(
	kernel
)

target_include_directories(kernel PUBLIC csrc)
target_compile_options(
	kernel
	PUBLIC -nostdlib
)

