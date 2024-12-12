cmake_minimum_required(VERSION 3.28)
project(lang_cmake VERSION 0.0.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(lang_cmake ${SOURCES})

# Include directories
target_include_directories(lang_cmake PRIVATE include)

#target_link_directories(lang_cmake PRIVATE lib)
#target_link_libraries(lang_cmake PRIVATE library_name)

target_compile_options(lang_cmake PRIVATE
        $<$<CONFIG:Debug>:-g>
#        $<$<CONFIG:Release>:-O3>
)

# Metadata
message(STATUS "Building ${PROJECT_NAME} version ${PROJECT_VERSION}")
