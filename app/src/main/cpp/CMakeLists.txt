cmake_minimum_required(VERSION 3.22.1)
project(baeagn)

add_library(baeagn SHARED engine_jni.c)
target_compile_options(baeagn PRIVATE -O2)
target_link_libraries(baeagn m)
