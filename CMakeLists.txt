cmake_minimum_required(VERSION 3.12)
project(ArithmeticCoding C)

set(CMAKE_C_STANDARD 11)

add_executable(ArithmeticCoding main.c bool.h errors.h coding.c coding.h iodefinitions.h test.c test.h)