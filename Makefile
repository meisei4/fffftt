.DEFAULT_GOAL := help

SHELL := /bin/bash
SELF_MAKEFILE := $(lastword $(MAKEFILE_LIST))

BUILD_DIR := build
BIN_DIR := bin
SRC_DIR := src
FFTW_1997_DIR := fftw_1997
ROMDISK_DIR := $(SRC_DIR)/romdisk
GLDC_DIR := GLdc
GLDC_BUILD_DIR := $(BUILD_DIR)/dc/gldc
GLDC_LIB := $(GLDC_BUILD_DIR)/libGL.a
GLDC_INCLUDE_DIR := $(abspath $(GLDC_DIR))/include
GLDC_INCLUDE_CFLAG := -I$(GLDC_INCLUDE_DIR)
GLDC_TOOLCHAIN := $(KOS_BASE)/utils/cmake/kallistios.toolchain.cmake
############################ FLOAT PRECISION ####################### 
# This will not work as i expect, need to figure out whats happening
# FFT_FLAGS += -DFFTW_ENABLE_FLOAT

############################ NUMERIC OPTIMIZATION OFF ##############
# fast math does not neccessarily mean "faster", TODO: figure out if convention is needed per example
FFT_FLAGS += -fno-fast-math -ffp-contract=off

############################ FFT PROFILING ######################
#FFT_FLAGS += -DFFT_ENABLE_PROFILE

############################ PERIOD ################################ 
ALT_AUDIO_DEVICE_PERIOD_FRAMES_KOS_CFLAGS :=
PERIOD_FRAMES := 1024 # Uncomment  below line for 1024-period testing
#ALT_AUDIO_DEVICE_PERIOD_FRAMES_KOS_CFLAGS := KOS_CFLAGS="$(KOS_CFLAGS) -DAUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=$(PERIOD_FRAMES)"

GL33_RAYLIB_DIR := $(BUILD_DIR)/gl33/raylib
GL11_RAYLIB_DIR := $(BUILD_DIR)/gl11/raylib
DC_RAYLIB_DIR := $(BUILD_DIR)/dc/raylib
SH4_RAYLIB_DIR := $(BUILD_DIR)/sh4/raylib
DC_TARGET_DIR := $(BUILD_DIR)/dc/cool_dc
SH4_TARGET_DIR := $(BUILD_DIR)/sh4/sh4zam_butterfly
DC_FFTW_TARGET_DIR := $(BUILD_DIR)/dc/fftw_dc
AUDIO_ONLY_WAV_TARGET_DIR := $(BUILD_DIR)/dc/audio_only_wav_dc
AUDIO_ONLY_MP3_TARGET_DIR := $(BUILD_DIR)/dc/audio_only_mp3_dc
PICKING_OUT_NOTES_DC_TARGET_DIR := $(BUILD_DIR)/dc/picking_out_notes_dc
WAVEFORM_DC_TARGET_DIR := $(BUILD_DIR)/dc/waveform_dc
SOUND_ENVELOPE_DC_TARGET_DIR := $(BUILD_DIR)/dc/sound_envelope_dc
SOUND_ENVELOPE_3D_DC_TARGET_DIR := $(BUILD_DIR)/dc/sound_envelope_3d_dc
SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR := $(BUILD_DIR)/dc/sound_envelope_3d_audio_cadence_dc
WAVEFORM_TERRAIN_3D_DC_TARGET_DIR := $(BUILD_DIR)/dc/waveform_terrain_3d_dc
FFT_TERRAIN_3D_DC_TARGET_DIR := $(BUILD_DIR)/dc/fft_terrain_3d_dc
DC_MESH_DRAW_MODE_TARGET_DIR := $(BUILD_DIR)/dc/gldc_mesh_draw_mode_dc

