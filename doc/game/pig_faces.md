# Pig Facial Animations

## Frontend

This part of the implementation is specific to the frontend, which is basically the main menu for the game. 

Some of this may coincide with the implementation used for the main game - in which case they can be brought together under the same implementation, though the implementation for the game itself is more contextual than the frontend.

Apparently the commando is a special case, but I'll need to investigate this further.

Here on the frontend, the pigs use the following expression groups.
* Happy
* Sad
* Cheeky
* Surprised
* Angry
* Thoughtful

**Keep in mind I don't have access to all my notes right now so I'll be revisiting this with more details later!**

### Happy

There is a 1 in 8 chance that the pig will 'blink' in this state, and then a 1 in 15 chance the pig will 'blink' for an extended period (up to 40 ticks maximum, 20 ticks minimum) otherwise they will 'blink' for just 3 ticks.

If the pig doesn't 'blink' in this state, the pig will provide a 'normal' eyes expression from 3 ticks minimum and up to 6 ticks maximum.

There is a 1 in 3 chance that the pig's mouth will be 'closed' for 30 ticks, otherwise it will be 'open' for 30 ticks.

### Sad

There is a 1 in 10 chance that the pig will 'blink' in this state which will only occur for 3 ticks. Their standard 'sad' eyes expression, otherwise, will appear for 5 ticks.

The mouth will continually use the 'surprised' expression during this state until the expression state has changed.

### Cheeky

Eyes alternate every three ticks from 'angry' to 'normal' (so 'angry' + 3 ticks, then 'normal' + 3 ticks).

Mouth is continuously updated to 'open' every tick, so this will switch over as soon as the expression state has changed.

### Surprised

There is a 1 in 10 chance that the pig will 'blink', otherwise the pig will use their 'surprised' eyes. Either route will last for 5 ticks.

The mount is continuously updated to 'surprised' every tick, so this will switch over as soon as the expression state has changed.

### Angry

Eyes don't blink at all while the pig is in this state - delay is set to 10 ticks before eyes are updated again.

Mouth is continuously updated to 'angry' every tick, so this will switch over as soon as the expression state has changed.

### Thoughtful

There is a 1 in 2 chance that the pig will show a 'sad' eyes expression for 20 ticks, otherwise there is a 1 in 2 chance they will either show a 'normal' eye expression for 20 ticks or 'blink' for three ticks.

There is a 1 in 3 chance the pig's mouth will show a 'sad' expression for 30 ticks but otherwise will show a 'closed' expression for 30 ticks instead.

----

Some face frames are enforced by the current animation.

### Sneezing

If the frame is less than 1024, the pig should show a surprised expression for both their eyes and mouth. Otherwise the eyes are set to their blink state and the mouth is closed.

### Dancing

The eyes should be set to their normal state and the mouth should be surprised.