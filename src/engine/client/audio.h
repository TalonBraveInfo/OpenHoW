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

class AudioSource {
public:
    AudioSource(unsigned int al_sample, float gain, float pitch, bool looping);
    AudioSource(unsigned int al_sample, PLVector3 pos, PLVector3 vel, float gain, float pitch, bool looping);
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

    unsigned int al_source_id_{0};
};

class AudioManager {
    friend class AudioSource;

public:
    static AudioManager *GetInstance() {
        if(instance_ == nullptr) {
            instance_ = new AudioManager();
        }
        return instance_;
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    AudioManager();
    ~AudioManager();

    void Tick();

    void CacheSample(const std::string &path, bool preserve);

    AudioSource *CreateSource(const std::string &path, float gain = 1.0f, float pitch = 1.0f, bool looping = false);
    AudioSource *CreateSource(const std::string &path, PLVector3 pos, PLVector3 vel, float gain, float pitch, bool looping);

    void PlayGlobalSound(const std::string &path);

    void SilenceSources();

    void FreeSources();
    void FreeSamples(bool force = false);

protected:
private:
    enum {
        AUDIO_EXT_EFX,
        AUDIO_EXT_SOFT_BUFFER_SAMPLES,

        MAX_AUDIO_EXT_SLOTS
    };
    bool al_extensions_[MAX_AUDIO_EXT_SLOTS];

    typedef struct AudioSample {
        AudioSample(uint8_t *data, unsigned int freq, unsigned int format, unsigned int length, bool preserve);
        ~AudioSample();

        unsigned int    al_buffer_id_;
        uint8_t         *data_{};
        bool            preserve_;
    } AudioSample;

    std::map<std::string, AudioSample> samples_;
    std::set<AudioSource*> sources_;

    std::unique_ptr<AudioSource> global_source_;

    const AudioSample *GetCachedSample(const std::string &path);

    static AudioManager *instance_;
};
