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
#include "client/frontend.h"

#ifdef __APPLE__
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
//# include <OpenAL/alext.h>
//# include <OpenAL/efx-presets.h>
#else
#   include <AL/al.h>
#   include <AL/alc.h>
#   include <AL/alext.h>
#   include <AL/efx-presets.h>
#endif

#include <PL/platform_graphics_camera.h>

/* todo: provide fallback to SDL2 Audio? maybe dynamically load OpenAL?? */
/* todo: get this working on macOS */

static void OALCheckErrors() {
    ALenum err = alGetError();
    if(err != AL_NO_ERROR) {
        /* alut is apparently deprecated in OpenAL Soft, yay... */
        /*Error("%s\n", alutGetErrorString(err));*/

        switch(err) {
            default: Error("unknown openal error, aborting!\n");
            case AL_OUT_OF_MEMORY: Error("openal ran out of memory, aborting!\n");
            case AL_INVALID_VALUE: Error("invalid value passed to openal, aborting!\n");
            case AL_INVALID_OPERATION: Error("invalid operation performed with openal, aborting!\n");
        }
    }
}

/************************************************************/

LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
LPALEFFECTI alEffecti;
LPALEFFECTIV alEffectiv;
LPALEFFECTF alEffectf;
LPALEFFECTFV alEffectfv;
LPALGETEFFECTI alGetEffecti;
LPALGETEFFECTIV alGetEffectiv;
LPALGETEFFECTF alGetEffectf;
LPALGETEFFECTFV alGetEffectfv;

LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

static bool audio_enabled = false;

enum {
    AUDIO_EXT_EFX,
    AUDIO_EXT_SOFT_BUFFER_SAMPLES,

    MAX_AUDIO_EXT_SLOTS
};
static bool audio_extensions[MAX_AUDIO_EXT_SLOTS];

void Audio_Initialize(void) {
    if(audio_enabled) {
        LogInfo("resetting audio...\n");
        Audio_Shutdown();
    }

    memset(audio_extensions, false, MAX_AUDIO_EXT_SLOTS);

    ALCdevice *device = alcOpenDevice(nullptr);
    if(device == nullptr) {
        LogWarn("failed to open audio device, aborting audio initialisation!\n");
        Audio_Shutdown();
        return;
    }

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

        audio_extensions[AUDIO_EXT_EFX] = true;
    }

    int attr[]={
            ALC_FREQUENCY, 44100, /* todo: tune this */
            ALC_MAX_AUXILIARY_SENDS, 4,
            0
    };

    ALCcontext *context = alcCreateContext(device, attr);
    if(context == nullptr || !alcMakeContextCurrent(context)) {
        LogWarn("failed to create audio context, aborting audio initialisation!\n");
        Audio_Shutdown();
        return;
    }

    if(alIsExtensionPresent("AL_SOFT_buffer_samples")) {
        LogInfo("AL_SOFT_buffer_samples detected\n");
        audio_extensions[AUDIO_EXT_SOFT_BUFFER_SAMPLES] = true;
    }

    alDopplerFactor(4.f);
    alDopplerVelocity(350.f);

    audio_enabled = true;
}

#include <AL/alut.h>

bool Audio_CacheSample(const char *path, bool preserve) {
    if(plIsEmptyString(path)) {
        LogWarn("invalid path, aborting!\n");
        return false;
    }

    /* todo: ensure the path is under <basedir>/audio/ */

    return false;
}

void Audio_Stop(void) {
    if(!audio_enabled) {
        return;
    }

    /* todo: stop all currently playing audio samples */
}

void Audio_Simulate(void) {
    if(!audio_enabled) {
        return;
    }

    PLVector3 position = {0,0,0}, angles = {0,0,0};
    if(FE_GetState() == FE_MODE_GAME) {
        position = g_state.camera->position;
        angles = g_state.camera->angles;
    }

    PLVector3 forward, left, up;
    plAnglesAxes(angles, &left, &up, &forward);

    float ori[6];
    ori[0] = forward.x; ori[3] = up.x;
    ori[1] = forward.y; ori[4] = up.y;
    ori[2] = forward.z; ori[5] = up.z;

    alListener3f(AL_POSITION, position.x, position.y, position.z);
    alListenerfv(AL_ORIENTATION, ori);
}

void Audio_Shutdown(void) {
    if(!audio_enabled) {
        return;
    }
    
    LogInfo("shutting down audio sub-system...\n");

    //Audio_StopSounds();
    //Audio_ClearSamples();

    ALCcontext *context = alcGetCurrentContext();
    if(context != nullptr) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);

        ALCdevice *device = alcGetContextsDevice(context);
        if(device != nullptr) {
            alcCloseDevice(device);
        }
    }

    audio_enabled = false;
}

class AudioSource {
public:
    AudioSource(PLVector3 pos, PLVector3 vel, float gain, float pitch, bool looping);
    ~AudioSource();

    void SetPosition(PLVector3 position);
    void SetVelocity(PLVector3 velocity);
    void SetGain(float gain);
    void SetPitch(float pitch);

    PLVector3 GetPosition() { return position_; }
    PLVector3 GetVelocity() { return velocity_; }
    float GetGain() { return gain_; }
    float GetPitch() { return pitch_; }

    void StartPlaying();
    void StopPlaying();

private:
    PLVector3 position_{0, 0, 0};
    PLVector3 velocity_{0, 0, 0};

    float gain_{1.0f};
    float pitch_{1.0f};

    unsigned int source_{0};
};

AudioSource::AudioSource(PLVector3 pos, PLVector3 vel, float gain, float pitch, bool looping) {
    alGenSources(1, &source_);
    OALCheckErrors();

    SetPosition(pos);
    SetVelocity(vel);
    SetGain(gain);
    SetPitch(pitch);

    alSourcei(source_, AL_LOOPING, looping);
    OALCheckErrors();
}

AudioSource::~AudioSource() {
    alDeleteSources(1, &source_);
}

void AudioSource::SetPosition(PLVector3 position) {
    alSource3f(source_, AL_POSITION, position.x, position.y, position.z);
    OALCheckErrors();
    position_ = position;
}

void AudioSource::SetVelocity(PLVector3 velocity) {
    alSource3f(source_, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    OALCheckErrors();
    velocity_ = velocity;
}

void AudioSource::SetGain(float gain) {
    alSourcef(source_, AL_GAIN, gain);
    OALCheckErrors();
    gain_ = gain;
}

void AudioSource::SetPitch(float pitch) {
    alSourcef(source_, AL_PITCH, pitch);
    OALCheckErrors();
    pitch_ = pitch;
}

void AudioSource::StartPlaying() {
    alSourcePlay(source_);
    OALCheckErrors();
}

static std::vector<AudioSource> audio_sources;
static std::map<std::string, unsigned int> audio_buffers;
