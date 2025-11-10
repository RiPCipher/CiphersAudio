/**************************************************************************/
/*  audio_stream_osc.cpp                                                  */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             CIPHERS AUDIO                              */
/*                        https://github.com/RiPCipher/CiphersAudio       */
/**************************************************************************/

#include "audio_stream_osc.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

// AudioStreamPlaybackOsc Implementation

AudioStreamPlaybackOsc::AudioStreamPlaybackOsc() {
	phase = 0.0;
	sample_rate = AudioServer::get_singleton()->get_mix_rate();
}

AudioStreamPlaybackOsc::~AudioStreamPlaybackOsc() {
}

void AudioStreamPlaybackOsc::_bind_methods() {
}

void AudioStreamPlaybackOsc::set_stream(const Ref<AudioStreamOsc> &p_stream) {
	stream = p_stream;
}

float AudioStreamPlaybackOsc::generate_sample() {
	if (stream.is_null()) {
		return 0.0f;
	}

	float sample = 0.0f;
	float amplitude = stream->get_amplitude_linear();

	switch (stream->get_waveform_type()) {
		case AudioStreamOsc::WAVEFORM_SINE:
			sample = amplitude * std::sin(2.0 * Math_PI * phase);
			break;

		case AudioStreamOsc::WAVEFORM_SAW:
			sample = amplitude * (2.0f * phase - 1.0f);
			break;

		case AudioStreamOsc::WAVEFORM_SQUARE:
			sample = amplitude * (phase < 0.5 ? 1.0f : -1.0f);
			break;
	}

	return sample;
}

void AudioStreamPlaybackOsc::_start(double p_from_pos) {
	phase = 0.0;
}

void AudioStreamPlaybackOsc::_stop() {
	// Nothing to do
}

bool AudioStreamPlaybackOsc::_is_playing() const {
	return true; // Oscillator is always "playing" when active
}

int32_t AudioStreamPlaybackOsc::_get_loop_count() const {
	return 0; // Infinite loop
}

double AudioStreamPlaybackOsc::_get_playback_position() const {
	return 0.0; // Continuous stream has no meaningful position
}

void AudioStreamPlaybackOsc::_seek(double p_time) {
	// No seeking for continuous oscillator
}

int AudioStreamPlaybackOsc::_mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) {
	if (stream.is_null()) {
		// Fill with silence if no stream
		for (int32_t i = 0; i < p_frames; i++) {
			p_buffer[i] = AudioFrame(0.0f, 0.0f);
		}
		return p_frames;
	}

	float frequency = stream->get_frequency() * p_rate_scale;
	double phase_increment = frequency / sample_rate;

	for (int32_t i = 0; i < p_frames; i++) {
		float sample = generate_sample();
		
		// Output same signal to both channels (mono to stereo)
		p_buffer[i] = AudioFrame(sample, sample);

		// Advance phase
		phase += phase_increment;
		
		// Wrap phase to [0, 1)
		if (phase >= 1.0) {
			phase -= 1.0;
		}
	}

	return p_frames;
}

void AudioStreamPlaybackOsc::_tag_used_streams() {
	if (stream.is_valid()) {
		stream->tag_used(0);
	}
}

// AudioStreamOsc Implementation

AudioStreamOsc::AudioStreamOsc() {
	waveform_type = WAVEFORM_SINE;
	frequency = 440.0f; // A440
	amplitude_db = -6.0f; // ~0.5 linear amplitude by default
}

AudioStreamOsc::~AudioStreamOsc() {
}

void AudioStreamOsc::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_waveform_type", "type"), &AudioStreamOsc::set_waveform_type);
	ClassDB::bind_method(D_METHOD("get_waveform_type"), &AudioStreamOsc::get_waveform_type);

	ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &AudioStreamOsc::set_frequency);
	ClassDB::bind_method(D_METHOD("get_frequency"), &AudioStreamOsc::get_frequency);

	ClassDB::bind_method(D_METHOD("set_amplitude_db", "amplitude_db"), &AudioStreamOsc::set_amplitude_db);
	ClassDB::bind_method(D_METHOD("get_amplitude_db"), &AudioStreamOsc::get_amplitude_db);

	ClassDB::bind_method(D_METHOD("get_amplitude_linear"), &AudioStreamOsc::get_amplitude_linear);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "waveform_type", PROPERTY_HINT_ENUM, "Sine,Saw,Square"), 
				 "set_waveform_type", "get_waveform_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency", PROPERTY_HINT_RANGE, "20.0,20000.0,0.01,suffix:Hz"), 
				 "set_frequency", "get_frequency");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude_db", PROPERTY_HINT_RANGE, "-60.0,0.0,0.01,suffix:dB"), 
				 "set_amplitude_db", "get_amplitude_db");

	BIND_ENUM_CONSTANT(WAVEFORM_SINE);
	BIND_ENUM_CONSTANT(WAVEFORM_SAW);
	BIND_ENUM_CONSTANT(WAVEFORM_SQUARE);
}

void AudioStreamOsc::set_waveform_type(WaveformType p_type) {
	waveform_type = p_type;
}

AudioStreamOsc::WaveformType AudioStreamOsc::get_waveform_type() const {
	return waveform_type;
}

void AudioStreamOsc::set_frequency(float p_frequency) {
	frequency = CLAMP(p_frequency, 20.0f, 20000.0f);
}

float AudioStreamOsc::get_frequency() const {
	return frequency;
}

void AudioStreamOsc::set_amplitude_db(float p_amplitude_db) {
	amplitude_db = CLAMP(p_amplitude_db, -60.0f, 0.0f);
}

float AudioStreamOsc::get_amplitude_db() const {
	return amplitude_db;
}

float AudioStreamOsc::get_amplitude_linear() const {
	// Convert decibels to linear amplitude
	// Formula: linear = 10^(dB/20)
	return std::pow(10.0f, amplitude_db / 20.0f);
}

Ref<AudioStreamPlayback> AudioStreamOsc::_instantiate_playback() const {
	Ref<AudioStreamPlaybackOsc> playback;
	playback.instantiate();
	playback->set_stream(Ref<AudioStreamOsc>(this));
	return playback;
}

String AudioStreamOsc::_get_stream_name() const {
	return "Oscillator";
}

double AudioStreamOsc::_get_length() const {
	return 0.0; // Infinite stream
}

bool AudioStreamOsc::_is_monophonic() const {
	return true; // Single oscillator voice
}

double AudioStreamOsc::_get_bpm() const {
	return 0.0; // No tempo concept
}

int32_t AudioStreamOsc::_get_beat_count() const {
	return 0; // No beat concept
}