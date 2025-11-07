/**************************************************************************/
/*  register_types.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#ifndef CIPHERSAUDIO_REGISTER_TYPES_H
#define CIPHERSAUDIO_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_pffft_module(ModuleInitializationLevel p_level);
void uninitialize_pffft_module(ModuleInitializationLevel p_level);

#endif // CIPHERSAUDIO_REGISTER_TYPES_H