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

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <SDL2/SDL_audio.h>

#include <PL/platform_filesystem.h>

#include "engine.h"
#include "frontend.h"

#include "graphics/camera.h"

#include "stb_vorbis.c"

using namespace ohw;

/* todo: provide fallback to SDL2 Audio? maybe dynamically load OpenAL??
 * todo: stream music and large samples */

/* Why the hell isn't this in OpenAL?! */
static const char *OALErrorString(ALenum err) {
	switch ( err ) {
		case AL_OUT_OF_MEMORY:     return "OpenAL ran out of memory";
		case AL_INVALID_VALUE:     return "Invalid value passed to OpenAL";
		case AL_INVALID_OPERATION: return "Invalid operation performed with OpenAL";
		case AL_INVALID_ENUM:      return "Invalid enum passed to OpenAL";
		case AL_INVALID_NAME:      return "Invalid name passed to OpenAL";

		default: return "Unknown OpenAL error";
	}
}

#define OALCheckErrors() \
{ \
	ALenum err = alGetError(); \
	if ( err != AL_NO_ERROR ) { \
		Error( "%s, aborting!\n", OALErrorString(err) ); \
	} \
}

static unsigned int reverb_effect_slot = 0;
static unsigned int reverb_sound_slot = 0;

/************************************************************/
/* Audio Source */

AudioSource::AudioSource( const AudioSample *sample, float gain, float pitch, bool looping ) :
	AudioSource( sample, PLVector3( 0, 0, 0 ), PLVector3( 0, 0, 0 ), false, gain, pitch, looping ) {
	alSourcei( alSourceId, AL_SOURCE_RELATIVE, AL_TRUE );
}

AudioSource::AudioSource( const AudioSample *sample, PLVector3 pos, PLVector3 vel,
						  bool reverb, float gain, float pitch, bool looping ) {
	alGenSources( 1, &alSourceId );
	OALCheckErrors();

	SetPosition( pos );
	SetVelocity( vel );
	SetGain( gain );
	SetPitch( pitch );
	SetLooping( looping );
	SetReferenceDistance( 300.0F );
	SetRolloffFactor( 0.1F );

	if ( reverb && Engine::Audio()->SupportsExtension( AudioManager::ExtensionType::AUDIO_EXT_EFX )) {
		alSource3i( alSourceId, AL_AUXILIARY_SEND_FILTER, reverb_sound_slot, 0, AL_FILTER_NULL );
		OALCheckErrors();
	}

	if ( sample != nullptr ) {
		SetSample( sample );
	}

	Engine::Audio()->sources_.insert( this );
}

AudioSource::~AudioSource() {
	StopPlaying();

	if ( current_sample_ != nullptr ) {
		/* Need to clear AL_LOOPING flag before unqueueing buffer. */
		alSourcei( alSourceId, AL_LOOPING, AL_FALSE );
		OALCheckErrors();

		unsigned int buf = current_sample_->alBufferId;
		alSourceUnqueueBuffers( alSourceId, 1, &buf );
		OALCheckErrors();
	}

	alSourcei( alSourceId, AL_BUFFER, 0 );
	OALCheckErrors();

	alDeleteSources( 1, &alSourceId );
	OALCheckErrors();

	Engine::Audio()->sources_.erase( this );
}

void AudioSource::SetSample( const AudioSample *sample ) {
	if ( sample == current_sample_ ) {
		return;
	}

	if ( current_sample_ != nullptr ) {
		StopPlaying();

		/* Need to clear AL_LOOPING flag before unqueueing buffer. */
		alSourcei( alSourceId, AL_LOOPING, AL_FALSE );
		OALCheckErrors();

		unsigned int buf = current_sample_->alBufferId;
		alSourceUnqueueBuffers( alSourceId, 1, &buf );
		u_assert( buf == current_sample_->alBufferId );
		OALCheckErrors();
	}

	if ( sample != nullptr ) {
		alSourceQueueBuffers( alSourceId, 1, &sample->alBufferId );
		OALCheckErrors();
	}

	current_sample_ = sample;

	if ( sample != nullptr ) {
		/* Need to reset looping flag after queueing buffer as it can't
		 * be set when no buffer is queued. */
		SetLooping(looping);
	}
}

