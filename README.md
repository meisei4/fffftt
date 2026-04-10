(f)ixed (f)unction and (f)ast (f)ourier (t)ransform (t)esting
---
fffftt is an educational exhibit for audio signal processing <-> graphic visualization demos. It is highly inspired by [Shadertoy](https://www.shadertoy.com/) and uses [raylib](https://github.com/raysan5/raylib) as a bridge between modern GLSL-driven shaders and traditional [Fixed Function](https://en.wikipedia.org/wiki/Fixed-function_(computer_graphics)) pre-shader graphics pipelines using Dreamcast [KOS](https://github.com/KallistiOS/KallistiOS) and [GLdc](https://github.com/kazade/GLdc)raylib backend case study (more information at [third party section](#third-party))

The project to act as a reference for approximate historical approaches of:
1. Modern glsl shader effect implementations
2. Dreamcast* ports of shader effects as software-style routines -> fixed-function rendering

*Dreamcast is the current focus because it has the most active development scene for raylib OpenGL 1.1 targets

For demos scroll down to [fft shadertoy ports](#fft-shadertoy-ports) or [example targets](#benchmarking).

---

<br>

[![GitHub Stars](https://img.shields.io/github/stars/meisei4/fffftt?style=flat&label=stars)](https://github.com/meisei4/fffftt/stargazers)
[![License](https://img.shields.io/badge/license-GPL%20v2%2B-blue.svg)](LICENSE)

features
--------
  - Shadertoy-inspired audio visualization experiments built in plain C each under ~150 LOC
  - Dreamcast (suffixed `-dc`) targets built through raylib4Consoles and KallistiOS
  - Desktop reference targets for OpenGL 3.3  and OpenGL 1.1 (suffixed `-gl33` or `-gl11` )
  - Generated `./bin/*` launchers for both desktop and flycast Dreamcast runs

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
    <td width="50%" align="center"><strong>Shadertoy capture</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/358c96bd-2520-42e1-acb0-8aec52358ad5" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/9538fb97-79ec-434c-b659-74deec5030a3" controls muted playsinline width="100%"></video>
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
    <td width="50%" align="center"><strong>Shadertoy capture</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/f942edb3-cb31-4095-851c-5b907f006d29" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/ff68e8b7-095f-4a61-b838-3e82e0a52e5d" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

waveform shadertoy ports
------------------------
meisei4: sound envelope reference: [https://www.shadertoy.com/view/7fXSWB](https://www.shadertoy.com/view/7fXSWB)
--------------------------------------------------------------------------------------------------------------------
```bash
make sound-envelope-dc
make sound-envelope-3d-dc
make sound-envelope-gl33
```
<table>
  <tr>
    <td width="25%" align="center"><strong>Dreamcast 2D capture</strong></td>
    <td width="25%" align="center"><strong>Dreamcast 3D capture</strong></td>
    <td width="25%" align="center"><strong>GL33 capture</strong></td>
    <td width="25%" align="center"><strong>Shadertoy capture</strong></td>
  </tr>
  <tr>
    <td width="25%">
      <video src="https://github.com/user-attachments/assets/ac36e5e0-c8a5-42bc-aa92-9303712dc4a7" controls muted playsinline width="100%"></video>
    </td>
    <td width="25%">
      <video src="https://github.com/user-attachments/assets/d9d90e0c-82a5-44ed-a485-5b57db02a299" controls muted playsinline width="100%"></video>
    </td>
    <td width="25%">
      <video src="https://github.com/user-attachments/assets/d34794b5-c1d7-47bd-9e90-fc808389ccb4" controls muted playsinline width="100%"></video>
    </td>
    <td width="25%">
      <video src="https://github.com/user-attachments/assets/71ac43c5-8667-4efa-9bea-558f183f5602" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

meisei4: basic waveform data visualizer: [https://www.shadertoy.com/view/tcSXRz](https://www.shadertoy.com/view/tcSXRz)
--------------------------------------------------------------------------------------------------------------------------
```bash
make waveform-dc
```
<table>
  <tr>
    <td width="50%" align="center"><strong>Dreamcast capture</strong></td>
    <td width="50%" align="center"><strong>Shadertoy capture</strong></td>
  </tr>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/9dab963d-eabd-40d1-b541-5c6f3e64c544" controls muted playsinline width="100%"></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/df219ba5-18be-4a7e-a023-edb22fd3e6ea" controls muted playsinline width="100%"></video>
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

benchmarking
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
