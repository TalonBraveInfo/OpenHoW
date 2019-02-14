# OpenHoW

[![Project Stats](https://www.openhub.net/p/openhow/widgets/project_thin_badge.gif)](https://www.openhub.net/p/openhow) 

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FTalonBraveInfo%2FOpenHoW.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2FTalonBraveInfo%2FOpenHoW?ref=badge_shield)
[![CodeFactor](https://www.codefactor.io/repository/github/talonbraveinfo/openhow/badge)](https://www.codefactor.io/repository/github/talonbraveinfo/openhow)

### What's this? :astonished:
OpenHoW aims to be an open-source reimplementation of [Hogs of War](https://en.wikipedia.org/wiki/Hogs_of_War),
a turn-based strategy game produced by [Gremlin Interactive](https://en.wikipedia.org/wiki/Gremlin_Interactive).

The entire project is being built from the ground, supporting 
content from both the PC and PSX releases of the game, and 
the end goal will be to produce a completely open and free 
version of the game, featuring completely new assets to 
replace those of the original (think 
[FreeDOOM](https://freedoom.github.io/)).

### Features :page_with_curl:
Consider this more of a wish-list at this  point in time ;)
* Support for Windows and Linux, with possibly more to follow!
* An editor to allow for easy modification of game-data
* Enhanced graphics over the original game

### Status :mag:
Right now the project is only in it's infancy and a lot of focus
is currently going towards producing an engine for the game to be 
built upon; right now the primary part of this is creatively called 
the [platform library](https://github.com/TalonBraveInfo/platform).

If you want to see what tasks are currently being worked on and what
tasks still need doing, it might be worth taking a look at our [public
Trello](https://trello.com/b/qdZ7LswA/openhow) or any tasks here marked as '[help wanted](https://github.com/TalonBraveInfo/OpenHoW/issues?q=is%3Aopen+is%3Aissue+label%3A%22help+wanted%22)'.

Another task that is currently being looked into are tools that will
allow for easy modification of OpenHoW -
though compatibility with the original isn't a huge concern right 
now.

### Contributing :hammer:
Interested in contributing to the project? Know C/C++ and looking to get deep down
into some code? Then what are you waiting for!?

The best place right now to get in touch is through our [Discord](https://discord.gg/YMHJa6p),
and makes things a hell of a lot easier to coordinate and collaborate as well. If you're
interested then jump on in!

Otherwise if you're just simply interested in the progression of the project, you too
are more than welcome to join us. :smirk:

#### Getting Started

Here's a rough guide to getting up and running.

There are several requirements in order to compile the project successfully, each of which
have been listed below.

* [CMake](https://cmake.org/) (needed for project generation)
* [SDL2](https://www.libsdl.org/) (needed for input, window creation and audio)
* [OpenAL Soft](https://github.com/kcat/openal-soft) (needed for audio)
* [FFmpeg](https://www.ffmpeg.org/) (needed for video playback)

At this time the project should compile for Linux (_Ubuntu 18.10_) with these depedencies 100%
but Windows needs some further care before it will be up and running correctly.

Once you've got everything you need, you'll need to compile the [extraction](https://github.com/TalonBraveInfo/OpenHoW/tree/master/src/tools/extractor) utility. In order to do this you'll need to generate your project files
using the CMakeLists found under 'src/tools/'.

Using the extraction tool, you'll need to point it to your Hogs of War installation directory and the tool will then copy across and update any of Hogs of War's files as necessary. If you want the files to be copied over somewhere else, add `-<output-dir>` after specifying your Hogs of War directory.

It's advised that you point the extractor to the 'bin' directory, otherwise you won't be able to get OpenHoW up and running.

Once that's done, you're now set to generate the project files via the 
CMakeLists found under the root OpenHoW directory and compile the rest of the project.

----

![Wireframe view of pig](https://github.com/TalonBraveInfo/HogViewer/blob/master/preview/wireframe00.png?raw=true)