const AudioSample *AudioSource::GetSample() const {
	return current_sample_;
}

void AudioSource::SetPosition( PLVector3 position ) {
	alSource3f( alSourceId, AL_POSITION, position.x, position.y, position.z );
	OALCheckErrors();
	position_ = position;
}

void AudioSource::SetVelocity( PLVector3 velocity ) {
	alSource3f( alSourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z );
	OALCheckErrors();
	velocity_ = velocity;
}

void AudioSource::SetGain( float gain ) {
	alSourcef( alSourceId, AL_GAIN, gain );
	OALCheckErrors();
	gain_ = gain;
}

void AudioSource::SetPitch( float pitch ) {
	alSourcef( alSourceId, AL_PITCH, pitch );
	OALCheckErrors();
	pitch_ = pitch;
}

void AudioSource::SetLooping( bool looping ) {
	this->looping = looping;

	if( current_sample_ != nullptr ) {
		alSourcei( alSourceId, AL_LOOPING, looping ? AL_TRUE : AL_FALSE );
		OALCheckErrors();
	}
}

void AudioSource::SetReferenceDistance( float value ) {
	alSourcef( alSourceId, AL_REFERENCE_DISTANCE, value );	
	OALCheckErrors();
}

void AudioSource::SetMaximumDistance( float value ) {
	alSourcef( alSourceId, AL_MAX_DISTANCE, value );	
	OALCheckErrors();
}

void AudioSource::SetRolloffFactor( float value ) {
	alSourcef( alSourceId, AL_ROLLOFF_FACTOR, value );	
	OALCheckErrors();
}

void AudioSource::StartPlaying() {
	alSourcePlay( alSourceId );
	OALCheckErrors();
}

void AudioSource::StopPlaying() {
	int state;
	alGetSourcei( alSourceId, AL_SOURCE_STATE, &state );
	OALCheckErrors();
	if ( state != AL_PLAYING ) {
		return;
	}

	alSourceStop( alSourceId );
	OALCheckErrors();
}

bool AudioSource::IsPlaying() {
	int state;
	alGetSourcei( alSourceId, AL_SOURCE_STATE, &state );
	OALCheckErrors();

	return ( state == AL_PLAYING );
}

bool AudioSource::IsPaused() {
	int state;
	alGetSourcei( alSourceId, AL_SOURCE_STATE, &state );
	OALCheckErrors();

	return ( state == AL_PAUSED );
}

void AudioSource::Pause() {
	if ( !IsPlaying()) {
		// nothing to pause
		return;
	}

	alSourcePause( alSourceId );
	OALCheckErrors();
}

/************************************************************/

static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
//static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
//static LPALEFFECTFV alEffectfv;
//static LPALGETEFFECTI alGetEffecti;
//static LPALGETEFFECTIV alGetEffectiv;
//static LPALGETEFFECTF alGetEffectf;
//static LPALGETEFFECTFV alGetEffectfv;

static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
//static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
//static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
//static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
//static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
//static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
//static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
//static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

