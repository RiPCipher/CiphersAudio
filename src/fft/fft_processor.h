/**************************************************************************/
/*  fft_processor.h                                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#ifndef FFT_PROCESSOR_H
#define FFT_PROCESSOR_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>

#include "fft_buffer.h"

// Forward declare pffft types
struct PFFFT_Setup;
typedef struct PFFFT_Setup PFFFT_Setup;

using namespace godot;

class FFTProcessor : public RefCounted {
	GDCLASS(FFTProcessor, RefCounted);

public:
	enum TransformType {
		TRANSFORM_REAL = 0,
		TRANSFORM_COMPLEX = 1
	};

	enum Direction {
		FORWARD = 0,
		INVERSE = 1
	};

private:
	PFFFT_Setup *setup = nullptr;
	float *work_buffer = nullptr;
	int fft_size = 0;
	TransformType transform_type = TRANSFORM_REAL;

	void _cleanup();
	bool _validate_size(int p_size) const;

protected:
	static void _bind_methods();

public:
	FFTProcessor();
	~FFTProcessor();

	// Setup
	Error setup_fft(int p_size, TransformType p_type = TRANSFORM_REAL);
	bool is_valid() const { return setup != nullptr; }
	int get_fft_size() const { return fft_size; }
	TransformType get_transform_type() const { return transform_type; }

	// Core FFT operations
	PackedVector2Array forward_real(const PackedFloat32Array &p_input);
	PackedFloat32Array inverse_real(const PackedVector2Array &p_spectrum);

	// Buffer operations (more efficient for repeated use)
	void forward_real_buffer(const Ref<FFTBuffer> &p_input, const Ref<FFTBuffer> &p_output);
	void inverse_real_buffer(const Ref<FFTBuffer> &p_input, const Ref<FFTBuffer> &p_output);

	// Utility functions
	PackedFloat32Array get_magnitude_spectrum(const PackedVector2Array &p_spectrum);
	PackedFloat32Array get_phase_spectrum(const PackedVector2Array &p_spectrum);
	PackedFloat32Array get_power_spectrum(const PackedVector2Array &p_spectrum);
	int get_spectrum_size() const;

	// Static utility functions
	static bool is_valid_fft_size(int p_size, TransformType p_type = TRANSFORM_REAL);
	static int get_nearest_valid_size(int p_size, TransformType p_type = TRANSFORM_REAL, bool p_higher = true);
	static int get_minimum_fft_size(TransformType p_type = TRANSFORM_REAL);
};

VARIANT_ENUM_CAST(FFTProcessor::TransformType);
VARIANT_ENUM_CAST(FFTProcessor::Direction);

#endif // FFT_PROCESSOR_H