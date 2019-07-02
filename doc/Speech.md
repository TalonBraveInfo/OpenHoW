# Pigs
Speech file names for pigs are formatted as follows: `WWXXYYZZ.wav`

`WW` = PigIndex (`01` - `09`)  

`XX` = Team 
- `AM` = America
- `EN` = Britsh
- `FR` = French
- `GE` = German
- `JA` = Japanese
- `RU` = Russian

`YY` = Category
- `01` = READY
- `02` = FIRE
- `03` = FIRE2 (for MP or NG?)
- `04` = DEATH
- `05` = DEATH2 (for MP or NG?)

`ZZ` = LineIndex (`01` - `06`)

# Sarge
Speech file names for the sergeants are formatted as follows: `SGENXXYY.wav`

XX = Category   

| Cagetory | Name                           | Description                                                                      |
| -------- | ------------------------------ | -------------------------------------------------------------------------------- |
| `01`     | SP_DEATH                       | Plays when a pig from your team dies. (POST TURN)                                |
| `02`     | SP_LOSING                      | Plays when the game thinks you're loosing. (PRE TURN)                            |
| `03`     | SP_KILLED                      | Plays when a pig from the enemy team dies. (POST TURN)                           |
| `04`     | SP_WINNING                     | Plays when the game thinks you're winning. (PRE TURN)                            |
| `05`     | SP_OBJECTIVE_COMPLETE_CRATE    | Plays when completing an objective that yields an item crate.                    |
| `06`     | SP_OBJECTIVE_COMPLETE_HEALTH   | Plays when completing an objective that yields a health crate.                   |
| `07`     | SP_OBJECTIVE_COMPLETE_PROPOINT | Plays when completing an objective that yields a propoint crate.                 |
| `08`     | SP_OBJECTIVE_FAILED            | Plays when an object marked for protection has been destroyed.                   |
| `09`     | SP_DROP_ZONE_CRATE             | Plays when entering a drop zone that yields an item crate.                       |
| `10`     | SP_DROP_ZONE_PROPOINT          | Plays when entering a drop zone that yields a propoint crate.                    |
| `11`     | SP_PROPOINT_OBJECTIVE_LINGER   | Plays when you've not completed all objectives with propoint rewards. (PRE TURN) |
| `12`     | SP_PROPOINT_PICKUP_LINGER      | Plays when you've not picked up all dropped propoints. (PRE TURN)                |
| `13`     | MP_TIME_LINGER                 | Plays when you've 10 seconds left.                                               |
| `14`     | MP_TIME_LINGER_CRITICAL        |                                                                                  |
| `15`     | MP_TEAM_BRITISH                | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `16`     | MP_TEAM_AMERICAN               | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `17`     | MP_TEAM_FRENCH                 | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `18`     | MP_TEAM_GERMAN                 | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `19`     | MP_TEAM_RUSSIAN                | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `20`     | MP_TEAM_JAPANESE               | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `21`     | MP_TEAM_LARD                   | Plays `01` - `04` when winning, `05` - `08` when loosing. (POST TURN)            |
| `22`     | SP_REWARD                      | Plays in order after conquering a continent.                                     |

YY = LineIndex (`01` - `08`)


# Train1
> Suggests there where multiple training's planned or cut.

Speech file names for Train1 are formatted as follows: `TR1_ENXX.wav`

XX = LineIndex

| Line | Description
| ---- | -----------
| `01` | Plays when spawning
| `02` | Plays when walking first time
| `03` | Plays when picking up `7 [CRATE1] (Bayonet)`
| `04` | Plays when opening weapon menu first time
| `05` | Plays when equiping bayonet first time
| `06` | Plays when destroying group 1
| `07` | Plays when lingering on picking crates.
| `08` | Plays when picking up `12 [CRATE1] (Rifle)`
| `09` | Plays when equiping rifle first time
| `10` | Plays when destroying group 2
| `11` | Plays when picking up `11 [CRATE1] (Sniper-Rifle)`
| `12` | Plays when destroying group 3
| `13` | Plays when picking up `13 [CRATE1] (Granade)`
| `14` | Plays when destroying first object in group 4
| `15` | Plays when destroying group 4
| `16` | Plays when picking up `16 [CRATE1] (Granade)`
| `17` | Plays when destroying group 5
| `18` | Plays when picking up `18 [CRATE2] (Health)`
| `19` | Plays when picking up `62 [CRATE2] (Health)`
| `20` | Plays when picking up `53 [CRATE1] (TNT)`
| `21` | Plays when triggering first mine
| `22` | Plays when destroying group 89 (DOOR)
| `23` | Plays when picking up `52 [CRATE2] (Health)`
| `24` | Plays when picking up `56 [CRATE2] (Health)`
| `25` | Plays when picking up `19 [CRATE1] (Bazooka)`
| `26` | Plays when running out of time first time
| `27` | Plays when destroying group 7
| `28` | Plays when getting a record time?