GL33_BIN := $(BIN_DIR)/cool_gl33
FFTW_BIN := $(BIN_DIR)/fftw_gl11
GL11_BIN := $(BIN_DIR)/cool_gl11
DC_LAUNCHER := $(BIN_DIR)/cool_dc
SH4_LAUNCHER := $(BIN_DIR)/sh4zam_butterfly
DC_FFTW_LAUNCHER := $(BIN_DIR)/fftw_dc
AUDIO_ONLY_WAV_LAUNCHER := $(BIN_DIR)/audio_only_wav_dc
AUDIO_ONLY_MP3_LAUNCHER := $(BIN_DIR)/audio_only_mp3_dc
PICKING_OUT_NOTES_DC_LAUNCHER := $(BIN_DIR)/picking_out_notes_dc
WAVEFORM_DC_LAUNCHER := $(BIN_DIR)/waveform_dc
SOUND_ENVELOPE_DC_LAUNCHER := $(BIN_DIR)/sound_envelope_dc
SOUND_ENVELOPE_3D_DC_LAUNCHER := $(BIN_DIR)/sound_envelope_3d_dc
SOUND_ENVELOPE_GL33_LAUNCHER := $(BIN_DIR)/sound_envelope_gl33
SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_LAUNCHER := $(BIN_DIR)/sound_envelope_3d_audio_cadence_dc
WAVEFORM_TERRAIN_3D_DC_LAUNCHER := $(BIN_DIR)/waveform_terrain_3d_dc
FFT_TERRAIN_3D_DC_LAUNCHER := $(BIN_DIR)/fft_terrain_3d_dc
DC_MESH_DRAW_MODE_LAUNCHER := $(BIN_DIR)/gldc_mesh_draw_mode_dc

DC_TARGET := $(DC_TARGET_DIR)/cool_dc.elf
SH4_TARGET := $(SH4_TARGET_DIR)/sh4zam_butterfly.elf
DC_FFTW_TARGET := $(DC_FFTW_TARGET_DIR)/fftw_dc.elf
AUDIO_ONLY_WAV_TARGET := $(AUDIO_ONLY_WAV_TARGET_DIR)/audio_only_wav_dc.elf
AUDIO_ONLY_MP3_TARGET := $(AUDIO_ONLY_MP3_TARGET_DIR)/audio_only_mp3_dc.elf
PICKING_OUT_NOTES_DC_TARGET := $(PICKING_OUT_NOTES_DC_TARGET_DIR)/picking_out_notes_dc.elf
WAVEFORM_DC_TARGET := $(WAVEFORM_DC_TARGET_DIR)/waveform_dc.elf
SOUND_ENVELOPE_DC_TARGET := $(SOUND_ENVELOPE_DC_TARGET_DIR)/sound_envelope_dc.elf
SOUND_ENVELOPE_3D_DC_TARGET := $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/sound_envelope_3d_dc.elf
SOUND_ENVELOPE_GL33_TARGET := $(BIN_DIR)/sound_envelope_gl33
SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET := $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/sound_envelope_3d_audio_cadence_dc.elf
WAVEFORM_TERRAIN_3D_DC_TARGET := $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/waveform_terrain_3d_dc.elf
FFT_TERRAIN_3D_DC_TARGET := $(FFT_TERRAIN_3D_DC_TARGET_DIR)/fft_terrain_3d_dc.elf
DC_MESH_DRAW_MODE_TARGET := $(DC_MESH_DRAW_MODE_TARGET_DIR)/gldc_mesh_draw_mode_dc.elf

