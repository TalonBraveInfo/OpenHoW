/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
 * Copyright (C) 2017 Daniel Collins <solemnwarning@solemnwarning.net>
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

#include <PL/platform_filesystem.h>
#include <PL/platform_image.h>
#include <PL/platform_console.h>

#include "../../engine/engine.h"
#include "../../engine/util.h"

#undef LogInfo
#undef LogWarn
#undef Error
#define LogInfo(...)  plLogMessage(0, __VA_ARGS__)
#define LogWarn(...)  plLogMessage(1, __VA_ARGS__)
#define Error(...)    plLogMessage(2, __VA_ARGS__)

typedef struct CopyPath {
    const char *input, *output;
} CopyPath;

static CopyPath pc_music_paths[]={
        {"/MUSIC/Track02.ogg", "/music/"},
        {"/MUSIC/Track03.ogg", "/music/"},
        {"/MUSIC/Track04.ogg", "/music/"},
        {"/MUSIC/Track05.ogg", "/music/"},
        {"/MUSIC/Track06.ogg", "/music/"},
        {"/MUSIC/Track07.ogg", "/music/"},
        {"/MUSIC/Track08.ogg", "/music/"},
        {"/MUSIC/Track09.ogg", "/music/"},
        {"/MUSIC/Track10.ogg", "/music/"},
        {"/MUSIC/Track11.ogg", "/music/"},
        {"/MUSIC/Track12.ogg", "/music/"},
        {"/MUSIC/Track13.ogg", "/music/"},
        {"/MUSIC/Track14.ogg", "/music/"},
        {"/MUSIC/Track15.ogg", "/music/"},
        {"/MUSIC/Track16.ogg", "/music/"},
        {"/MUSIC/Track17.ogg", "/music/"},
        {"/MUSIC/Track18.ogg", "/music/"},
        {"/MUSIC/Track19.ogg", "/music/"},
        {"/MUSIC/Track20.ogg", "/music/"},
        {"/MUSIC/Track21.ogg", "/music/"},
        {"/MUSIC/Track22.ogg", "/music/"},
        {"/MUSIC/Track23.ogg", "/music/"},
        {"/MUSIC/Track24.ogg", "/music/"},
        {"/MUSIC/Track25.ogg", "/music/"},
        {"/MUSIC/Track26.ogg", "/music/"},
        {"/MUSIC/Track27.ogg", "/music/"},
        {"/MUSIC/Track28.ogg", "/music/"},
        {"/MUSIC/Track29.ogg", "/music/"},
        {"/MUSIC/Track30.ogg", "/music/"},
        {"/MUSIC/Track31.ogg", "/music/"},
        {"/MUSIC/Track32.ogg", "/music/"},
};

