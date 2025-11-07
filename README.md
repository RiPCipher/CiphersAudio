# CiphersAudio
Audio Extension for Godot

<img width="1153" height="646" alt="FFT" src="https://github.com/user-attachments/assets/7e4e9fba-02a9-47a6-8751-f53285e80c8b" />


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
