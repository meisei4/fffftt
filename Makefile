.DEFAULT_GOAL := help

BUILD_DIR := build
DC_BUILD_DIR := $(BUILD_DIR)/dc
DESKTOP_BUILD_DIR := $(BUILD_DIR)/desktop
BIN_DIR := bin
DC_BIN_DIR := $(BIN_DIR)/dc
DESKTOP_BIN_DIR := $(BIN_DIR)/desktop
SRC_DIR := src
RES_DIR := $(SRC_DIR)/resources

SH4ZAM_DIR := sh4zam
GLDC_DIR := GLdc
RAYLIB_DC_SRC_DIR := raylib_dc/src
FFTW3_DIR := fftw3
RAYLIB_DESKTOP_SRC_DIR := raylib_desktop/src

GLDC_BUILD_DIR := $(DC_BUILD_DIR)/gldc
GLDC_LIB := $(GLDC_BUILD_DIR)/libGL.a
GLDC_TOOLCHAIN := $(KOS_BASE)/utils/cmake/kallistios.toolchain.cmake
CMAKE_GENERATOR ?= Unix Makefiles

FFTW3_SOLVTAB_C := $(DESKTOP_BUILD_DIR)/fftw3_empty_solvtab.c
FFTW3_SOLVTAB_OBJ := $(DESKTOP_BUILD_DIR)/fftw3_empty_solvtab.o
DESKTOP_GL_SHIM := $(DESKTOP_BUILD_DIR)/GL/gl.h

RAYLIB_DC_DIR := $(DC_BUILD_DIR)/raylib
RAYLIB_DESKTOP_DIR := $(DESKTOP_BUILD_DIR)/raylib

RAYLIB_DC_LIB := $(RAYLIB_DC_DIR)/libraylib.a
RAYLIB_DESKTOP_LIB := $(RAYLIB_DESKTOP_DIR)/libraylib.a

AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES ?=
DC_LD ?= kos-cc
DESKTOP_LD ?= cc
ifeq ($(OS),Windows_NT)
	HOST_OS := Windows
else
	HOST_OS := $(shell uname -s)
endif

FFT_CFLAGS := -fno-fast-math -ffp-contract=off
FFT_CFLAGS += -DFFT_ENABLE_PROFILE
DC_BASE_CFLAGS := $(filter-out -I$(KOS_PORTS)/include,$(KOS_CFLAGS)) -std=gnu2x
DC_KOS_CFLAGS := $(filter-out -I$(KOS_PORTS)/include,$(KOS_CFLAGS)) -I$(abspath $(GLDC_DIR))/include

