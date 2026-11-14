#!/bin/sh
set -e
cd "$(dirname "$0")/.."
cmake --build build-xcode --target AudioDSPSynth editorhost --config Debug -- -quiet
exec build-xcode/bin/Debug/editorhost.app/Contents/MacOS/editorhost \
    build-xcode/VST3/Debug/AudioDSPSynth.vst3
