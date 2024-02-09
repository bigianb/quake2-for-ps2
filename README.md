
# Quake II port for the PlayStation 2

## Overview

This is an unofficial fan made port, targeting the PS2 Console, of the original
[Quake II source code released by id Software][link_id_repo].

This repository is a fork of Guilherme Lampert's original which he stopped working on several years ago.
The main changes are to bring it up to date with the current ps2dev and to fill in some of the missing gaps.

It's really just for tech asmusement - there is a lot of stuff missing.

## Dev environment

I use the ps2dev docker on M1 macbook. This is the amd64 image running under rosetta.
Given that I always forget how to use docket, the notes below act as a crib sheet.
If you want to use the same, change the /Users/ian/dev reference to point to where you have the source code.

```
docker run --volume=/Users/ian/dev:/home/src --workdir=/home/src -t -d ps2dev/ps2dev:latest
```

we then need to add some things to the image:
```
apk update
apk add make mpc mpfr gmp mpc1 mpfr4 cdrkit
```

## Game files

You will need to copy the baseq2 directory from a real Q2 game or the demo into the fs directory.

## Building

Running make in the root directory should generate an ISO file in the build directory.
You can run that ISO in an emulator. I've not tried on a real PS2 yet.

## License

Quake II was originally released as GPL, and it remains as such. New code written
for the PS2 port or any changes made to the original source code are also released under the
GNU General Public License version 2. See the accompanying LICENSE file for the details.

You can also find a copy of the GPL version 2 [in here][link_gpl_v2].

[link_id_repo]: https://github.com/id-Software/Quake-2
[link_ps2_dev]: https://github.com/ps2dev
[link_gpl_v2]:  https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html


