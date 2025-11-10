/**************************************************************************/
/*  audio_stream_osc.h                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             CIPHERS AUDIO                              */
/*                        https://github.com/RiPCipher/CiphersAudio       */
/**************************************************************************/

#ifndef AUDIO_STREAM_OSC_H
#define AUDIO_STREAM_OSC_H

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/classes/audio_server.hpp>

using namespace godot;

class AudioStreamOsc;

class AudioStreamPlaybackOsc : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackOsc, AudioStreamPlayback)

private:
	Ref<AudioStreamOsc> stream;
	double phase;
	double sample_rate;

	float generate_sample();

protected:
	static void _bind_methods();

public:
	AudioStreamPlaybackOsc();
	~AudioStreamPlaybackOsc();

	void set_stream(const Ref<AudioStreamOsc> &p_stream);

	virtual void _start(double p_from_pos = 0.0) override;
	virtual void _stop() override;
	virtual bool _is_playing() const override;
	virtual int32_t _get_loop_count() const override;
	virtual double _get_playback_position() const override;
	virtual void _seek(double p_time) override;
	virtual int _mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) override;
	virtual void _tag_used_streams() override;
};

class AudioStreamOsc : public AudioStream {
	GDCLASS(AudioStreamOsc, AudioStream)

public:
	enum WaveformType {
		WAVEFORM_SINE,
		WAVEFORM_SAW,
		WAVEFORM_SQUARE
	};

private:
	WaveformType waveform_type;
	float frequency;
	float amplitude_db;

protected:
	static void _bind_methods();

public:
	AudioStreamOsc();
	~AudioStreamOsc();

	void set_waveform_type(WaveformType p_type);
	WaveformType get_waveform_type() const;

	void set_frequency(float p_frequency);
	float get_frequency() const;

	void set_amplitude_db(float p_amplitude_db);
	float get_amplitude_db() const;

	float get_amplitude_linear() const;

	virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;
	virtual String _get_stream_name() const override;
	virtual double _get_length() const override;
	virtual bool _is_monophonic() const override;
	virtual double _get_bpm() const override;
	virtual int32_t _get_beat_count() const override;
};

VARIANT_ENUM_CAST(AudioStreamOsc::WaveformType);

#endif // AUDIO_STREAM_OSC_H