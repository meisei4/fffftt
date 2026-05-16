<samp><b>(f)ixed (f)unction and (f)ast (f)ourier (t)ransform (t)esting</b></samp>
---
<samp>
  fffftt is an audio-geometry exhibit for fixed-function graphics signal visualization, originally inspired by <a href="https://www.shadertoy.com/">Shadertoy</a>. It targets Dreamcast as a hardware case study using <a href="https://github.com/raysan5/raylib">raylib</a>, OpenGL 1.1 <a href="https://en.wikipedia.org/wiki/Fixed-function_(computer_graphics)">fixed-function</a> rendering, <a href="https://github.com/kazade/GLdc">GLdc</a>, and <a href="https://github.com/gyrovorbis/sh4zam">sh4zam</a>'s <code>shz_fft()</code>.<br><br>
</samp>


<samp>[overview]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;.h file ........................... shared state/functions in <a href="https://github.com/meisei4/fffftt/blob/main/src/fffftt.h">src/fffftt.h</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;.c files .......................... one demo per file in <a href="https://github.com/meisei4/fffftt/tree/main/src">src/</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;.wav files ........................ <a href="https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/">Shadertoy media files</a> -> ADPCM .wavs in <a href="https://github.com/meisei4/fffftt/tree/main/src/resources">src/resources/</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;controls .......................... cycle tracks, pause/seek/resume, move light<br><!-- TODO: add a nice control scheme for gamepad-->
  &nbsp;&nbsp;&nbsp;&nbsp;audio terrain demos ............... see <a href="#audio-terrain">[audio terrain]</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;Shadertoy ports ................... see <a href="#shadertoy-ports">[shadertoy ports]</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;proxy plots ....................... MathWorks-like audio analysis plots in <a href="https://github.com/meisei4/fffftt/tree/main/mathworks">mathworks/</a> directory<br>
<br>
</samp>

### <samp>[audio terrain]</samp>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;[fft bands terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<a href="https://github.com/meisei4/fffftt/blob/main/src/fft_bands_terrain_3d_dc.c">src/fft_bands_terrain_3d_dc.c</a> ..... FFT bands; mid: <b>chroma</b>, low: <b>onset strength</b>, high: <b>spectral flatness</b><br>
</samp>