GL33_SOURCE := $(SRC_DIR)/cool_gl33.c
FFTW_SOURCE := $(SRC_DIR)/fftw_gl11.c
FFTW_DC_SOURCE := $(SRC_DIR)/fftw_dc.c
GL11_SOURCE := $(SRC_DIR)/cool_gl11.c
DC_SOURCE := $(SRC_DIR)/cool_dc.c
SH4_SOURCE := $(SRC_DIR)/sh4zam_butterfly.c
SH4ZAM_COMPLEX_SOURCE := sh4zam/source/shz_complex.c
FFTW_1997_SOURCES := $(wildcard $(FFTW_1997_DIR)/*.c)
FFTW_1997_OBJS := $(patsubst $(FFTW_1997_DIR)/%.c,$(DC_FFTW_TARGET_DIR)/fftw_1997_%.o,$(FFTW_1997_SOURCES))
AUDIO_ONLY_WAV_SOURCE := $(SRC_DIR)/audio_only_wav_dc.c
AUDIO_ONLY_MP3_SOURCE := $(SRC_DIR)/audio_only_mp3_dc.c
PICKING_OUT_NOTES_DC_SOURCE := $(SRC_DIR)/picking_out_notes_dc.c
WAVEFORM_DC_SOURCE := $(SRC_DIR)/waveform_dc.c
SOUND_ENVELOPE_DC_SOURCE := $(SRC_DIR)/sound_envelope_dc.c
SOUND_ENVELOPE_3D_DC_SOURCE := $(SRC_DIR)/sound_envelope_3d_dc.c
SOUND_ENVELOPE_GL33_SOURCE := $(SRC_DIR)/sound_envelope_gl33.c
SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_SOURCE := $(SRC_DIR)/sound_envelope_3d_audio_cadence_dc.c
WAVEFORM_TERRAIN_3D_DC_SOURCE := $(SRC_DIR)/waveform_terrain_3d_dc.c
FFT_TERRAIN_3D_DC_SOURCE := $(SRC_DIR)/fft_terrain_3d_dc.c
DC_MESH_DRAW_MODE_SOURCE := $(SRC_DIR)/gldc_mesh_draw_mode.c

RAYLIB_DESKTOP_SRC := raylib_desktop/src
RAYLIB_DC_SRC := raylib_dc/src

KOS_PORTS_INCLUDE := -I$(KOS_PORTS)/include
FMT ?= $(shell command -v clang-format 2>/dev/null || { [ -x /usr/bin/clang-format ] && echo /usr/bin/clang-format; } || { [ -x /opt/homebrew/bin/clang-format ] && echo /opt/homebrew/bin/clang-format; } || { [ -x /Library/Developer/CommandLineTools/usr/bin/clang-format ] && echo /Library/Developer/CommandLineTools/usr/bin/clang-format; } || echo clang-format)
FMT_STYLE ?= .clang-format
FMT_GLSL_SOURCES := $(wildcard $(SRC_DIR)/resources/*.glsl)
FMT_SOURCES := $(GL33_SOURCE) $(FFTW_SOURCE) $(FFTW_DC_SOURCE) $(GL11_SOURCE) $(SH4_SOURCE) $(AUDIO_ONLY_WAV_SOURCE) $(AUDIO_ONLY_MP3_SOURCE) $(PICKING_OUT_NOTES_DC_SOURCE) $(WAVEFORM_DC_SOURCE) $(WAVEFORM_SANDBOX_DC_SOURCE) $(SOUND_ENVELOPE_DC_SOURCE) $(SOUND_ENVELOPE_3D_DC_SOURCE) $(SOUND_ENVELOPE_GL33_SOURCE) $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_SOURCE) $(WAVEFORM_TERRAIN_3D_DC_SOURCE) $(FFT_TERRAIN_3D_DC_SOURCE) $(DC_MESH_DRAW_MODE_SOURCE) $(SRC_DIR)/fffftt.h $(FMT_GLSL_SOURCES)
UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
CC ?= cc

ifeq ($(UNAME_S),Darwin)
DESKTOP_LIBS := -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo
else
DESKTOP_LIBS := -lGL -lc -lm -lpthread -ldl -lrt -lX11
endif

define WRITE_BIN
	printf '%s\n' \
		'#!/usr/bin/env bash' \
		'set -euo pipefail' \
		'd="$$(cd "$$(dirname "$$0")/.." && pwd)"' \
		'exec "$$d/flycast_run.sh" "$$d/$(1)" "$$@"' \
		> $(2)
	chmod +x $(2)
endef

.PHONY: help all cool-gl33 fftw-gl11 cool-gl11 cool-dc sh4zam-butterfly fftw-dc audio-only-wav-dc audio-only-mp3-dc picking-out-notes-dc waveform-dc sound-envelope-dc sound-envelope-3d-dc sound-envelope-gl33 sound-envelope-3d-audio-cadence-dc waveform-terrain-3d-dc fft-terrain-3d-dc gldc-mesh-draw-mode-dc fmt clean-all
help:
	$(error PLEASE DEFINE A TARGET: make cool-gl33 | make cool-gl11 | make fftw-gl11 | make cool-dc | make sh4zam-butterfly | make fftw-dc | make audio-only-wav-dc | make audio-only-mp3-dc | make picking-out-notes-dc | make waveform-dc | make sound-envelope-dc | make sound-envelope-3d-dc | make sound-envelope-gl33 | make sound-envelope-3d-audio-cadence-dc | make waveform-terrain-3d-dc | make fft-terrain-3d-dc | make fmt | make clean-all)

all: help

.PHONY: gldc-lib
gldc-lib:
	@test -f $(GLDC_DIR)/CMakeLists.txt || { echo "GLdc checkout missing at $(GLDC_DIR). Initialize the submodule first."; exit 1; }
	cd $(GLDC_DIR) && cmake -S . -B $(abspath $(GLDC_BUILD_DIR)) --fresh -DCMAKE_TOOLCHAIN_FILE=$(GLDC_TOOLCHAIN) -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
	cmake --build $(GLDC_BUILD_DIR) --target GL --parallel
	@test -f $(GLDC_LIB)

$(GL33_RAYLIB_DIR)/libraylib.a:
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC) clean
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC) PLATFORM=PLATFORM_DESKTOP
	mkdir -p $(GL33_RAYLIB_DIR)
	cp -f $(RAYLIB_DESKTOP_SRC)/libraylib.a $(RAYLIB_DESKTOP_SRC)/raylib.h $(RAYLIB_DESKTOP_SRC)/raymath.h $(RAYLIB_DESKTOP_SRC)/rlgl.h $(GL33_RAYLIB_DIR)/

$(GL11_RAYLIB_DIR)/libraylib.a:
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC) clean
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC) PLATFORM=PLATFORM_DESKTOP GRAPHICS=GRAPHICS_API_OPENGL_11
	mkdir -p $(GL11_RAYLIB_DIR)
	cp -f $(RAYLIB_DESKTOP_SRC)/libraylib.a $(RAYLIB_DESKTOP_SRC)/raylib.h $(RAYLIB_DESKTOP_SRC)/raymath.h $(RAYLIB_DESKTOP_SRC)/rlgl.h $(GL11_RAYLIB_DIR)/

cool-gl33: $(GL33_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR)
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL33_RAYLIB_DIR) $(GL33_SOURCE) $(GL33_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(GL33_BIN)

sound-envelope-gl33: $(GL33_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR)
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL33_RAYLIB_DIR) $(SOUND_ENVELOPE_GL33_SOURCE) $(GL33_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(SOUND_ENVELOPE_GL33_TARGET)

fftw-gl11: $(GL11_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR)
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL11_RAYLIB_DIR) -I$(FFTW_1997_DIR) $(FFT_FLAGS) $(FFTW_SOURCE) $(FFTW_1997_SOURCES) $(GL11_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(FFTW_BIN)

cool-gl11: $(GL11_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR)
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL11_RAYLIB_DIR) $(GL11_SOURCE) $(GL11_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(GL11_BIN)

$(DC_RAYLIB_DIR)/libraylib.a: gldc-lib
	$(MAKE) -C $(RAYLIB_DC_SRC) clean
	$(MAKE) -C $(RAYLIB_DC_SRC) all PLATFORM=PLATFORM_DREAMCAST GRAPHICS=GRAPHICS_API_OPENGL_11 KOS_CFLAGS="$(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG)"
	mkdir -p $(DC_RAYLIB_DIR)
	cp -f $(RAYLIB_DC_SRC)/libraylib.a $(RAYLIB_DC_SRC)/raylib.h $(RAYLIB_DC_SRC)/raymath.h $(RAYLIB_DC_SRC)/rlgl.h $(DC_RAYLIB_DIR)/

$(SH4_RAYLIB_DIR)/libraylib.a: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(SH4_RAYLIB_DIR)
	cp -f $(DC_RAYLIB_DIR)/libraylib.a $(DC_RAYLIB_DIR)/raylib.h $(DC_RAYLIB_DIR)/raymath.h $(DC_RAYLIB_DIR)/rlgl.h $(SH4_RAYLIB_DIR)/


$(DC_FFTW_TARGET_DIR)/fftw_1997_%.o: $(FFTW_1997_DIR)/%.c
	mkdir -p $(DC_FFTW_TARGET_DIR)
	kos-cc -I$(FFTW_1997_DIR) $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(FFT_FLAGS) -std=gnu2x -c $< -o $@

cool-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(DC_TARGET_DIR) $(DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(DC_TARGET_DIR)/romdisk.img -d $(DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(DC_TARGET_DIR)/romdisk.img $(DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(DC_TARGET_DIR)/romdisk_tmp.o -c $(DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(DC_TARGET_DIR)/romdisk.o -r $(DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(DC_SOURCE) -o $(DC_TARGET_DIR)/dc.o
	kos-cc -o $(DC_TARGET) $(DC_TARGET_DIR)/dc.o $(DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,build/dc/cool_dc/cool_dc.elf,$(DC_LAUNCHER))
	rm -f $(DC_TARGET_DIR)/romdisk_tmp.c $(DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(DC_TARGET_DIR)/romdisk

sh4zam-butterfly: $(SH4_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(SH4_TARGET_DIR)
	$(KOS_GENROMFS) -f $(SH4_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(SH4_TARGET_DIR)/romdisk.img $(SH4_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(SH4_TARGET_DIR)/romdisk_tmp.o -c $(SH4_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(SH4_TARGET_DIR)/romdisk.o -r $(SH4_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -iquote sh4zam/include -I$(SH4_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(SH4_SOURCE) -o $(SH4_TARGET_DIR)/sh4.o
	kos-cc -iquote sh4zam/include -DPLATFORM_DREAMCAST $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) -std=gnu2x -c $(SH4ZAM_COMPLEX_SOURCE) -o $(SH4_TARGET_DIR)/shz_complex.o
	kos-cc -o $(SH4_TARGET) $(SH4_TARGET_DIR)/sh4.o $(SH4_TARGET_DIR)/shz_complex.o $(SH4_TARGET_DIR)/romdisk.o $(SH4_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,build/sh4/sh4zam_butterfly/sh4zam_butterfly.elf,$(SH4_LAUNCHER))
	rm -f $(SH4_TARGET_DIR)/romdisk_tmp.c $(SH4_TARGET_DIR)/romdisk_tmp.o

fftw-dc: $(DC_RAYLIB_DIR)/libraylib.a $(FFTW_1997_OBJS)
	mkdir -p $(BIN_DIR) $(DC_FFTW_TARGET_DIR)
	$(KOS_GENROMFS) -f $(DC_FFTW_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(DC_FFTW_TARGET_DIR)/romdisk.img $(DC_FFTW_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(DC_FFTW_TARGET_DIR)/romdisk_tmp.o -c $(DC_FFTW_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(DC_FFTW_TARGET_DIR)/romdisk.o -r $(DC_FFTW_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(FFTW_1997_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(FFTW_DC_SOURCE) -o $(DC_FFTW_TARGET_DIR)/fftw_dc.o
	kos-cc -o $(DC_FFTW_TARGET) $(DC_FFTW_TARGET_DIR)/fftw_dc.o $(FFTW_1997_OBJS) $(DC_FFTW_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(DC_FFTW_TARGET),$(DC_FFTW_LAUNCHER))
	rm -f $(DC_FFTW_TARGET_DIR)/romdisk_tmp.c $(DC_FFTW_TARGET_DIR)/romdisk_tmp.o

audio-only-wav-dc: gldc-lib
	$(MAKE) -C $(RAYLIB_DC_SRC) clean
	$(MAKE) -C $(RAYLIB_DC_SRC) all PLATFORM=PLATFORM_DREAMCAST GRAPHICS=GRAPHICS_API_OPENGL_11 KOS_CFLAGS="$(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG)"
	mkdir -p $(BIN_DIR) $(AUDIO_ONLY_WAV_TARGET_DIR)
	$(KOS_GENROMFS) -f $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk.img $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk_tmp.o -c $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk.o -r $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(RAYLIB_DC_SRC) -I$(SRC_DIR) -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) -std=gnu2x -c $(AUDIO_ONLY_WAV_SOURCE) -o $(AUDIO_ONLY_WAV_TARGET_DIR)/audio_only_wav_dc.o
	kos-cc -o $(AUDIO_ONLY_WAV_TARGET) $(AUDIO_ONLY_WAV_TARGET_DIR)/audio_only_wav_dc.o $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk.o $(RAYLIB_DC_SRC)/libraylib.a $(GLDC_LIB) -lkosutils -lm -lpthread
	$(call WRITE_BIN,$(AUDIO_ONLY_WAV_TARGET),$(AUDIO_ONLY_WAV_LAUNCHER))
	rm -f $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk_tmp.c $(AUDIO_ONLY_WAV_TARGET_DIR)/romdisk_tmp.o

audio-only-mp3-dc: gldc-lib
	$(MAKE) -C $(RAYLIB_DC_SRC) clean
	$(MAKE) -C $(RAYLIB_DC_SRC) all PLATFORM=PLATFORM_DREAMCAST GRAPHICS=GRAPHICS_API_OPENGL_11 KOS_CFLAGS="$(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG)"
	mkdir -p $(BIN_DIR) $(AUDIO_ONLY_MP3_TARGET_DIR)
	$(KOS_GENROMFS) -f $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk.img $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk_tmp.o -c $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk.o -r $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(RAYLIB_DC_SRC) -I$(SRC_DIR) -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) -std=gnu2x -c $(AUDIO_ONLY_MP3_SOURCE) -o $(AUDIO_ONLY_MP3_TARGET_DIR)/audio_only_mp3_dc.o
	kos-cc -o $(AUDIO_ONLY_MP3_TARGET) $(AUDIO_ONLY_MP3_TARGET_DIR)/audio_only_mp3_dc.o $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk.o $(RAYLIB_DC_SRC)/libraylib.a $(GLDC_LIB) -lkosutils -lm -lpthread
	$(call WRITE_BIN,$(AUDIO_ONLY_MP3_TARGET),$(AUDIO_ONLY_MP3_LAUNCHER))
	rm -f $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk_tmp.c $(AUDIO_ONLY_MP3_TARGET_DIR)/romdisk_tmp.o

picking-out-notes-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(PICKING_OUT_NOTES_DC_TARGET_DIR) $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_electronebulae_one_fourth_22050hz_pcm16_mono.wav $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk.img -d $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk.img $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk_tmp.o -c $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk.o -r $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -iquote sh4zam/include -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(PICKING_OUT_NOTES_DC_SOURCE) -o $(PICKING_OUT_NOTES_DC_TARGET_DIR)/picking_out_notes_dc.o
	kos-cc -iquote sh4zam/include -DPLATFORM_DREAMCAST $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) -std=gnu2x -c $(SH4ZAM_COMPLEX_SOURCE) -o $(PICKING_OUT_NOTES_DC_TARGET_DIR)/shz_complex.o
	kos-cc -o $(PICKING_OUT_NOTES_DC_TARGET) $(PICKING_OUT_NOTES_DC_TARGET_DIR)/picking_out_notes_dc.o $(PICKING_OUT_NOTES_DC_TARGET_DIR)/shz_complex.o $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lm -lpthread
	$(call WRITE_BIN,$(PICKING_OUT_NOTES_DC_TARGET),$(PICKING_OUT_NOTES_DC_LAUNCHER))
	rm -f $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk_tmp.c $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(PICKING_OUT_NOTES_DC_TARGET_DIR)/romdisk

waveform-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(WAVEFORM_DC_TARGET_DIR) $(WAVEFORM_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(WAVEFORM_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(WAVEFORM_DC_TARGET_DIR)/romdisk.img -d $(WAVEFORM_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(WAVEFORM_DC_TARGET_DIR)/romdisk.img $(WAVEFORM_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(WAVEFORM_DC_TARGET_DIR)/romdisk_tmp.o -c $(WAVEFORM_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(WAVEFORM_DC_TARGET_DIR)/romdisk.o -r $(WAVEFORM_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(WAVEFORM_DC_SOURCE) -o $(WAVEFORM_DC_TARGET_DIR)/waveform_dc.o
	kos-cc -o $(WAVEFORM_DC_TARGET) $(WAVEFORM_DC_TARGET_DIR)/waveform_dc.o $(WAVEFORM_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(WAVEFORM_DC_TARGET),$(WAVEFORM_DC_LAUNCHER))
	rm -f $(WAVEFORM_DC_TARGET_DIR)/romdisk_tmp.c $(WAVEFORM_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(WAVEFORM_DC_TARGET_DIR)/romdisk

sound-envelope-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(SOUND_ENVELOPE_DC_TARGET_DIR) $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk.img -d $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk.img $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk_tmp.o -c $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk.o -r $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(SOUND_ENVELOPE_DC_SOURCE) -o $(SOUND_ENVELOPE_DC_TARGET_DIR)/sound_envelope_dc.o
	kos-cc -o $(SOUND_ENVELOPE_DC_TARGET) $(SOUND_ENVELOPE_DC_TARGET_DIR)/sound_envelope_dc.o $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(SOUND_ENVELOPE_DC_TARGET),$(SOUND_ENVELOPE_DC_LAUNCHER))
	rm -f $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk_tmp.c $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(SOUND_ENVELOPE_DC_TARGET_DIR)/romdisk

sound-envelope-3d-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(SOUND_ENVELOPE_3D_DC_TARGET_DIR) $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/resources/hellion_one_fifth_22050hz_pcm16_mono.wav $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk.img -d $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk.img $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk_tmp.o -c $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk.o -r $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(SOUND_ENVELOPE_3D_DC_SOURCE) -o $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/sound_envelope_3d_dc.o
	kos-cc -o $(SOUND_ENVELOPE_3D_DC_TARGET) $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/sound_envelope_3d_dc.o $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(SOUND_ENVELOPE_3D_DC_TARGET),$(SOUND_ENVELOPE_3D_DC_LAUNCHER))
	rm -f $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk_tmp.c $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(SOUND_ENVELOPE_3D_DC_TARGET_DIR)/romdisk

sound-envelope-3d-audio-cadence-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR) $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/resources/dds_ffm_22050hz_pcm16_mono.wav $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk.img -d $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk.img $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk_tmp.o -c $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk.o -r $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_SOURCE) -o $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/sound_envelope_3d_audio_cadence_dc.o
	kos-cc -o $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET) $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/sound_envelope_3d_audio_cadence_dc.o $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET),$(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_LAUNCHER))
	rm -f $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk_tmp.c $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET_DIR)/romdisk

waveform-terrain-3d-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR) $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/romdisk/small_text_8_16px.fnt $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/romdisk/small_text_8_16px.png $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
	cp -f $(SRC_DIR)/romdisk/vga_rom_f16.fnt $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
	cp -f $(SRC_DIR)/romdisk/vga_rom_f16.png $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/resources/dds_ffm_22050hz_pcm16_mono.wav $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/resources/rama_22050hz_pcm16_mono.wav $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk.img -d $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk.img $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.o -c $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk.o -r $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(WAVEFORM_TERRAIN_3D_DC_SOURCE) -o $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/waveform_terrain_3d_dc.o
	kos-cc -o $(WAVEFORM_TERRAIN_3D_DC_TARGET) $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/waveform_terrain_3d_dc.o $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(WAVEFORM_TERRAIN_3D_DC_TARGET),$(WAVEFORM_TERRAIN_3D_DC_LAUNCHER))
	rm -f $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.c $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(WAVEFORM_TERRAIN_3D_DC_TARGET_DIR)/romdisk

fft-terrain-3d-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(FFT_TERRAIN_3D_DC_TARGET_DIR) $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk
	cp -f $(SRC_DIR)/resources/shadertoy_experiment_22050hz_pcm16_mono.wav $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/resources/dds_ffm_22050hz_pcm16_mono.wav $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/resources/rama_22050hz_pcm16_mono.wav $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/romdisk/small_text_8_16px.fnt $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
#	cp -f $(SRC_DIR)/romdisk/small_text_8_16px.png $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
	cp -f $(SRC_DIR)/romdisk/vga_rom_f16.fnt $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
	cp -f $(SRC_DIR)/romdisk/vga_rom_f16.png $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk/
	$(KOS_GENROMFS) -f $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk.img -d $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk.img $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.o -c $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk.o -r $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -iquote sh4zam/include -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) $(FFT_FLAGS) -std=gnu2x -c $(FFT_TERRAIN_3D_DC_SOURCE) -o $(FFT_TERRAIN_3D_DC_TARGET_DIR)/fft_terrain_3d_dc.o
	kos-cc -iquote sh4zam/include -DPLATFORM_DREAMCAST $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) -std=gnu2x -c $(SH4ZAM_COMPLEX_SOURCE) -o $(FFT_TERRAIN_3D_DC_TARGET_DIR)/shz_complex.o
	kos-cc -o $(FFT_TERRAIN_3D_DC_TARGET) $(FFT_TERRAIN_3D_DC_TARGET_DIR)/fft_terrain_3d_dc.o $(FFT_TERRAIN_3D_DC_TARGET_DIR)/shz_complex.o $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(FFT_TERRAIN_3D_DC_TARGET),$(FFT_TERRAIN_3D_DC_LAUNCHER))
	rm -f $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.c $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk_tmp.o
	rm -rf $(FFT_TERRAIN_3D_DC_TARGET_DIR)/romdisk

gldc-mesh-draw-mode-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(DC_MESH_DRAW_MODE_TARGET_DIR)
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(GLDC_INCLUDE_CFLAG) -std=gnu2x -c $(DC_MESH_DRAW_MODE_SOURCE) -o $(DC_MESH_DRAW_MODE_TARGET_DIR)/gldc_mesh_draw_mode_dc.o
	kos-cc -o $(DC_MESH_DRAW_MODE_TARGET) $(DC_MESH_DRAW_MODE_TARGET_DIR)/gldc_mesh_draw_mode_dc.o $(DC_RAYLIB_DIR)/libraylib.a $(GLDC_LIB) -lkosutils -lm -lpthread
	$(call WRITE_BIN,$(DC_MESH_DRAW_MODE_TARGET),$(DC_MESH_DRAW_MODE_LAUNCHER))

fmt:
	@F="$(FMT)"; (command -v "$$F" >/dev/null 2>&1 || [ -x "$$F" ]) || { echo "clang-format not found. Install it or set FMT=path/to/clang-format"; exit 1; }
	@if [ -f "$(FMT_STYLE)" ]; then $(FMT) -style=file:$(FMT_STYLE) -i $(FMT_SOURCES); else $(FMT) -i $(FMT_SOURCES); fi

clean-all:
	rm -rf $(BUILD_DIR)
	rm -f $(GL33_BIN) $(FFTW_BIN) $(GL11_BIN) $(DC_LAUNCHER) $(SH4_LAUNCHER) $(DC_FFTW_LAUNCHER) $(AUDIO_ONLY_WAV_LAUNCHER) $(AUDIO_ONLY_MP3_LAUNCHER) $(PICKING_OUT_NOTES_DC_LAUNCHER) $(WAVEFORM_DC_LAUNCHER) $(WAVEFORM_SANDBOX_DC_LAUNCHER) $(SOUND_ENVELOPE_DC_LAUNCHER) $(SOUND_ENVELOPE_3D_DC_LAUNCHER) $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_LAUNCHER) $(SOUND_ENVELOPE_GL33_LAUNCHER) $(WAVEFORM_TERRAIN_3D_DC_LAUNCHER) $(FFT_TERRAIN_3D_DC_LAUNCHER) $(DC_MESH_DRAW_MODE_LAUNCHER) $(DC_TARGET) $(SH4_TARGET) $(DC_FFTW_TARGET) $(AUDIO_ONLY_WAV_TARGET) $(AUDIO_ONLY_MP3_TARGET) $(PICKING_OUT_NOTES_DC_TARGET) $(WAVEFORM_DC_TARGET) $(WAVEFORM_SANDBOX_DC_TARGET) $(SOUND_ENVELOPE_DC_TARGET) $(SOUND_ENVELOPE_3D_DC_TARGET) $(SOUND_ENVELOPE_GL33_TARGET) $(SOUND_ENVELOPE_3D_AUDIO_CANDENCE_DC_TARGET) $(WAVEFORM_TERRAIN_3D_DC_TARGET) $(FFT_TERRAIN_3D_DC_TARGET) $(DC_MESH_DRAW_MODE_TARGET)
	rm -rf logs/*
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC) clean || true
	$(MAKE) -C $(RAYLIB_DC_SRC) clean || true