static CopyPath pc_copy_paths[] = {
        /* text */
        {"/FEText/BIG.tab", "/fe/text/"},
        {"/FEText/BigChars.tab", "/fe/text/"},
        {"/FEText/chars3.tab", "/fe/text/"},
        {"/FEText/CHARS2.tab", "/fe/text/"},
        {"/FEText/GameChars.tab", "/fe/text/"},
        {"/FEText/SMALL.tab", "/fe/text/"},
        {"/FEText/BIG.BMP", "/fe/text/"},
        {"/FEText/BigChars.BMP", "/fe/text/"},
        {"/FEText/chars2D.bmp", "/fe/text/"},
        {"/FEText/chars2L.bmp", "/fe/text/"},
        {"/FEText/Chars2.bmp", "/fe/text/"},
        {"/FEText/CHARS3.BMP", "/fe/text/"},
        {"/FEText/GameChars.bmp", "/fe/text/"},
        {"/FEText/SMALL.BMP", "/fe/text/"},

        /* frontend */

        {"/Language/Tims/Pigbkpc1.BMP", "/fe/"},

        /* briefing */

        {"/Language/Tims/Briefing/loadmult.bmp", "/fe/briefing/"},

        {"/Language/Tims/Briefing/Level0.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1n0.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1n1.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1n2.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1n3.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1n4.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level1n5.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level2.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level3.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level4.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/Level5.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level6.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level7.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level8.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level9.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level10.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level11.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level12.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level13.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level14.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level15.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level16.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level17.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level18.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level19.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level20.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level21.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level22.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level23.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level24.bmp", "/fe/briefing/"},
        {"/Language/Tims/Briefing/level25.bmp", "/fe/briefing/"},

        {"/Language/Tims/Title/ANY.bmp", "/fe/title/"},
        {"/Language/Tims/Title/Bruno1.bmp", "/fe/title/"},
        {"/Language/Tims/Title/KEY.bmp", "/fe/title/"},
        {"/Language/Tims/Title/PRESS.bmp", "/fe/title/"},
        {"/Language/Tims/Title/title.bmp", "/fe/title/"},
        {"/Language/Tims/Title/titlemon.bmp", "/fe/title/"},

        //{"/Language/Tims/Briefing/loadbar.bmp", "/fe/"},  // we're not using this
        //{"/Language/Tims/Debrief/deathm1.bmp", "/fe/debrief/"},
        //{"/Language/Tims/Debrief/deathm2.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc1.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc2.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc3.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc4.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc5.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc6.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc7.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc8.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/Facepc9.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw1.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw2.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw3.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw4.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw5.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw6.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw7.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw8.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/facepcw9.bmp", "/fe/debrief/"},
        //{"/Language/Tims/Debrief/Hgren.BMP", "/fe/debrief/"},
        //{"/Language/Tims/Debrief/Hpara.BMP", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcCmmndo.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcEngine.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcGrenad.BMP", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcHeavy.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcLegend.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcMedic.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcParatr.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcPip1.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcPip2.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/pcSniper.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/r_i_p.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/unifeng.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/uniffren.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/unifgerm.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/unifjap.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/uniflard.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/unifruss.bmp", "/fe/debrief/"},
        {"/Language/Tims/Debrief/unifusa.bmp", "/fe/debrief/"},

        {"/Language/Tims/Papers/American.bmp", "/fe/papers/"},
        {"/Language/Tims/Papers/British.bmp", "/fe/papers/"},
        {"/Language/Tims/Papers/French.bmp", "/fe/papers/"},
        {"/Language/Tims/Papers/German.bmp", "/fe/papers/"},
        {"/Language/Tims/Papers/Japan.bmp", "/fe/papers/"},
        {"/Language/Tims/Papers/Russian.bmp", "/fe/papers/"},
        {"/Language/Tims/Papers/teamlard.bmp", "/fe/papers/"},

        {"/Language/Streams/FMV 01.bik", "/streams/"},
        {"/Language/Streams/FMV 02.bik", "/streams/"},
        {"/Language/Streams/FMV 03.bik", "/streams/"},
        {"/Language/Streams/FMV 04.bik", "/streams/"},
        {"/Language/Streams/FMV 05.bik", "/streams/"},
        {"/Language/Streams/FMV 06.bik", "/streams/"},
        {"/Language/Streams/FMV 07.bik", "/streams/"},
        {"/Language/Streams/FMV 08.bik", "/streams/"},
        {"/Language/Streams/FMV 09.bik", "/streams/"},
        {"/Language/Streams/InfoLogo.bik", "/streams/"},
        {"/Language/Streams/Sheff.bik", "/streams/"},

        /* misc */
        {"/Language/Tims/Mine.tim", "/textures/"},
        {"/Language/Tims/wat01.TIM", "/textures/"},
        {"/Language/Tims/wat02.tim", "/textures/"},

        /* maps */
        {"/Maps/BAY.PMG", "/campaigns/how/maps/bay/"},
        {"/Maps/ICE.PMG", "/campaigns/how/maps/ice/"},
        {"/Maps/BOOM.PMG", "/campaigns/how/maps/boom/"},
        {"/Maps/BUTE.PMG", "/campaigns/how/maps/bute/"},
        {"/Maps/CAMP.PMG", "/campaigns/how/maps/camp/"},
        {"/Maps/DEMO.PMG", "/campaigns/how/maps/demo/"},
        {"/Maps/DEVI.PMG", "/campaigns/how/maps/devi/"},
        {"/Maps/DVAL.PMG", "/campaigns/how/maps/dval/"},
        {"/Maps/EASY.PMG", "/campaigns/how/maps/easy/"},
        {"/Maps/ESTU.PMG", "/campaigns/how/maps/estu/"},
        {"/Maps/FOOT.PMG", "/campaigns/how/maps/foot/"},
        {"/Maps/GUNS.PMG", "/campaigns/how/maps/guns/"},
        {"/Maps/KEEP.PMG", "/campaigns/how/maps/keep/"},
        {"/Maps/LAKE.PMG", "/campaigns/how/maps/lake/"},
        {"/Maps/MAZE.PMG", "/campaigns/how/maps/maze/"},
        {"/Maps/ROAD.PMG", "/campaigns/how/maps/road/"},
        {"/Maps/TWIN.PMG", "/campaigns/how/maps/twin/"},
        {"/Maps/ARCHI.PMG", "/campaigns/how/maps/archi/"},
        {"/Maps/BHILL.PMG", "/campaigns/how/maps/bhill/"},
        {"/Maps/CMASS.PMG", "/campaigns/how/maps/cmass/"},
        {"/Maps/DBOWL.PMG", "/campaigns/how/maps/dbowl/"},
        {"/Maps/DEMO2.PMG", "/campaigns/how/maps/demo2/"},
        {"/Maps/DVAL2.PMG", "/campaigns/how/maps/dval2/"},
        {"/Maps/EYRIE.PMG", "/campaigns/how/maps/eyrie/"},
        {"/Maps/FINAL.PMG", "/campaigns/how/maps/final/"},
        {"/Maps/HELL2.PMG", "/campaigns/how/maps/hell2/"},
        {"/Maps/HELL3.PMG", "/campaigns/how/maps/hell3/"},
        {"/Maps/MEDIX.PMG", "/campaigns/how/maps/medix/"},
        {"/Maps/MLAKE.PMG", "/campaigns/how/maps/mlake/"},
        {"/Maps/OASIS.PMG", "/campaigns/how/maps/oasis/"},
        {"/Maps/PLAY1.PMG", "/campaigns/how/maps/play1/"},
        {"/Maps/PLAY2.PMG", "/campaigns/how/maps/play2/"},
        {"/Maps/RIDGE.PMG", "/campaigns/how/maps/ridge/"},
        {"/Maps/SNAKE.PMG", "/campaigns/how/maps/snake/"},
        {"/Maps/ZULUS.PMG", "/campaigns/how/maps/zulus/"},
        {"/Maps/ARTGUN.PMG", "/campaigns/how/maps/artgun/"},
        {"/Maps/BRIDGE.PMG", "/campaigns/how/maps/bridge/"},
        {"/Maps/DESVAL.PMG", "/campaigns/how/maps/desval/"},
        {"/Maps/FJORDS.PMG", "/campaigns/how/maps/fjords/"},
        {"/Maps/GENMUD.PMG", "/campaigns/how/maps/genmud/"},
        {"/Maps/ISLAND.PMG", "/campaigns/how/maps/island/"},
        {"/Maps/LUNAR1.PMG", "/campaigns/how/maps/lunar1/"},
        {"/Maps/MASHED.PMG", "/campaigns/how/maps/mashed/"},
        {"/Maps/ONEWAY.PMG", "/campaigns/how/maps/oneway/"},
        {"/Maps/RUMBLE.PMG", "/campaigns/how/maps/rumble/"},
        {"/Maps/SEPIA1.PMG", "/campaigns/how/maps/sepia1/"},
        {"/Maps/SNIPER.PMG", "/campaigns/how/maps/sniper/"},
        {"/Maps/TRENCH.PMG", "/campaigns/how/maps/trench/"},
        {"/Maps/CREEPY2.PMG", "/campaigns/how/maps/creepy/"},
        {"/Maps/EMPLACE.PMG", "/campaigns/how/maps/emplace/"},
        {"/Maps/GENLAVA.PMG", "/campaigns/how/maps/genlava/"},
        {"/Maps/GENSNOW.PMG", "/campaigns/how/maps/gensnow/"},
        {"/Maps/ICEFLOW.PMG", "/campaigns/how/maps/iceflow/"},
        {"/Maps/LECPROD.PMG", "/campaigns/how/maps/lecprod/"},
        {"/Maps/SUPLINE.PMG", "/campaigns/how/maps/supline/"},
        {"/Maps/GENBRACK.PMG", "/campaigns/how/maps/genbrack/"},
        {"/Maps/GENCHALK.PMG", "/campaigns/how/maps/genchalk/"},
        {"/Maps/GENDESRT.PMG", "/campaigns/how/maps/gendesrt/"},
        {"/Maps/HILLBASE.PMG", "/campaigns/how/maps/hillbase/"},
        {"/Maps/LIBERATE.PMG", "/campaigns/how/maps/liberate/"},
        {"/Maps/BAY.PTG", "/campaigns/how/maps/bay/"},
        {"/Maps/ICE.PTG", "/campaigns/how/maps/ice/"},
        {"/Maps/BOOM.PTG", "/campaigns/how/maps/boom/"},
        {"/Maps/BUTE.PTG", "/campaigns/how/maps/bute/"},
        {"/Maps/CAMP.PTG", "/campaigns/how/maps/camp/"},
        {"/Maps/DEMO.PTG", "/campaigns/how/maps/demo/"},
        {"/Maps/DEVI.PTG", "/campaigns/how/maps/devi/"},
        {"/Maps/DVAL.PTG", "/campaigns/how/maps/dval/"},
        {"/Maps/EASY.PTG", "/campaigns/how/maps/easy/"},
        {"/Maps/ESTU.PTG", "/campaigns/how/maps/estu/"},
        {"/Maps/FOOT.PTG", "/campaigns/how/maps/foot/"},
        {"/Maps/GUNS.PTG", "/campaigns/how/maps/guns/"},
        {"/Maps/KEEP.PTG", "/campaigns/how/maps/keep/"},
        {"/Maps/LAKE.PTG", "/campaigns/how/maps/lake/"},
        {"/Maps/MAZE.PTG", "/campaigns/how/maps/maze/"},
        {"/Maps/ROAD.PTG", "/campaigns/how/maps/road/"},
        {"/Maps/TWIN.PTG", "/campaigns/how/maps/twin/"},
        {"/Maps/ARCHI.PTG", "/campaigns/how/maps/archi/"},
        {"/Maps/BHILL.PTG", "/campaigns/how/maps/bhill/"},
        {"/Maps/CMASS.PTG", "/campaigns/how/maps/cmass/"},
        {"/Maps/DBOWL.PTG", "/campaigns/how/maps/dbowl/"},
        {"/Maps/DEMO2.PTG", "/campaigns/how/maps/demo2/"},
        {"/Maps/DVAL2.PTG", "/campaigns/how/maps/dval2/"},
        {"/Maps/EYRIE.PTG", "/campaigns/how/maps/eyrie/"},
        {"/Maps/FINAL.PTG", "/campaigns/how/maps/final/"},
        {"/Maps/HELL2.PTG", "/campaigns/how/maps/hell2/"},
        {"/Maps/HELL3.PTG", "/campaigns/how/maps/hell3/"},
        {"/Maps/MEDIX.PTG", "/campaigns/how/maps/medix/"},
        {"/Maps/MLAKE.PTG", "/campaigns/how/maps/mlake/"},
        {"/Maps/OASIS.PTG", "/campaigns/how/maps/oasis/"},
        {"/Maps/PLAY1.PTG", "/campaigns/how/maps/play1/"},
        {"/Maps/PLAY2.PTG", "/campaigns/how/maps/play2/"},
        {"/Maps/RIDGE.PTG", "/campaigns/how/maps/ridge/"},
        {"/Maps/SNAKE.PTG", "/campaigns/how/maps/snake/"},
        {"/Maps/ZULUS.PTG", "/campaigns/how/maps/zulus/"},
        {"/Maps/ARTGUN.PTG", "/campaigns/how/maps/artgun/"},
        {"/Maps/BRIDGE.PTG", "/campaigns/how/maps/bridge/"},
        {"/Maps/DESVAL.PTG", "/campaigns/how/maps/desval/"},
        {"/Maps/FJORDS.PTG", "/campaigns/how/maps/fjords/"},
        {"/Maps/GENMUD.PTG", "/campaigns/how/maps/genmud/"},
        {"/Maps/ISLAND.PTG", "/campaigns/how/maps/island/"},
        {"/Maps/LUNAR1.PTG", "/campaigns/how/maps/lunar1/"},
        {"/Maps/MASHED.PTG", "/campaigns/how/maps/mashed/"},
        {"/Maps/ONEWAY.PTG", "/campaigns/how/maps/oneway/"},
        {"/Maps/RUMBLE.PTG", "/campaigns/how/maps/rumble/"},
        {"/Maps/SEPIA1.PTG", "/campaigns/how/maps/sepia1/"},
        {"/Maps/SNIPER.PTG", "/campaigns/how/maps/sniper/"},
        {"/Maps/TRENCH.PTG", "/campaigns/how/maps/trench/"},
        {"/Maps/CREEPY2.PTG", "/campaigns/how/maps/creepy/"},
        {"/Maps/EMPLACE.PTG", "/campaigns/how/maps/emplace/"},
        {"/Maps/GENLAVA.PTG", "/campaigns/how/maps/genlava/"},
        {"/Maps/GENSNOW.PTG", "/campaigns/how/maps/gensnow/"},
        {"/Maps/ICEFLOW.PTG", "/campaigns/how/maps/iceflow/"},
        {"/Maps/LECPROD.PTG", "/campaigns/how/maps/lecprod/"},
        {"/Maps/SUPLINE.PTG", "/campaigns/how/maps/supline/"},
        {"/Maps/genbrack.ptg", "/campaigns/how/maps/genbrack/"},
        {"/Maps/GENCHALK.PTG", "/campaigns/how/maps/genchalk/"},
        {"/Maps/GENDESRT.PTG", "/campaigns/how/maps/gendesrt/"},
        {"/Maps/HILLBASE.PTG", "/campaigns/how/maps/hillbase/"},
        {"/Maps/LIBERATE.PTG", "/campaigns/how/maps/liberate/"},
        {"/Maps/BAY.POG", "/campaigns/how/maps/bay/"},
        {"/Maps/ICE.POG", "/campaigns/how/maps/ice/"},
        {"/Maps/BOOM.POG", "/campaigns/how/maps/boom/"},
        {"/Maps/BUTE.POG", "/campaigns/how/maps/bute/"},
        {"/Maps/CAMP.POG", "/campaigns/how/maps/camp/"},
        {"/Maps/DEMO.POG", "/campaigns/how/maps/demo/"},
        {"/Maps/DEVI.POG", "/campaigns/how/maps/devi/"},
        {"/Maps/DVAL.POG", "/campaigns/how/maps/dval/"},
        {"/Maps/EASY.POG", "/campaigns/how/maps/easy/"},
        {"/Maps/ESTU.POG", "/campaigns/how/maps/estu/"},
        {"/Maps/FOOT.POG", "/campaigns/how/maps/foot/"},
        {"/Maps/GUNS.POG", "/campaigns/how/maps/guns/"},
        {"/Maps/KEEP.POG", "/campaigns/how/maps/keep/"},
        {"/Maps/LAKE.POG", "/campaigns/how/maps/lake/"},
        {"/Maps/MAZE.POG", "/campaigns/how/maps/maze/"},
        {"/Maps/ROAD.POG", "/campaigns/how/maps/road/"},
        {"/Maps/TWIN.POG", "/campaigns/how/maps/twin/"},
        {"/Maps/ARCHI.POG", "/campaigns/how/maps/archi/"},
        {"/Maps/BHILL.POG", "/campaigns/how/maps/bhill/"},
        {"/Maps/CMASS.POG", "/campaigns/how/maps/cmass/"},
        {"/Maps/DBOWL.POG", "/campaigns/how/maps/dbowl/"},
        {"/Maps/DEMO2.POG", "/campaigns/how/maps/demo2/"},
        {"/Maps/DVAL2.POG", "/campaigns/how/maps/dval2/"},
        {"/Maps/EYRIE.POG", "/campaigns/how/maps/eyrie/"},
        {"/Maps/FINAL.POG", "/campaigns/how/maps/final/"},
        {"/Maps/HELL2.POG", "/campaigns/how/maps/hell2/"},
        {"/Maps/HELL3.POG", "/campaigns/how/maps/hell3/"},
        {"/Maps/MEDIX.POG", "/campaigns/how/maps/medix/"},
        {"/Maps/MLAKE.POG", "/campaigns/how/maps/mlake/"},
        {"/Maps/OASIS.POG", "/campaigns/how/maps/oasis/"},
        {"/Maps/PLAY1.POG", "/campaigns/how/maps/play1/"},
        {"/Maps/PLAY2.POG", "/campaigns/how/maps/play2/"},
        {"/Maps/RIDGE.POG", "/campaigns/how/maps/ridge/"},
        {"/Maps/SNAKE.POG", "/campaigns/how/maps/snake/"},
        {"/Maps/ZULUS.POG", "/campaigns/how/maps/zulus/"},
        {"/Maps/ARTGUN.POG", "/campaigns/how/maps/artgun/"},
        {"/Maps/BRIDGE.POG", "/campaigns/how/maps/bridge/"},
        {"/Maps/DESVAL.POG", "/campaigns/how/maps/desval/"},
        {"/Maps/FJORDS.POG", "/campaigns/how/maps/fjords/"},
        {"/Maps/GENMUD.POG", "/campaigns/how/maps/genmud/"},
        {"/Maps/ISLAND.POG", "/campaigns/how/maps/island/"},
        {"/Maps/LUNAR1.POG", "/campaigns/how/maps/lunar1/"},
        {"/Maps/MASHED.POG", "/campaigns/how/maps/mashed/"},
        {"/Maps/ONEWAY.POG", "/campaigns/how/maps/oneway/"},
        {"/Maps/RUMBLE.POG", "/campaigns/how/maps/rumble/"},
        {"/Maps/SEPIA1.POG", "/campaigns/how/maps/sepia1/"},
        {"/Maps/SNIPER.POG", "/campaigns/how/maps/sniper/"},
        {"/Maps/TRENCH.POG", "/campaigns/how/maps/trench/"},
        {"/Maps/CREEPY2.POG", "/campaigns/how/maps/creepy/"},
        {"/Maps/EMPLACE.POG", "/campaigns/how/maps/emplace/"},
        {"/Maps/GENLAVA.POG", "/campaigns/how/maps/genlava/"},
        {"/Maps/GENSNOW.POG", "/campaigns/how/maps/gensnow/"},
        {"/Maps/ICEFLOW.POG", "/campaigns/how/maps/iceflow/"},
        {"/Maps/LECPROD.POG", "/campaigns/how/maps/lecprod/"},
        {"/Maps/SUPLINE.POG", "/campaigns/how/maps/supline/"},
        {"/Maps/GENBRACK.POG", "/campaigns/how/maps/genbrack/"},
        {"/Maps/GENCHALK.POG", "/campaigns/how/maps/genchalk/"},
        {"/Maps/GENDESRT.POG", "/campaigns/how/maps/gendesrt/"},
        {"/Maps/HILLBASE.POG", "/campaigns/how/maps/hillbase/"},
        {"/Maps/LIBERATE.POG", "/campaigns/how/maps/liberate/"},

        /* chars */
        {"/Chars/pig.HIR", "/chars/"},
        {"/Chars/mcap.mad", "/chars/"},

        /* audio */
        {"/Audio/AMB_1D.wav", "/audio/"},
        {"/Audio/AMB_1N.wav", "/audio/"},
        {"/Audio/AMB_2D.wav", "/audio/"},
        {"/Audio/AMB_2N.wav", "/audio/"},
        {"/Audio/BATT_L1.wav", "/audio/"},
        {"/Audio/BATT_L2.wav", "/audio/"},
        {"/Audio/BATT_L3.wav", "/audio/"},
        {"/Audio/BATT_S1.wav", "/audio/"},
        {"/Audio/BATT_S2.wav", "/audio/"},
        {"/Audio/BATT_S3.wav", "/audio/"},
        {"/Audio/BG_GAS.wav", "/audio/"},
        {"/Audio/BG_PLANE.wav", "/audio/"},
        {"/Audio/EN_BIP.wav", "/audio/"},
        {"/Audio/EN_TANK.wav", "/audio/"},
        {"/Audio/E_1.wav", "/audio/"},
        {"/Audio/E_1000P.wav", "/audio/"},
        {"/Audio/FT_GRASS.wav", "/audio/"},
        {"/Audio/FT_ICE.wav", "/audio/"},
        {"/Audio/FT_LAVA.wav", "/audio/"},
        {"/Audio/FT_METAL.wav", "/audio/"},
        {"/Audio/FT_MUD.wav", "/audio/"},
        {"/Audio/FT_QUAG.wav", "/audio/"},
        {"/Audio/FT_ROCK.wav", "/audio/"},
        {"/Audio/FT_SAND.wav", "/audio/"},
        {"/Audio/FT_SNOW.wav", "/audio/"},
        {"/Audio/FT_STONE.wav", "/audio/"},
        {"/Audio/FT_SWIM.wav", "/audio/"},
        {"/Audio/FT_WATER.wav", "/audio/"},
        {"/Audio/FT_WOOD.wav", "/audio/"},
        {"/Audio/I_BUILD.wav", "/audio/"},
        {"/Audio/I_BULIT1.wav", "/audio/"},
        {"/Audio/I_METAL.wav", "/audio/"},
        {"/Audio/I_PICKUP.wav", "/audio/"},
        {"/Audio/I_SPLASH.wav", "/audio/"},
        {"/Audio/I_STAB.wav", "/audio/"},
        {"/Audio/I_SWMISS.wav", "/audio/"},
        {"/Audio/I_SWORD.wav", "/audio/"},
        {"/Audio/L_ARTIL.wav", "/audio/"},
        {"/Audio/L_BAZOO.wav", "/audio/"},
        {"/Audio/L_FLAME.wav", "/audio/"},
        {"/Audio/L_HVYMG.wav", "/audio/"},
        {"/Audio/L_MGUN.wav", "/audio/"},
        {"/Audio/L_MINETR.wav", "/audio/"},
        {"/Audio/L_MORT.wav", "/audio/"},
        {"/Audio/L_PISTOL.wav", "/audio/"},
        {"/Audio/L_RIFLE.wav", "/audio/"},
        {"/Audio/L_ROC.wav", "/audio/"},
        {"/Audio/L_SHOTG.wav", "/audio/"},
        {"/Audio/P_BOUNCE.wav", "/audio/"},
        {"/Audio/P_BREAT1.wav", "/audio/"},
        {"/Audio/P_BREAT2.wav", "/audio/"},
        {"/Audio/P_BRRR.wav", "/audio/"},
        {"/Audio/P_BRUSH.wav", "/audio/"},
        {"/Audio/P_BURP.wav", "/audio/"},
        {"/Audio/P_BUSH.wav", "/audio/"},
        {"/Audio/P_CHUTE.wav", "/audio/"},
        {"/Audio/P_CHUTE1.wav", "/audio/"},
        {"/Audio/P_CLAP.wav", "/audio/"},
        {"/Audio/P_COUGH1.wav", "/audio/"},
        {"/Audio/P_COWER1.wav", "/audio/"},
        {"/Audio/P_COWER2.wav", "/audio/"},
        {"/Audio/P_COWER3.wav", "/audio/"},
        {"/Audio/P_DROWN.wav", "/audio/"},
        {"/Audio/P_EXERT.wav", "/audio/"},
        {"/Audio/P_FART1.wav", "/audio/"},
        {"/Audio/P_FART2.wav", "/audio/"},
        {"/Audio/P_FART3.wav", "/audio/"},
        {"/Audio/P_FLAP1.wav", "/audio/"},
        {"/Audio/P_HAND1.wav", "/audio/"},
        {"/Audio/P_HEAL.wav", "/audio/"},
        {"/Audio/P_HEAL1.wav", "/audio/"},
        {"/Audio/P_HMMM.wav", "/audio/"},
        {"/Audio/P_ITCH.wav", "/audio/"},
        {"/Audio/P_LAND1.wav", "/audio/"},
        {"/Audio/P_LAUGH1.wav", "/audio/"},
        {"/Audio/P_LAUGH2.wav", "/audio/"},
        {"/Audio/P_LAUGH3.wav", "/audio/"},
        {"/Audio/P_MAD1.wav", "/audio/"},
        {"/Audio/P_MAD2.wav", "/audio/"},
        {"/Audio/P_MOAN.wav", "/audio/"},
        {"/Audio/P_MUTTER.wav", "/audio/"},
        {"/Audio/P_OWW.wav", "/audio/"},
        {"/Audio/P_PANT.wav", "/audio/"},
        {"/Audio/P_PUNCH.wav", "/audio/"},
        {"/Audio/P_SICK.wav", "/audio/"},
        {"/Audio/P_SIGH.wav", "/audio/"},
        {"/Audio/P_SLIP.wav", "/audio/"},
        {"/Audio/P_SNEEZE.wav", "/audio/"},
        {"/Audio/P_SNIFF.wav", "/audio/"},
        {"/Audio/P_SNORT1.wav", "/audio/"},
        {"/Audio/P_SNORT2.wav", "/audio/"},
        {"/Audio/P_SQUEA1.wav", "/audio/"},
        {"/Audio/P_SQUEA2.wav", "/audio/"},
        {"/Audio/P_WHIST1.wav", "/audio/"},
        {"/Audio/P_WHISTL.wav", "/audio/"},
        {"/Audio/P_WHOHO.wav", "/audio/"},
        {"/Audio/P_WHOOPE.wav", "/audio/"},
        {"/Audio/S_CLOCK.wav", "/audio/"},
        {"/Audio/S_OPEN.wav", "/audio/"},
        {"/Audio/S_SELECT.wav", "/audio/"},
        {"/Audio/S_UNHOLS.wav", "/audio/"},

        { "/Speech/Sku1/Pig01/01JA0105.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0403.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0204.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0504.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0404.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0301.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0503.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0105.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0405.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0506.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0504.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0504.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0506.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0203.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0204.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0306.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0501.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0201.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0206.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0402.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0301.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0203.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0101.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0205.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0505.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0405.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0405.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0104.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0102.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0504.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0106.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0406.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0201.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0402.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0406.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0203.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0301.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0305.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0503.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0401.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0306.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0303.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0406.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0101.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0402.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0205.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0104.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0203.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0304.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0205.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0403.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0105.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0106.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0305.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0403.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0202.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0104.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0501.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0302.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0204.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0201.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0202.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0302.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0402.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0106.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0505.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0305.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0106.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0501.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0302.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0506.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0506.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0105.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0105.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0306.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0206.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0206.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0306.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0103.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0302.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0404.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0401.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0303.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0405.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0203.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0101.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0305.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0301.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0502.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0302.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0103.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0401.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0404.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0304.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0206.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0405.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0205.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0205.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0401.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0106.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0501.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0202.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0205.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0403.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0201.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0503.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0504.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0206.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0503.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0503.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0301.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0101.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0104.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0304.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0502.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0402.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0102.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0103.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0305.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0103.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0506.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0502.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0303.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0404.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0502.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0103.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0101.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0102.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0401.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0402.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0403.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0101.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0304.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0501.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0505.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0104.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0201.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0203.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0302.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0401.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0304.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0404.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0103.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0202.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0301.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0404.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0204.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0306.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0304.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0303.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0506.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0505.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0206.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0406.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0202.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0303.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0202.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0106.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0305.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0204.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0505.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0204.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0505.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0102.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0403.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0502.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0502.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0104.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0406.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0504.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0503.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0406.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01EN0405.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01ge0306.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01JA0501.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0102.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01AM0105.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0303.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01fr0102.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig01/01RU0201.wav", "/speech/sku1/pig01/" },
        { "/Speech/Sku1/Pig03/03JA0101.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0501.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0103.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0202.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0506.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0205.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0402.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0301.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0201.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0204.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0501.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0405.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0306.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0206.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0306.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0106.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0504.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0403.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0305.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0206.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0303.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0305.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0301.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0305.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0402.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0106.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0105.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0206.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0506.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0406.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0502.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0101.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0505.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0306.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0202.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0403.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0104.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0404.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0403.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0504.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0206.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0401.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0502.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0106.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0505.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0204.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0503.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0501.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0102.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0101.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0405.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0205.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0204.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0503.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0102.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0401.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0203.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0403.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0203.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0406.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0402.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0406.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0305.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0405.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0303.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0201.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0102.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0305.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0403.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0201.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0303.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0205.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0203.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0302.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0403.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0302.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0401.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0106.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0502.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0201.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0505.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0304.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0104.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0202.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0304.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0306.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0405.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0101.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0104.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0504.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0302.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0301.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0306.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0406.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0401.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0503.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0202.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0103.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0102.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0505.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0204.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0106.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0401.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0501.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0103.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0104.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0205.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0105.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0302.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0501.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0504.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0504.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0503.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0201.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0106.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0402.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0301.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0405.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0505.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0101.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0501.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0404.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0406.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0304.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0204.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0306.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0305.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0304.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0202.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0506.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0302.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0202.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0303.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0205.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0506.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0503.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0103.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0105.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0506.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0203.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0502.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0201.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0303.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0104.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0404.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0404.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0404.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0303.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0406.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0102.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0503.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0301.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0204.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0105.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0304.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0401.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0206.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0402.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0405.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0105.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0302.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0502.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0102.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0205.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0105.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0103.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0101.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0402.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0506.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03RU0404.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0504.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0104.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03AM0103.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0203.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0203.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03GE0301.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0505.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03FR0304.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03EN0206.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig03/03JA0502.wav", "/speech/sku1/pig03/" },
        { "/Speech/Sku1/Pig08/08AM0302.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0305.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0401.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0303.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0504.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0301.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0303.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0206.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0401.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0104.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0202.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0505.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0306.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0403.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0206.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0203.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0401.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0201.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0501.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0506.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0503.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0303.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0505.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0505.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0105.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0404.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0503.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0204.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0101.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0104.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0301.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0105.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0503.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0503.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0206.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0101.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0404.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0502.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0404.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0105.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0102.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0201.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0302.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0203.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0505.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0506.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0402.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0205.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0405.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0402.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0405.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0205.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0406.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0401.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0501.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0301.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0306.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0402.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0304.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0104.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0101.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0204.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0102.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0504.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0203.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0504.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0504.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0101.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0304.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0406.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0406.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0306.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0305.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0203.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0106.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0301.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0501.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0201.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0404.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0104.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0405.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0205.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0302.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0302.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0106.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0302.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0506.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0504.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0403.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0402.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0105.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0205.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0301.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0403.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0103.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0106.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0405.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0203.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0405.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0404.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0304.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0202.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0103.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0506.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0305.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0106.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08ge0305.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0104.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0104.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0503.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0103.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0406.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0406.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0505.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0401.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0106.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0202.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0402.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0206.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0103.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0102.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0106.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0102.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0201.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0206.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0101.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0403.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0404.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0403.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0102.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0506.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0101.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0105.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0304.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0405.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0204.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0204.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08ge0306.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0305.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0203.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0402.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0303.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0502.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0406.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0302.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0305.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0306.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0501.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0501.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0505.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0205.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0504.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0303.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0304.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0204.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0501.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0201.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0502.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0202.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0502.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08EN0202.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0204.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0206.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0102.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0304.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0506.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0403.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0103.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0301.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0306.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0205.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0401.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0201.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0103.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08JA0502.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0503.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08AM0502.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08RU0303.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08FR0202.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig08/08GE0105.wav", "/speech/sku1/pig08/" },
        { "/Speech/Sku1/Pig05/05RU0203.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0506.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0304.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0105.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0302.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0405.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0304.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0205.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0306.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0201.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0201.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0303.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0501.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0503.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0203.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0204.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0201.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0302.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0203.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0305.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0305.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0503.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0106.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0105.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0505.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0402.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0201.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0201.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0402.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0202.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0403.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0304.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0103.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0101.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0206.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0102.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0504.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0206.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0406.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0404.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0204.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0405.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0306.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0103.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0203.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0106.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0204.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0503.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0406.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0501.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0105.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0203.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0205.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0405.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0504.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0505.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0503.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0201.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0502.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0503.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0104.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0305.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0102.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0502.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0506.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0501.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0302.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0505.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0406.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0104.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0505.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0403.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0104.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0401.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0401.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0402.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0402.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0205.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0305.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0504.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0501.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0102.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0206.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0305.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0106.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0403.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0203.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0406.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0103.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0301.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0302.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0104.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0406.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0304.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0404.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0302.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0202.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0106.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0505.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0301.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0502.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0503.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0202.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0402.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0502.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0204.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0105.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0103.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0404.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0302.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0204.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0101.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0106.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0204.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0206.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0105.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0306.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0301.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0105.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0502.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0306.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0402.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0104.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0405.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0405.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0304.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0101.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0301.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0202.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0404.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0103.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0202.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0306.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0301.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0305.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0405.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0104.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0404.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0401.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0506.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0401.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0403.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0202.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0403.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0505.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0401.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0304.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0504.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0501.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0102.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0506.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0504.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0106.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0303.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0205.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0102.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0206.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0404.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0103.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0306.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0506.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0101.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0303.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0101.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0506.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0303.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0303.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0101.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0401.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0403.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05EN0205.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05ge0502.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0102.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05JA0301.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05RU0406.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0206.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0501.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05AM0205.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0303.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig05/05FR0504.wav", "/speech/sku1/pig05/" },
        { "/Speech/Sku1/Pig02/02RU0504.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0203.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0105.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0101.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0502.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0501.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0302.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0403.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0404.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0406.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0504.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0501.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0401.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0304.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0204.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0204.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0404.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0106.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0404.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0504.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0203.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0106.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0405.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0501.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0202.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0104.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0404.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0403.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0202.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0503.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0302.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0402.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0501.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0303.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0205.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0406.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0402.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0105.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0305.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0305.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0405.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0102.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0302.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0104.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0102.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0106.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0301.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0505.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0406.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0101.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0506.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0102.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0103.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0202.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0501.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0101.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0105.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0201.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0203.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0203.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0302.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0502.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0401.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0103.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0105.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0303.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0303.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0303.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0101.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0503.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0504.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0206.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0106.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0205.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0502.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0201.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0503.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0203.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0503.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0301.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0101.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0102.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0304.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0306.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0403.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0403.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0505.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0304.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0205.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0305.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0103.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0302.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0405.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0506.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0202.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0306.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0201.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0403.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0506.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0101.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0305.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0306.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0404.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0103.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0402.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0205.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0206.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0305.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0405.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0401.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0205.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0104.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0505.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0201.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0505.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0506.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0304.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0301.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0206.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0505.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0206.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0406.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0302.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0206.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0306.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0402.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0406.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0503.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0204.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0401.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0501.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0304.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0502.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0103.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0301.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0102.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0506.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0401.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0505.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0405.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0405.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0103.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0104.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0301.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0205.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0402.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0104.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0303.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0201.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0106.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0202.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0303.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0402.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0504.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0203.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0104.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0106.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0202.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0204.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0301.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0502.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0502.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0403.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0503.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02GE0102.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0201.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0304.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0506.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0204.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02FR0404.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0105.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0504.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02EN0105.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0401.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0306.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0206.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02AM0406.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02fr0305.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02JA0306.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig02/02RU0204.wav", "/speech/sku1/pig02/" },
        { "/Speech/Sku1/Pig04/04FR0502.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0404.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0501.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0206.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0306.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0501.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0305.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0305.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0101.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0301.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0103.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0403.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0203.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0406.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0204.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0402.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0306.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0106.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0202.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0405.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0505.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0404.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0203.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0303.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0506.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0406.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0401.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0401.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0102.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0402.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0506.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0401.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0405.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0104.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0304.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0203.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0202.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0201.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0305.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0304.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0301.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0503.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0104.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0106.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0501.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0301.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0303.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0502.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0205.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0504.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0406.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0105.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0102.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0304.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0401.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0105.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0101.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0402.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0403.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0502.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0205.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0403.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0205.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0506.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0105.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0503.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0106.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0304.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0203.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0302.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0102.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0506.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0302.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0202.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0503.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0206.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0106.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0406.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0301.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0204.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0202.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0201.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0104.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0506.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0503.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0103.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0404.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0103.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0505.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0305.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0504.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0303.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0203.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0201.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0404.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0201.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0503.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0206.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0306.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0404.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0305.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0504.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0502.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0506.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0201.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0306.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0202.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0104.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0101.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0404.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0206.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0102.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0106.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0102.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0205.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0205.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0405.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0405.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0401.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0303.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0403.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0206.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0205.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0101.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0203.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0402.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0406.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0502.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0103.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0306.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0504.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0501.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0302.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0504.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0102.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0501.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0303.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0503.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0504.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0406.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0302.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0405.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0403.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0105.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0304.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0505.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0302.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0502.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0505.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0103.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0101.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0402.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0402.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0304.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0106.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0105.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0103.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0204.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0105.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0305.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0104.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0303.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04EN0302.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0301.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0204.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0204.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0501.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0201.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0403.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0202.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04AM0505.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0104.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04GE0206.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0306.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0301.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0505.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0401.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04JA0405.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04FR0101.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Pig04/04RU0204.wav", "/speech/sku1/pig04/" },
        { "/Speech/Sku1/Sarge/SGEN1403.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0105.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0208.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2104.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0207.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2203.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1606.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0507.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1402.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1103.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0106.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2205.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0504.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0407.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0505.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2108.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0903.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1406.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0904.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0607.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1301.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1805.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1308.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0307.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2206.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1905.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0708.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2007.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1706.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1202.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1902.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0405.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2003.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1307.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0701.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1502.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0502.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0608.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0102.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1107.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0408.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1005.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0503.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0402.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1608.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0803.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1701.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0908.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0305.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1104.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1404.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1604.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0805.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0202.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1906.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0702.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1106.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1001.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0906.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0703.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1302.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2005.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1102.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0802.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0806.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0103.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0902.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2008.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0304.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0404.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1508.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1207.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1903.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1203.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1708.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1401.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0308.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1807.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1801.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1408.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0104.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1004.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0204.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1407.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1908.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1305.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1808.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1007.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0101.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0506.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1605.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0601.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1008.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1501.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0807.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2101.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1607.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2202.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1901.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0704.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1804.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2103.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1105.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0403.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1205.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1505.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1306.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0203.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0302.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0901.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1802.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0306.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1002.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1108.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2204.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1705.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1601.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1907.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0301.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1201.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1506.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0905.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0201.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1904.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1003.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0707.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0603.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0907.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0108.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1101.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2004.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2006.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1507.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1204.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0706.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2106.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0406.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2207.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0604.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2107.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2105.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0206.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2102.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1703.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1006.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0804.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2201.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1504.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1702.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1602.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1208.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1603.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0508.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1304.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2002.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0606.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0808.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0205.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1704.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1707.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1803.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1206.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0401.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1405.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0501.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0107.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0602.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1303.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1503.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0303.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0705.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN2001.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0801.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN1806.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Sarge/SGEN0605.wav", "/speech/sku1/sarge/" },
        { "/Speech/Sku1/Train1/tr1_en13.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en17.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en06.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en05.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en25.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en15.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en11.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en28.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en09.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en16.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en08.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en23.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en10.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en24.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en12.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en03.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en22.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en04.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en07.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en20.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en27.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en14.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en19.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en18.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en26.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en01.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en21.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Train1/tr1_en02.wav", "/speech/sku1/train1/" },
        { "/Speech/Sku1/Pig09/09RU0301.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0506.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0403.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0105.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0103.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0203.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0203.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0206.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0306.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0302.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0101.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0302.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0404.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0401.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0402.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0506.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0406.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0104.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0305.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0404.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0104.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0404.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0303.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0403.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0101.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0303.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0104.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0205.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0206.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0302.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0403.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0303.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0502.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0106.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0105.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0204.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0502.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0504.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0105.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0502.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0303.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0205.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0302.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0204.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0105.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0201.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0104.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0204.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0401.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0106.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0505.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0301.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0305.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0401.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0402.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0201.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0502.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0503.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0103.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0501.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0506.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0105.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0405.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0404.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0305.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0206.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0504.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0503.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0502.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0101.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0503.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0304.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0202.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0202.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0501.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0302.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0504.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0501.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0101.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0103.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0504.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0201.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0401.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0403.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0402.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0503.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0301.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0406.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0102.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0405.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0106.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0203.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0106.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0503.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0104.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0504.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0405.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0402.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0102.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0501.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0106.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0406.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0206.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0203.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0505.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0201.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0405.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0102.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0301.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0303.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0102.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0306.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0305.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0304.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0506.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0102.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0205.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0403.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0101.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0502.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0206.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0101.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0505.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0404.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0401.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0406.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0301.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0201.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0501.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0205.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0406.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0201.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0506.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0303.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0306.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0306.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0103.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0304.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0504.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0306.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0405.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0205.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0304.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0301.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0102.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0202.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0204.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0105.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0304.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0505.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0302.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0203.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0404.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0402.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0406.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0505.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0103.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0403.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0505.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0202.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0501.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0103.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0306.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0305.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0405.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0205.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0506.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0305.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0202.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0204.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09GE0106.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09JA0206.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0402.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09RU0401.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09EN0104.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0304.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09FR0204.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0203.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0503.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig09/09AM0202.wav", "/speech/sku1/pig09/" },
        { "/Speech/Sku1/Pig07/07GE0301.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0404.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0403.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0101.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0305.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0303.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0503.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0102.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0202.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0303.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0202.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0502.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0504.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0504.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0301.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0304.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0406.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0102.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0506.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0201.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0306.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0501.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0206.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0503.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0301.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0203.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0304.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0301.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0402.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0304.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0201.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0406.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0204.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0201.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0103.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0401.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0303.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0506.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0106.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0504.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0401.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0305.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0202.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0503.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0302.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0103.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0405.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0406.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0502.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0102.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0406.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0302.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0104.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0404.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0204.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0201.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0506.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0105.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0404.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0505.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0304.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0203.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0405.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0503.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0205.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0204.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0104.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0304.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0101.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0306.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0401.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0105.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0505.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0402.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0306.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0206.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0306.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0101.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0402.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0501.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0201.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0403.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0305.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0206.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0104.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0202.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0402.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0501.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0402.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0505.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0102.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0502.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0504.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0505.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0206.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0106.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0104.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0105.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0202.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0505.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0106.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0305.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0106.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0403.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0205.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0403.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0401.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0401.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0105.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0102.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0401.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0501.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0206.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0306.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0203.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0204.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0103.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0204.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0502.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0105.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0106.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0205.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0104.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0402.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0405.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0404.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0305.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0103.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0206.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0102.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0406.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0503.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0404.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0103.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0505.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0303.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0201.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0405.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0404.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0506.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0301.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0503.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0101.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0504.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0406.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0203.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0501.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0103.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0306.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0302.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0405.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0205.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0202.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0506.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0504.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0305.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0101.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0405.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0301.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0302.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0502.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0203.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0303.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0304.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0203.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0501.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0205.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0101.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0204.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07RU0403.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0302.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0302.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0104.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0303.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07JA0502.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0106.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07AM0403.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07FR0506.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07EN0205.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig07/07GE0105.wav", "/speech/sku1/pig07/" },
        { "/Speech/Sku1/Pig06/06RU0106.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0103.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0303.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0504.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0306.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0205.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0104.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0204.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0104.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0302.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0506.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0103.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0303.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0502.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0402.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0506.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0203.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0405.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0105.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0204.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0202.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0401.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0304.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0503.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0305.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0102.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0506.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0403.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0402.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0101.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0104.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0405.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0104.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0306.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0303.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0106.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0404.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0301.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0101.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0105.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0205.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0102.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0404.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0203.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0102.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0206.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0103.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0301.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0306.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0202.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0206.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0401.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0101.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0401.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0502.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0406.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0203.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0405.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0101.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0105.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0504.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0306.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0201.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0203.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0306.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0301.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0505.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0106.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0406.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0305.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0105.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0505.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0304.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0104.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0105.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0301.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0303.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0504.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0104.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0401.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0102.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0203.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0302.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0304.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0301.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0103.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0404.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0403.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0206.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0202.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0205.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0205.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0402.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0106.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0502.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0304.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0405.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0503.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0201.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0502.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0501.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0403.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0503.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0501.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0106.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0501.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0404.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0202.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0506.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0406.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0505.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0106.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0204.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0201.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0503.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0406.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0206.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0101.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0103.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0306.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0502.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0302.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0305.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0102.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0503.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0302.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0504.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0403.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0403.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0204.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0405.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0504.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0105.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0305.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0305.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0404.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0405.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0303.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0503.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0201.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0304.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0505.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0502.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0201.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0404.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0302.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0202.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0102.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0501.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0304.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0203.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0506.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0303.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0505.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0205.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0406.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0402.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0205.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0402.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0301.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0305.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0403.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0302.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0402.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0204.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0501.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0501.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0401.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0506.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0101.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0206.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0201.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0406.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06EN0202.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06RU0206.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0505.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06FR0401.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06JA0103.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06AM0204.wav", "/speech/sku1/pig06/" },
        { "/Speech/Sku1/Pig06/06GE0504.wav", "/speech/sku1/pig06/" },

        {"/FESounds/click1.wav", "/audio/fesounds/"},
        {"/FESounds/click2.wav", "/audio/fesounds/"},
        {"/FESounds/click3.wav", "/audio/fesounds/"},
        {"/FESounds/click4.wav", "/audio/fesounds/"},
        {"/FESounds/click5.wav", "/audio/fesounds/"},
};

