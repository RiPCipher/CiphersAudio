/**************************************************************************/
/*  register_types.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#ifndef PFFFT_REGISTER_TYPES_H
#define PFFFT_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_pffft_module(ModuleInitializationLevel p_level);
void uninitialize_pffft_module(ModuleInitializationLevel p_level);

#endif // PFFFT_REGISTER_TYPES_H