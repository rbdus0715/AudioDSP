#include "controller.hpp"
#include "cids.h"

using namespace Steinberg;

namespace AudioDSP {

tresult PLUGIN_API SynthController::initialize(FUnknown *context) {
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk)
    return result;

  parameters.addParameter(new Vst::RangeParameter(STR16("Volume"), kParamVolume,
                                                  STR16(""), 0, 1.0, 0.2));
  parameters.addParameter(new Vst::RangeParameter(STR16("Attack"), kParamAttack,
                                                  STR16("s"), 1e-9, 1.0, 0.01));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Release"), kParamRelease, STR16("s"), 1e-9, 1.0, 0.2));

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