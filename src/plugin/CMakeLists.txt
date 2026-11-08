smtg_add_vst3plugin(AudioDSPSynth
    version.h
    cids.h
    processor.hpp
    processor.cpp
    controller.hpp
    controller.cpp
    entry.cpp
)

target_link_libraries(AudioDSPSynth
    PRIVATE
        sdk
        synth_core
)
smtg_target_setup_universal_binary(synth_core)

smtg_target_configure_version_file(AudioDSPSynth)
target_include_directories(AudioDSPSynth PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

if(SMTG_MAC)
    smtg_target_set_bundle(AudioDSPSynth
        BUNDLE_IDENTIFIER com.rbdus0715.audiodspsynth
        COMPANY_NAME "rbdus0715"
    )
endif()

if(SMTG_MAC AND XCODE AND SMTG_ENABLE_AUV2_BUILDS)
    include(SMTG_AddVST3AuV2)
    smtg_target_add_auv2(AudioDSPSynth_AU
        BUNDLE_NAME         AudioDSPSynth
        BUNDLE_IDENTIFIER   com.rbdus0715.audiodspsynth.audiounit
        INFO_PLIST_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/resource/au-info.plist 
        VST3_PLUGIN_TARGET  AudioDSPSynth
    )
    set_target_properties(AudioDSPSynth_AU PROPERTIES CXX_STANDARD 23)
endif()