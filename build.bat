@echo off
setlocal
cd /D "%~dp0"

set target=%1
if "%target%"=="" set target=all

set build=build\desktop
set bin=bin\desktop
set obj=%build%\obj
set raylib_build=%build%\raylib
set fftw=%build%\fftw3
set gl=%build%\GL

set fftw_stub=%build%\fftw3_empty_solvtab.c
set fftw_stub_obj=%build%\fftw3_empty_solvtab.obj
set raylib_lib=%raylib_build%\raylib.lib

if "%target%"=="clean" (
  rmdir /s /q "%build%" 2>nul
  rmdir /s /q "%bin%" 2>nul
  exit /b 0
)

mkdir "%build%" 2>nul
mkdir "%bin%" 2>nul
mkdir "%obj%" 2>nul
mkdir "%raylib_build%" 2>nul
mkdir "%gl%" 2>nul

rmdir /s /q "%fftw%" 2>nul

cmake -S fftw3 -B "%fftw%" -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DBUILD_TESTS=OFF ^
  -DDISABLE_FORTRAN=ON ^
  -DENABLE_FLOAT=OFF || exit /b 1

cmake --build "%fftw%" --target fftw3 || exit /b 1

for /f "delims=" %%F in ('dir /b /s "%fftw%\*fftw3*.lib"') do set fftw_lib=%%F

> "%fftw_stub%" echo #include "config.h"
>>"%fftw_stub%" echo #include "kernel/ifftw.h"
>>"%fftw_stub%" echo const solvtab X(solvtab_dft_standard) = { SOLVTAB_END };
>>"%fftw_stub%" echo const solvtab X(solvtab_rdft_r2cf) = { SOLVTAB_END };
>>"%fftw_stub%" echo const solvtab X(solvtab_rdft_r2cb) = { SOLVTAB_END };
>>"%fftw_stub%" echo const solvtab X(solvtab_rdft_r2r) = { SOLVTAB_END };

cl /nologo /c /O2 /MD /std:c17 ^
  /I"%fftw%" /I"fftw3" /I"fftw3\kernel" ^
  "%fftw_stub%" /Fo"%fftw_stub_obj%" || exit /b 1

set raylib_src=raylib_desktop\src
set raylib_inc=/I"%raylib_src%" /I"%raylib_src%\external\glfw\include"
set raylib_defs=/DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_11 /DBUILD_LIBTYPE_STATIC /D_CRT_SECURE_NO_WARNINGS

del /q "%raylib_build%\*.obj" "%raylib_build%\*.lib" 2>nul

cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\raudio.c"    /Fo"%raylib_build%\raudio.obj"    || exit /b 1
cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\rcore.c"     /Fo"%raylib_build%\rcore.obj"     || exit /b 1
cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\rglfw.c"     /Fo"%raylib_build%\rglfw.obj"     || exit /b 1
cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\rmodels.c"   /Fo"%raylib_build%\rmodels.obj"   || exit /b 1
cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\rshapes.c"   /Fo"%raylib_build%\rshapes.obj"   || exit /b 1
cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\rtext.c"     /Fo"%raylib_build%\rtext.obj"     || exit /b 1
cl /nologo /c /O2 /MD /std:c17 %raylib_defs% %raylib_inc% "%raylib_src%\rtextures.c" /Fo"%raylib_build%\rtextures.obj" || exit /b 1

lib /nologo /OUT:"%raylib_lib%" "%raylib_build%\*.obj" || exit /b 1

set sdk_gl=%WindowsSdkDir%Include\%WindowsSDKVersion%um\GL\gl.h
set sdk_gl=%sdk_gl:\=/%

> "%gl%\gl.h" echo #ifndef APIENTRY
>>"%gl%\gl.h" echo #define APIENTRY __stdcall
>>"%gl%\gl.h" echo #endif
>>"%gl%\gl.h" echo #ifndef WINGDIAPI
>>"%gl%\gl.h" echo #define WINGDIAPI __declspec(dllimport)
>>"%gl%\gl.h" echo #endif
>>"%gl%\gl.h" echo #include "%sdk_gl%"

