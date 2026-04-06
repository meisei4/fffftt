(f)ixed (f)unction and (f)ast (f)ourier (t)ransform (t)esting
---
submodule init: (this project uses submodules as an experiment with dependency flexibility):

```bash
git clone --recurse-submodules https://github.com/meisei4/fffftt.git
# otherwise:
git clone https://github.com/meisei4/fffftt.git
cd fffftt
git submodule update --init --recursive
```

## SHADERTOY PORTS (WIP):

### 1. [meisei4: basic fft visualizer](https://www.shadertoy.com/view/t3jGzm):
- This just uses cool-dc now to target the shadertoy media asset of "experiment":  
- https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/ -> `#Musics:` -> `- Experiment`  
- https://www.shadertoy.com/media/a/29de534ed5e4a6a224d2dfffab240f2e19a9d95f5e39de8898e850efdb2a99de.mp3

<table>
  <tr>
    <th>DC (var fps..)</th>
    <th>Shadertoy (120fps..)</th>
  </tr>
  <tr>
    <td>
      <video src="./docs/fft-dc-640x480-60.mp4" controls muted playsinline width="100%"></video>
    </td>
    <td>
      <video src="./docs/fft-shadertoy-dc-parity-640x480-60.mp4" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>

---

### 2. [Cotterzz: Visualiser - Picking out notes](https://www.shadertoy.com/view/tcG3Rm):
```bash
# [KOS]
make picking-out-notes-dc # -> out: build/dc/picking_out_notes_dc/picking_out_notes_dc.elf + run: ./bin/picking_out_notes_dc
```

<table>
  <tr>
    <th>DC (var fps..)</th>
    <th>Shadertoy (120 fps..)</th>
  </tr>
  <tr>
    <td>
      <video src="./docs/picking-out-notes-dc-640x480-60.mp4" controls muted playsinline width="100%"></video>
    </td>
    <td>
      <video src="./docs/picking-out-notes-shadertoy-dc-parity-640x480-60.mp4" controls muted playsinline width="100%"></video>
    </td>
  </tr>
</table>


--- 
# FFT TESTS

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

```bash
# [KOS]
make fftw-dc # -> out: build/dc/fftw_dc/fftw_dc.elf + run: ./bin/fftw_dc
```

## sh4zam butterfly shenanigans:

```bash
# [KOS + sh4zam]
make sh4zam-butterfly # -> out: build/sh4/sh4zam_butterfly/sh4zam_butterfly.elf + run: ./bin/sh4zam_butterfly
```

## audio only (miniaudio stuff)
```bash
# [KOS]
make audio-only-wav-dc # -> then run: ./bin/audio_only_wav_dc
make audio-only-mp3-dc # -> then run:  ./bin/audio_only_mp3_dc
```

---
#### flycast:

* `flycast_run.sh` lives at the repo root and is used by the generated `./bin/cool_dc` and `./bin/sh4zam_butterfly` launcher wrappers

#### tmux note:

* `tmux_matrix.sh` is a rough helper for building multiple targets, launching each one in its own tmux session, and capturing logs side by side under `logs/`.
* builds `cool-gl33`, `cool-gl11`, `fftw-gl11`, `cool-dc`, and `sh4zam-butterfly` and then lanches them all at once, I want to learn tmux with this if ican so just a beginner idea for now, unreliable i think currently

* `tmux_audio_only_matrix.sh` builds `audio-only-wav-dc` and `audio-only-mp3-dc` for both 1024 and 2048 period sizes (4 total targets)

#### misc info:

* fftw_1997 is temporary local copy of fftw stuff, will formalize later

* Runtime logs (tmux stuff) are written to `logs/` for now

* Runtime assets live under `src/resources` and Dreamcast romdisk contents live under `src/romdisk`

* Dreamcast audio assets ffprobes results:

  - `country_22050hz_pcm16_mono.wav` 
  ```bash
  [ ~/fffftt]$ ffprobe src/romdisk/country_22050hz_pcm16_mono.wav
  ffprobe version 8.1 Copyright (c) 2007-2026 the FFmpeg developers
  [...]
  Input #0, wav, from 'src/romdisk/country_22050hz_pcm16_mono.wav':
    Metadata:
      encoder         : Lavf62.3.100
    Duration: 00:01:33.56, bitrate: 352 kb/s
    Stream #0:0: Audio: pcm_s16le ([1][0][0][0] / 0x0001), 22050 Hz, 1 channels, s16, 352 kb/s
  ```

  - `country_44100hz_128kbps_stereo.mp3` (identical mp3 as core raylib examples)
    - [raylib4Dreamcast/blob/main/samples/audio/audio_music_stream/romdisk/country.mp3](https://github.com/raylib4Consoles/raylib4Dreamcast/blob/main/samples/audio/audio_music_stream/romdisk/country.mp3)
    - [raylib/blob/master/examples/audio/resources/country.mp3](https://github.com/raysan5/raylib/blob/master/examples/audio/resources/country.mp3)

  ```bash
  [ ~/fffftt]$ ffprobe src/romdisk/country_44100hz_128kbps_stereo.mp3
  [...]
  Input #0, mp3, from 'src/romdisk/country_44100hz_128kbps_stereo.mp3':
    Duration: 00:01:33.56, start: 0.025057, bitrate: 128 kb/s
    Stream #0:0: Audio: mp3 (mp3float), 44100 Hz, stereo, fltp, 128 kb/s, start 0.025057
      Metadata:
        encoder         : LAME3.98r
  ```

* ```bash
  make fmt # -> clang-format stuff
  ```

* ```bash
  make clean-all # clean `build/`, `bin/`, `logs/` stuff
  ```


#### TODO:

* add waveform visualization + more shadertoy ports
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
