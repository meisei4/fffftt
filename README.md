(f)ixed (f)unction and (f)ast (f)ourier (t)ransform (t)esting
---
submodule init: (this project uses submodules as an experiment with dependency flexibility):

```bash
git clone --recurse-submodules https://github.com/meisei4/fffftt.git
```

otherwise:
```bash
git clone https://github.com/meisei4/fffftt.git
cd fffftt
git submodule update --init --recursive
```

## cooley tukey lame cool:

```bash
# [default]
make cool-gl33 # -> then run: ./bin/cool_gl33
```

```bash
# [OpenGL 1.1]
make cool-gl11 # -> then run: ./bin/cool_gl11
```

```bash
# [KOS]
make cool-dc # -> out: build/dc/cool_dc/cool_dc.elf + run: ./bin/cool_dc
```

## wild west:
```bash
# [OpenGL 1.1]
make fftw-gl11 # -> then run: ./bin/fftw_gl11
```

## sh4zam butterfly shenanigans:

```bash
# [KOS + sh4zam]
make sh4zam-butterfly # -> out: build/sh4/sh4zam_butterfly/sh4zam_butterfly.elf + run: ./bin/sh4zam_butterfly
```
---
#### flycast:

* `flycast_run.sh` lives at the repo root and is used by the generated `./bin/cool_dc` and `./bin/sh4zam_butterfly` launcher wrappers

#### tmux note:

* `tmux_matrix.sh` is a rough helper for building multiple targets, launching each one in its own tmux session, and capturing logs side by side under `logs/`.
* builds `cool-gl33`, `cool-gl11`, `fftw-gl11`, `cool-dc`, and `sh4zam-butterfly` and then lanches them all at once, I want to learn tmux with this if ican so just a beginner idea for now, unreliable i think currently


#### misc info:

* fftw_1997 is temporary local copy of fftw stuff, will formalize later

* Runtime logs (tmux stuff) are written to `logs/` for now

* Runtime assets live under `src/resources` and Dreamcast romdisk contents live under `src/romdisk`

* ```bash
  make fmt # -> clang-format stuff
  ```

* ```bash
  make clean-all # clean `build/`, `bin/`, `logs/` stuff
  ```


#### TODO:

* integrate `GLdc` git submodule as an actual local version like the other submodules
* probably alot probably nothing idk


#### third party:

* `raylib`

  * GitHub: [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)
  * Website: [https://www.raylib.com/](https://www.raylib.com/)

* `raylib4Consoles`

  * GitHub: [https://github.com/raylib4Consoles](https://github.com/raylib4Consoles)
  * Dreamcast: [https://github.com/raylib4Consoles/raylib4Dreamcast](https://github.com/raylib4Consoles/raylib4Dreamcast)

* `KallistiOS`

  * GitHub: [https://github.com/KallistiOS/KallistiOS](https://github.com/KallistiOS/KallistiOS)
  * Docs: [https://kos-docs.dreamcast.wiki/](https://kos-docs.dreamcast.wiki/)

* `FFTW`

  * Website: [https://www.fftw.org/](https://www.fftw.org/)
  * Download page: [https://www.fftw.org/download.html](https://www.fftw.org/download.html)


#### license:

This project is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This project is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [LICENSE](./LICENSE) 
file for details.
