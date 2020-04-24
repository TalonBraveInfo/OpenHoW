/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/* included again here just
 * so we don't have to provide
 * the OpenAL headers here.     */
typedef enum {
	AUDIO_REVERB_GENERIC,
	AUDIO_REVERB_PADDEDCELL,
	AUDIO_REVERB_ROOM,
	AUDIO_REVERB_BATHROOM,
	AUDIO_REVERB_LIVINGROOM,
	AUDIO_REVERB_STONEROOM,
	AUDIO_REVERB_AUDITORIUM,
	AUDIO_REVERB_CONCERTHALL,
	AUDIO_REVERB_CAVE,
	AUDIO_REVERB_ARENA,
	AUDIO_REVERB_HANGAR,
	AUDIO_REVERB_CARPETEDHALLWAY,
	AUDIO_REVERB_HALLWAY,
	AUDIO_REVERB_STONECORRIDOR,
	AUDIO_REVERB_ALLEY,
	AUDIO_REVERB_FOREST,
	AUDIO_REVERB_CITY,
	AUDIO_REVERB_MOUNTAINS,
	AUDIO_REVERB_QUARRY,
	AUDIO_REVERB_PLAIN,
	AUDIO_REVERB_PARKINGLOT,
	AUDIO_REVERB_SEWERPIPE,
	AUDIO_REVERB_UNDERWATER,
	AUDIO_REVERB_DRUGGED,
	AUDIO_REVERB_DIZZY,
	AUDIO_REVERB_PSYCHOTIC,
	AUDIO_REVERB_CHAPEL
} AudioEffectReverb;

class AudioSource;

#define AUDIO_MUSIC_FIELD    "music/track01.ogg"
#define AUDIO_MUSIC_MENU    "music/track02.ogg"
#define AUDIO_MUSIC_VICTORY "music/track31.ogg"

struct AudioSample {
	AudioSample( uint8_t *data, unsigned int freq, unsigned int format, unsigned int length, bool preserve );
	~AudioSample();

	unsigned int alBufferId{ 0 };
	uint8_t *data_{ nullptr };
	bool preserve_{ false };
};

namespace openhow {
class Engine;
}

class AudioManager {
	friend class AudioSample;
	friend class AudioSource;

private:
	AudioManager();
	~AudioManager();

public:
	void SetupMusicSource();

	void Tick();

	const AudioSample *GetCachedSample( const std::string &path );
	const AudioSample *CacheSample( const std::string &path, bool preserve = false );

	AudioSource *CreateSource( const std::string &path, float gain = 1.0f, float pitch = 1.0f, bool looping = false );
	AudioSource *CreateSource( const std::string &path, PLVector3 pos, PLVector3 vel, bool reverb = false,
							   float gain = 1.0f, float pitch = 1.0f, bool looping = false );
	AudioSource *CreateSource( const AudioSample *sample = nullptr,
							   PLVector3 pos = { 0, 0, 0 },
							   PLVector3 vel = { 0, 0, 0 },
							   bool reverb = false,
							   float gain = 1.0f,
							   float pitch = 1.0f,
							   bool looping = false );

	void PlayGlobalSound( const std::string &path );
	void PlayGlobalSound( const AudioSample *sample );
	void PlayLocalSound( const std::string &path, PLVector3 pos, PLVector3 vel = { 0, 0, 0 }, bool reverb = false,
						 float gain = 1.0f, float pitch = 1.0f );
	void PlayLocalSound( const AudioSample *sample, PLVector3 pos, PLVector3 vel = { 0, 0, 0 }, bool reverb = false,
						 float gain = 1.0f, float pitch = 1.0f );

	void PlayMusic( const std::string &path );
	void PauseMusic();
	void StopMusic();
	void SetMusicVolume( float gain );

	void SilenceSources();

	void FreeSources();
	void FreeSamples( bool force = false );

	void DrawSources();

	enum ExtensionType {
		AUDIO_EXT_EFX,
		AUDIO_EXT_SOFT_BUFFER_SAMPLES,

		MAX_AUDIO_EXT_SLOTS
	};
	inline bool SupportsExtension( ExtensionType extension ) {
		return al_extensions_[ extension ];
	}

protected:
private:
	bool al_extensions_[MAX_AUDIO_EXT_SLOTS]{
		false, false
	};

	static void SetMusicVolumeCommand( const PLConsoleVariable *var );
	static void StopMusicCommand( unsigned int argc, char *argv[] );

	std::map<std::string, AudioSample> samples_;
	std::set<AudioSource *> sources_;
	std::set<AudioSource *> temp_sources_;

	AudioSource *musicSource{ nullptr };

	friend class openhow::Engine;
};

class AudioSource {
public:
	explicit AudioSource( const AudioSample *sample, float gain = 1.0f, float pitch = 1.0f, bool looping = false );
	AudioSource( const AudioSample *sample,
				 PLVector3 pos,
				 PLVector3 vel,
				 bool reverb = false,
				 float gain = 1.0f,
				 float pitch = 1.0f,
				 bool looping = false );
	~AudioSource();

	void SetSample( const AudioSample *sample );
	const AudioSample *GetSample() const;
	void SetPosition( PLVector3 position );
	void SetVelocity( PLVector3 velocity );
	void SetGain( float gain );
	void SetPitch( float pitch );
	void SetLooping( bool looping );
	void SetReferenceDistance( float value );
	void SetMaximumDistance( float value );
	void SetRolloffFactor( float value );

	PLVector3 GetPosition() { return position_; }
	PLVector3 GetVelocity() { return velocity_; }
	float GetGain() { return gain_; }
	float GetPitch() { return pitch_; }

	void StartPlaying();
	void StopPlaying();
	void Pause();

	bool IsPlaying();
	bool IsPaused();

private:
	PLVector3 position_{ 0, 0, 0 };
	PLVector3 velocity_{ 0, 0, 0 };

	float gain_{ 1.0f };
	float pitch_{ 1.0f };

	unsigned int alSourceId{ 0 };
	const AudioSample *current_sample_{ nullptr };
	bool looping{ false };
};
