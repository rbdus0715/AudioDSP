#include "cids.h"
#include "controller.hpp"
#include "processor.hpp"
#include "version.h"

#include "public.sdk/source/main/pluginfactory.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

BEGIN_FACTORY_DEF("rbdus0715", "https://github.com/rbdus0715",
                  "mailto:rbdus0715@gmail.com")

// ---- Processor (소리 만드는 쪽) ----
DEF_CLASS2(INLINE_UID_FROM_FUID(AudioDSP::kSynthProcessorUID),
           PClassInfo::kManyInstances, // 트랙 여러 개에 꽂을 수 있음
           kVstAudioEffectClass,
           "AudioDSP Synth", // DAW 목록에 보일 이름
           Vst::kDistributable, AudioDSPSynthVST3Category, FULL_VERSION_STR,
           kVstVersionString, AudioDSP::SynthProcessor::createInstance)

// ---- Controller (파라미터/UI 쪽) ----
DEF_CLASS2(INLINE_UID_FROM_FUID(AudioDSP::kSynthControllerUID),
           PClassInfo::kManyInstances,
           kVstComponentControllerClass, // controller
           "AudioDSP Synth Controller",
           0,  // Controller는 안 씀
           "", // Controller는 안 씀
           FULL_VERSION_STR, kVstVersionString,
           AudioDSP::SynthController::createInstance) // ⑤

END_FACTORY