<table width="1056">
  <tr>
    <td width="464" rowspan="2" align="left">
      <video src="https://github.com/user-attachments/assets/3523a775-f174-4ba3-92e6-8fc39faacb32" autoplay loop muted playsinline width="464" height="348"></video>
      <sub><samp>audio should work^^^ otherwise: <a href="https://github.com/user-attachments/assets/3523a775-f174-4ba3-92e6-8fc39faacb32">fft-bands-terrain-3d-dc.webm</a>)</samp></sub>
    </td>
    <td width="296" rowspan="2" align="center">
      <img src="https://github.com/user-attachments/assets/87971a00-daf0-4e57-a2cc-70d343af97dd" alt="fft_bands_terrain.png" width="292"><br>
      <samp>fft bands terrain</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/6bddceb9-1566-4629-9dc8-bba4da1f6e8d" alt="fft_bands_terrain_chroma.png" width="142"><br>
      <samp>mid-chroma</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/f09c9416-ced4-4595-8bd0-7808599cd87b" alt="fft_high_band_terrain_spectral_flatness.png" width="142"><br>
      <samp>high-spectral-flatness</samp>
    </td>
  </tr>
  <tr>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/6b7040e7-a58b-4fdb-a4e7-f1edc1ed73fe" alt="fft_low_band_terrain_onset_curtain.png" width="142"><br>
      <samp>low-onset-curtains</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/52cee05d-8884-4395-a322-d0ccf58beb4c" alt="fft_low_band_terrain_onset_scribbles.png" width="142"><br>
      <samp>low-onset-scribbles</samp>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;[waveform terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<a href="https://github.com/meisei4/fffftt/blob/main/src/waveform_terrain_3d_dc.c">src/waveform_terrain_3d_dc.c</a> ...... <b>Hilbert envelope</b>, <b>RMS</b>, and <b>onset strength</b><br>
</samp>

<table width="1056">
  <tr>
    <td width="464" rowspan="2" align="left">
      <video src="https://github.com/user-attachments/assets/d13351b8-2867-495a-a53f-82f97047bd62" autoplay loop muted playsinline width="464" height="348"></video>
      <sub><samp>audio should work^^^ otherwise: <a href="https://github.com/user-attachments/assets/d13351b8-2867-495a-a53f-82f97047bd62">waveform-terrain-3d-dc.webm</a>)</samp></sub>
    </td>
    <td width="296" rowspan="2" align="center">
      <img src="https://github.com/user-attachments/assets/fea18140-8d4c-4117-9283-74b7b15742d1" alt="waveform_terrain.png" width="292"><br>
      <samp>waveform terrain</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/a50316b4-21fb-4ffb-8e5f-bd6980f2d1c8" alt="waveform_terrain_hilbert_overlay.png" width="142"><br>
      <samp>hilbert</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/9c48420e-90d9-4dc2-90c4-eafd3ed73682" alt="waveform_terrain_rms.png" width="142"><br>
      <samp>rms</samp>
    </td>
  </tr>
  <tr>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/5403340f-0bcc-426b-94d6-67ddae7e98cb" alt="waveform_terrain_onset_curtain.png" width="142"><br>
      <samp>onset-curtains</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/f04592af-597d-4985-a508-d2868b7e7121" alt="waveform_terrain_onset_scribbles.png" width="142"><br>
      <samp>onset-scribbles</samp>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;[fft terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<a href="https://github.com/meisei4/fffftt/blob/main/src/fft_terrain_3d_dc.c">src/fft_terrain_3d_dc.c</a> ........... <b>chroma/pitch-class</b> and <b>onset strength</b><br>
</samp>

<table width="1056">
  <tr>
    <td width="464" rowspan="2" align="left">
      <video src="https://github.com/user-attachments/assets/fb570c32-689a-4234-ad4b-731058060f1c" autoplay loop muted playsinline width="464" height="348"></video>
      <sub><samp>audio should work^^^ otherwise: <a href="https://github.com/user-attachments/assets/fb570c32-689a-4234-ad4b-731058060f1c">fft-terrain-3d-dc.webm</a>)</samp></sub>
    </td>
    <td width="296" rowspan="2" align="center">
      <img src="https://github.com/user-attachments/assets/5a220009-1159-43a5-9bc0-177c611bc6a4" alt="fft_terrain.png" width="292"><br>
      <samp>fft terrain</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/5e665f0b-0eb5-4e2a-8806-c7071e8de617" alt="fft_terrain_chroma.png" width="142"><br>
      <samp>chroma</samp>
    </td>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/a86284ca-6ced-4a8b-a9df-765beec42606" alt="fft_terrain_onset_curtain.png" width="142"><br>
      <samp>onset-curtains</samp>
    </td>
  </tr>
  <tr>
    <td width="148" align="center">
      <img src="https://github.com/user-attachments/assets/2d7ba571-c45e-43ac-888a-8fa9f462f3ba" alt="fft_terrain_onset_scribbles.png" width="142"><br>
      <samp>onset-scribbles</samp>
    </td>
    <td width="148" align="center">
      <samp>&nbsp;</samp>
    </td>
  </tr>
</table>
<br>

### <samp>[shadertoy ports]</samp>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[meisei4: basic fft visualizer]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Shadertoy: <a href="https://www.shadertoy.com/view/t3jGzm">https://www.shadertoy.com/view/t3jGzm</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/sh4zam_butterfly.c">src/sh4zam_butterfly.c</a><br>
</samp>
<table width="928">
  <tr>
    <td width="464" align="center"><samp>Dreamcast capture</samp></td>
    <td width="464" align="center"><samp>Shadertoy capture</samp></td>
  </tr>
  <tr>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/5ae63435-af8d-43d6-ad17-4fd3561f9044" autoplay loop muted playsinline width="464" height="261"></video><br>
    </td>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/6307767e-6daa-4373-b82d-ea3874780da1" autoplay loop muted playsinline width="464" height="261"></video><br>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Cotterzz: Visualiser - Picking out notes]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Shadertoy: <a href="https://www.shadertoy.com/view/tcG3Rm">https://www.shadertoy.com/view/tcG3Rm</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/picking_out_notes_dc.c">src/picking_out_notes_dc.c</a><br>