DC_CPPFLAGS := -I$(RAYLIB_DC_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -I$(abspath $(GLDC_DIR))/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11
DESKTOP_CPPFLAGS := -I$(DESKTOP_BUILD_DIR) -I$(RAYLIB_DESKTOP_DIR) -I$(SRC_DIR) -I$(RAYLIB_DESKTOP_SRC_DIR)/external/glfw/include -I$(FFTW3_DIR)/api -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_11

# prevention of Windows only issue of MSYS2 test builds expanding the romdisk paths to have a drive prefix
export MSYS2_ARG_CONV_EXCL := $(if $(MSYS2_ARG_CONV_EXCL),$(MSYS2_ARG_CONV_EXCL);)-DAUDIO_ASSET_PATH_PREFIX=;-DRESOURCE_ASSET_PATH_PREFIX=
AUDIO_ASSET_PATH_PREFIX := /rd/
# toggle below comment on and off for release .cdi vs direct romdisk packing
# AUDIO_ASSET_PATH_PREFIX := /pc/
DC_CPPFLAGS += -DAUDIO_ASSET_PATH_PREFIX=\"$(AUDIO_ASSET_PATH_PREFIX)\"
DC_CPPFLAGS += $(if $(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),-DAUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=$(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),)
SH4ZAM_CPPFLAGS := -iquote $(SH4ZAM_DIR)/include

DC_LDFLAGS := $(KOS_LIB_PATHS) -L$(KOS_PORTS)/libwav/inst/lib
DC_LDLIBS := $(RAYLIB_DC_LIB) $(GLDC_LIB) -lkosutils -lm -lpthread
DC_WAV_LDLIBS := $(RAYLIB_DC_LIB) $(GLDC_LIB) -lkosutils -lwav -lm -lpthread

DESKTOP_CFLAGS := -std=gnu2x -Wall -Wextra
DESKTOP_LDLIBS := $(RAYLIB_DESKTOP_LIB) $(FFTW3_SOLVTAB_OBJ) $(DESKTOP_BUILD_DIR)/fftw3/libfftw3.a -lm

ifeq ($(HOST_OS),Darwin)
	DESKTOP_LDLIBS += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
else ifneq ($(HOST_OS),Windows)
	DESKTOP_LDLIBS += -ldl -lpthread -lGL -lX11
endif

RES_EXPERIMENT_WAV := $(RES_DIR)/experiment_22k_mono_adpcm.wav
RES_ELECTRONEBULAE_WAV := $(RES_DIR)/electronebl_22k_mono_adpcm.wav
RES_8BIT_WAV := $(RES_DIR)/8bit_22k_mono_adpcm.wav
RES_GEOMETRIC_PERSON_WAV := $(RES_DIR)/geometric_person_22k_mono_adpcm.wav
RES_TROPICAL_WAV := $(RES_DIR)/tropical_22k_mono_adpcm.wav
RES_XTRACK_WAV := $(RES_DIR)/xtrack_22k_mono_adpcm.wav
RES_FONT_ASSETS := $(RES_DIR)/vga.fnt $(RES_DIR)/vga.png

SH4ZAM_BUTTERFLY_SRC := $(SRC_DIR)/sh4zam_butterfly.c
PICKING_OUT_NOTES_SRC := $(SRC_DIR)/picking_out_notes.c
WAVEFORM_SRC := $(SRC_DIR)/waveform.c
SOUND_ENVELOPE_2D_ISO_SRC := $(SRC_DIR)/sound_envelope_2d_iso.c
SOUND_ENVELOPE_3D_SRC := $(SRC_DIR)/sound_envelope_3d.c
WAVEFORM_TERRAIN_3D_SRC := $(SRC_DIR)/waveform_terrain_3d.c
FFT_TERRAIN_3D_SRC := $(SRC_DIR)/fft_terrain_3d.c
FFT_BANDS_TERRAIN_3D_SRC := $(SRC_DIR)/fft_bands_terrain_3d.c

SH4ZAM_COMPLEX_SRC := $(SH4ZAM_DIR)/source/shz_complex.c
SH4ZAM_COMPLEX_OBJ := $(DC_BUILD_DIR)/sh4zam/shz_complex.o

SH4ZAM_BUTTERFLY_DC_ELF := $(DC_BUILD_DIR)/sh4zam-butterfly/sh4zam-butterfly.elf
PICKING_OUT_NOTES_DC_ELF := $(DC_BUILD_DIR)/picking-out-notes/picking-out-notes.elf
WAVEFORM_DC_ELF := $(DC_BUILD_DIR)/waveform/waveform.elf
SOUND_ENVELOPE_2D_ISO_DC_ELF := $(DC_BUILD_DIR)/sound-envelope-2d-iso/sound-envelope-2d-iso.elf
SOUND_ENVELOPE_3D_DC_ELF := $(DC_BUILD_DIR)/sound-envelope-3d/sound-envelope-3d.elf
WAVEFORM_TERRAIN_3D_DC_ELF := $(DC_BUILD_DIR)/waveform-terrain-3d/waveform-terrain-3d.elf
FFT_TERRAIN_3D_DC_ELF := $(DC_BUILD_DIR)/fft-terrain-3d/fft-terrain-3d.elf
FFT_BANDS_TERRAIN_3D_DC_ELF := $(DC_BUILD_DIR)/fft-bands-terrain-3d/fft-bands-terrain-3d.elf

SH4ZAM_BUTTERFLY_DC_RUN := $(DC_BIN_DIR)/sh4zam-butterfly
PICKING_OUT_NOTES_DC_RUN := $(DC_BIN_DIR)/picking-out-notes
WAVEFORM_DC_RUN := $(DC_BIN_DIR)/waveform
SOUND_ENVELOPE_2D_ISO_DC_RUN := $(DC_BIN_DIR)/sound-envelope-2d-iso
SOUND_ENVELOPE_3D_DC_RUN := $(DC_BIN_DIR)/sound-envelope-3d
WAVEFORM_TERRAIN_3D_DC_RUN := $(DC_BIN_DIR)/waveform-terrain-3d
FFT_TERRAIN_3D_DC_RUN := $(DC_BIN_DIR)/fft-terrain-3d
FFT_BANDS_TERRAIN_3D_DC_RUN := $(DC_BIN_DIR)/fft-bands-terrain-3d

SH4ZAM_BUTTERFLY_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/sh4zam-butterfly
PICKING_OUT_NOTES_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/picking-out-notes
WAVEFORM_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/waveform
SOUND_ENVELOPE_2D_ISO_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/sound-envelope-2d-iso
SOUND_ENVELOPE_3D_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/sound-envelope-3d
WAVEFORM_TERRAIN_3D_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/waveform-terrain-3d
FFT_TERRAIN_3D_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/fft-terrain-3d
FFT_BANDS_TERRAIN_3D_DESKTOP_RUN := $(DESKTOP_BIN_DIR)/fft-bands-terrain-3d

DC_ELFS := $(SH4ZAM_BUTTERFLY_DC_ELF) $(PICKING_OUT_NOTES_DC_ELF) $(WAVEFORM_DC_ELF) $(SOUND_ENVELOPE_2D_ISO_DC_ELF) $(SOUND_ENVELOPE_3D_DC_ELF) $(WAVEFORM_TERRAIN_3D_DC_ELF) $(FFT_TERRAIN_3D_DC_ELF) $(FFT_BANDS_TERRAIN_3D_DC_ELF)
DC_RUNS := $(SH4ZAM_BUTTERFLY_DC_RUN) $(PICKING_OUT_NOTES_DC_RUN) $(WAVEFORM_DC_RUN) $(SOUND_ENVELOPE_2D_ISO_DC_RUN) $(SOUND_ENVELOPE_3D_DC_RUN) $(WAVEFORM_TERRAIN_3D_DC_RUN) $(FFT_TERRAIN_3D_DC_RUN) $(FFT_BANDS_TERRAIN_3D_DC_RUN)
PHONY_DC_TARGETS := dc-sh4zam-butterfly dc-picking-out-notes dc-waveform dc-sound-envelope-2d-iso dc-sound-envelope-3d dc-waveform-terrain-3d dc-fft-terrain-3d dc-fft-bands-terrain-3d

DESKTOP_RUNS := $(SH4ZAM_BUTTERFLY_DESKTOP_RUN) $(PICKING_OUT_NOTES_DESKTOP_RUN) $(WAVEFORM_DESKTOP_RUN) $(SOUND_ENVELOPE_2D_ISO_DESKTOP_RUN) $(SOUND_ENVELOPE_3D_DESKTOP_RUN) $(WAVEFORM_TERRAIN_3D_DESKTOP_RUN) $(FFT_TERRAIN_3D_DESKTOP_RUN) $(FFT_BANDS_TERRAIN_3D_DESKTOP_RUN)
PHONY_DESKTOP_TARGETS := desktop-sh4zam-butterfly desktop-picking-out-notes desktop-waveform desktop-sound-envelope-2d-iso desktop-sound-envelope-3d desktop-waveform-terrain-3d desktop-fft-terrain-3d desktop-fft-bands-terrain-3d

FMT ?= $(shell command -v clang-format 2>/dev/null || xcrun --find clang-format 2>/dev/null || echo clang-format)
FMT_STYLE ?= ./.clang-format
FMT_SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*.h) $(wildcard $(RES_DIR)/*.glsl)

.PHONY: help all fmt clean-all gldc-lib $(PHONY_DC_TARGETS) $(PHONY_DESKTOP_TARGETS)

help:
ifeq ($(HOST_OS),Windows)
	$(error PLEASE DEFINE A TARGET: make $(PHONY_DC_TARGETS) fmt clean-all, for Windows DESKTOP target use build.bat. Makefile is only for DREAMCAST target on Windows)
else
	$(error PLEASE DEFINE A TARGET: make $(PHONY_DC_TARGETS) $(PHONY_DESKTOP_TARGETS) fmt clean-all)
endif

all: help

dc-sh4zam-butterfly: $(SH4ZAM_BUTTERFLY_DC_RUN)
dc-picking-out-notes: $(PICKING_OUT_NOTES_DC_RUN)
dc-waveform: $(WAVEFORM_DC_RUN)
dc-sound-envelope-2d-iso: $(SOUND_ENVELOPE_2D_ISO_DC_RUN)
dc-sound-envelope-3d: $(SOUND_ENVELOPE_3D_DC_RUN)
dc-waveform-terrain-3d: $(WAVEFORM_TERRAIN_3D_DC_RUN)
dc-fft-terrain-3d: $(FFT_TERRAIN_3D_DC_RUN)
dc-fft-bands-terrain-3d: $(FFT_BANDS_TERRAIN_3D_DC_RUN)

ifeq ($(HOST_OS),Windows)
$(PHONY_DESKTOP_TARGETS):
	$(error for Windows DESKTOP target use build.bat. Makefile is only for DREAMCAST target on Windows)
else
desktop-sh4zam-butterfly: $(SH4ZAM_BUTTERFLY_DESKTOP_RUN)
desktop-picking-out-notes: $(PICKING_OUT_NOTES_DESKTOP_RUN)
desktop-waveform: $(WAVEFORM_DESKTOP_RUN)
desktop-sound-envelope-2d-iso: $(SOUND_ENVELOPE_2D_ISO_DESKTOP_RUN)
desktop-sound-envelope-3d: $(SOUND_ENVELOPE_3D_DESKTOP_RUN)
desktop-waveform-terrain-3d: $(WAVEFORM_TERRAIN_3D_DESKTOP_RUN)
desktop-fft-terrain-3d: $(FFT_TERRAIN_3D_DESKTOP_RUN)
desktop-fft-bands-terrain-3d: $(FFT_BANDS_TERRAIN_3D_DESKTOP_RUN)
endif

define BUILD_ROMDISK
	$(KOS_GENROMFS) -f $(1)/romdisk.img -d $(2) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(1)/romdisk.img $(1)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(1)/romdisk_tmp.o -c $(1)/romdisk_tmp.c
	$(KOS_CC) -o $(1)/romdisk.o -r $(1)/romdisk_tmp.o $(DC_LDFLAGS) -Wl,--whole-archive -lromdiskbase
	rm -f $(1)/romdisk_tmp.c $(1)/romdisk_tmp.o
endef

define WRITE_RUN
	echo '#!/usr/bin/env bash' > $(2)
	echo 'set -euo pipefail' >> $(2)
	echo 'd="$$(cd "$$(dirname "$$0")/../.." && pwd)"' >> $(2)
	echo 'exec "$$d/flycast_run.sh" "$$d/$(1)" "$$@"' >> $(2)
	chmod +x $(2)
endef

define RUN_RULE
$(1): $(2)
	$$(call WRITE_RUN,$$<,$$@)
endef

$(eval $(call RUN_RULE,$(SH4ZAM_BUTTERFLY_DC_RUN),$(SH4ZAM_BUTTERFLY_DC_ELF)))
$(eval $(call RUN_RULE,$(PICKING_OUT_NOTES_DC_RUN),$(PICKING_OUT_NOTES_DC_ELF)))
$(eval $(call RUN_RULE,$(WAVEFORM_DC_RUN),$(WAVEFORM_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_2D_ISO_DC_RUN),$(SOUND_ENVELOPE_2D_ISO_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_3D_DC_RUN),$(SOUND_ENVELOPE_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(WAVEFORM_TERRAIN_3D_DC_RUN),$(WAVEFORM_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(FFT_TERRAIN_3D_DC_RUN),$(FFT_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(FFT_BANDS_TERRAIN_3D_DC_RUN),$(FFT_BANDS_TERRAIN_3D_DC_ELF)))

$(DC_ELFS): TARGET_CPPFLAGS := $(DC_CPPFLAGS)
$(DC_ELFS): TARGET_CFLAGS := $(DC_BASE_CFLAGS) $(FFT_CFLAGS)
$(DC_ELFS): TARGET_LDLIBS := $(DC_WAV_LDLIBS)
$(DC_ELFS): TARGET_LDFLAGS := $(DC_LDFLAGS)
$(DC_ELFS): EXTRA_OBJS :=
$(DC_ELFS): ASSETS := $(RES_FONT_ASSETS)
$(DC_ELFS): ASSETS += $(RES_EXPERIMENT_WAV) $(RES_ELECTRONEBULAE_WAV) $(RES_8BIT_WAV) $(RES_GEOMETRIC_PERSON_WAV) $(RES_TROPICAL_WAV) $(RES_XTRACK_WAV)

$(SH4ZAM_BUTTERFLY_DC_ELF): SRC := $(SH4ZAM_BUTTERFLY_SRC)
$(SH4ZAM_BUTTERFLY_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(SH4ZAM_BUTTERFLY_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(SH4ZAM_BUTTERFLY_DC_ELF): $(SH4ZAM_BUTTERFLY_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(PICKING_OUT_NOTES_DC_ELF): SRC := $(PICKING_OUT_NOTES_SRC)
$(PICKING_OUT_NOTES_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(PICKING_OUT_NOTES_DC_ELF): TARGET_LDLIBS := $(DC_LDLIBS)
$(PICKING_OUT_NOTES_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(PICKING_OUT_NOTES_DC_ELF): $(PICKING_OUT_NOTES_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(WAVEFORM_DC_ELF): SRC := $(WAVEFORM_SRC)
$(WAVEFORM_DC_ELF): $(WAVEFORM_SRC)

$(SOUND_ENVELOPE_2D_ISO_DC_ELF): SRC := $(SOUND_ENVELOPE_2D_ISO_SRC)
$(SOUND_ENVELOPE_2D_ISO_DC_ELF): $(SOUND_ENVELOPE_2D_ISO_SRC)

$(SOUND_ENVELOPE_3D_DC_ELF): SRC := $(SOUND_ENVELOPE_3D_SRC)
$(SOUND_ENVELOPE_3D_DC_ELF): $(SOUND_ENVELOPE_3D_SRC)

$(WAVEFORM_TERRAIN_3D_DC_ELF): SRC := $(WAVEFORM_TERRAIN_3D_SRC)
$(WAVEFORM_TERRAIN_3D_DC_ELF): $(WAVEFORM_TERRAIN_3D_SRC)

$(FFT_TERRAIN_3D_DC_ELF): SRC := $(FFT_TERRAIN_3D_SRC)
$(FFT_TERRAIN_3D_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(FFT_TERRAIN_3D_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(FFT_TERRAIN_3D_DC_ELF): $(FFT_TERRAIN_3D_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(FFT_BANDS_TERRAIN_3D_DC_ELF): SRC := $(FFT_BANDS_TERRAIN_3D_SRC)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): $(FFT_BANDS_TERRAIN_3D_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(DC_RUNS): | $(DC_BIN_DIR)

$(DC_ELFS): $(RAYLIB_DC_LIB)
	rm -rf $(@D)/pc && mkdir -p $(@D)/pc
	for asset in $(ASSETS); do cp -f $$asset $(@D)/pc/; done
	rm -rf $(@D)/romdisk && mkdir -p $(@D)/romdisk
	for asset in $(ASSETS); do cp -f $$asset $(@D)/romdisk/; done
	$(call BUILD_ROMDISK,$(@D),$(@D)/romdisk)
	$(KOS_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -c $(SRC) -o $(basename $@).o
	$(DC_LD) $(TARGET_LDFLAGS) -o $@ $(basename $@).o $(EXTRA_OBJS) $(@D)/romdisk.o $(TARGET_LDLIBS)
	rm -rf $(@D)/romdisk

$(SH4ZAM_COMPLEX_OBJ): $(SH4ZAM_COMPLEX_SRC)
	mkdir -p $(@D)
	$(KOS_CC) $(SH4ZAM_CPPFLAGS) -DPLATFORM_DREAMCAST $(DC_BASE_CFLAGS) -I$(abspath $(GLDC_DIR))/include -c $< -o $@

gldc-lib:
	@test -f $(GLDC_DIR)/CMakeLists.txt || { echo "GLdc checkout missing at $(GLDC_DIR). Initialize the submodule first."; exit 1; }
	cd $(GLDC_DIR) && cmake --fresh -G "$(CMAKE_GENERATOR)" -S . -B $(abspath $(GLDC_BUILD_DIR)) -DCMAKE_TOOLCHAIN_FILE=$(GLDC_TOOLCHAIN) -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
	cmake --build $(GLDC_BUILD_DIR) --target GL --parallel
	@test -f $(GLDC_LIB)

$(RAYLIB_DC_LIB): gldc-lib
	@set -e; \
	if [ -n "$(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES)" ]; then \
		trap 'rm -f $(SRC_DIR)/config_redef.h' EXIT; \
		echo '#undef AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES' > $(SRC_DIR)/config_redef.h; \
		echo '#define AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES $(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES)' >> $(SRC_DIR)/config_redef.h; \
	fi; \
	$(MAKE) -C $(RAYLIB_DC_SRC_DIR) clean; \
	$(MAKE) -C $(RAYLIB_DC_SRC_DIR) all PLATFORM=PLATFORM_DREAMCAST GRAPHICS=GRAPHICS_API_OPENGL_11 KOS_CFLAGS="$(DC_KOS_CFLAGS) $(if $(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),-include $(abspath raylib_dc/src/config.h) -include $(abspath $(SRC_DIR))/config_redef.h,)"
	mkdir -p $(RAYLIB_DC_DIR)
	cp -f $(RAYLIB_DC_SRC_DIR)/libraylib.a $(RAYLIB_DC_SRC_DIR)/raylib.h $(RAYLIB_DC_SRC_DIR)/raymath.h $(RAYLIB_DC_SRC_DIR)/rlgl.h $(RAYLIB_DC_DIR)/

ifneq ($(HOST_OS),Windows)
$(DESKTOP_RUNS): TARGET_CPPFLAGS := $(DESKTOP_CPPFLAGS)
$(DESKTOP_RUNS): TARGET_CFLAGS := $(DESKTOP_CFLAGS) $(FFT_CFLAGS)
$(DESKTOP_RUNS): TARGET_LDLIBS := $(DESKTOP_LDLIBS)

$(SH4ZAM_BUTTERFLY_DESKTOP_RUN): SRC := $(SH4ZAM_BUTTERFLY_SRC)
$(SH4ZAM_BUTTERFLY_DESKTOP_RUN): $(SH4ZAM_BUTTERFLY_SRC)

$(PICKING_OUT_NOTES_DESKTOP_RUN): SRC := $(PICKING_OUT_NOTES_SRC)
$(PICKING_OUT_NOTES_DESKTOP_RUN): $(PICKING_OUT_NOTES_SRC)

$(WAVEFORM_DESKTOP_RUN): SRC := $(WAVEFORM_SRC)
$(WAVEFORM_DESKTOP_RUN): $(WAVEFORM_SRC)

$(SOUND_ENVELOPE_2D_ISO_DESKTOP_RUN): SRC := $(SOUND_ENVELOPE_2D_ISO_SRC)
$(SOUND_ENVELOPE_2D_ISO_DESKTOP_RUN): $(SOUND_ENVELOPE_2D_ISO_SRC)

$(SOUND_ENVELOPE_3D_DESKTOP_RUN): SRC := $(SOUND_ENVELOPE_3D_SRC)
$(SOUND_ENVELOPE_3D_DESKTOP_RUN): $(SOUND_ENVELOPE_3D_SRC)

$(WAVEFORM_TERRAIN_3D_DESKTOP_RUN): SRC := $(WAVEFORM_TERRAIN_3D_SRC)
$(WAVEFORM_TERRAIN_3D_DESKTOP_RUN): $(WAVEFORM_TERRAIN_3D_SRC)

$(FFT_TERRAIN_3D_DESKTOP_RUN): SRC := $(FFT_TERRAIN_3D_SRC)
$(FFT_TERRAIN_3D_DESKTOP_RUN): $(FFT_TERRAIN_3D_SRC)

$(FFT_BANDS_TERRAIN_3D_DESKTOP_RUN): SRC := $(FFT_BANDS_TERRAIN_3D_SRC)
$(FFT_BANDS_TERRAIN_3D_DESKTOP_RUN): $(FFT_BANDS_TERRAIN_3D_SRC)

$(DESKTOP_RUNS): $(RAYLIB_DESKTOP_LIB) $(FFTW3_SOLVTAB_OBJ) $(DESKTOP_GL_SHIM) | $(DESKTOP_BIN_DIR)
	$(DESKTOP_LD) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) $(SRC) -o $@ $(TARGET_LDLIBS)

$(RAYLIB_DESKTOP_LIB): | $(RAYLIB_DESKTOP_DIR)
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) PLATFORM=PLATFORM_DESKTOP GRAPHICS=GRAPHICS_API_OPENGL_11 RAYLIB_RELEASE_PATH=$(abspath $(RAYLIB_DESKTOP_DIR)) RAYLIB_LIBTYPE=STATIC
	cp -f $(RAYLIB_DESKTOP_SRC_DIR)/raylib.h $(RAYLIB_DESKTOP_SRC_DIR)/raymath.h $(RAYLIB_DESKTOP_SRC_DIR)/rlgl.h $(RAYLIB_DESKTOP_DIR)/

$(DESKTOP_BUILD_DIR)/fftw3/libfftw3.a:
	cmake -S $(FFTW3_DIR) -B $(DESKTOP_BUILD_DIR)/fftw3 -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -DDISABLE_FORTRAN=ON -DENABLE_FLOAT=OFF
	cmake --build $(DESKTOP_BUILD_DIR)/fftw3 --target fftw3 --parallel

$(FFTW3_SOLVTAB_C): | $(DESKTOP_BUILD_DIR)
	printf '%s\n' '#include "config.h"' '#include "kernel/ifftw.h"' 'const solvtab X(solvtab_dft_standard) = { SOLVTAB_END };' 'const solvtab X(solvtab_rdft_r2cf) = { SOLVTAB_END };' 'const solvtab X(solvtab_rdft_r2cb) = { SOLVTAB_END };' 'const solvtab X(solvtab_rdft_r2r) = { SOLVTAB_END };' > $@

$(FFTW3_SOLVTAB_OBJ): $(FFTW3_SOLVTAB_C) $(DESKTOP_BUILD_DIR)/fftw3/libfftw3.a
	$(DESKTOP_LD) -I$(DESKTOP_BUILD_DIR)/fftw3 -I$(FFTW3_DIR) -I$(FFTW3_DIR)/kernel $(DESKTOP_CFLAGS) -c $< -o $@

$(DESKTOP_GL_SHIM): | $(DESKTOP_BUILD_DIR)
	mkdir -p $(@D)
ifeq ($(HOST_OS),Darwin)
	printf '%s\n' '#include <OpenGL/gl.h>' > $@
else
	printf '%s\n' '#include_next <GL/gl.h>' > $@
endif
endif

fmt:
	$(FMT) -style=file:$(FMT_STYLE) -i $(FMT_SRCS)

$(DC_BIN_DIR) $(DESKTOP_BIN_DIR) $(RAYLIB_DESKTOP_DIR) $(DESKTOP_BUILD_DIR):
	mkdir -p $@

clean-all:
	rm -rf $(BUILD_DIR) $(DC_BIN_DIR) $(DESKTOP_BIN_DIR) logs/*
	$(MAKE) -C $(RAYLIB_DC_SRC_DIR) clean || true
ifneq ($(HOST_OS),Windows)
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) clean || true
endif
