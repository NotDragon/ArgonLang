cmake_minimum_required(VERSION 3.28)
project(lang_cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(lang_cmake ${SOURCES})

target_include_directories(lang_cmake PRIVATE include)
#target_link_directories(lang_cmake PRIVATE lib)
#target_link_libraries(lang_cmake PRIVATE library_name)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")