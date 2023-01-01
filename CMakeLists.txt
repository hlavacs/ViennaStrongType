
cmake_minimum_required (VERSION 3.0.0)
project (VSTY)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/bin)
set(INCLUDE ${CMAKE_HOME_DIRECTORY})

set(TARGET vsty)

set(SOURCE vsty.cpp)

set(HEADERS vsty.h)

add_executable(${TARGET} ${SOURCE} ${HEADERS})

target_compile_features(${TARGET} PUBLIC cxx_std_20)
