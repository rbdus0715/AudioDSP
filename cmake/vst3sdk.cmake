include(FetchContent)

set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
# 호스팅 예제 중 editorhost(플러그인 UI 만 띄우는 작은 앱)를 GUI 편집용으로 씀.
# Xcode 는 요청한 타겟만 빌드하므로 켜 둬도 됨. Makefile 은 전부 빌드하다가
# audiohost 링크가 깨지므로 끔.
if(XCODE)
    set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES ON CACHE BOOL "" FORCE)
else()
    set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
endif()
set(SMTG_ENABLE_VSTGUI_SUPPORT ON CACHE BOOL "" FORCE)

FetchContent_Declare(vst3sdk
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG        v3.8.0_build_66
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES base cmake pluginterfaces public.sdk vstgui4
    GIT_PROGRESS   TRUE
    PATCH_COMMAND sed -i "" -e "s/getVariable<AudioUnit> (VarNamePlugView)/getVariable<AudioUnit> (VarNameAudioUnit)/" -e "s/[[]self dealloc[]]/[self release]/" public.sdk/source/vst/auwrapper/aucocoaview.mm
)
FetchContent_MakeAvailable(vst3sdk)

smtg_enable_vst3_sdk()

list(APPEND CMAKE_MODULE_PATH "${vst3sdk_SOURCE_DIR}/cmake/modules")

# VSTGUI 는 macOS 에서 -Werror 로 빌드됨. 새 macOS SDK 가 wstring_convert 를
# deprecated 로 표시해서 경고 -> 에러가 됨. 이 경고만 에러에서 제외.
foreach(target vstgui vstgui_uidescription vstgui_support)
    if(TARGET ${target})
        target_compile_options(${target} PRIVATE -Wno-error=deprecated-declarations)
    endif()
endforeach()
