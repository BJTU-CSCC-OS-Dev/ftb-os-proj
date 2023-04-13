cmake_minimum_required(VERSION 3.18)
project(ftb)

add_executable(
	kernel
	csrc/main.c
	csrc/lock/spinlock.c
)