AudioManager::AudioManager() {
	ALCdevice *device = alcOpenDevice( nullptr );
	if ( device == nullptr ) {
		Error( "failed to open audio device, aborting audio initialisation!\n" );
	}
		
	memset( al_extensions_, 0, sizeof( al_extensions_ ));

	if ( alcIsExtensionPresent( device, "ALC_EXT_EFX" )) {
		LogInfo( "ALC_EXT_EFX detected\n" );

		alGenEffects = ( LPALGENEFFECTS ) alGetProcAddress( "alGenEffects" );
		alDeleteEffects = ( LPALDELETEEFFECTS ) alGetProcAddress( "alDeleteEffects" );
		alIsEffect = ( LPALISEFFECT ) alGetProcAddress( "alIsEffect" );
		alEffecti = ( LPALEFFECTI ) alGetProcAddress( "alEffecti" );
		alEffectf = ( LPALEFFECTF ) alGetProcAddress( "alEffectf" );

		alGenAuxiliaryEffectSlots = ( LPALGENAUXILIARYEFFECTSLOTS ) alGetProcAddress( "alGenAuxiliaryEffectSlots" );
		alDeleteAuxiliaryEffectSlots = ( LPALDELETEAUXILIARYEFFECTSLOTS ) alGetProcAddress(
			"alDeleteAuxiliaryEffectSlots" );
		alIsAuxiliaryEffectSlot = ( LPALISAUXILIARYEFFECTSLOT ) alGetProcAddress( "alIsAuxiliaryEffectSlot" );
		alAuxiliaryEffectSloti = ( LPALAUXILIARYEFFECTSLOTI ) alGetProcAddress( "alAuxiliaryEffectSloti" );

		al_extensions_[ AUDIO_EXT_EFX ] = true;
	}

	ALCcontext *context = alcCreateContext( device, nullptr );
	if ( context == nullptr || !alcMakeContextCurrent( context )) {
		Error( "Failed to create audio context, aborting audio initialisation!\n" );
	}

	if ( alIsExtensionPresent( "AL_SOFT_buffer_samples" )) {
		LogInfo( "AL_SOFT_buffer_samples detected\n" );
		al_extensions_[ AUDIO_EXT_SOFT_BUFFER_SAMPLES ] = true;
	}

	alDopplerFactor( 4.f );
	alDopplerVelocity( 350.f );

	if ( al_extensions_[ AUDIO_EXT_EFX ] ) {
		alGenEffects( 1, &reverb_effect_slot );
		alEffecti( reverb_effect_slot, AL_EFFECT_TYPE, AL_EFFECT_REVERB );
		const EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON;
		// EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON
		// EFX_REVERB_PRESET_OUTDOORS_VALLEY
		alEffectf( reverb_effect_slot, AL_REVERB_DENSITY, reverb.flDensity );
		alEffectf( reverb_effect_slot, AL_REVERB_DIFFUSION, reverb.flDiffusion );
		alEffectf( reverb_effect_slot, AL_REVERB_GAIN, reverb.flGain );
		alEffectf( reverb_effect_slot, AL_REVERB_GAINHF, reverb.flGainHF );
		alEffectf( reverb_effect_slot, AL_REVERB_DECAY_TIME, reverb.flDecayTime );
		alEffectf( reverb_effect_slot, AL_REVERB_DECAY_HFRATIO, reverb.flDecayHFRatio );
		alEffectf( reverb_effect_slot, AL_REVERB_REFLECTIONS_GAIN, reverb.flReflectionsGain );
		alEffectf( reverb_effect_slot, AL_REVERB_REFLECTIONS_DELAY, reverb.flReflectionsDelay );
		alEffectf( reverb_effect_slot, AL_REVERB_LATE_REVERB_GAIN, reverb.flLateReverbGain );
		alEffectf( reverb_effect_slot, AL_REVERB_LATE_REVERB_DELAY, reverb.flLateReverbDelay );
		alEffectf( reverb_effect_slot, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb.flAirAbsorptionGainHF );
		alEffectf( reverb_effect_slot, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.flRoomRolloffFactor );
		alEffecti( reverb_effect_slot, AL_REVERB_DECAY_HFLIMIT, reverb.iDecayHFLimit );
		alGenAuxiliaryEffectSlots( 1, &reverb_sound_slot );
		alAuxiliaryEffectSloti( reverb_sound_slot, AL_EFFECTSLOT_EFFECT, reverb_effect_slot );
	}
	alDistanceModel(AL_EXPONENT_DISTANCE);

	plRegisterConsoleCommand( "stopMusic", StopMusicCommand, "Stops the current music track." );
}

