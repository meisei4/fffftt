<samp><b>(f)ixed (f)unction and (f)ast (f)ourier (t)ransform (t)esting</b></samp>
---
<samp>
  <code>fffftt</code> is an audio-geometry exhibit for fixed-function signal visualization, originally inspired by <a href="https://www.shadertoy.com/">shadertoy</a>. It targets Dreamcast as a hardware case study using <a href="https://github.com/raysan5/raylib">raylib</a>, OpenGL 1.1 <a href="https://en.wikipedia.org/wiki/Fixed-function_(computer_graphics)">fixed-function</a> rendering, <a href="https://github.com/kazade/GLdc">GLdc</a>, and <a href="https://github.com/gyrovorbis/sh4zam">sh4zam</a>'s <code>shz_fft()</code>.<br><br>
</samp>


<samp>[overview]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;.h file ........................... shared prototypes/functions in <a href="https://github.com/meisei4/fffftt/blob/main/src/fffftt.h">src/fffftt.h</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;.c files .......................... one demo per file in <a href="https://github.com/meisei4/fffftt/tree/main/src">src/</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;.wav files ........................ <a href="https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/">shadertoy media files</a>; adpcm .wavs in <a href="https://github.com/meisei4/fffftt/tree/main/src/resources">src/resources</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;controls .......................... cycle tracks, pause/resume, seek, move light<br><!-- add a nice control scheme for gamepad-->
  &nbsp;&nbsp;&nbsp;&nbsp;audio terrain demos ............... see <a href="#audio-terrain">[audio terrain]</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;shadertoy ports ................... see <a href="#shadertoy-ports">[shadertoy ports]</a><br>
<br>
</samp>


