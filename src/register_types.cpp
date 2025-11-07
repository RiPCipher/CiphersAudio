/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#include "register_types.h"

#include "core/object/class_db.h"
#include "fft_buffer.h"
#include "fft_processor.h"

void initialize_pffft_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// Register FFT classes
	GDREGISTER_CLASS(FFTBuffer);
	GDREGISTER_CLASS(FFTProcessor);
}

void uninitialize_pffft_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// Cleanup if needed
}