AudioManager::~AudioManager() {
	LogInfo( "Shutting down audio sub-system...\n" );

	FreeSources();

	/* FreeSources() doesn't delete musicSource. */
	delete musicSource;
	musicSource = nullptr;

	if ( al_extensions_[ AUDIO_EXT_EFX ] ) {
		alDeleteAuxiliaryEffectSlots( 1, &reverb_sound_slot );
		alDeleteEffects( 1, &reverb_effect_slot );
	}

	FreeSamples( true );

	ALCcontext *context = alcGetCurrentContext();
	if ( context != nullptr ) {
		ALCdevice *device = alcGetContextsDevice( context );
		if ( device != nullptr ) {
			alcCloseDevice( device );
		}

		alcMakeContextCurrent( nullptr );
		alcDestroyContext( context );
	}
}

const AudioSample *AudioManager::CacheSample( const std::string &path, bool preserve ) {
	auto i = samples_.find( path );
	if ( i != samples_.end()) {
		return &( i->second );
	}

	const char *ext = plGetFileExtension( path.c_str());
	if ( ext == nullptr ) {
		LogWarn( "Unable to identify audio format, \"%s\"!\n", path.c_str());
		return nullptr;
	}

	unsigned int format = 0;
	int freq = 0;
	uint32_t length;
	uint8_t *buffer;
	if ( pl_strcasecmp( ext, "wav" ) == 0 ) {
		// Attempt to load the Wav file
		PLFile *wavFile = plOpenFile( path.c_str(), false );
		if ( wavFile == nullptr ) {
			LogWarn( "Failed to load \"%s\"!\n", path.c_str());
			return nullptr;
		}

		// Allocate buffer
		int len = plGetFileSize( wavFile );
		uint8_t *buf = static_cast<uint8_t *>(u_alloc( len, 1, true ));

		// For now, read the whole thing into memory
		// todo: stream!!!
		plReadFile( wavFile, buf, 1, len );
		plCloseFile( wavFile );

		SDL_AudioSpec spec;
		if ( SDL_LoadWAV_RW( SDL_RWFromMem( buf, len ), 1, &spec, &buffer, &length ) == nullptr ) {
			LogWarn( "Failed to load \"%s\"!\n", path.c_str());
			return nullptr;
		}

		u_free( buf );

		/* translate the spec over to oal
		 * todo: conversion... https://github.com/solemnwarning/armageddon-recorder/blob/master/src/resample.hpp#L42
		 * */
		switch ( spec.format ) {
			case AUDIO_U8:
				if ( spec.channels == 1 ) {
					format = AL_FORMAT_MONO8;
				} else if ( spec.channels == 2 ) {
					format = AL_FORMAT_STEREO8;
				}
				break;
			case AUDIO_S16:
				if ( spec.channels == 1 ) {
					format = AL_FORMAT_MONO16;
				} else if ( spec.channels == 2 ) {
					format = AL_FORMAT_STEREO16;
				}
				break;
			default:break;
		}

		if ( format == 0 ) {
			LogWarn( "Invalid audio format for \"%s\"!\n", path.c_str());
			SDL_FreeWAV( buffer );
			return nullptr;
		}

		freq = spec.freq;
	} else if ( pl_strcasecmp( ext, "ogg" ) == 0 ) {
		// Attempt to load the Ogg file
		PLFile *oggFile = plOpenFile( path.c_str(), false );
		if ( oggFile == nullptr ) {
			LogWarn( "Failed to load \"%s\"!\n", path.c_str());
			return nullptr;
		}

		// Allocate buffer
		int len = plGetFileSize( oggFile );
		uint8_t *buf = static_cast<uint8_t *>(u_alloc( len, 1, true ));

		// For now, read the whole thing into memory (todo: stream!!!)
		plReadFile( oggFile, buf, 1, len );
		plCloseFile( oggFile );

		int vchan;
		int samples = stb_vorbis_decode_memory( buf, len, &vchan, &freq,
												reinterpret_cast<short **>(&buffer));
		if ( samples == -1 ) {
			LogWarn( "Failed to decode ogg audio data, \"%s\"!\n", path.c_str());
			return nullptr;
		}

		length = samples * vchan * sizeof( int16_t );
		if ( vchan == 2 ) {
			format = AL_FORMAT_STEREO16;
		} else {
			format = AL_FORMAT_MONO16;
		}

		u_free( buf );
	}

	auto sample = samples_.emplace(
		std::piecewise_construct,
		std::forward_as_tuple( path ),
		std::forward_as_tuple( buffer, freq, format, length, preserve )
	);

	return &( sample.first->second );
}