<samp>[audio terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;[waveform terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<a href="https://github.com/meisei4/fffftt/blob/main/src/waveform_terrain_3d_dc.c">src/waveform_terrain_3d_dc.c</a> ...... uses <b>Hilbert envelope</b>, <b>RMS</b>, and naive <b>onset strength</b><br>
</samp>
<!-- ADD WAVFROM TERRAIN VIDEOS!!!!!!-->

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;[fft terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<a href="https://github.com/meisei4/fffftt/blob/main/src/fft_terrain_3d_dc.c">src/fft_terrain_3d_dc.c</a> ........... uses <b>chroma</b> and spectral-flux <b>onset strength</b><br>
</samp>
<!-- ADD FFT TERRAIN VIDEOS!!!!!!-->

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;[fft bands terrain]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<a href="https://github.com/meisei4/fffftt/blob/main/src/fft_bands_terrain_3d_dc.c">src/fft_bands_terrain_3d_dc.c</a> ..... uses low/mid/high FFT bands + <b>spectral flatness</b><br><br>
</samp>
<!-- ADD BANDS TERRAIN VIDEOS!!!!!!-->

<!-- ADD THE PRIMARY PROXY PLOTS FOR EACH TERRAIN VIDEO UNDERNEATH-->

<!-- POTENTIALLY MOVE THIS TO A SEPARATE DOC
<samp>[audio terrain context]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<b>onset strength</b> .................... <a href="https://en.wikipedia.org/wiki/Onset_(audio)">Wikipedia: onset</a>, <a href="https://librosa.org/doc/main/generated/librosa.onset.onset_strength.html">librosa: onset_strength</a>, <a href="https://librosa.org/doc/main/generated/librosa.onset.onset_detect.html">librosa: onset_detect</a><br>
</samp>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;<b>spectral flatness</b> ................. <a href="https://en.wikipedia.org/wiki/Spectral_flatness">Wikipedia: spectral flatness</a>, <a href="https://librosa.org/doc/main/generated/librosa.feature.spectral_flatness.html">librosa: spectral_flatness</a><br>
</samp>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;<b>chroma</b> ............................ <a href="https://en.wikipedia.org/wiki/Chroma_feature">Wikipedia: chroma feature</a>, <a href="https://librosa.org/doc/main/generated/librosa.feature.chroma_stft.html">librosa: chroma_stft</a>, <a href="https://librosa.org/doc/main/generated/librosa.filters.chroma.html">librosa: chroma filters</a><br>
</samp>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;<b>Hilbert-filtered envelope</b> ......... <a href="https://en.wikipedia.org/wiki/Hilbert_transform">Wikipedia: Hilbert transform</a>, <a href="https://en.wikipedia.org/wiki/Analytic_signal">Wikipedia: analytic signal</a>, <a href="https://www.mathworks.com/help/signal/ref/envelope.html">MATLAB: envelope</a>, <a href="https://www.mathworks.com/help/signal/ref/hilbert.html">MATLAB: hilbert</a><br>
</samp>
<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;<b>RMS</b> ............................... <a href="https://en.wikipedia.org/wiki/Root_mean_square">Wikipedia: root mean square</a>, <a href="https://librosa.org/doc/main/generated/librosa.feature.rms.html">librosa: rms</a>, <a href="https://www.mathworks.com/help/signal/ref/envelope.html">MATLAB: envelope</a><br>
<br>
</samp>
-->


<!-- NEW HUD INCLUDED VIDEOS COMING-->
<!-- REFERENCE TO drafting branch?-->
<samp>[shadertoy ports]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[meisei4: basic fft visualizer]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;shadertoy: <a href="https://www.shadertoy.com/view/t3jGzm">https://www.shadertoy.com/view/t3jGzm</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/sh4zam_butterfly.c">src/sh4zam_butterfly.c</a><br>
  <!-- FIGURE OUT A READABLE CONVENTION HERE FOR HOW TO MAKE IT CLEAR WHERE THE C PROGRAM IS OR IF WE MENTION THE MAKE TARGET IDK!!!!!!!-->
</samp>
<table width="928">
  <tr>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/358c96bd-2520-42e1-acb0-8aec52358ad5" autoplay loop muted playsinline width="464" height="261"></video><br>
      <sub><samp>Dreamcast capture</samp></sub>
    </td>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/9538fb97-79ec-434c-b659-74deec5030a3" autoplay loop muted playsinline width="464" height="261"></video><br>
      <sub><samp>Shadertoy capture</samp></sub>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Cotterzz: Visualiser - Picking out notes]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;shadertoy: <a href="https://www.shadertoy.com/view/tcG3Rm">https://www.shadertoy.com/view/tcG3Rm</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/picking_out_notes_dc.c">src/picking_out_notes_dc.c</a><br>
</samp>
<table width="928">
  <tr>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/f942edb3-cb31-4095-851c-5b907f006d29" autoplay loop muted playsinline width="464" height="261"></video><br>
      <sub><samp>Dreamcast capture</samp></sub>
    </td>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/ff68e8b7-095f-4a61-b838-3e82e0a52e5d" autoplay loop muted playsinline width="464" height="261"></video><br>
      <sub><samp>Shadertoy capture</samp></sub>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[meisei4: basic waveform data visualizer]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;shadertoy: <a href="https://www.shadertoy.com/view/tcSXRz">https://www.shadertoy.com/view/tcSXRz</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/waveform_dc.c">src/waveform_dc.c</a><br>
</samp>
<table width="928">
  <tr>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/9dab963d-eabd-40d1-b541-5c6f3e64c544" autoplay loop muted playsinline width="464" height="261"></video><br>
      <sub><samp>Dreamcast capture</samp></sub>
    </td>
    <td width="464" align="center">
      <video src="https://github.com/user-attachments/assets/df219ba5-18be-4a7e-a023-edb22fd3e6ea" autoplay loop muted playsinline width="464" height="261"></video><br>
      <sub><samp>Shadertoy capture</samp></sub>
    </td>
  </tr>
</table>
<br>

<samp>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[meisei4: sound envelope reference]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;shadertoy: <a href="https://www.shadertoy.com/view/7fXSWB">https://www.shadertoy.com/view/7fXSWB</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src: <a href="https://github.com/meisei4/fffftt/blob/main/src/sound_envelope_dc.c">src/sound_envelope_dc.c</a> / <a href="https://github.com/meisei4/fffftt/blob/main/src/sound_envelope_3d_dc.c">sound_envelope_3d_dc.c</a><br>
    <!-- Maybe explain iso vs 3d -->
</samp>
<table width="928">
  <tr>
    <td width="232" align="center">
      <video src="https://github.com/user-attachments/assets/ac36e5e0-c8a5-42bc-aa92-9303712dc4a7" autoplay loop muted playsinline width="232" height="131"></video><br>
      <sub><samp>Dreamcast 2D isometric</samp></sub>
    </td>
    <td width="232" align="center">
      <video src="https://github.com/user-attachments/assets/d9d90e0c-82a5-44ed-a485-5b57db02a299" autoplay loop muted playsinline width="232" height="131"></video><br>
      <sub><samp>Dreamcast 3D</samp></sub>
    </td>
    <td width="232" align="center">
      <video src="https://github.com/user-attachments/assets/d34794b5-c1d7-47bd-9e90-fc808389ccb4" autoplay loop muted playsinline width="232" height="131"></video><br>
      <sub><samp>GL33 capture</samp></sub>
    </td>
    <td width="232" align="center">
      <video src="https://github.com/user-attachments/assets/71ac43c5-8667-4efa-9bea-558f183f5602" autoplay loop muted playsinline width="232" height="131"></video><br>
      <sub><samp>Shadertoy capture</samp></sub>
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


<!-- 
<samp>[Makefile]<br>
&nbsp;&nbsp;&nbsp;&nbsp;Makefile <a href="https://github.com/meisei4/fffftt/blob/main/Makefile">Makefile</a> expects a configured <code>KallistiOS</code> / <code>KOS</code> environment.<br>
&nbsp;&nbsp;&nbsp;&nbsp;Dreamcast development <a href="https://dreamcast.wiki/Getting_Started_with_Dreamcast_development">Getting Started with Dreamcast development</a>.<br>
</samp>
-->


<samp>[all demos]</samp>

```bash
# [fft]
make sh4zam-butterfly             # run: ./bin/sh4zam_butterfly
make picking-out-notes-dc         # run: ./bin/picking_out_notes_dc
make fft-terrain-3d-dc            # run: ./bin/fft_terrain_3d_dc
make fft-bands-terrain-3d-dc      # run: ./bin/fft_bands_terrain_3d_dc

# [waveform]
make waveform-dc                  # run: ./bin/waveform_dc
make sound-envelope-dc            # run: ./bin/sound_envelope_dc
make sound-envelope-3d-dc         # run: ./bin/sound_envelope_3d_dc
make waveform-terrain-3d-dc       # run: ./bin/waveform_terrain_3d_dc
```


<samp>[run / build / clean]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;flycast_run.sh .................... <a href="https://github.com/meisei4/fffftt/blob/main/flycast_run.sh">flycast_run.sh</a> runs wrappers in <code>./bin/</code><br>
  &nbsp;&nbsp;&nbsp;&nbsp;.elf targets ...................... <code>/pc</code> assets enter CDI build via <code>mkdcdisc</code><br>
  &nbsp;&nbsp;&nbsp;&nbsp;build.sh .......................... <a href="https://github.com/meisei4/fffftt/blob/main/build.sh">build.sh</a> builds all examples<br>
  &nbsp;&nbsp;&nbsp;&nbsp;make clean-all .................... removes build artifacts and <code>.cdi</code> images<br>
</samp>


<samp>[third party]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;raylib ............................ <a href="https://www.raylib.com/">raylib.com</a> / <a href="https://github.com/raysan5/raylib">github.com/raysan5/raylib</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;raylib4Consoles ................... <a href="https://github.com/raylib4Consoles">github.com/raylib4Consoles</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;raylib4Dreamcast .................. <a href="https://github.com/raylib4Consoles/raylib4Dreamcast">github.com/raylib4Consoles/raylib4Dreamcast</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;GLdc .............................. <a href="https://gitlab.com/simulant/GLdc/-/tree/master">gitlab.com/simulant/GLdc</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;sh4zam ............................ <a href="https://github.com/gyrovorbis/sh4zam/">github.com/gyrovorbis/sh4zam</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;KallistiOS ........................ <a href="https://github.com/KallistiOS/KallistiOS">github.com/KallistiOS/KallistiOS</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;KOS docs .......................... <a href="https://kos-docs.dreamcast.wiki/">kos-docs.dreamcast.wiki</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;mkdcdisc .......................... <a href="https://gitlab.com/simulant/mkdcdisc">gitlab.com/simulant/mkdcdisc</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;shadertoy ......................... <a href="https://www.shadertoy.com/">shadertoy.com</a><br>
  &nbsp;&nbsp;&nbsp;&nbsp;shadertoy media ................... <a href="https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/">shadertoy media files</a><br>
</samp>


<samp>[todo]<br>
  &nbsp;&nbsp;&nbsp;&nbsp;add polygon mode for QUAD wire frame in GLdc<br>
  &nbsp;&nbsp;&nbsp;&nbsp;add gates for OpenGL 1.1 raylib desktop builds
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
