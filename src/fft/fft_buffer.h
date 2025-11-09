/**************************************************************************/
/*  fft_buffer.h                                                          */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#ifndef FFT_BUFFER_H
#define FFT_BUFFER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

class FFTBuffer : public RefCounted {
	GDCLASS(FFTBuffer, RefCounted);

private:
	float *buffer = nullptr;
	int size = 0;
	bool is_aligned = false;

	void _allocate(int p_size);
	void _deallocate();

protected:
	static void _bind_methods();

public:
	FFTBuffer();
	~FFTBuffer();

	// Buffer management
	void resize(int p_size);
	int get_size() const { return size; }
	bool is_allocated() const { return buffer != nullptr; }

	// Data access
	void set_data(const PackedFloat32Array &p_data);
	PackedFloat32Array get_data() const;
	void clear();

	// Direct access (for internal use by FFTProcessor)
	float *get_buffer_ptr() { return buffer; }
	const float *get_buffer_ptr() const { return buffer; }

	// Utility functions
	void fill(float p_value);
	float get_value(int p_index) const;
	void set_value(int p_index, float p_value);
};

#endif // FFT_BUFFER_H