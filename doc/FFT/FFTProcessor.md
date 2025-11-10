# FFTProcessor & FFTBuffer

A Fast Fourier Transform processor for frequency analysis and spectral processing. Uses the optimized PFFFT library for high-performance FFT operations.

## Overview

The FFT system consists of two main classes:
- **FFTProcessor** - Handles FFT transforms and spectral analysis
- **FFTBuffer** - Manages aligned memory buffers for efficient FFT operations

## Usage in GDScript

### Basic FFT Analysis

```gdscript
# Create and configure the FFT processor
var fft = FFTProcessor.new()
var buffer_size = 2048

# Setup for real-valued audio (most common use case)
fft.setup_fft(buffer_size, FFTProcessor.TRANSFORM_REAL)

# Prepare your audio samples
var audio_samples = PackedFloat32Array()
audio_samples.resize(buffer_size)
# ... fill with audio data ...

# Perform forward FFT (time domain -> frequency domain)
var spectrum = fft.forward_real(audio_samples)

# Get magnitude spectrum for visualization
var magnitudes = fft.get_magnitude_spectrum(spectrum)

# Process the spectrum...
# You now have frequency bins from DC to Nyquist frequency

# Convert back to time domain if needed
var output = fft.inverse_real(spectrum)
```

### Using FFTBuffer for Repeated Processing

```gdscript
# For better performance with repeated FFT operations, use FFTBuffer
var fft = FFTProcessor.new()
fft.setup_fft(2048, FFTProcessor.TRANSFORM_REAL)

# Create reusable buffers
var input_buffer = FFTBuffer.new()
var output_buffer = FFTBuffer.new()
input_buffer.resize(2048)
output_buffer.resize(2048)

# Fill input buffer with audio data
input_buffer.set_data(audio_samples)

# Perform FFT using buffers (more efficient)
fft.forward_real_buffer(input_buffer, output_buffer)

# Extract results
var spectrum_data = output_buffer.get_data()
```

### Spectral Analysis

```gdscript
var fft = FFTProcessor.new()
fft.setup_fft(2048, FFTProcessor.TRANSFORM_REAL)

# Perform FFT
var spectrum = fft.forward_real(audio_samples)

# Get different spectral representations
var magnitudes = fft.get_magnitude_spectrum(spectrum)  # |H(f)|
var phases = fft.get_phase_spectrum(spectrum)          # ∠H(f)
var power = fft.get_power_spectrum(spectrum)           # |H(f)|²

# Spectrum size for real FFT is N/2 + 1
var spectrum_size = fft.get_spectrum_size()  # Returns 1025 for 2048-point FFT

# Each bin represents a frequency range
var sample_rate = 44100.0
var bin_width = sample_rate / fft.get_fft_size()  # Frequency resolution
```

### Choosing FFT Size

```gdscript
# Check if a size is valid
if FFTProcessor.is_valid_fft_size(2048, FFTProcessor.TRANSFORM_REAL):
    print("Valid FFT size")

# Get nearest valid size
var desired_size = 2000
var valid_size = FFTProcessor.get_nearest_valid_size(desired_size, FFTProcessor.TRANSFORM_REAL, true)
print("Using FFT size: ", valid_size)  # Returns 2048

# Get minimum size for transform type
var min_size = FFTProcessor.get_minimum_fft_size(FFTProcessor.TRANSFORM_REAL)  # Returns 32
```

## Technical Details

### Architecture

**FFTProcessor:**
- Resource class (inherits from `RefCounted`)
- Wraps PFFFT library for optimized transforms
- Supports both real and complex transforms
- Provides spectral analysis utilities

**FFTBuffer:**
- Resource class (inherits from `RefCounted`)
- Manages SIMD-aligned memory buffers
- Reduces allocation overhead for repeated operations

### Transform Types

**TRANSFORM_REAL (0):**
- Input: Real-valued samples (audio signal)
- Output: Complex spectrum (N/2 + 1 bins)
- Minimum size: 32 samples
- More efficient than complex transform (ie, usually better for audio)

**TRANSFORM_COMPLEX (1):**
- Input: Complex samples (N points)
- Output: Complex spectrum (N bins)
- Minimum size: 16 samples

### FFT Size Requirements
Valid FFT sizes must be factorable by 2, 3, and 5:
- Valid: 32, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 2048, 4096, etc.
- Invalid: 100, 127, 200, 1000

