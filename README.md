# NeuralAmp (JUCE)

Emulador de amplificador de guitarra estilo Neural DSP (C++/JUCE). Incluye GUI moderna con perillas (Gain, Bass, Mid, Treble, Presence, Volume, Master), bypass, medidores de entrada/salida y presets. Se ejecuta como Standalone y Plugin (VST3, AU).

## Requisitos
- CMake >= 3.21
- Compilador C++17 (MSVC 2022 / Xcode 14+ / clang-15+)
- JUCE se descarga automáticamente vía FetchContent
- Windows: (opcional) ASIO SDK para soporte ASIO en el Standalone
  - Colocar el SDK en `JUCE/extras/SDKs/ASIO` o establecer `JUCE_ASIO_PATH`

## Compilación

### macOS (AU + VST3 + Standalone)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
```
- Standalone: `build/NeuralAmp_artefacts/Release/Standalone/NeuralAmp.app`
- VST3: `build/NeuralAmp_artefacts/Release/VST3/NeuralAmp.vst3`
- AU: `build/NeuralAmp_artefacts/Release/AU/NeuralAmp.component`

Instalación rápida:
```bash
# VST3
mkdir -p ~/Library/Audio/Plug-Ins/VST3
cp -R build/NeuralAmp_artefacts/Release/VST3/NeuralAmp.vst3 ~/Library/Audio/Plug-Ins/VST3/
# AU
mkdir -p ~/Library/Audio/Plug-Ins/Components
cp -R build/NeuralAmp_artefacts/Release/AU/NeuralAmp.component ~/Library/Audio/Plug-Ins/Components/
```

### Windows (VST3 + Standalone)
```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
```
Artefactos:
- Standalone: `build/NeuralAmp_artefacts/Release/Standalone/NeuralAmp.exe`
- VST3: `build/NeuralAmp_artefacts/Release/VST3/NeuralAmp.vst3`

Instalación VST3 (usuario):
```
%APPDATA%\Common\VST3\
```
Copie `NeuralAmp.vst3` a esa carpeta.

ASIO: compile con `-DNEURAL_AMP_USE_SYSTEM_JUCE=OFF` (por defecto). Para ASIO, coloque el SDK en `JUCE/extras/SDKs/ASIO/` antes de compilar. Si no está disponible, el Standalone usará WASAPI.

## Uso
- Conecte su guitarra a la interfaz de audio
- Ejecute el Standalone o cargue el plugin en su DAW
- Ajuste `Gain`, ecualización (`Bass/Mid/Treble/Presence`), `Volume` y `Master`
- `Bypass` desactiva el amp
- `Save Preset` y `Load Preset` guardan/cargan el estado (`.neuralamp`)

## Latencia
- El DSP usa sobremuestreo x4 con saturación suave y ecualizador básico; en hardware moderno mantiene <10 ms en buffer pequeño. Ajuste el tamaño de buffer en el dispositivo de audio.

## Estructura
- `src/` GUI, Processor, Engine y DSP simple (triode waveshaper + tone stack)
- `CMakeLists.txt` configura JUCE (FetchContent), Standalone, AU, VST3