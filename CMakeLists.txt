cmake_minimum_required(VERSION 3.25)

# macOS 최소 지원 버전. project() 전에 정해야 한다.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

project(AudioDSP VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# ---------------------------------------------------------------------------
# 1) synth_core: 신스 엔진. SDL도 VST3도 모르는 순수 C++.
#    SDL 앱과 VST3 플러그인이 이 라이브러리를 똑같이 가져다 쓴다.
#    (Vital의 src/synthesis 와 같은 역할)
# ---------------------------------------------------------------------------
add_library(synth_core STATIC
    src/audiobuffer.cpp
    src/synth/synth.cpp
)
target_include_directories(synth_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
# 플러그인(.vst3)은 공유 라이브러리라서, 거기에 링크되는 정적 라이브러리도 PIC로 빌드해야 한다.
set_target_properties(synth_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# 2) AudioDSP: SDL로 소리를 내는 실험용 프로그램 (Vital의 src/standalone 역할)
# ---------------------------------------------------------------------------
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE APP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# 엔진(synth_core에 이미 들어 있음)과 플러그인 코드는 앱에서 제외
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

# ---------------------------------------------------------------------------
# 3) VST3 플러그인 (Vital의 src/plugin 역할)
#    VST3 SDK는 cmake/vst3sdk.cmake 에서 FetchContent로 자동으로 받는다.
#    SDL 앱만 빌드하려면: cmake .. -DAUDIODSP_BUILD_VST3=OFF
# ---------------------------------------------------------------------------
option(AUDIODSP_BUILD_VST3 "Build the VST3 plug-in (downloads the VST3 SDK)" ON)
if(AUDIODSP_BUILD_VST3)
    include(cmake/vst3sdk.cmake)   # 외부 의존성 준비
    add_subdirectory(src/plugin)   # 내 플러그인 코드
endif()
