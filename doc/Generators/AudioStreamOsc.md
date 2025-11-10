## AudioStreamOsc

A basic oscillator that generates sine, sawtooth, and square waveforms.


### Usage in GDScript

```gdscript
# Create the oscillator
var osc = AudioStreamOsc.new()

# Configure it
osc.waveform_type = AudioStreamOsc.WAVEFORM_SINE
osc.frequency = 440.0  # A440
osc.amplitude_db = -6.0  # About half volume

# Use it with an AudioStreamPlayer
var player = AudioStreamPlayer.new()
player.stream = osc
player.play()

# Change parameters at runtime
osc.waveform_type = AudioStreamOsc.WAVEFORM_SAW
osc.frequency = 880.0  # One octave up
```

### Technical Details

**Architecture:**
- `AudioStreamOsc` - Resource class (inherits from `AudioStream`)
- `AudioStreamPlaybackOsc` - Generator class (inherits from `AudioStreamPlayback`)

**Waveform Generation:**
- **Sine:** Pure sine wave using `sin(2π * phase)`
- **Sawtooth:** Linear ramp from -1 to 1
- **Square:** Step function alternating between 1 and -1

**Phase Management:**
- Uses double precision to prevent phase accumulation errors
- Phase wraps continuously from 0.0 to 1.0
- Phase increment calculated as `frequency / sample_rate`

**Audio Output:**
- Generates mono signal
- Duplicated to stereo channels for compatibility
- Integrates with Godot's AudioServer for mixing and effects