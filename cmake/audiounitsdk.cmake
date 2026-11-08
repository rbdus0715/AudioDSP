include(FetchContent)

FetchContent_Declare(audiounitsdk
    GIT_REPOSITORY https://github.com/apple/AudioUnitSDK.git
    GIT_TAG        AudioUnitSDK-1.4.0
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  do-not-add
)
FetchContent_MakeAvailable(audiounitsdk)

set(SMTG_AUDIOUNIT_SDK_PATH "${audiounitsdk_SOURCE_DIR}" CACHE PATH "" FORCE)