/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include "engine.h"
#include "audio.h"
#include "frontend.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <SDL2/SDL_audio.h>
#include <PL/platform_graphics_camera.h>

#include <list>

/* todo: provide fallback to SDL2 Audio? maybe dynamically load OpenAL?? */
/* todo: get this working on macOS */

static void OALCheckErrors() {
    ALenum err = alGetError();
    if(err != AL_NO_ERROR) {
        /* alut is apparently deprecated in OpenAL Soft, yay... */
        /*Error("%s\n", alutGetErrorString(err));*/

        switch(err) {
            default: Error("Unknown openal error, aborting!\n");
            case AL_OUT_OF_MEMORY: Error("Openal ran out of memory, aborting!\n");
            case AL_INVALID_VALUE: Error("Invalid value passed to openal, aborting!\n");
            case AL_INVALID_OPERATION: Error("Invalid operation performed with openal, aborting!\n");
            case AL_INVALID_ENUM: Error("Invalid enum passed to openal, aborting!\n");
            case AL_INVALID_NAME: Error("Invalid name passed to openal, aborting!\n");
        }
    }
}

unsigned int reverb_effect_slot = 0;
unsigned int reverb_sound_slot = 0;

/************************************************************/
/* Audio Source */

AudioSource::AudioSource(const AudioSample* sample, float gain, float pitch, bool looping) :
        AudioSource(sample, PLVector3(0, 0, 0), PLVector3(0, 0, 0), false, gain, pitch, looping) {
    alSourcei(al_source_id_, AL_SOURCE_RELATIVE, AL_TRUE);
}

AudioSource::AudioSource(const AudioSample* sample, PLVector3 pos, PLVector3 vel,
        bool reverb, float gain, float pitch, bool looping) {
    alGenSources(1, &al_source_id_);
    OALCheckErrors();

    SetPosition(pos);
    SetVelocity(vel);
    SetGain(gain);
    SetPitch(pitch);

    alSourcei(al_source_id_, AL_LOOPING, looping);
    OALCheckErrors();
    alSourcef(al_source_id_, AL_REFERENCE_DISTANCE, 300.0f);
    OALCheckErrors();
    alSourcef(al_source_id_, AL_ROLLOFF_FACTOR, 1.0f);
    OALCheckErrors();

    if(reverb && AudioManager::GetInstance()->SupportsExtension(AudioManager::ExtType::AUDIO_EXT_EFX)) {
        alSource3i(al_source_id_, AL_AUXILIARY_SEND_FILTER, reverb_sound_slot, 0, AL_FILTER_NULL);
        OALCheckErrors();
    }

    if(sample != nullptr) {
        SetSample(sample);
    }

    AudioManager::GetInstance()->sources_.insert(this);
}

AudioSource::~AudioSource() {
    AudioManager::GetInstance()->sources_.erase(this);
    if(current_sample_ != nullptr) {
        unsigned int buf = current_sample_->al_buffer_id_;
        alSourceUnqueueBuffers(al_source_id_, 1, &buf);
    }
    alDeleteSources(1, &al_source_id_);
}

void AudioSource::SetSample(const AudioSample* sample) {
    if(sample == nullptr) {
        LogWarn("Invalid sample passed, aborting!\n");
        return;
    }

    if(sample == current_sample_) {
        return;
    }

    if(current_sample_ != nullptr && sample != current_sample_) {
        unsigned int buf = current_sample_->al_buffer_id_;
        alSourceUnqueueBuffers(al_source_id_, 1, &buf);
        u_assert(buf == current_sample_->al_buffer_id_);
        OALCheckErrors();
    }

    alSourceQueueBuffers(al_source_id_, 1, &sample->al_buffer_id_);
    OALCheckErrors();

    current_sample_ = sample;
}

void AudioSource::SetPosition(PLVector3 position) {
    alSource3f(al_source_id_, AL_POSITION, position.x, position.y, position.z);
    OALCheckErrors();
    position_ = position;
}

