cmake_minimum_required(VERSION 3.16)

project(semestralka LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable export of compile_commands.json for IDEs (VS Code, Neovim, CLion, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect sources
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/commands/*.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include the commands directory for headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/commands
)

# Put the executable directly in the build directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Common warnings
set(COMMON_WARNINGS
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wformat=2
    -Wimplicit-fallthrough
)

# Extra GCC warnings
set(GCC_WARNINGS
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wuseless-cast
    -Wdouble-promotion
)

# Extra Clang warnings
set(CLANG_WARNINGS
    -Weverything
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
)

# Apply compiler-specific options
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_WARNINGS} ${GCC_WARNINGS})
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_WARNINGS} ${CLANG_WARNINGS})
endif()