mkdir "%bin%\src\resources" 2>nul
copy /Y "src\resources\vga.fnt" "%bin%\src\resources\" >nul
copy /Y "src\resources\vga.png" "%bin%\src\resources\" >nul
copy /Y "src\resources\experiment_22k_mono_adpcm.wav" "%bin%\src\resources\" >nul
copy /Y "src\resources\electronebl_22k_mono_adpcm.wav" "%bin%\src\resources\" >nul 2>nul
copy /Y "src\resources\8bit_22k_mono_adpcm.wav" "%bin%\src\resources\" >nul 2>nul
copy /Y "src\resources\geometric_person_22k_mono_adpcm.wav" "%bin%\src\resources\" >nul 2>nul
copy /Y "src\resources\tropical_22k_mono_adpcm.wav" "%bin%\src\resources\" >nul 2>nul
copy /Y "src\resources\xtrack_22k_mono_adpcm.wav" "%bin%\src\resources\" >nul 2>nul

set demo_inc=/I"%build%" /I"raylib_desktop\src" /I"raylib_desktop\src\external\glfw\include" /I"fftw3\api" /I"%fftw%" /I"src"
set demo_defs=/DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_11 /D_CRT_SECURE_NO_WARNINGS
set demo_libs="%raylib_lib%" "%fftw_lib%" "%fftw_stub_obj%" opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib

if "%target%"=="all" (
  call :build_demo sh4zam-butterfly src\sh4zam_butterfly.c || exit /b 1
  call :build_demo picking-out-notes src\picking_out_notes.c || exit /b 1
  call :build_demo waveform src\waveform.c || exit /b 1
  call :build_demo sound-envelope-2d-iso src\sound_envelope_2d_iso.c || exit /b 1
  call :build_demo sound-envelope-3d src\sound_envelope_3d.c || exit /b 1
  call :build_demo waveform-terrain-3d src\waveform_terrain_3d.c || exit /b 1
  call :build_demo fft-terrain-3d src\fft_terrain_3d.c || exit /b 1
  call :build_demo fft-bands-terrain-3d src\fft_bands_terrain_3d.c || exit /b 1
  exit /b 0
)

if "%target%"=="sh4zam-butterfly" call :build_demo sh4zam-butterfly src\sh4zam_butterfly.c & exit /b %errorlevel%
if "%target%"=="picking-out-notes" call :build_demo picking-out-notes src\picking_out_notes.c & exit /b %errorlevel%
if "%target%"=="waveform" call :build_demo waveform src\waveform.c & exit /b %errorlevel%
if "%target%"=="sound-envelope-2d-iso" call :build_demo sound-envelope-2d-iso src\sound_envelope_2d_iso.c & exit /b %errorlevel%
if "%target%"=="sound-envelope-3d" call :build_demo sound-envelope-3d src\sound_envelope_3d.c & exit /b %errorlevel%
if "%target%"=="waveform-terrain-3d" call :build_demo waveform-terrain-3d src\waveform_terrain_3d.c & exit /b %errorlevel%
if "%target%"=="fft-terrain-3d" call :build_demo fft-terrain-3d src\fft_terrain_3d.c & exit /b %errorlevel%
if "%target%"=="fft-bands-terrain-3d" call :build_demo fft-bands-terrain-3d src\fft_bands_terrain_3d.c & exit /b %errorlevel%

exit /b 1

:build_demo
cl /nologo /O2 /MD /std:c17 %demo_defs% %demo_inc% ^
  "%~2" ^
  /Fo"%obj%\%~1.obj" ^
  /Fe"%bin%\%~1.exe" ^
  /link %demo_libs%
exit /b %errorlevel%