</samp>
<table width="928">
  <tr>
    <td width="464" align="center"><samp>Dreamcast capture</samp></td>
    <td width="464" align="center"><samp>Shadertoy capture</samp></td>
  </tr>
  <tr>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/7ea166a4-ad9a-4b63-bc0d-38deca84fe7d" autoplay loop muted playsinline width="464" height="261"></video><br>
    </td>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/585d6fdc-e112-4abf-b1d5-6ca5e61e10fc" autoplay loop muted playsinline width="464" height="261"></video><br>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[meisei4: basic waveform data visualizer]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Shadertoy: <a href="https://www.shadertoy.com/view/tcSXRz">https://www.shadertoy.com/view/tcSXRz</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/waveform_dc.c">src/waveform_dc.c</a><br>
</samp>
<table width="928">
  <tr>
    <td width="464" align="center"><samp>Dreamcast capture</samp></td>
    <td width="464" align="center"><samp>Shadertoy capture</samp></td>
  </tr>
  <tr>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/e5b5b2c4-6591-4bec-a266-483c9039ba99" autoplay loop muted playsinline width="464" height="261"></video><br>
    </td>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/30336a15-8fbd-474a-a6d1-4a0be1c7f44c" autoplay loop muted playsinline width="464" height="261"></video><br>
    </td>
  </tr>
</table>
<br>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[meisei4: sound envelope reference]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Shadertoy: <a href="https://www.shadertoy.com/view/7fXSWB">https://www.shadertoy.com/view/7fXSWB</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/sound_envelope_2d_iso_dc.c">src/sound_envelope_2d_iso_dc.c</a> / <a href="https://github.com/meisei4/fffftt/blob/main/src/sound_envelope_3d_dc.c">src/sound_envelope_3d_dc.c</a><br>
</samp>
<table width="927">
  <tr>
    <td width="309" align="center"><samp>Dreamcast 2D</samp></td>
    <td width="309" align="center"><samp>Dreamcast 3D</samp></td>
    <td width="309" align="center"><samp>Shadertoy capture</samp></td>
  </tr>
  <tr>
    <td width="309" align="center">
      <video src="https://github.com/user-attachments/assets/2ebd79ba-88e6-480b-b582-8af83984047b" autoplay loop muted playsinline width="309" height="174"></video><br>
    </td>
    <td width="309" align="center">
      <video src="https://github.com/user-attachments/assets/2e76902a-c7d0-4699-8088-43c411da253f" autoplay loop muted playsinline width="309" height="174"></video><br>
    </td>
    <td width="309" align="center">
      <video src="https://github.com/user-attachments/assets/9f779cbc-380e-4335-bbba-8880347f82e5" autoplay loop muted playsinline width="309" height="174"></video><br>
    </td>
  </tr>
</table>
<br>


<samp>[submodules]</samp>

```bash
git clone --recurse-submodules https://github.com/meisei4/fffftt.git
# otherwise:
git clone https://github.com/meisei4/fffftt.git
cd fffftt
git submodule update --init --recursive
```

<samp>[all demos]</samp>

