# Space Boost

Space Boost is a high score game where you pilot a space shuttle as far into space as you can

Use of the thrusters will deplete the fuel tanks, make sure you hit as many of the space boost platforms as you can to top the tanks back up, but beware, the space boost platforms rotate and could scrub off your momentum costing you more than you'll get.

## Features

Single player game where you try and beat your own score.

Play against 3 other friends in multiplayer, there is a standalone server which can be run up in ```./server/spaceboostserver``` and then each client connects to the host's machine.

## How To Play

Controlling the Shuttle:
* Keyboard 'W' - Use both thrusters
* Keyboard 'A' - Shuts off the left thruster turning you right
* Keyboard 'D' - Shuts off the right thruster turning you left

## Dependancies

- g++
- mesa-common-dev
- libglfw3-dev

## Supported Platforms

- Linux (OpenGL 2.1+)

## Installation and Running

Multiplayer Server

```
./server/make
./server/spaceboostserver
```

Main Game

```
./make
./spaceboost
```