const AudioSample *AudioManager::GetCachedSample( const std::string &path ) {
	auto i = samples_.find( path );
	if ( i == samples_.end()) {
		CacheSample( path, false );
		i = samples_.find( path );
		if ( i == samples_.end()) {
			Error( "Failed to load sample, \"%s\"!\n", path.c_str());
			/* todo: in future, fall back to first loaded sound and continue? if it exists... */
		}
	}

	return &( i->second );
}

AudioSource *AudioManager::CreateSource( const std::string &path, float gain, float pitch, bool looping ) {
	return new AudioSource( GetCachedSample( path ), gain, pitch, looping );
}

AudioSource *AudioManager::CreateSource( const std::string &path, PLVector3 pos, PLVector3 vel, bool reverb, float gain,
										 float pitch, bool looping ) {
	return new AudioSource( GetCachedSample( path ), pos, vel, reverb, gain, pitch, looping );
}

AudioSource *AudioManager::CreateSource( const AudioSample *sample, PLVector3 pos, PLVector3 vel, bool reverb,
										 float gain, float pitch, bool looping ) {
	return new AudioSource( sample, pos, vel, reverb, gain, pitch, looping );
}

void AudioManager::SetupMusicSource() {
	// Setup our global music source
	musicSource = new AudioSource( nullptr, cv_audio_volume_music->f_value, 1.0f, false );
}

void AudioManager::Tick() {
	PLVector3 position = { 0, 0, 0 }, angles = { 0, 0, 0 };

	Camera *camera = Engine::Game()->GetCamera();
	if ( FrontEnd_GetState() == FE_MODE_GAME && camera != nullptr ) {
		position = camera->GetPosition();
		angles = camera->GetAngles();
	}

	PLVector3 forward, left, up;
	plAnglesAxes( angles, &left, &up, &forward );

	float ori[6];
	ori[ 0 ] = forward.z;
	ori[ 3 ] = up.x;
	ori[ 1 ] = forward.y;
	ori[ 4 ] = up.y;
	ori[ 2 ] = forward.x;
	ori[ 5 ] = up.z;

	alListener3f( AL_POSITION, position.x, position.y, position.z );
	alListenerfv( AL_ORIENTATION, ori );
	alListenerf( AL_GAIN, cv_audio_volume->f_value );

	// ensure destruction of temporary sources
	for ( auto source = temp_sources_.begin(); source != temp_sources_.end(); ) {
		if (( *source )->IsPlaying() || ( *source )->IsPaused()) {
			++source;
			continue;
		}

		// this will automatically kill it everywhere
		delete ( *source );
		source = temp_sources_.erase( source );
	}
}

void AudioManager::PlayGlobalSound( const std::string &path ) {
	const AudioSample *sample = GetCachedSample( path );
	PlayGlobalSound( sample );
}

void AudioManager::PlayGlobalSound( const AudioSample *sample ) {
	if ( sample == nullptr ) {
		return;
	}

	auto *source = new AudioSource( sample );
	temp_sources_.insert( source );
	source->StartPlaying();
}

void AudioManager::PlayLocalSound( const std::string &path, PLVector3 pos, PLVector3 vel, bool reverb, float gain,
								   float pitch ) {
	const AudioSample *sample = GetCachedSample( path );
	PlayLocalSound( sample, pos, vel, reverb, gain, pitch );
}

