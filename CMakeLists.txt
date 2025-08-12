cmake_minimum_required(VERSION 3.28)
project(ArgonLang VERSION 0.0.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

# GTest Installation
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Source files for the library (excluding main.cpp)
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp") # Exclude main.cpp

# Create a library for shared code
add_library(ArgonLangLib STATIC ${LIB_SOURCES})
target_include_directories(ArgonLangLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Create the main executable
add_executable(ArgonLang src/main.cpp)
target_link_libraries(ArgonLang PRIVATE ArgonLangLib)

# Add unit tests
enable_testing()
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp") # Include all test files in 'tests/' directory
add_executable(ArgonLangTests ${TEST_SOURCES})
target_link_libraries(ArgonLangTests PRIVATE ArgonLangLib gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(ArgonLangTests)

target_compile_options(ArgonLang PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-O3>
)

# Metadata
message(STATUS "Building ${PROJECT_NAME} version ${PROJECT_VERSION}")