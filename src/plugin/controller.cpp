#include "controller.hpp"

using namespace Steinberg;

namespace AudioDSP {

tresult PLUGIN_API SynthController::initialize(FUnknown *context) {
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk)
    return result;

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