(f)ixed (f)unction and (f)ast (f)ourier (t)ransform (t)esting
---
fffftt is an educational exhibit for audio signal processing <-> graphic visualization demos. It is highly inspired by Shaderto and uses raylib as a bridge between modern GLSL-driven shaders and traditional [Fixed Function](https://en.wikipedia.org/wiki/Fixed-function_(computer_graphics)) pre-shader graphics pipelines

The project to act as a reference for approximate historical approaches of:
1. Modern glsl shader effect implementations
2. Dreamcast* ports of shader effects as software-style routines -> fixed-function rendering

*Dreamcast is the current focus because it has the most active development scene for raylib OpenGL 1.1 targets

For demos jump to [fft shadertoy ports](#fft-shadertoy-ports) or [example targets](#example-targets).

---

<br>

[![GitHub Stars](https://img.shields.io/github/stars/meisei4/fffftt?style=flat&label=stars)](https://github.com/meisei4/fffftt/stargazers)
[![License](https://img.shields.io/badge/license-GPL%20v2%2B-blue.svg)](LICENSE)

features
--------
  - Shadertoy-inspired audio visualization experiments built in plain C
  - Desktop reference targets for OpenGL 3.3  and OpenGL 1.1 (suffixed `-gl11` or `-gl33`)
  - Dreamcast (suffixed `-dc`) targets built through raylib4Consoles and KallistiOS
  - all examples simple and readable each under 200 LOC
  - Generated `./bin/*` launchers for both desktop and Dreamcast runs

fft shadertoy ports
-------------------
meisei4: basic fft visualizer [https://www.shadertoy.com/view/t3jGzm](https://www.shadertoy.com/view/t3jGzm)
---------------------------------------------------------------------------------------------------------------
```bash
make cool-dc # -> out: build/dc/cool_dc/cool_dc.elf + run: ./bin/cool_dc
```
Audio source: [Shadertoy media files index](https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/) ->  [`Experiment` MP3](https://www.shadertoy.com/media/a/29de534ed5e4a6a224d2dfffab240f2e19a9d95f5e39de8898e850efdb2a99de.mp3)

<table>
  <tr>
    <td width="50%" align="center"><strong>Dreamcast capture</strong></td>
    <td width="50%" align="center"><strong>Shadertoy reference</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/afb82de8-e7eb-408b-a098-4f7deeb20b51" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/de3515d9-4d13-4d0c-bcdb-51aa4b9da37a" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

Cotterzz: Visualiser - Picking out notes: [https://www.shadertoy.com/view/tcG3Rm](https://www.shadertoy.com/view/tcG3Rm)
------------------------------------------------------------------------------------------------------------------------
```bash
make picking-out-notes-dc # -> out: build/dc/picking_out_notes_dc/picking_out_notes_dc.elf + run: ./bin/picking_out_notes_dc
```
<table>
  <tr>
    <td width="50%" align="center"><strong>Dreamcast capture</strong></td>
    <td width="50%" align="center"><strong>Shadertoy reference</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/c44f8ca0-7b23-4970-acec-50e3f2022368" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/1db9ec0d-8410-4718-b934-bb7bee2a63b7" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

waveform shadertoy ports
------------------------
meisei4: sound envelope reference: [https://www.shadertoy.com/view/7fXSWB](https://www.shadertoy.com/view/7fXSWB)
--------------------------------------------------------------------------------------------------------------------
<table>
  <tr>
    <td width="50%" align="center"><strong>Dreamcast capture</strong></td>
    <td width="50%" align="center"><strong>Shadertoy reference</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/c44f8ca0-7b23-4970-acec-50e3f2022368" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/1db9ec0d-8410-4718-b934-bb7bee2a63b7" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

meisei4: basic waveform data visualizer: [https://www.shadertoy.com/view/tcSXRz](https://www.shadertoy.com/view/tcSXRz)
--------------------------------------------------------------------------------------------------------------------------
<table>
  <tr>
    <td width="50%" align="center"><strong>Dreamcast capture</strong></td>
    <td width="50%" align="center"><strong>Shadertoy reference</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/c44f8ca0-7b23-4970-acec-50e3f2022368" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/1db9ec0d-8410-4718-b934-bb7bee2a63b7" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

build and installation
----------------------
```bash
git clone --recurse-submodules https://github.com/meisei4/fffftt.git
# otherwise:
git clone https://github.com/meisei4/fffftt.git
cd fffftt
git submodule update --init --recursive
```

The current Makefile expects:

- `bear` to be installed
- a working desktop OpenGL toolchain for the desktop targets
- a configured KallistiOS and raylib4Dreamcast environment for Dreamcast targets

[src directory](https://github.com/meisei4/fffftt/tree/master/src) contains the examples and the shared analyzer code used across the demos.

example benchmarking
---------------
cooley tukey lame cool
----------------------
```bash
make cool-gl33 # -> then run: ./bin/cool_gl33

make cool-gl11 # -> then run: ./bin/cool_gl11

make cool-dc # -> out: build/dc/cool_dc/cool_dc.elf + run: ./bin/cool_dc
```

wild west
---------
```bash
make fftw-gl11 # -> then run: ./bin/fftw_gl11

make fftw-dc # -> out: build/dc/fftw_dc/fftw_dc.elf + run: ./bin/fftw_dc
```

sh4zam butterfly shenanigans
---------------------------------
```bash
make sh4zam-butterfly # -> out: build/sh4/sh4zam_butterfly/sh4zam_butterfly.elf + run: ./bin/sh4zam_butterfly
```

waveform and sound-envelope work
--------------------------------
```bash
make waveform-dc # -> out: build/dc/waveform_dc/waveform_dc.elf + run: ./bin/waveform_dc
make waveform-sandbox-dc # -> out: build/dc/waveform_sandbox_dc/waveform_sandbox_dc.elf + run: ./bin/waveform_sandbox_dc
make sound-envelope-dc # -> out: build/dc/sound_envelope_dc/sound_envelope_dc.elf + run: ./bin/sound_envelope_dc
make sound-envelope-3d-dc # -> out: build/dc/sound_envelope_3D_dc/sound_envelope_3D_dc.elf + run: ./bin/sound_envelope_3D_dc
make sound-envelope-gl33 # -> then run: ./bin/sound_envelope_gl33
```

audio only (miniaudio stuff)
----------------------------
```bash
make audio-only-wav-dc # -> then run: ./bin/audio_only_wav_dc
make audio-only-mp3-dc # -> then run: ./bin/audio_only_mp3_dc
```

capture and runtime notes
-------------------------
- `flycast_run.sh` lives at the repo root and is used by the generated Dreamcast launcher wrappers under `./bin/`
- `tmux_matrix.sh` is a rough helper for building multiple targets, launching each one in its own tmux session, and capturing logs side by side under `logs/`
- `tmux_audio_only_matrix.sh` builds `audio-only-wav-dc` and `audio-only-mp3-dc` for both `1024` and `2048` period sizes
- `fftw_1997/` is currently a temporary local copy of FFTW code and will be formalized later

third party
-----------
`raylib`
- GitHub: [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)
- Website: [https://www.raylib.com/](https://www.raylib.com/)

`raylib4Consoles`
- GitHub: [https://github.com/raylib4Consoles](https://github.com/raylib4Consoles)
- Dreamcast: [https://github.com/raylib4Consoles/raylib4Dreamcast](https://github.com/raylib4Consoles/raylib4Dreamcast)

`KallistiOS`
- GitHub: [https://github.com/KallistiOS/KallistiOS](https://github.com/KallistiOS/KallistiOS)
- Docs: [https://kos-docs.dreamcast.wiki/](https://kos-docs.dreamcast.wiki/)

`FFTW`
- Website: [https://www.fftw.org/](https://www.fftw.org/)
- Download page: [https://www.fftw.org/download.html](https://www.fftw.org/download.html)

todo
----
- REMOVE bear dependency/make bear optional!
- add raylib shadertoy host convention for more desktop comparisons (essentially will be based on sound_envelope_gl33.c and cool_gl33.c for waveform vs fft)
- and more Shadertoy ports
- integrate `GLdc` as a formal local submodule experiment

license
-------
This project is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This project is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [LICENSE](./LICENSE)
file for details.