static CopyPath pc_package_paths[] = {
        {"/Chars/british.mad",          "/chars/pigs/"},    /* actually contains all the pig models */
        {"/Chars/FACES.MTD",            "/chars/faces/"},   /* contains all the face textures       */

        /* hats */
        {"/Chars/BRITHATS.MAD",         "/chars/hats/"},
        {"/Chars/FHATS.MAD",            "/chars/hats/"},

        /* teams */
        {"/Chars/british.mtd",          "/chars/british/"},
        {"/Chars/AMERICAN.MTD",         "/chars/american/"},
        {"/Chars/FRENCH.MTD",           "/chars/french/"},
        {"/Chars/GERMAN.MTD",           "/chars/german/"},
        {"/Chars/JAPANESE.MTD",         "/chars/japanese/"},
        {"/Chars/RUSSIAN.MTD",          "/chars/russian/"},
        {"/Chars/TEAMLARD.MTD",         "/chars/teamlard/"},

        /* sky */
        {"/Chars/SKYDOME.MAD",          "/skys/"},
        //{"/Chars/SKYDOME.MTD", "/chars/sky/"},    /* unused ?? */
        {"/Chars/TOY.MAD",              "/skys/toy/"},
        {"/Chars/SPACE.MAD",            "/skys/space/"},
        {"/Chars/SUNSET.MAD",           "/skys/sunset/"},
        {"/Chars/SUNRISE.MAD",          "/skys/sunrise/"},
        {"/Chars/SUNNY.MAD",            "/skys/sunny/"},
        {"/Chars/OMINOUS.MAD",          "/skys/ominous/"},
        {"/Chars/DESERT.MAD",           "/skys/desert/"},
        {"/Chars/COLDSKY.MAD",          "/skys/coldsky/"},
        {"/Chars/NIGHT1.MAD",           "/skys/night1/"},

        /* weapons */
        {"/Chars/WEAPONS.MAD",          "/chars/weapons/"},
        {"/Chars/WEAPONS.MTD",          "/chars/weapons/"},

        /* frontend */
        {"/FEBmps/FEBMP.MAD",           "/fe/bitmaps/"},
        {"/Language/Tims/FEFXTIMS.MTD", "/fe/fx/"},
        {"/Language/Tims/EXPLTIMS.MAD", "/fe/expl/"},
        {"/Language/Tims/FACETIMS.MAD", "/fe/dash/"},
        {"/Language/Tims/FLAGTIMS.MAD", "/fe/dash/"},
        {"/Language/Tims/dashtims.mad", "/fe/dash/"},
        {"/Language/Tims/MAPICONS.MTD", "/fe/map/"},
        {"/Language/Tims/MENUTIMS.MAD", "/fe/dash/menu/"},
        {"/Language/Tims/TBOXTIMS.MAD", "/fe/dash/"},

#if 0
        {"/Maps/BAY.PTG",        "/campaigns/how/maps/bay/tiles/"},
        {"/Maps/ICE.PTG",        "/campaigns/how/maps/ice/tiles/"},
        {"/Maps/BOOM.PTG",       "/campaigns/how/maps/boom/tiles/"},
        {"/Maps/BUTE.PTG",       "/campaigns/how/maps/bute/tiles/"},
        {"/Maps/CAMP.PTG",       "/campaigns/how/maps/camp/tiles/"},
        {"/Maps/DEMO.PTG",       "/campaigns/how/maps/demo/tiles/"},
        {"/Maps/DEVI.PTG",       "/campaigns/how/maps/devi/tiles/"},
        {"/Maps/DVAL.PTG",       "/campaigns/how/maps/dval/tiles/"},
        {"/Maps/EASY.PTG",       "/campaigns/how/maps/easy/tiles/"},
        {"/Maps/ESTU.PTG",       "/campaigns/how/maps/estu/tiles/"},
        {"/Maps/FOOT.PTG",       "/campaigns/how/maps/foot/tiles/"},
        {"/Maps/GUNS.PTG",       "/campaigns/how/maps/guns/tiles/"},
        {"/Maps/KEEP.PTG",       "/campaigns/how/maps/keep/tiles/"},
        {"/Maps/LAKE.PTG",       "/campaigns/how/maps/lake/tiles/"},
        {"/Maps/MAZE.PTG",       "/campaigns/how/maps/maze/tiles/"},
        {"/Maps/ROAD.PTG",       "/campaigns/how/maps/road/tiles/"},
        {"/Maps/TWIN.PTG",       "/campaigns/how/maps/twin/tiles/"},
        {"/Maps/ARCHI.PTG",      "/campaigns/how/maps/archi/tiles/"},
        {"/Maps/BHILL.PTG",      "/campaigns/how/maps/bhill/tiles/"},
        {"/Maps/CMASS.PTG",      "/campaigns/how/maps/cmass/tiles/"},
        {"/Maps/DBOWL.PTG",      "/campaigns/how/maps/dbowl/tiles/"},
        {"/Maps/DEMO2.PTG",      "/campaigns/how/maps/demo2/tiles/"},
        {"/Maps/DVAL2.PTG",      "/campaigns/how/maps/dval2/tiles/"},
        {"/Maps/EYRIE.PTG",      "/campaigns/how/maps/eyrie/tiles/"},
        {"/Maps/FINAL.PTG",      "/campaigns/how/maps/final/tiles/"},
        {"/Maps/HELL2.PTG",      "/campaigns/how/maps/hell2/tiles/"},
        {"/Maps/HELL3.PTG",      "/campaigns/how/maps/hell3/tiles/"},
        {"/Maps/MEDIX.PTG",      "/campaigns/how/maps/medix/tiles/"},
        {"/Maps/MLAKE.PTG",      "/campaigns/how/maps/mlake/tiles/"},
        {"/Maps/OASIS.PTG",      "/campaigns/how/maps/oasis/tiles/"},
        {"/Maps/PLAY1.PTG",      "/campaigns/how/maps/play1/tiles/"},
        {"/Maps/PLAY2.PTG",      "/campaigns/how/maps/play2/tiles/"},
        {"/Maps/RIDGE.PTG",      "/campaigns/how/maps/ridge/tiles/"},
        {"/Maps/SNAKE.PTG",      "/campaigns/how/maps/snake/tiles/"},
        {"/Maps/ZULUS.PTG",      "/campaigns/how/maps/zulus/tiles/"},
        {"/Maps/ARTGUN.PTG",     "/campaigns/how/maps/artgun/tiles/"},
        {"/Maps/BRIDGE.PTG",     "/campaigns/how/maps/bridge/tiles/"},
        {"/Maps/DESVAL.PTG",     "/campaigns/how/maps/desval/tiles/"},
        {"/Maps/FJORDS.PTG",     "/campaigns/how/maps/fjords/tiles/"},
        {"/Maps/GENMUD.PTG",     "/campaigns/how/maps/genmud/tiles/"},
        {"/Maps/ISLAND.PTG",     "/campaigns/how/maps/island/tiles/"},
        {"/Maps/LUNAR1.PTG",     "/campaigns/how/maps/lunar1/tiles/"},
        {"/Maps/MASHED.PTG",     "/campaigns/how/maps/mashed/tiles/"},
        {"/Maps/ONEWAY.PTG",     "/campaigns/how/maps/oneway/tiles/"},
        {"/Maps/RUMBLE.PTG",     "/campaigns/how/maps/rumble/tiles/"},
        {"/Maps/SEPIA1.PTG",     "/campaigns/how/maps/sepia1/tiles/"},
        {"/Maps/SNIPER.PTG",     "/campaigns/how/maps/sniper/tiles/"},
        {"/Maps/TRENCH.PTG",     "/campaigns/how/maps/trench/tiles/"},
        {"/Maps/CREEPY2.PTG",    "/campaigns/how/maps/creepy/tiles/"},
        {"/Maps/EMPLACE.PTG",    "/campaigns/how/maps/emplace/tiles/"},
        {"/Maps/GENLAVA.PTG",    "/campaigns/how/maps/genlava/tiles/"},
        {"/Maps/GENSNOW.PTG",    "/campaigns/how/maps/gensnow/tiles/"},
        {"/Maps/ICEFLOW.PTG",    "/campaigns/how/maps/iceflow/tiles/"},
        {"/Maps/LECPROD.PTG",    "/campaigns/how/maps/lecprod/tiles/"},
        {"/Maps/SUPLINE.PTG",    "/campaigns/how/maps/supline/tiles/"},
        {"/Maps/genbrack.ptg",   "/campaigns/how/maps/genbrack/tiles/"},
        {"/Maps/GENCHALK.PTG",   "/campaigns/how/maps/genchalk/tiles/"},
        {"/Maps/GENDESRT.PTG",   "/campaigns/how/maps/gendesrt/tiles/"},
        {"/Maps/HILLBASE.PTG",   "/campaigns/how/maps/hillbase/tiles/"},
        {"/Maps/LIBERATE.PTG",   "/campaigns/how/maps/liberate/tiles/"},

        {"/Skys/COLD/COLD1.PTG", "/skys/cold/"},
#endif
};

