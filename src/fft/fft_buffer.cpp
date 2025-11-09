/**************************************************************************/
/*  fft_buffer.cpp                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#include "fft_buffer.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cstring>

// Include pffft for aligned memory allocation
#include "pffft.h"

FFTBuffer::FFTBuffer() {
}

FFTBuffer::~FFTBuffer() {
	_deallocate();
}

void FFTBuffer::_allocate(int p_size) {
	_deallocate();

	if (p_size <= 0) {
		return;
	}

	// Use pffft's aligned allocation
	buffer = (float *)pffft_aligned_malloc(p_size * sizeof(float));
	size = p_size;
	is_aligned = true;

	// Initialize to zero
	memset(buffer, 0, p_size * sizeof(float));
}

void FFTBuffer::_deallocate() {
	if (buffer) {
		if (is_aligned) {
			pffft_aligned_free(buffer);
		}
		buffer = nullptr;
		size = 0;
		is_aligned = false;
	}
}

void FFTBuffer::resize(int p_size) {
	if (p_size == size) {
		return;
	}

	_allocate(p_size);
}

void FFTBuffer::set_data(const PackedFloat32Array &p_data) {
	int data_size = p_data.size();
	if (data_size != size) {
		resize(data_size);
	}

	if (buffer && data_size > 0) {
		const float *data_ptr = p_data.ptr();
		memcpy(buffer, data_ptr, data_size * sizeof(float));
	}
}

PackedFloat32Array FFTBuffer::get_data() const {
	PackedFloat32Array result;
	if (buffer && size > 0) {
		result.resize(size);
		float *result_ptr = result.ptrw();
		memcpy(result_ptr, buffer, size * sizeof(float));
	}
	return result;
}

void FFTBuffer::clear() {
	if (buffer && size > 0) {
		memset(buffer, 0, size * sizeof(float));
	}
}

void FFTBuffer::fill(float p_value) {
	if (buffer) {
		for (int i = 0; i < size; i++) {
			buffer[i] = p_value;
		}
	}
}

float FFTBuffer::get_value(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, size, 0.0f);
	return buffer[p_index];
}

void FFTBuffer::set_value(int p_index, float p_value) {
	ERR_FAIL_INDEX(p_index, size);
	buffer[p_index] = p_value;
}

void FFTBuffer::_bind_methods() {
	// Buffer management
	ClassDB::bind_method(D_METHOD("resize", "size"), &FFTBuffer::resize);
	ClassDB::bind_method(D_METHOD("get_size"), &FFTBuffer::get_size);
	ClassDB::bind_method(D_METHOD("is_allocated"), &FFTBuffer::is_allocated);

	// Data access
	ClassDB::bind_method(D_METHOD("set_data", "data"), &FFTBuffer::set_data);
	ClassDB::bind_method(D_METHOD("get_data"), &FFTBuffer::get_data);
	ClassDB::bind_method(D_METHOD("clear"), &FFTBuffer::clear);

	// Utility functions
	ClassDB::bind_method(D_METHOD("fill", "value"), &FFTBuffer::fill);
	ClassDB::bind_method(D_METHOD("get_value", "index"), &FFTBuffer::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "index", "value"), &FFTBuffer::set_value);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "size"), "", "get_size");
}