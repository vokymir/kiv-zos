cmake_minimum_required(VERSION 3.16)
project(zos LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# with neovim using
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable debug symbols
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# include headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect sources
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# include libraries
target_link_libraries(${PROJECT_NAME} PRIVATE readline)

# set binaries folder for output
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# copy compile commands if exists
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json" DESTINATION "${PROJECT_SOURCE_DIR}")
endif()

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
