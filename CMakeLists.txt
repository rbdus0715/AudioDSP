cmake_minimum_required(VERSION 3.16)
project(AudioDSP LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(AudioDSP ${SRCS})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(AudioDSP PRIVATE -Wall)
    target_compile_options(AudioDSP PRIVATE $<$<CONFIG:Release>:-O2>)
endif()

target_link_libraries(AudioDSP PRIVATE 
    SDL2::SDL2
)

target_include_directories(AudioDSP PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
