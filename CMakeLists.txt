cmake_minimum_required(VERSION 3.25)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

project(AudioDSP VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_library(synth_core STATIC
    src/audiobuffer.cpp
    src/synth/synth.cpp
    src/synth/voice.cpp
)
target_include_directories(synth_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(synth_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE APP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER APP_SRCS EXCLUDE REGEX "/src/(synth|plugin)/")
list(FILTER APP_SRCS EXCLUDE REGEX "/src/audiobuffer\\.cpp$")

add_executable(AudioDSP ${APP_SRCS})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(AudioDSP PRIVATE -Wall)
    target_compile_options(AudioDSP PRIVATE $<$<CONFIG:Release>:-O2>)
endif()

target_link_libraries(AudioDSP PRIVATE
    SDL2::SDL2
    synth_core
)

option(AUDIODSP_BUILD_VST3 "Build the VST3 plug-in (downloads the VST3 SDK)" ON)
if(AUDIODSP_BUILD_VST3)
    if(APPLE)
        include(cmake/audiounitsdk.cmake)
    endif()
    include(cmake/vst3sdk.cmake) 
    add_subdirectory(src/plugin)  
endif()
