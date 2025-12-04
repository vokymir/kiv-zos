cmake_minimum_required(VERSION 3.16)
project(zos LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# with neovim using
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect sources
file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# include headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# 1. Force highest level of debug info (gdb and lldb compatible)
# 2. Force NO optimization to prevent code rearrangement
target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0)

# include libraries
target_link_libraries(${PROJECT_NAME} PRIVATE readline) # cmd-like arrow up/down usage

# set binaries folder for output
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# copy compile commands if exists
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json" DESTINATION "${PROJECT_SOURCE_DIR}")
endif()
