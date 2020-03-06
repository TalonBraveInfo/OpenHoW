# OpenHoW

[![Project Stats](https://www.openhub.net/p/openhow/widgets/project_thin_badge.gif)](https://www.openhub.net/p/openhow) 

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FTalonBraveInfo%2FOpenHoW.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2FTalonBraveInfo%2FOpenHoW?ref=badge_shield)
[![CodeFactor](https://www.codefactor.io/repository/github/talonbraveinfo/openhow/badge)](https://www.codefactor.io/repository/github/talonbraveinfo/openhow)
[![Build Status](https://badge.buildkite.com/91e09e49d899659cf45b4898c8aaeb9d5927f2a7b23eaafb28.svg)](https://buildkite.com/solemnwarning/openhow)
[![version](https://img.shields.io/github/release-pre/TalonBraveInfo/OpenHoW)](https://github.com/TalonBraveInfo/OpenHoW/releases/latest)

## What's this? :pig:
OpenHoW aims to be an open-source reimplementation of [Hogs of War](https://en.wikipedia.org/wiki/Hogs_of_War),
a turn-based strategy game produced by [Gremlin Interactive](https://en.wikipedia.org/wiki/Gremlin_Interactive).

The entire project is being built from the ground up, supporting 
content from both the PC and PSX releases of the game, and 
the end goal will be to produce a completely open and free 
version of the game, featuring completely new assets to 
replace those of the original (think 
[FreeDOOM](https://freedoom.github.io/)).

## Features :page_with_curl:
Consider this more of a wish-list at this  point in time ;)
* Support for Windows and Linux, with possibly more to follow!
* An editor to allow for easy modification of game-data
* Enhanced graphics over the original game

## Status :mag:
![Screenshot](./preview/preview00.png?raw=true)

As of October 1st, 2019, the project is still in a very early phase of development.
As we shift onto our next milestone (0.5.0) a lot of work will begin on actually getting
the game into a basic playable form.

The current goal is to get the game into a rough playable state.

## Contributing :hammer:
Interested in contributing to the project? Know C/C++ and looking to get deep down
into some code? Then what are you waiting for!?

The best place right now to get in touch is through our [Discord](https://discord.gg/EdmwgVk),
and makes things a hell of a lot easier to coordinate and collaborate as well. If you're
interested then jump on in!

Otherwise if you're just simply interested in the progression of the project, you too
are more than welcome to join us. :smirk:

### Getting Started

Here's a rough guide to getting up and running.

There are several requirements in order to compile the project successfully, each of which
have been listed below.

* [CMake](https://cmake.org/) (needed for project generation)
* [SDL2](https://www.libsdl.org/) (needed for input, window creation and audio)
* [OpenAL Soft](https://github.com/kcat/openal-soft) (needed for audio)

At this time with these dependencies the project should compile for Linux (_Ubuntu 19.10_: ```apt install cmake libglew-dev libopenal-dev libsdl2-dev```)
but Windows needs some further care before it will be up and running correctly (see Windows section below).

Once compiled, you need to use the [extraction](https://github.com/TalonBraveInfo/OpenHoW/tree/master/src/tools/extractor) utility: point it to your Hogs of War installation directory. The tool will then copy across and update any of the original Hogs of War's assets as necessary. If you want the files to be copied over somewhere else, add `-<output-dir>` after specifying your Hogs of War directory, but the default ```bin``` directory is required to get OpenHoW up and running.


#### Linux

```
git clone https://github.com/TalonBraveInfo/OpenHoW.git
cd OpenHoW
git submodule update --init
mkdir build
cd build
cmake ../
```

#### Windows

On Windows, the project has been successfully compiled against [MinGW 64-bit](https://mingw-w64.org/doku.php/download/mingw-builds). One additional step
you may need to perform is to download a copy of the [SDL2 development libraries](https://www.libsdl.org/release/SDL2-devel-2.0.9-mingw.tar.gz)
for MinGW 64-bit.

After you've downloaded these files, place the 'include' and 'lib' directories under 'SDL2-2.0.9/x86_64-w64-mingw32'
in a new 'SDL2' directory under 'src/3rdparty'.

This will be changing at a later point to make things a little easier.

#### C/C++ Style Guide

A style guide is available [here](https://github.com/TalonBraveInfo/OpenHoW/tree/master/doc/coding_style.md).

At the moment the project is a mess of different styles, which is very much an unfortunate side effect of
experimenting with different styles during earlier stages of development.