```bash
# [fft]
# [fft]
make sh4zam-butterfly             # run: ./bin/sh4zam_butterfly        # elf: ./build/dc/sh4zam_butterfly/sh4zam_butterfly.elf
make picking-out-notes-dc         # run: ./bin/picking_out_notes_dc    # elf: ./build/dc/picking_out_notes_dc/picking_out_notes_dc.elf
make fft-terrain-3d-dc            # run: ./bin/fft_terrain_3d_dc       # elf: ./build/dc/fft_terrain_3d_dc/fft_terrain_3d_dc.elf
make fft-bands-terrain-3d-dc      # run: ./bin/fft_bands_terrain_3d_dc # elf: ./build/dc/fft_bands_terrain_3d_dc/fft_bands_terrain_3d_dc.elf

# [waveform]
make waveform-dc                  # run: ./bin/waveform_dc             # elf: ./build/dc/waveform_dc/waveform_dc.elf
make sound-envelope-2d-iso-dc     # run: ./bin/sound_envelope_2d_iso_dc # elf: ./build/dc/sound_envelope_2d_iso_dc/sound_envelope_2d_iso_dc.elf
make sound-envelope-3d-dc         # run: ./bin/sound_envelope_3d_dc    # elf: ./build/dc/sound_envelope_3d_dc/sound_envelope_3d_dc.elf
make waveform-terrain-3d-dc       # run: ./bin/waveform_terrain_3d_dc  # elf: ./build/dc/waveform_terrain_3d_dc/waveform_terrain_3d_dc.elf
```


<samp>[run / build / clean]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;flycast_run.sh .................... <a href="https://github.com/meisei4/fffftt/blob/main/flycast_run.sh">flycast_run.sh</a> runs wrappers in ./bin/<br>
  &nbsp;&nbsp;&nbsp;&nbsp;.elf targets ...................... /pc assets enter CDI build via mkdcdisc<br>
  &nbsp;&nbsp;&nbsp;&nbsp;build.sh .......................... <a href="https://github.com/meisei4/fffftt/blob/main/build.sh">build.sh</a> builds all examples<br>
  &nbsp;&nbsp;&nbsp;&nbsp;make clean-all .................... removes build artifacts and .cdi images<br>
</samp>


<samp>[third party]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;raylib ............................ <a href="https://www.raylib.com/">raylib.com</a> / <a href="https://github.com/raysan5/raylib">github.com/raysan5/raylib</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;raylib4Consoles ................... <a href="https://github.com/raylib4Consoles/raylib">github.com/raylib4Consoles/raylib</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;raylib4Dreamcast .................. <a href="https://github.com/raylib4Consoles/raylib4Dreamcast">github.com/raylib4Consoles/raylib4Dreamcast</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;GLdc .............................. <a href="https://gitlab.com/simulant/GLdc/-/tree/master">gitlab.com/simulant/GLdc</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;sh4zam ............................ <a href="https://github.com/gyrovorbis/sh4zam/">github.com/gyrovorbis/sh4zam</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;KallistiOS ........................ <a href="https://github.com/KallistiOS/KallistiOS">github.com/KallistiOS/KallistiOS</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;KOS docs .......................... <a href="https://kos-docs.dreamcast.wiki/">kos-docs.dreamcast.wiki</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;mkdcdisc .......................... <a href="https://gitlab.com/simulant/mkdcdisc">gitlab.com/simulant/mkdcdisc</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;Shadertoy ......................... <a href="https://www.shadertoy.com/">shadertoy.com</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;Shadertoy media ................... <a href="https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/">Shadertoy media files</a><br>
</samp>


<samp>[todo]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;add polygon mode for QUAD wire frame in GLdc<br>
  &nbsp;&nbsp;&nbsp;&nbsp;build gates for an OpenGL 1.1 desktop platform port
</samp>


<samp>[license]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;This project is free software: you can redistribute it and/or modify it<br>
  &nbsp;&nbsp;&nbsp;&nbsp;under the terms of the GNU General Public License as published by the<br>
  &nbsp;&nbsp;&nbsp;&nbsp;Free Software Foundation, either version 2 of the License, or<br>
  &nbsp;&nbsp;&nbsp;&nbsp;(at your option) any later version.<br>
  <br>
  &nbsp;&nbsp;&nbsp;&nbsp;This project is distributed in the hope that it will be useful, but<br>
  &nbsp;&nbsp;&nbsp;&nbsp;WITHOUT ANY WARRANTY; without even the implied warranty of<br>
  &nbsp;&nbsp;&nbsp;&nbsp;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br>
  <br>
  &nbsp;&nbsp;&nbsp;&nbsp;See <a href="./LICENSE">LICENSE</a> for details.<br>
</samp>
