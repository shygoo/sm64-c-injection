# SM64 C Injection Demo

This is an example setup for injecting compiled C code into N64 games. It uses n64chain to compile the C, and armips to assemble some bootstrap code and handle the linking.

## Requirements

### The following tools are required to build:

* n64chain
* n64crc
* armips

#### Prebuilt Windows binaries
* n64chain & n64crc: https://mega.nz/#!RTg0yaCJ!8ZAzK98XRFw0dSa0RHsezgRNPYLJM90qctW64UHB-PA
* armips: https://buildbot.orphis.net/armips/

#### Sources
* n64chain: https://github.com/tj90241/n64chain
* n64crc: http://n64dev.org/n64crc.html
* armips: https://github.com/Kingcom/armips

### Installation
* Put the n64chain directory in /
* Put an n64crc binary in /n64chain/bin if there isn't already
* Put an armips binary in /n64chain/bin
* Add /n64chain/bin to the PATH environment variable

## Building
* Legally obtain a US Super Mario 64 ROM (Must be `.z64` format)
* Rename it `sm64.z64` and drop it here in the project root
* Run `make`

The patched rom will be written to `sm64.mod.z64`.

## Where to find everything
```
src/
  hooks.c ......... entry point functions that will be called by the game
  hello_world.c ... hello world print example

include/
  variables.h ..... declarations for game variables
  functions.h ..... declarations for game functions
  types.h ......... declarations for game types
  sm64.h .......... includes all the above

asm/
  sections.asm .... ROM/RAM section address information
  variables.asm ... address definitions for game variables
  functions.asm ... address definitions for game functions

link.asm .......... linker tasks, bootstrap code, hook inserts
```
