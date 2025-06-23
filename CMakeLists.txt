cmake_minimum_required(VERSION 3.10)
project(alloc_test LANGUAGES CXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -rdynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Locate Tracy headers and library
find_path(TRACY_INCLUDE_DIR
        NAMES Tracy.hpp
        PATHS /usr/local/include /usr/include
)
find_library(TRACY_CLIENT_LIB
        NAMES tracy
        PATHS /usr/local/lib /usr/lib
)

if(NOT TRACY_INCLUDE_DIR OR NOT TRACY_CLIENT_LIB)
    message(FATAL_ERROR "Tracy profiler not found. Please install Tracy and/or adjust TRACY_INCLUDE_DIR and TRACY_CLIENT_LIB paths.")
endif()

# Optional: find threading library for Tracy
find_package(Threads REQUIRED)

# Define executable
add_executable(alloc_test alloc_test.cpp)

# Enable Tracy macros
target_compile_definitions(alloc_test PRIVATE TRACY_ENABLE)

# Include Tracy headers
target_include_directories(alloc_test PRIVATE ${TRACY_INCLUDE_DIR})

# Link against Vulkan, Tracy client lib, and pthread
target_link_libraries(alloc_test
        PRIVATE
        Vulkan::Vulkan
        ${TRACY_CLIENT_LIB}
        Threads::Threads
)
# Enable debug symbols and symbol visibility