void AudioSource::SetVelocity(PLVector3 velocity) {
    alSource3f(al_source_id_, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    OALCheckErrors();
    velocity_ = velocity;
}

void AudioSource::SetGain(float gain) {
    alSourcef(al_source_id_, AL_GAIN, gain);
    OALCheckErrors();
    gain_ = gain;
}

void AudioSource::SetPitch(float pitch) {
    alSourcef(al_source_id_, AL_PITCH, pitch);
    OALCheckErrors();
    pitch_ = pitch;
}

void AudioSource::StartPlaying() {
    alSourcePlay(al_source_id_);
    OALCheckErrors();
}

void AudioSource::StopPlaying() {
    int state;
    alGetSourcei(al_source_id_, AL_SOURCE_STATE, &state);
    OALCheckErrors();
    if(state != AL_PLAYING) {
        return;
    }

    alSourceStop(al_source_id_);
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

AudioManager *AudioManager::instance_ = nullptr;

AudioManager::AudioManager() {
    ALCdevice *device = alcOpenDevice(nullptr);
    if(device == nullptr) {
        Error("failed to open audio device, aborting audio initialisation!\n");
    }

    memset(al_extensions_, 0, sizeof(al_extensions_));

    if(alcIsExtensionPresent(device, "ALC_EXT_EFX")) {
        LogInfo("ALC_EXT_EFX detected\n");

        alGenEffects = (LPALGENEFFECTS) alGetProcAddress("alGenEffects");
        alDeleteEffects = (LPALDELETEEFFECTS) alGetProcAddress("alDeleteEffects");
        alIsEffect = (LPALISEFFECT) alGetProcAddress("alIsEffect");
        alEffecti = (LPALEFFECTI) alGetProcAddress("alEffecti");
        alEffectf = (LPALEFFECTF) alGetProcAddress("alEffectf");

        alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS) alGetProcAddress("alGenAuxiliaryEffectSlots");
        alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS) alGetProcAddress(
                "alDeleteAuxiliaryEffectSlots");
        alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT) alGetProcAddress("alIsAuxiliaryEffectSlot");
        alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI) alGetProcAddress("alAuxiliaryEffectSloti");

        al_extensions_[AUDIO_EXT_EFX] = true;
    }

    ALCcontext *context = alcCreateContext(device, nullptr);
    if(context == nullptr || !alcMakeContextCurrent(context)) {
        Error("failed to create audio context, aborting audio initialisation!\n");
    }

    if(alIsExtensionPresent("AL_SOFT_buffer_samples")) {
        LogInfo("AL_SOFT_buffer_samples detected\n");
        al_extensions_[AUDIO_EXT_SOFT_BUFFER_SAMPLES] = true;
    }

    alDopplerFactor(4.f);
    alDopplerVelocity(350.f);

    if(al_extensions_[AUDIO_EXT_EFX]) {
        alGenEffects(1, &reverb_effect_slot);
        alEffecti(reverb_effect_slot, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        const EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_OUTDOORS_VALLEY;
        // EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON
        // EFX_REVERB_PRESET_OUTDOORS_VALLEY
        alEffectf(reverb_effect_slot, AL_REVERB_DENSITY, reverb.flDensity);
        alEffectf(reverb_effect_slot, AL_REVERB_DIFFUSION, reverb.flDiffusion);
        alEffectf(reverb_effect_slot, AL_REVERB_GAIN, reverb.flGain);
        alEffectf(reverb_effect_slot, AL_REVERB_GAINHF, reverb.flGainHF);
        alEffectf(reverb_effect_slot, AL_REVERB_DECAY_TIME, reverb.flDecayTime);
        alEffectf(reverb_effect_slot, AL_REVERB_DECAY_HFRATIO, reverb.flDecayHFRatio);
        alEffectf(reverb_effect_slot, AL_REVERB_REFLECTIONS_GAIN, reverb.flReflectionsGain);
        alEffectf(reverb_effect_slot, AL_REVERB_REFLECTIONS_DELAY, reverb.flReflectionsDelay);
        alEffectf(reverb_effect_slot, AL_REVERB_LATE_REVERB_GAIN, reverb.flLateReverbGain);
        alEffectf(reverb_effect_slot, AL_REVERB_LATE_REVERB_DELAY, reverb.flLateReverbDelay);
        alEffectf(reverb_effect_slot, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb.flAirAbsorptionGainHF);
        alEffectf(reverb_effect_slot, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.flRoomRolloffFactor);
        alEffecti(reverb_effect_slot, AL_REVERB_DECAY_HFLIMIT, reverb.iDecayHFLimit);
        alGenAuxiliaryEffectSlots(1, &reverb_sound_slot);
        alAuxiliaryEffectSloti(reverb_sound_slot, AL_EFFECTSLOT_EFFECT, reverb_effect_slot);
    }
}

AudioManager::~AudioManager() {
    LogInfo("shutting down audio sub-system...\n");

    FreeSources();

    if(al_extensions_[AUDIO_EXT_EFX]) {
        alDeleteAuxiliaryEffectSlots(1, &reverb_sound_slot);
        alDeleteEffects(1, &reverb_effect_slot);
    }

    FreeSamples(true);

    ALCcontext *context = alcGetCurrentContext();
    if(context != nullptr) {
        ALCdevice *device = alcGetContextsDevice(context);
        if(device != nullptr) {
            alcCloseDevice(device);
        }

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }
}

