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

PL_EXTERN_C

void Audio_Initialize(void);
void Audio_Shutdown(void);

PL_EXTERN_C_END
