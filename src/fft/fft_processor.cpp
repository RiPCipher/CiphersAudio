/**************************************************************************/
/*  fft_processor.cpp                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#include "fft_processor.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "pffft.h"
#include <cmath>
#include <cstring>

FFTProcessor::FFTProcessor() {
}

FFTProcessor::~FFTProcessor() {
	_cleanup();
}

void FFTProcessor::_cleanup() {
	if (setup != nullptr) {
		pffft_destroy_setup(setup);
		setup = nullptr;
	}

	if (work_buffer != nullptr) {
		pffft_aligned_free(work_buffer);
		work_buffer = nullptr;
	}

	fft_size = 0;
}

bool FFTProcessor::_validate_size(int p_size) const {
	int min_size = (transform_type == TRANSFORM_REAL) ? 32 : 16;
	if (p_size < min_size) {
		return false;
	}

	// Check if size is valid for pffft (must be factorable by 2, 3, 5)
	return pffft_is_valid_size(p_size, (pffft_transform_t)transform_type);
}

Error FFTProcessor::setup_fft(int p_size, TransformType p_type) {
	_cleanup();

	ERR_FAIL_COND_V(p_size <= 0, ERR_INVALID_PARAMETER);

	transform_type = p_type;

	if (!_validate_size(p_size)) {
		UtilityFunctions::printerr("Invalid FFT size ", p_size, ". Must be >= ", 
			(p_type == TRANSFORM_REAL) ? 32 : 16, " and factorable by 2, 3, 5.");
		return ERR_INVALID_PARAMETER;
	}

	// Create pffft setup
	setup = pffft_new_setup(p_size, (pffft_transform_t)transform_type);
	ERR_FAIL_NULL_V(setup, ERR_CANT_CREATE);

	fft_size = p_size;

	// Allocate work buffer
	work_buffer = (float *)pffft_aligned_malloc(p_size * sizeof(float));
	ERR_FAIL_NULL_V(work_buffer, ERR_OUT_OF_MEMORY);

	return OK;
}

PackedVector2Array FFTProcessor::forward_real(const PackedFloat32Array &p_input) {
	PackedVector2Array result;

	ERR_FAIL_COND_V(!is_valid(), result);
	ERR_FAIL_COND_V(transform_type != TRANSFORM_REAL, result);
	ERR_FAIL_COND_V(p_input.size() != fft_size, result);

	// Allocate input and output buffers
	float *input_buffer = (float *)pffft_aligned_malloc(fft_size * sizeof(float));
	float *output_buffer = (float *)pffft_aligned_malloc(fft_size * sizeof(float));

	// Copy input data
	const float *input_data = p_input.ptr();
	memcpy(input_buffer, input_data, fft_size * sizeof(float));

	// Perform FFT
	pffft_transform_ordered(setup, input_buffer, output_buffer, work_buffer, PFFFT_FORWARD);

	// Convert output to PackedVector2Array (complex numbers)
	// For real FFT, output format is: [DC, N/2, Re(1), Im(1), Re(2), Im(2), ...]
	int spectrum_size = fft_size / 2 + 1;
	result.resize(spectrum_size);

	// DC component (index 0) - purely real
	result.set(0, Vector2(output_buffer[0], 0.0f));

	// Nyquist component (index N/2) - purely real
	result.set(spectrum_size - 1, Vector2(output_buffer[1], 0.0f));

	// All other components
	for (int i = 1; i < spectrum_size - 1; i++) {
		float real = output_buffer[i * 2];
		float imag = output_buffer[i * 2 + 1];
		result.set(i, Vector2(real, imag));
	}

	// Cleanup
	pffft_aligned_free(input_buffer);
	pffft_aligned_free(output_buffer);

	return result;
}

PackedFloat32Array FFTProcessor::inverse_real(const PackedVector2Array &p_spectrum) {
	PackedFloat32Array result;

	ERR_FAIL_COND_V(!is_valid(), result);
	ERR_FAIL_COND_V(transform_type != TRANSFORM_REAL, result);

	int expected_spectrum_size = fft_size / 2 + 1;
	ERR_FAIL_COND_V(p_spectrum.size() != expected_spectrum_size, result);

	// Allocate input and output buffers
	float *input_buffer = (float *)pffft_aligned_malloc(fft_size * sizeof(float));
	float *output_buffer = (float *)pffft_aligned_malloc(fft_size * sizeof(float));

	// Convert PackedVector2Array to pffft format
	// Format: [DC, N/2, Re(1), Im(1), Re(2), Im(2), ...]
	input_buffer[0] = p_spectrum[0].x; // DC (real only)
	input_buffer[1] = p_spectrum[expected_spectrum_size - 1].x; // Nyquist (real only)

	for (int i = 1; i < expected_spectrum_size - 1; i++) {
		input_buffer[i * 2] = p_spectrum[i].x; // Real part
		input_buffer[i * 2 + 1] = p_spectrum[i].y; // Imaginary part
	}

	// Perform inverse FFT
	pffft_transform_ordered(setup, input_buffer, output_buffer, work_buffer, PFFFT_BACKWARD);

	// Copy output and scale by 1/N (pffft doesn't scale)
	result.resize(fft_size);
	float scale = 1.0f / fft_size;
	for (int i = 0; i < fft_size; i++) {
		result.set(i, output_buffer[i] * scale);
	}

	// Cleanup
	pffft_aligned_free(input_buffer);
	pffft_aligned_free(output_buffer);

	return result;
}

void FFTProcessor::forward_real_buffer(const Ref<FFTBuffer> &p_input, const Ref<FFTBuffer> &p_output) {
	ERR_FAIL_COND(!is_valid());
	ERR_FAIL_COND(transform_type != TRANSFORM_REAL);
	ERR_FAIL_COND(p_input.is_null());
	ERR_FAIL_COND(p_output.is_null());
	ERR_FAIL_COND(p_input->get_size() != fft_size);
	ERR_FAIL_COND(p_output->get_size() != fft_size);

	// Perform FFT directly on buffers
	pffft_transform_ordered(setup,
			p_input->get_buffer_ptr(),
			p_output->get_buffer_ptr(),
			work_buffer,
			PFFFT_FORWARD);
}

void FFTProcessor::inverse_real_buffer(const Ref<FFTBuffer> &p_input, const Ref<FFTBuffer> &p_output) {
	ERR_FAIL_COND(!is_valid());
	ERR_FAIL_COND(transform_type != TRANSFORM_REAL);
	ERR_FAIL_COND(p_input.is_null());
	ERR_FAIL_COND(p_output.is_null());
	ERR_FAIL_COND(p_input->get_size() != fft_size);
	ERR_FAIL_COND(p_output->get_size() != fft_size);

	// Perform inverse FFT
	pffft_transform_ordered(setup,
			p_input->get_buffer_ptr(),
			p_output->get_buffer_ptr(),
			work_buffer,
			PFFFT_BACKWARD);

	// Scale by 1/N
	float scale = 1.0f / fft_size;
	float *output_ptr = p_output->get_buffer_ptr();
	for (int i = 0; i < fft_size; i++) {
		output_ptr[i] *= scale;
	}
}

PackedFloat32Array FFTProcessor::get_magnitude_spectrum(const PackedVector2Array &p_spectrum) {
	PackedFloat32Array result;
	int size = p_spectrum.size();
	result.resize(size);

	for (int i = 0; i < size; i++) {
		Vector2 complex = p_spectrum[i];
		float magnitude = sqrt(complex.x * complex.x + complex.y * complex.y);
		result.set(i, magnitude);
	}

	return result;
}

PackedFloat32Array FFTProcessor::get_phase_spectrum(const PackedVector2Array &p_spectrum) {
	PackedFloat32Array result;
	int size = p_spectrum.size();
	result.resize(size);

	for (int i = 0; i < size; i++) {
		Vector2 complex = p_spectrum[i];
		float phase = atan2(complex.y, complex.x);
		result.set(i, phase);
	}

	return result;
}

PackedFloat32Array FFTProcessor::get_power_spectrum(const PackedVector2Array &p_spectrum) {
	PackedFloat32Array result;
	int size = p_spectrum.size();
	result.resize(size);

	for (int i = 0; i < size; i++) {
		Vector2 complex = p_spectrum[i];
		float power = complex.x * complex.x + complex.y * complex.y;
		result.set(i, power);
	}

	return result;
}

int FFTProcessor::get_spectrum_size() const {
	if (!is_valid()) {
		return 0;
	}

	if (transform_type == TRANSFORM_REAL) {
		return fft_size / 2 + 1;
	} else {
		return fft_size;
	}
}

// Static utility functions
bool FFTProcessor::is_valid_fft_size(int p_size, TransformType p_type) {
	int min_size = (p_type == TRANSFORM_REAL) ? 32 : 16;
	if (p_size < min_size) {
		return false;
	}
	return pffft_is_valid_size(p_size, (pffft_transform_t)p_type);
}

int FFTProcessor::get_nearest_valid_size(int p_size, TransformType p_type, bool p_higher) {
	return pffft_nearest_transform_size(p_size, (pffft_transform_t)p_type, p_higher ? 1 : 0);
}

int FFTProcessor::get_minimum_fft_size(TransformType p_type) {
	return (p_type == TRANSFORM_REAL) ? 32 : 16;
}

void FFTProcessor::_bind_methods() {
	// Setup
	ClassDB::bind_method(D_METHOD("setup_fft", "size", "type"), &FFTProcessor::setup_fft, DEFVAL(TRANSFORM_REAL));
	ClassDB::bind_method(D_METHOD("is_valid"), &FFTProcessor::is_valid);
	ClassDB::bind_method(D_METHOD("get_fft_size"), &FFTProcessor::get_fft_size);
	ClassDB::bind_method(D_METHOD("get_transform_type"), &FFTProcessor::get_transform_type);

	// Core FFT operations
	ClassDB::bind_method(D_METHOD("forward_real", "input"), &FFTProcessor::forward_real);
	ClassDB::bind_method(D_METHOD("inverse_real", "spectrum"), &FFTProcessor::inverse_real);

	// Buffer operations
	ClassDB::bind_method(D_METHOD("forward_real_buffer", "input", "output"), &FFTProcessor::forward_real_buffer);
	ClassDB::bind_method(D_METHOD("inverse_real_buffer", "input", "output"), &FFTProcessor::inverse_real_buffer);

	// Utility functions
	ClassDB::bind_method(D_METHOD("get_magnitude_spectrum", "spectrum"), &FFTProcessor::get_magnitude_spectrum);
	ClassDB::bind_method(D_METHOD("get_phase_spectrum", "spectrum"), &FFTProcessor::get_phase_spectrum);
	ClassDB::bind_method(D_METHOD("get_power_spectrum", "spectrum"), &FFTProcessor::get_power_spectrum);
	ClassDB::bind_method(D_METHOD("get_spectrum_size"), &FFTProcessor::get_spectrum_size);

	// Static functions
	ClassDB::bind_static_method("FFTProcessor", D_METHOD("is_valid_fft_size", "size", "type"), &FFTProcessor::is_valid_fft_size, DEFVAL(TRANSFORM_REAL));
	ClassDB::bind_static_method("FFTProcessor", D_METHOD("get_nearest_valid_size", "size", "type", "higher"), &FFTProcessor::get_nearest_valid_size, DEFVAL(TRANSFORM_REAL), DEFVAL(true));
	ClassDB::bind_static_method("FFTProcessor", D_METHOD("get_minimum_fft_size", "type"), &FFTProcessor::get_minimum_fft_size, DEFVAL(TRANSFORM_REAL));

	// Enums
	BIND_ENUM_CONSTANT(TRANSFORM_REAL);
	BIND_ENUM_CONSTANT(TRANSFORM_COMPLEX);

	BIND_ENUM_CONSTANT(FORWARD);
	BIND_ENUM_CONSTANT(INVERSE);

	// Properties
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fft_size"), "", "get_fft_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "transform_type", PROPERTY_HINT_ENUM, "Real,Complex"), "", "get_transform_type");
}