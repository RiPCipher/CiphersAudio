# CiphersAudio
Audio Extension for Godot


Currently Features:
- FFTProcessor Class
- FFTBuffer Class

# Going Forward
Near Goals:
- Wrap pffft to be usable in GDScript
- Create AudioStreamAdditive to enable additive synthesis within the engine
- Implement an Audio Manager to simplfy audio management and usage throughout scenes
- Implement an "AudioZone3D" node to enable simpler audio effects management witihn/throughout a scene

Far Goals:
- Implement a proper convolution reverb effect that utilizes provided impulse response (and ideally could be modified at runtime using the AudioZone node)
- TBD
