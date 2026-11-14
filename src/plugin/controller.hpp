#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace AudioDSP {
class SynthController : public Steinberg::Vst::EditControllerEx1 {
public:
  SynthController() = default;
  ~SynthController() SMTG_OVERRIDE = default;

  static Steinberg::FUnknown *createInstance(void *) {
    return (Steinberg::Vst::IEditController *)new SynthController();
  }

  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown *context)
      SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API terminate() SMTG_OVERRIDE;

  Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream *state)
      SMTG_OVERRIDE;
  Steinberg::IPlugView *PLUGIN_API createView(Steinberg::FIDString name)
      SMTG_OVERRIDE;
};
} // namespace AudioDSP
#endif