**Choosing FFT Size:**
- Larger size = better frequency resolution, worse time resolution
- Smaller size = better time resolution, worse frequency resolution
- Common sizes for audio: 512, 1024, 2048, 4096
- Must be at least 32 for real transforms, 16 for complex

### Frequency Mapping
For real FFT with sample rate `fs` and size `N`:

```gdscript
var spectrum = fft.forward_real(audio)  # Returns N/2 + 1 bins
var sample_rate = 44100.0
var bin_width = sample_rate / fft.get_fft_size()

# Bin indices map to frequencies:
# bin[0]     -> 0 Hz (DC component)
# bin[1]     -> bin_width Hz
# bin[k]     -> k * bin_width Hz
# bin[N/2]   -> sample_rate/2 Hz (Nyquist frequency)
```

### Spectrum Format
The `forward_real()` output is a `PackedVector2Array` where each Vector2 represents a complex number:
- `x` component = Real part
- `y` component = Imaginary part

```gdscript
var spectrum = fft.forward_real(audio)
for i in spectrum.size():
    var complex_value = spectrum[i]
    var real_part = complex_value.x
    var imag_part = complex_value.y
    var magnitude = sqrt(real_part * real_part + imag_part * imag_part)
    var phase = atan2(imag_part, real_part)
```

### Spectral Utilities

**Magnitude Spectrum:**
- `get_magnitude_spectrum(spectrum)` returns `|H(f)|`
- Linear amplitude at each frequency
- Good for visualization and peak detection

**Power Spectrum:**
- `get_power_spectrum(spectrum)` returns `|H(f)|²`
- Energy content at each frequency
- Used in spectral analysis and filtering

**Phase Spectrum:**
- `get_phase_spectrum(spectrum)` returns `∠H(f)` in radians
- Phase information for each frequency
- Important for reconstruction and time-domain relationships

### Memory Management
```gdscript
# One-time setup
var input_buf = FFTBuffer.new()
input_buf.resize(2048)

# Reuse in processing loop
func process_audio_block(samples: PackedFloat32Array):
    input_buf.set_data(samples)
    fft.forward_real_buffer(input_buf, output_buf)
    # Process output_buf...
```

### Performance Considerations
**Best Practices:**
- Use FFTBuffer for repeated operations
- Choose power-of-2 sizes when possible (fastest)
- Reuse FFTProcessor instances
- Avoid frequent setup/teardown

### Integration with Godot

**Audio Processing Pipeline:**
```gdscript
# Example: Real-time spectrum analyzer
var audio_effect = AudioServer.get_bus_effect(0, 0)  # Get spectrum analyzer
var fft = FFTProcessor.new()
fft.setup_fft(2048, FFTProcessor.TRANSFORM_REAL)

func _process(_delta):
    # Get audio data from Godot's AudioEffectSpectrumAnalyzer
    var magnitude_data = []
    for i in range(fft.get_spectrum_size()):
        var freq = i * AudioServer.get_mix_rate() / fft.get_fft_size()
        var magnitude = audio_effect.get_magnitude_for_frequency_range(freq, freq)
        magnitude_data.append(magnitude)
```


## Example

### Spectrum Visualizer

```gdscript
extends Node2D

var fft = FFTProcessor.new()
var audio_samples = PackedFloat32Array()
var fft_size = 2048

func _ready():
    fft.setup_fft(fft_size, FFTProcessor.TRANSFORM_REAL)
    audio_samples.resize(fft_size)

func _process(_delta):
    # Capture audio data (implementation depends on audio source)
    # audio_samples = get_audio_buffer()
    
    var spectrum = fft.forward_real(audio_samples)
    var magnitudes = fft.get_magnitude_spectrum(spectrum)
    
    # Draw spectrum bars
    queue_redraw()

func _draw():
    var spectrum = fft.forward_real(audio_samples)
    var magnitudes = fft.get_magnitude_spectrum(spectrum)
    
    var bar_width = get_viewport_rect().size.x / magnitudes.size()
    for i in range(magnitudes.size()):
        var height = magnitudes[i] * 100.0  # Scale for visualization
        var color = Color(1.0, 0.5, 0.0)
        draw_rect(Rect2(i * bar_width, 300 - height, bar_width - 2, height), color)
```

## See Also
- PFFFT library documentation: `thirdparty/pffft/README.md`