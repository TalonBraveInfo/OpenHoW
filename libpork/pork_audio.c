/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

#include "pork_engine.h"
#include "pork_audio.h"

#include "client/client_frontend.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>
#include <PL/platform_graphics_camera.h>

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

struct {
    /* extensions */
    bool ext_efx;
    bool ext_soft_buffer_samples;

    bool enabled;
} audio;

void InitAudio(void) {
    if(audio.enabled) {
        LogInfo("resetting audio...\n");
        ShutdownAudio();
    }

    memset(&audio, 0, sizeof(audio));

    ALCdevice *device = alcOpenDevice(NULL);
    if(device == NULL) {
        LogWarn("failed to open audio device, aborting audio initialisation!\n");
        ShutdownAudio();
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

        audio.ext_efx = true;
    }

    int attr[]={
            ALC_FREQUENCY, 44100, /* todo: tune this */
            ALC_MAX_AUXILIARY_SENDS, 4,
            0
    };

    ALCcontext *context = alcCreateContext(device, attr);
    if(context == NULL || !alcMakeContextCurrent(context)) {
        LogWarn("failed to create audio context, aborting audio initialisation!\n");
        ShutdownAudio();
        return;
    }

    if(alIsExtensionPresent("AL_SOFT_buffer_samples")) {
        LogInfo("AL_SOFT_buffer_samples detected\n");
        audio.ext_soft_buffer_samples = true;
    }

    alDopplerFactor(4.f);
    alDopplerVelocity(350.f);

    audio.enabled = true;
}

bool CacheAudioSample(const char *path, bool preserve) {
    if(plIsEmptyString(path)) {
        LogWarn("invalid path, aborting!\n");
        return false;
    }

    return false;
}

void StopAudio(void) {
    if(!audio.enabled) {
        return;
    }

    /* todo: stop all currently playing audio samples */
}

void SimulateAudio(void) {
    if(!audio.enabled) {
        return;
    }

    PLVector3 position = {0,0,0}, angles = {0,0,0};
    if(GetFrontendState() == FE_MODE_GAME) {
        position = g_state.camera->position;
        angles = g_state.camera->angles;
    }

    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void ShutdownAudio(void) {
    if(!audio.enabled) {
        return;
    }
    
    LogInfo("shutting down audio sub-system...\n");

    /* todo: clear sounds */

    ALCcontext *context = alcGetCurrentContext();
    if(context != NULL) {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);

        ALCdevice *device = alcGetContextsDevice(context);
        if(device != NULL) {
            alcCloseDevice(device);
        }
    }

    audio.enabled = false;
}