void AudioManager::PlayLocalSound( const AudioSample *sample, PLVector3 pos, PLVector3 vel, bool reverb, float gain,
								   float pitch ) {
	if ( sample == nullptr ) {
		return;
	}

	auto *source = new AudioSource( sample, pos, vel, reverb, gain, pitch );
	temp_sources_.insert( source );
	source->StartPlaying();
}

void AudioManager::SilenceSources() {
	for ( auto sound : sources_ ) {
		sound->StopPlaying();
	}
}

/* Will invalidate ALL references to AudioSource objects.
 * Only use in contexts where this is safe. */
void AudioManager::FreeSources() {
	LogInfo( "Freeing all audio sources...\n" );

	for ( auto s = sources_.begin(); s != sources_.end(); ) {
		AudioSource *source = *s;
		++s;

		// Don't destroy our music source, it needs to be preserved!
		if ( source == musicSource ) {
			continue;
		}

		source->StopPlaying();
		delete source;
	}

	temp_sources_.clear();
}

void AudioManager::FreeSamples( bool force ) {
	LogInfo( "Freeing all audio samples...\n" );

	if ( force ) {
		/* clears absolutely everything */
		samples_.clear();
	} else {
		/* clears only those not marked with preserve */
		for ( auto sample = samples_.begin(); sample != samples_.end(); ) {
			if ( !sample->second.preserve_ ) {
				sample = samples_.erase( sample );
			} else {
				++sample;
			}
		}
	}
}

/**
 * Debug function for drawing audio sources.
 */
void AudioManager::DrawSources() {
	if ( !cv_graphics_draw_audio_sources->b_value ) {
		return;
	}

	PLModel *sprite = Engine::Resource()->GetFallbackModel();
	PLMesh *mesh = sprite->levels[ 0 ].meshes[ 0 ];
	plSetMeshUniformColour( mesh, PLColour( 0, 255, 255, 255 ));
	for ( auto source : sources_ ) {
		if ( !source->IsPlaying() || source->IsPaused()) {
			continue;
		}

		sprite->model_matrix = plTranslateMatrix4( source->GetPosition());
		plDrawModel( sprite );
	}
	plSetMeshUniformColour( mesh, PLColour( 255, 0, 0, 255 ));
}

/**
 * Play the specified music globally.
 * @param path Path to the sample to be played.
 */
void AudioManager::PlayMusic( const std::string &path ) {
	const AudioSample *sample = CacheSample( path );
	if ( sample == nullptr ) {
		return;
	}

	musicSource->StopPlaying();

	musicSource->SetSample( sample );
	musicSource->StartPlaying();
}

void AudioManager::PauseMusic() {
	musicSource->Pause();
}

void AudioManager::StopMusic() {
	musicSource->StopPlaying();
}

void AudioManager::SetMusicVolume( float gain ) {
	musicSource->SetGain( gain );
}

void AudioManager::SetMusicVolumeCommand( const PLConsoleVariable *var ) {
	Engine::Audio()->SetMusicVolume( var->f_value );
}

void AudioManager::StopMusicCommand( unsigned int argc, char *argv[] ) {
	u_unused( argc );
	u_unused( argv );

	Engine::Audio()->StopMusic();
}

/************************************************************/
/* Audio Sample */

AudioSample::~AudioSample() {
	/* Unbind any AudioSource objects which are using this sample.
	 * Scanning all sources is slow, but this is only expected to be called
	 * during game teardown.
	*/
	for ( auto &source: Engine::Audio()->sources_ ) {
		if ( source->GetSample() == this ) {
			source->SetSample( nullptr );
		}
	}

	alDeleteBuffers( 1, &alBufferId );
	OALCheckErrors();

	SDL_FreeWAV( data_ );
}

AudioSample::AudioSample( uint8_t *data, unsigned int freq, unsigned int format, unsigned int length, bool preserve ) {
	alGenBuffers( 1, &alBufferId );
	OALCheckErrors();
	alBufferData( alBufferId, format, data, length, freq );
	OALCheckErrors();
}