/* Extraction process for initial setup */

static void ExtractPTGPackage(const char *input_path, const char *output_path) {
    u_assert(input_path != NULL && input_path[0] != '\0', "encountered invalid path for PTG!\n");
    char ptg_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(ptg_name, sizeof(ptg_name), plGetFileName(input_path));
    pl_strtolower(ptg_name);

    if(!plCreatePath(output_path)) {
        LogInfo("failed to create path %s, aborting!\n", output_path);
        return;
    }

    FILE *file = fopen(input_path, "rb");
    if(file == NULL) {
        LogInfo("failed to load %s, aborting!\n", input_path);
        return;
    }

    FILE *out = NULL;

    uint32_t num_textures;
    if(fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
        LogInfo("invalid PTG file, failed to get number of textures!\n");
        goto ABORT_PTG;
    }

    size_t tim_size = (plGetFileSize(input_path) - sizeof(num_textures)) / num_textures;
    for(unsigned int i = 0; i < num_textures; ++i) {
        uint8_t tim[tim_size];
        if(fread(tim, tim_size, 1, file) != 1) {
            LogInfo("failed to read tim, aborting!\n");
            goto ABORT_PTG;
        }

        char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(out_path, "%s/%d.tim", output_path, i);
        out = fopen(out_path, "wb");
        if(out == NULL) {
            LogInfo("failed to open %s for writing, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        //print(" %s\n", out_path);
        if(fwrite(tim, tim_size, 1, out) != 1) {
            LogInfo("failed to write %s, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        u_fclose(out);
    }

    ABORT_PTG:
    u_fclose(out);
    u_fclose(file);
}

static void ExtractMADPackage(const char *input_path, const char *output_path) {
    u_assert(input_path != NULL && input_path[0] != '\0', "encountered invalid path for MAD!\n");
    char package_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(package_name, sizeof(package_name), plGetFileName(input_path));
    pl_strtolower(package_name);

    if(!plCreatePath(output_path)) {
        LogInfo("failed to create input_path %s, aborting!\n", output_path);
        return;
    }

    FILE *file = fopen(input_path, "rb");
    if(file == NULL) {
        LogInfo("failed to load %s, aborting!\n", input_path);
        return;
    }

    char package_extension[4] = {'\0'};
    snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(input_path));
    pl_strtolower(package_extension);

    FILE *out_index = NULL;
    // check if it's necessary for us to produce an index file
    // since models expect each texture to have a particular
    // position within the MTD package - yay...
    if(strcmp(package_extension, "mtd") == 0) {
        char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        snprintf(index_path, sizeof(index_path), "%s/%s.index", output_path, package_name);
        out_index = fopen(index_path, "w");
        if (out_index == NULL) {
            printf("failed to open %s for writing!\n", index_path);
        }
    }

    typedef struct __attribute__((packed)) MADIndex {
        char file[16];

        uint32_t offset;
        uint32_t length;
    } MADIndex;

    uint8_t *data = NULL;
    FILE *out = NULL;
    unsigned int lowest_offset = UINT32_MAX;
    unsigned int cur_index = 0;
    long position;
    do {
        MADIndex index; cur_index++;
        if(fread(&index, sizeof(MADIndex), 1, file) != 1) {
            LogInfo("invalid index size for %s, aborting!\n", package_name);
            goto ABORT_MAD;
        }

        position = ftell(file);
        if(lowest_offset > index.offset) {
            lowest_offset = index.offset;
        }

        // this is where the fun begins...

        // how uses mixed case file-names, to make things
        // easier for linux/macos support we'll just output_path
        // everything as lowercase
        pl_strtolower(index.file);

        char file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(file_path, "%s/%s", output_path, index.file);

        // then check if we need to amend our index file
        if(out_index != NULL) {
            // remove the extension, since we're going to support
            // multiple formats for texture loading, and it's
            // unnecessary to read it in later when we're determining
            // what texture to load :)
            char index_file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
            plStripExtension(index_file_path, sizeof(index_file_path), index.file);
            fprintf(out_index, "%s\n", index_file_path);
        }

        CHECK_AGAIN:
        // check if we're throwing out any duplicates when copying
        // everything over. technically this shouldn't occur but
        // is primarily being left here for now just for debugging
        // purposes - eventually once we're settled with both the
        // PC and PSX content, we can probably remove it
        if(plFileExists(file_path)) {
            size_t size = plGetFileSize(file_path);
            if(size == index.length) {
                //LogInfo("duplicate file found for %s at %s, skipping!\n", index.file, file_path);
                continue;
            }

            // this part should never happen, but we'll check for it anyway, call me paranoid!
            LogInfo("duplicate file found for %s at %s with differing size (%d vs %zu), renaming!\n",
                  index.file, file_path, index.length, size);
            strcat(file_path, "_");
            goto CHECK_AGAIN;
        }

        if((data = calloc(index.length, sizeof(uint8_t))) == NULL) {
            Error("failed to allocate data handle!\n");
        }

        // go and grab the data so we can export!
        fseek(file, index.offset, SEEK_SET);
        if(fread(data, sizeof(uint8_t), index.length, file) != index.length) {
            LogInfo("failed to read %s in %s, aborting!\n", index.file, package_name);
            goto ABORT_MAD;
        }

        out = fopen(file_path, "wb");
        if(out == NULL) {
            LogInfo("failed to open %s for writing, aborting!\n", file_path);
            goto ABORT_MAD;
        }

        //print(" %s\n", file_path);
        if(fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
            LogInfo("failed to write %s!\n", file_path);
            goto ABORT_MAD;
        }

        u_fclose(out);
        u_free(data);

        // return us to where we were in the file
        fseek(file, position, SEEK_SET);
    } while(position < lowest_offset);

    ABORT_MAD:
    u_free(data);

    u_fclose(out_index);
    u_fclose(out);
    u_fclose(file);
}

static void ConvertImageToPNG(const char *path) {
    LogInfo("converting %s...\n", path);

    // figure out if the file already exists before
    // we even start trying to convert this thing
    char out_path[PL_SYSTEM_MAX_PATH];
    plStripExtension(out_path, sizeof(out_path), path);
    strcat(out_path, ".png");
    if(plFileExists(out_path)) {
        LogInfo("tim already converted, deleting original\n");
        plDeleteFile(path);
        return;
    }

    PLImage image;
    if(!plLoadImage(path, &image)) {
        LogInfo("failed to load \"%s\", %s, aborting!\n", path, plGetError());
        return;
    }

    const char *ext = plGetFileExtension(path);
    if(ext != NULL && ext[0] != '\0' && strcmp(ext, "tim") == 0) {
        // ensure that it's a format we're able to convert from
        if (image.format != PL_IMAGEFORMAT_RGB5A1) {
            LogInfo("unexpected pixel format in \"%s\", aborting!\n", path);
            goto ABORT;
        }

        if (!plConvertPixelFormat(&image, PL_IMAGEFORMAT_RGBA8)) {
            LogInfo("failed to convert \"%s\", %s, aborting!\n", path, plGetError());
            goto ABORT;
        }
    }

    plReplaceImageColour(&image, PLColour(255, 0, 255, 255), PLColour(0, 0, 0, 0));

#if 0
    if(!plFlipImageVertical(&image)) {
        LogInfo("failed to flip \"%s\", %s, aborting!\n", path, plGetError());
        goto ABORT;
    }
#endif

    if(!plWriteImage(&image, out_path)) {
        LogInfo("failed to write png \"%s\", %s, aborting!\n", out_path, plGetError());
        goto ABORT;
    }

    plFreeImage(&image);
    plDeleteFile(path);

    ABORT:
    plFreeImage(&image);
}

/************************************************************/
/* Texture Merger */

typedef struct TMergeTarget {
    const char *path;
    unsigned int x, y;
} TMergeTarget;

typedef struct TMerge {
    const char *output;
    unsigned int num_textures;
    unsigned int width, height;
    TMergeTarget targets[10];
} TMerge;

static TMerge texture_targets[]={
        {
            "fe/dash/ang.png", 5, 152, 121, {{
                "fe/dash/ang1.tim", 0, 0
            }, {
                "fe/dash/ang2.tim", 64, 22
            }, {
                "fe/dash/ang3.tim", 0, 64
            }, {
                "fe/dash/ang4.tim", 64, 64
            }, {
                "fe/dash/ang5.tim", 128, 31
            }}
        },

        {
            "fe/dash/clock.png", 4, 128, 96, {{
                "fe/dash/clock01.tim", 0, 0
            }, {
                "fe/dash/clock02.tim", 64, 0
            }, {
                "fe/dash/clock03.tim", 0, 28
            }, {
                "fe/dash/clock04.tim", 64, 28
            }}
        },

        {
            "fe/dash/timer.png", 10, 256, 32, {{
                "fe/dash/timer0.tim", 0, 0
            }, {
                "fe/dash/timer1.tim", 24, 0
            }, {
                "fe/dash/timer2.tim", 48, 0
            }, {
                "fe/dash/timer3.tim", 72, 0
            }, {
                "fe/dash/timer4.tim", 96, 0
            }, {
                "fe/dash/timer5.tim", 120, 0
            }, {
                "fe/dash/timer6.tim", 144, 0
            }, {
                "fe/dash/timer7.tim", 168, 0
            }, {
                "fe/dash/timer8.tim", 192, 0
            }, {
                "fe/dash/timer9.tim", 216, 0
            }}
        },

        {
            "fe/dash/pause.png", 8, 64, 64, {{
                "fe/dash/pause1.tim", 0, 0
            }, {
                "fe/dash/pause2.tim", 16, 0
            }, {
                "fe/dash/pause3.tim", 32, 0
            }, {
                "fe/dash/pause4.tim", 0, 16
            }, {
                "fe/dash/pause5.tim", 32, 16
            }, {
                "fe/dash/pause6.tim", 0, 32
            }, {
                "fe/dash/pause7.tim", 16, 32
            }, {
                "fe/dash/pause8.tim", 32, 32
            }}
        }
};

static void MergeTextureTargets(void) {
    unsigned int num_texture_targets = plArrayElements(texture_targets);
    LogInfo("merging %d texture targets...\n", num_texture_targets);
    for(unsigned int i = 0; i < num_texture_targets; ++i) {
        TMerge *merge = &texture_targets[i];

        LogInfo("generating %s\n", merge->output);

        PLImage output;
        memset(&output, 0, sizeof(PLImage));
        output.width            = merge->width;
        output.height           = merge->height;
        output.format           = PL_IMAGEFORMAT_RGBA8;
        output.colour_format    = PL_COLOURFORMAT_RGBA;
        output.levels           = 1;
        output.size             = plGetImageSize(output.format, output.width, output.height);

        if((output.data = calloc(output.levels, sizeof(uint8_t*))) == NULL) {
            Error("failed to allocate data handle!\n");
        }

        if((output.data[0] = calloc(output.size, sizeof(uint8_t))) == NULL) {
            Error("failed to allocate data handle!\n");
        }

        for(unsigned int j = 0; j < merge->num_textures; ++j) {
            PLImage image;
            const char *path = merge->targets[j].path;
            if(!plLoadImage(path, &image)) {
                LogWarn("failed to find image, \"%s\", for merge!\n", merge->targets[j].path);
                continue;
            }

            plConvertPixelFormat(&image, PL_IMAGEFORMAT_RGBA8);

            LogInfo("writing %s into %s\n", merge->targets[j].path, merge->output);

            uint8_t *pos = output.data[0] + ((merge->targets[j].y * output.width) + merge->targets[j].x) * 4;
            uint8_t *src = image.data[0];
            for(unsigned int y = 0; y < image.height; ++y) {
                memcpy(pos, src, (image.width * 4));
                src += image.width * 4;
                pos += output.width * 4;
            }

            plFreeImage(&image);
            plDeleteFile(path);
        }

        LogInfo("writing %s\n", merge->output);
        plWriteImage(&output, merge->output);
        plFreeImage(&output);
    }
}

/************************************************************/

enum {
    VERSION_UNKNOWN,

    VERSION_ENG_PSX,        /* English PSX version */
    VERSION_ENG_PC,         /* English PC version */
    VERSION_ENG_PC_DIGITAL, /* English PC/Digital version */
};

static unsigned int CheckGameVersion(const char *path) {
    char fcheck[PL_SYSTEM_MAX_PATH];
    snprintf(fcheck, sizeof(fcheck), "%s/system.cnf", path);
    if(plFileExists(fcheck)) {
        LogInfo("detected system.cnf, assuming PSX version\n");
        return VERSION_ENG_PSX;
    }

    snprintf(fcheck, sizeof(fcheck), "%s/Data/foxscale.d3d", path);
    if(plFileExists(fcheck)) {
        LogInfo("detected Data/foxscale.d3d, assuming PC version\n");
        snprintf(fcheck, sizeof(fcheck), "%s/MUSIC/Track02.ogg", path);
        if(plFileExists(fcheck)) {
            LogInfo("detected MUSIC/Track02.ogg, assuming GOG version\n");
            return VERSION_ENG_PC_DIGITAL;
        } else {
            return VERSION_ENG_PC;
        }
    }

    return VERSION_UNKNOWN;
}

static void ProcessPackagePaths(const char *in, const char *out, const CopyPath *paths, unsigned int length) {
    for (unsigned int i = 0; i < length; ++i) {
        char output_path[PL_SYSTEM_MAX_PATH];
        snprintf(output_path, sizeof(output_path), "%s%s", out, paths[i].output);
        if (!plCreatePath(output_path)) {
            LogWarn("%s\n", plGetError());
            continue;
        }

        char input_path[PL_SYSTEM_MAX_PATH];
        snprintf(input_path, sizeof(input_path), "%s%s", in, paths[i].input);
        LogInfo("copying %s to %s\n", input_path, output_path);
        const char *ext = plGetFileExtension(input_path);
        if(pl_strncasecmp(ext, "PTG", 3) == 0) {
            ExtractPTGPackage(input_path, output_path);
        } else {
            ExtractMADPackage(input_path, output_path);
        }
    }
}

static void ProcessCopyPaths(const char *in, const char *out, const CopyPath *paths, unsigned int length) {
    for (unsigned int i = 0; i < length; ++i) {
        char output_path[PL_SYSTEM_MAX_PATH];
        snprintf(output_path, sizeof(output_path), "%s%s", out, paths[i].output);
        if (!plCreatePath(output_path)) {
            LogWarn("%s\n", plGetError());
            continue;
        }

        strncat(output_path, plGetFileName(paths[i].input), sizeof(output_path) - strlen(output_path) - 1);
        pl_strtolower(output_path);

        char input_path[PL_SYSTEM_MAX_PATH];
        snprintf(input_path, sizeof(input_path), "%s%s", in, paths[i].input);
        LogInfo("copying %s to %s\n", input_path, output_path);
        plCopyFile(input_path, output_path);
    }
}

int main(int argc, char **argv) {
    if(argc < 1) {
        printf("invalid number of arguments ...\n"
               "  extractor <game_path> -<out_path>\n");
        return EXIT_SUCCESS;
    }

    char app_dir[PL_SYSTEM_MAX_PATH];
    plGetApplicationDataDirectory(ENGINE_APP_NAME, app_dir, PL_SYSTEM_MAX_PATH);

    if(!plCreatePath(app_dir)) {
        LogWarn("Unable to create %s: %s\nSettings will not be saved.", app_dir, plGetError());
    }

    char log_path[PL_SYSTEM_MAX_PATH];
    snprintf(log_path, PL_SYSTEM_MAX_PATH, "%s/extraction", app_dir);
    plSetupLogOutput(log_path);

    /* set our logs up */

    plSetupLogLevel(0, "info", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(1, "warning", PLColour(255, 255, 0, 255), true);
    plSetupLogLevel(2, "error", PLColour(255, 0, 0, 255), true);

    /* now deal with any arguments */

    char input_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    char output_path[PL_SYSTEM_MAX_PATH];
#if 0
    if(getcwd(output_path, sizeof(output_path) - 1) == NULL) {
        strcpy(output_path, "./");
    }
#else
    strcpy(output_path, "./");
#endif

    for(int i = 1; i < argc; ++i) {
        if(argv[i][0] == '-') {
            strncpy(output_path, argv[i] + 1, sizeof(output_path));
        } else {
            strncpy(input_path, argv[i], sizeof(input_path));
        }
    }

    if(plIsEmptyString(input_path)) {
        Error("invalid game path, aborting!\n");
        return EXIT_FAILURE;
    }

    LogInfo("\n"
            "output path: %s\n"
            "input path:  %s\n",
            output_path, input_path);

    /* ensure it's a valid version - original CD version requires
     * us to extract audio from the CD while GOG and Steam versions
     * provide OGG audio.
     *
     * PSX is a totally different story, with it's own fun adventure. */
    LogInfo("checking game version...\n");
    unsigned int version = CheckGameVersion(input_path);
    switch(version) {
        default:
        case VERSION_UNKNOWN: {
            Error("unknown game version, aborting!\n");
            return EXIT_FAILURE;
        }

        case VERSION_ENG_PSX: {
            u_assert(0, "Unsupported");
            return EXIT_FAILURE;
        }

        case VERSION_ENG_PC: {
            ProcessPackagePaths(input_path, output_path, pc_package_paths, plArrayElements(pc_package_paths));
            ProcessCopyPaths(input_path, output_path, pc_copy_paths, plArrayElements(pc_copy_paths));
            // todo: rip music from CD
        } break;

        case VERSION_ENG_PC_DIGITAL: {
            ProcessPackagePaths(input_path, output_path, pc_package_paths, plArrayElements(pc_package_paths));
            ProcessCopyPaths(input_path, output_path, pc_copy_paths, plArrayElements(pc_copy_paths));
            ProcessCopyPaths(input_path, output_path, pc_music_paths, plArrayElements(pc_music_paths));
        } break;
    }

    MergeTextureTargets();

    /* convert the remaining TIM textures to PNG */
    plScanDirectory(output_path, "tim", ConvertImageToPNG, true);

    LogInfo("complete\n");
    return EXIT_SUCCESS;
}
