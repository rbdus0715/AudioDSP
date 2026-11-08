#include "controller.hpp"
#include "cids.h"

using namespace Steinberg;

namespace AudioDSP {

tresult PLUGIN_API SynthController::initialize(FUnknown *context) {
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk)
    return result;

  parameters.addParameter(new Vst::RangeParameter(
      STR16("Volume"), kParamVolume, STR16(""), kVolumeRange.min,
      kVolumeRange.max, kVolumeRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Attack"), kParamAttack, STR16("s"), kAttackRange.min,
      kAttackRange.max, kAttackRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Release"), kParamRelease, STR16("s"), kReleaseRange.min,
      kReleaseRange.max, kReleaseRange.def));

  return kResultOk;
}

tresult PLUGIN_API SynthController::terminate() {
  return EditControllerEx1::terminate();
}

tresult PLUGIN_API SynthController::setComponentState(IBStream *state) {
  if (!state)
    return kResultFalse;
  return kResultOk;
}

} // namespace AudioDSP