const AudioSample* AudioManager::CacheSample(const std::string &path, bool preserve) {
    auto i = samples_.find(path);
    if(i != samples_.end()) {
        return &(i->second);
    }

    SDL_AudioSpec spec;
    uint32_t length;
    uint8_t *buffer;
    if(SDL_LoadWAV(u_find(path.c_str()), &spec, &buffer, &length) == nullptr) {
        LogWarn("Failed to load \"%s\"!\n", path.c_str());
        return nullptr;
    }

    /* translate the spec over to oal
     * todo: conversion... https://github.com/solemnwarning/armageddon-recorder/blob/master/src/resample.hpp#L42
     * */
    unsigned int format = 0;
    switch(spec.format) {
        case AUDIO_U8:
            if(spec.channels == 1) {
                format = AL_FORMAT_MONO8;
            } else if(spec.channels == 2) {
                format = AL_FORMAT_STEREO8;
            }
            break;
        case AUDIO_S16:
            if(spec.channels == 1) {
                format = AL_FORMAT_MONO16;
            } else if(spec.channels == 2) {
                format = AL_FORMAT_STEREO16;
            }
            break;
        default:break;
    }

    if(format == 0) {
        LogWarn("Invalid audio format for \"%s\"!\n", path.c_str());
        SDL_FreeWAV(buffer);
        return nullptr;
    }

    auto sample = samples_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(path),
            std::forward_as_tuple(buffer, spec.freq, format, length, preserve)
            );

    return &(sample.first->second);
}

const AudioSample *AudioManager::GetCachedSample(const std::string &path) {
    auto i = samples_.find(path);
    if(i == samples_.end()) {
        CacheSample(path, false);
        i = samples_.find(path);
        if(i == samples_.end()) {
            Error("failed to load sample, \"%s\"!\n", path.c_str());
            /* todo: in future, fall back to first loaded sound and continue? if it exists... */
        }
    }

    return &(i->second);
}

AudioSource *AudioManager::CreateSource(const std::string &path, float gain, float pitch, bool looping) {
    return new AudioSource(GetCachedSample(path), gain, pitch, looping);
}

AudioSource *AudioManager::CreateSource(const std::string &path, PLVector3 pos, PLVector3 vel, bool reverb, float gain,
                                        float pitch, bool looping) {
    return new AudioSource(GetCachedSample(path), pos, vel, reverb, gain, pitch, looping);
}

AudioSource *AudioManager::CreateSource(const AudioSample *sample, PLVector3 pos, PLVector3 vel, bool reverb,
        float gain, float pitch, bool looping) {
    return new AudioSource(sample, pos, vel, reverb, gain, pitch, looping);
}

void AudioManager::Tick() {
    PLVector3 position = {0,0,0}, angles = {0,0,0};
    if(FrontEnd_GetState() == FE_MODE_GAME) {
        position = g_state.camera->position;
        angles = g_state.camera->angles;
    }

    PLVector3 forward, left, up;
    plAnglesAxes(angles, &left, &up, &forward);

    float ori[6];
    ori[0] = forward.z; ori[3] = up.x;
    ori[1] = forward.y; ori[4] = up.y;
    ori[2] = forward.x; ori[5] = up.z;

    alListener3f(AL_POSITION, position.x, position.y, position.z);
    alListenerfv(AL_ORIENTATION, ori);
    alListenerf(AL_GAIN, cv_audio_volume->f_value);
}

void AudioManager::PlayGlobalSound(const std::string &path) {
    global_source_.reset(CreateSource(path));
    global_source_->StartPlaying();
}

void AudioManager::SilenceSources() {
    for(auto sound : sources_) {
        sound->StopPlaying();
    }

    if(global_source_ != nullptr) {
        global_source_->StopPlaying();
    }
}

/* Will invalidate ALL references to AudioSource objects.
 * Only use in contexts where this is safe. */
void AudioManager::FreeSources() {
    LogInfo("Freeing all audio sources...\n");

    SilenceSources();
    global_source_.reset(nullptr);
    sources_.clear();
}

void AudioManager::FreeSamples(bool force) {
    u_assert(sources_.empty(), "audio sources weren't emptied!\n");

    LogInfo("Freeing all audio samples...\n");

    if(force) {
        /* clears absolutely everything */
        samples_.clear();
    } else {
        /* clears only those not marked with preserve */
        for(auto sample = samples_.begin(); sample != samples_.end();) {
            if(!sample->second.preserve_) {
                sample = samples_.erase(sample);
            } else {
                ++sample;
            }
        }
    }
}

/************************************************************/
/* Audio Sample */

AudioSample::~AudioSample() {
    alDeleteBuffers(1, &al_buffer_id_);
    OALCheckErrors();

    SDL_FreeWAV(data_);
}

AudioSample::AudioSample(uint8_t *data, unsigned int freq, unsigned int format, unsigned int length, bool preserve) {
    alGenBuffers(1, &al_buffer_id_);
    OALCheckErrors();
    alBufferData(al_buffer_id_, format, data, length, freq);
    OALCheckErrors();
}
