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

smtg_target_configure_version_file(AudioDSPSynth)

if(SMTG_MAC)
    smtg_target_set_bundle(AudioDSPSynth
        BUNDLE_IDENTIFIER com.rbdus0715.audiodspsynth
        COMPANY_NAME "rbdus0715"
    )
endif()
