#include "processor.hpp"
#include "cids.h"

using namespace Steinberg;

namespace AudioDSP {

SynthProcessor::SynthProcessor() {
  // 이 Processor와 짝이 되는 Controller가 누구인지 알려줌
  setControllerClass(kSynthControllerUID);
}

tresult PLUGIN_API SynthProcessor::initialize(FUnknown *context) {
  tresult result = AudioEffect::initialize(context);
  if (result != kResultOk)
    return result;

  addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);
  addEventInput(STR16("Event In"), 1);

  return kResultOk;
}

tresult PLUGIN_API SynthProcessor::setupProcessing(Vst::ProcessSetup &setup) {
  // setup.sampleRate          : 샘플레이트 (double)
  // setup.maxSamplesPerBlock  : process() 한 번에 올 수 있는 최대 샘플 수
  m_synth = std::make_unique<Synth>(static_cast<int>(setup.sampleRate));
  m_buffer.SetSize(2, setup.maxSamplesPerBlock);

  return AudioEffect::setupProcessing(setup);
}

tresult PLUGIN_API
SynthProcessor::canProcessSampleSize(int32 symbolicSampleSize) {
  return symbolicSampleSize == Vst::kSample32 ? kResultTrue : kResultFalse;
}

tresult PLUGIN_API SynthProcessor::process(Vst::ProcessData &data) {
  // TODO(3-2b): 노트 이벤트 처리 + 소리 출력
  return kResultOk;
}

tresult PLUGIN_API SynthProcessor::setState(IBStream *) { return kResultOk; }
tresult PLUGIN_API SynthProcessor::getState(IBStream *) { return kResultOk; }

} // namespace AudioDSP