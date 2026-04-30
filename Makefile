.DEFAULT_GOAL := help

BUILD_DIR := build
BIN_DIR := bin
SRC_DIR := src
RES_DIR := $(SRC_DIR)/resources

FFTW_1997_DIR := fftw_1997
SH4ZAM_DIR := sh4zam
GLDC_DIR := GLdc
RAYLIB_DESKTOP_SRC_DIR := raylib_desktop/src
RAYLIB_DC_SRC_DIR := raylib_dc/src

GLDC_BUILD_DIR := $(BUILD_DIR)/dc/gldc
GLDC_LIB := $(GLDC_BUILD_DIR)/libGL.a
GLDC_TOOLCHAIN := $(KOS_BASE)/utils/cmake/kallistios.toolchain.cmake

RAYLIB_GL33_DIR := $(BUILD_DIR)/gl33/raylib
RAYLIB_GL11_DIR := $(BUILD_DIR)/gl11/raylib
RAYLIB_DC_DIR := $(BUILD_DIR)/dc/raylib

RAYLIB_GL33_LIB := $(RAYLIB_GL33_DIR)/libraylib.a
RAYLIB_GL11_LIB := $(RAYLIB_GL11_DIR)/libraylib.a
RAYLIB_DC_LIB := $(RAYLIB_DC_DIR)/libraylib.a

AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES ?=
DC_LD ?= kos-cc

BASE_CFLAGS := -std=c99 -O2 -Wall -Wextra
FFT_CFLAGS := -fno-fast-math -ffp-contract=off
FFT_CFLAGS += -DFFT_ENABLE_PROFILE
# FFT_CFLAGS += -DFFTW_ENABLE_FLOAT
DC_BASE_CFLAGS := $(filter-out -I$(KOS_PORTS)/include,$(KOS_CFLAGS)) -std=gnu2x
DC_KOS_CFLAGS := $(filter-out -I$(KOS_PORTS)/include,$(KOS_CFLAGS)) -I$(abspath $(GLDC_DIR))/include

GL33_CPPFLAGS := -DPLATFORM_DESKTOP -I$(RAYLIB_GL33_DIR)
GL11_CPPFLAGS := -DPLATFORM_DESKTOP -I$(RAYLIB_GL11_DIR)

DC_CPPFLAGS := -I$(RAYLIB_DC_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -I$(abspath $(GLDC_DIR))/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11
DC_CPPFLAGS += $(if $(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),-DAUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=$(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),)
SH4ZAM_CPPFLAGS := -iquote $(SH4ZAM_DIR)/include

ifeq ($(shell uname -s 2>/dev/null || echo Unknown),Darwin)
DESKTOP_LDLIBS := -lm -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo
else
DESKTOP_LDLIBS := -lGL -lX11 -lpthread -ldl -lrt -lm -lc
endif

DC_LDFLAGS := $(KOS_LIB_PATHS) -L$(KOS_PORTS)/libwav/inst/lib
DC_LDLIBS := $(RAYLIB_DC_LIB) $(GLDC_LIB) -lkosutils -lm -lpthread
DC_WAV_LDLIBS := $(RAYLIB_DC_LIB) $(GLDC_LIB) -lkosutils -lwav -lm -lpthread

RES_EXPERIMENT_WAV := $(RES_DIR)/shadertoy_experiment_22050hz_pcm16_mono.wav
RES_ELECTRONEBULAE_WAV := $(RES_DIR)/shadertoy_electronebulae_one_fourth_22050hz_pcm16_mono.wav
RES_COUNTRY_WAV := $(RES_DIR)/country_22050hz_pcm16_mono.wav
RES_COUNTRY_MP3 := $(RES_DIR)/country_44100hz_128kbps_stereo.mp3
RES_FONT_ASSETS := $(RES_DIR)/vga_rom_f16_0px_TIGHT.fnt $(RES_DIR)/vga_rom_f16_0px_TIGHT.png

GL33_SRC := $(SRC_DIR)/cool_gl33.c
GL11_SRC := $(SRC_DIR)/cool_gl11.c
FFTW_GL11_SRC := $(SRC_DIR)/fftw_gl11.c
SOUND_ENVELOPE_GL33_SRC := $(SRC_DIR)/sound_envelope_gl33.c

COOL_DC_SRC := $(SRC_DIR)/cool_dc.c
SH4ZAM_BUTTERFLY_SRC := $(SRC_DIR)/sh4zam_butterfly.c
FFTW_DC_SRC := $(SRC_DIR)/fftw_dc.c
AUDIO_ONLY_WAV_SRC := $(SRC_DIR)/audio_only_wav_dc.c
AUDIO_ONLY_MP3_SRC := $(SRC_DIR)/audio_only_mp3_dc.c
PICKING_OUT_NOTES_DC_SRC := $(SRC_DIR)/picking_out_notes_dc.c
WAVEFORM_DC_SRC := $(SRC_DIR)/waveform_dc.c
SOUND_ENVELOPE_DC_SRC := $(SRC_DIR)/sound_envelope_dc.c
SOUND_ENVELOPE_3D_DC_SRC := $(SRC_DIR)/sound_envelope_3d_dc.c
SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_SRC := $(SRC_DIR)/sound_envelope_3d_audio_cadence_dc.c
WAVEFORM_TERRAIN_3D_DC_SRC := $(SRC_DIR)/waveform_terrain_3d_dc.c
FFT_TERRAIN_3D_DC_SRC := $(SRC_DIR)/fft_terrain_3d_dc.c
FFT_BANDS_TERRAIN_3D_DC_SRC := $(SRC_DIR)/fft_bands_terrain_3d_dc.c
GLDC_MESH_DRAW_MODE_DC_SRC := $(SRC_DIR)/gldc_mesh_draw_mode.c

SH4ZAM_COMPLEX_SRC := $(SH4ZAM_DIR)/source/shz_complex.c
SH4ZAM_COMPLEX_OBJ := $(BUILD_DIR)/dc/sh4zam/shz_complex.o
FFTW_1997_SRCS := $(wildcard $(FFTW_1997_DIR)/*.c)
FFTW_1997_OBJS := $(patsubst $(FFTW_1997_DIR)/%.c,$(BUILD_DIR)/dc/fftw_dc/fftw_1997_%.o,$(FFTW_1997_SRCS))

COOL_GL33_PROG := $(BIN_DIR)/cool_gl33
COOL_GL11_PROG := $(BIN_DIR)/cool_gl11
FFTW_GL11_PROG := $(BIN_DIR)/fftw_gl11
SOUND_ENVELOPE_GL33_PROG := $(BIN_DIR)/sound_envelope_gl33

COOL_DC_ELF := $(BUILD_DIR)/dc/cool_dc/cool_dc.elf
SH4ZAM_BUTTERFLY_ELF := $(BUILD_DIR)/dc/sh4zam_butterfly/sh4zam_butterfly.elf
FFTW_DC_ELF := $(BUILD_DIR)/dc/fftw_dc/fftw_dc.elf
AUDIO_ONLY_WAV_DC_ELF := $(BUILD_DIR)/dc/audio_only_wav_dc/audio_only_wav_dc.elf
AUDIO_ONLY_MP3_DC_ELF := $(BUILD_DIR)/dc/audio_only_mp3_dc/audio_only_mp3_dc.elf
PICKING_OUT_NOTES_DC_ELF := $(BUILD_DIR)/dc/picking_out_notes_dc/picking_out_notes_dc.elf
WAVEFORM_DC_ELF := $(BUILD_DIR)/dc/waveform_dc/waveform_dc.elf
SOUND_ENVELOPE_DC_ELF := $(BUILD_DIR)/dc/sound_envelope_dc/sound_envelope_dc.elf
SOUND_ENVELOPE_3D_DC_ELF := $(BUILD_DIR)/dc/sound_envelope_3d_dc/sound_envelope_3d_dc.elf
SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_ELF := $(BUILD_DIR)/dc/sound_envelope_3d_audio_cadence_dc/sound_envelope_3d_audio_cadence_dc.elf
WAVEFORM_TERRAIN_3D_DC_ELF := $(BUILD_DIR)/dc/waveform_terrain_3d_dc/waveform_terrain_3d_dc.elf
FFT_TERRAIN_3D_DC_ELF := $(BUILD_DIR)/dc/fft_terrain_3d_dc/fft_terrain_3d_dc.elf
FFT_BANDS_TERRAIN_3D_DC_ELF := $(BUILD_DIR)/dc/fft_bands_terrain_3d_dc/fft_bands_terrain_3d_dc.elf
GLDC_MESH_DRAW_MODE_DC_ELF := $(BUILD_DIR)/dc/gldc_mesh_draw_mode_dc/gldc_mesh_draw_mode_dc.elf

COOL_DC_RUN := $(BIN_DIR)/cool_dc
SH4ZAM_BUTTERFLY_RUN := $(BIN_DIR)/sh4zam_butterfly
FFTW_DC_RUN := $(BIN_DIR)/fftw_dc
AUDIO_ONLY_WAV_DC_RUN := $(BIN_DIR)/audio_only_wav_dc
AUDIO_ONLY_MP3_DC_RUN := $(BIN_DIR)/audio_only_mp3_dc
PICKING_OUT_NOTES_DC_RUN := $(BIN_DIR)/picking_out_notes_dc
WAVEFORM_DC_RUN := $(BIN_DIR)/waveform_dc
SOUND_ENVELOPE_DC_RUN := $(BIN_DIR)/sound_envelope_dc
SOUND_ENVELOPE_3D_DC_RUN := $(BIN_DIR)/sound_envelope_3d_dc
SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_RUN := $(BIN_DIR)/sound_envelope_3d_audio_cadence_dc
WAVEFORM_TERRAIN_3D_DC_RUN := $(BIN_DIR)/waveform_terrain_3d_dc
FFT_TERRAIN_3D_DC_RUN := $(BIN_DIR)/fft_terrain_3d_dc
FFT_BANDS_TERRAIN_3D_DC_RUN := $(BIN_DIR)/fft_bands_terrain_3d_dc
GLDC_MESH_DRAW_MODE_DC_RUN := $(BIN_DIR)/gldc_mesh_draw_mode_dc

DESKTOP_PROGS := $(COOL_GL33_PROG) $(COOL_GL11_PROG) $(FFTW_GL11_PROG) $(SOUND_ENVELOPE_GL33_PROG)
DC_ELFS := $(COOL_DC_ELF) $(SH4ZAM_BUTTERFLY_ELF) $(FFTW_DC_ELF) $(AUDIO_ONLY_WAV_DC_ELF) $(AUDIO_ONLY_MP3_DC_ELF) $(PICKING_OUT_NOTES_DC_ELF) $(WAVEFORM_DC_ELF) $(SOUND_ENVELOPE_DC_ELF) $(SOUND_ENVELOPE_3D_DC_ELF) $(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_ELF) $(WAVEFORM_TERRAIN_3D_DC_ELF) $(FFT_TERRAIN_3D_DC_ELF) $(FFT_BANDS_TERRAIN_3D_DC_ELF) $(GLDC_MESH_DRAW_MODE_DC_ELF)
DC_RUNS := $(COOL_DC_RUN) $(SH4ZAM_BUTTERFLY_RUN) $(FFTW_DC_RUN) $(AUDIO_ONLY_WAV_DC_RUN) $(AUDIO_ONLY_WAV_DC_RUN)_1024 $(AUDIO_ONLY_WAV_DC_RUN)_2048 $(AUDIO_ONLY_MP3_DC_RUN) $(AUDIO_ONLY_MP3_DC_RUN)_1024 $(AUDIO_ONLY_MP3_DC_RUN)_2048 $(PICKING_OUT_NOTES_DC_RUN) $(WAVEFORM_DC_RUN) $(SOUND_ENVELOPE_DC_RUN) $(SOUND_ENVELOPE_3D_DC_RUN) $(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_RUN) $(WAVEFORM_TERRAIN_3D_DC_RUN) $(FFT_TERRAIN_3D_DC_RUN) $(FFT_BANDS_TERRAIN_3D_DC_RUN) $(GLDC_MESH_DRAW_MODE_DC_RUN)
PHONY_DESKTOP_TARGETS := cool-gl33 cool-gl11 fftw-gl11 sound-envelope-gl33
PHONY_DC_TARGETS := cool-dc sh4zam-butterfly fftw-dc audio-only-wav-dc audio-only-mp3-dc picking-out-notes-dc waveform-dc sound-envelope-dc sound-envelope-3d-dc sound-envelope-3d-audio-cadence-dc waveform-terrain-3d-dc fft-terrain-3d-dc fft-bands-terrain-3d-dc gldc-mesh-draw-mode-dc

FMT ?= $(shell command -v clang-format 2>/dev/null || xcrun --find clang-format 2>/dev/null || echo clang-format)
FMT_STYLE ?= ./.clang-format
FMT_SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*.h) $(wildcard $(RES_DIR)/*.glsl)

.PHONY: help all fmt clean-all gldc-lib $(PHONY_DESKTOP_TARGETS) $(PHONY_DC_TARGETS)

help:
	$(error PLEASE DEFINE A TARGET: make $(PHONY_DESKTOP_TARGETS) $(PHONY_DC_TARGETS) fmt clean-all)

all: help

cool-gl33: $(COOL_GL33_PROG)
cool-gl11: $(COOL_GL11_PROG)
fftw-gl11: $(FFTW_GL11_PROG)
sound-envelope-gl33: $(SOUND_ENVELOPE_GL33_PROG)

cool-dc: $(COOL_DC_RUN)
sh4zam-butterfly: $(SH4ZAM_BUTTERFLY_RUN)
fftw-dc: $(FFTW_DC_RUN)
audio-only-wav-dc: $(AUDIO_ONLY_WAV_DC_RUN)
audio-only-mp3-dc: $(AUDIO_ONLY_MP3_DC_RUN)
picking-out-notes-dc: $(PICKING_OUT_NOTES_DC_RUN)
waveform-dc: $(WAVEFORM_DC_RUN)
sound-envelope-dc: $(SOUND_ENVELOPE_DC_RUN)
sound-envelope-3d-dc: $(SOUND_ENVELOPE_3D_DC_RUN)
sound-envelope-3d-audio-cadence-dc: $(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_RUN)
waveform-terrain-3d-dc: $(WAVEFORM_TERRAIN_3D_DC_RUN)
fft-terrain-3d-dc: $(FFT_TERRAIN_3D_DC_RUN)
fft-bands-terrain-3d-dc: $(FFT_BANDS_TERRAIN_3D_DC_RUN)
gldc-mesh-draw-mode-dc: $(GLDC_MESH_DRAW_MODE_DC_RUN)

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
	echo 'd="$$(cd "$$(dirname "$$0")/.." && pwd)"' >> $(2)
	echo 'exec "$$d/flycast_run.sh" "$$d/$(1)" "$$@"' >> $(2)
	chmod +x $(2)
endef

define RUN_RULE
$(1): $(2)
	$$(call WRITE_RUN,$$<,$$@)
endef

$(eval $(call RUN_RULE,$(COOL_DC_RUN),$(COOL_DC_ELF)))
$(eval $(call RUN_RULE,$(SH4ZAM_BUTTERFLY_RUN),$(SH4ZAM_BUTTERFLY_ELF)))
$(eval $(call RUN_RULE,$(FFTW_DC_RUN),$(FFTW_DC_ELF)))
$(eval $(call RUN_RULE,$(AUDIO_ONLY_WAV_DC_RUN),$(AUDIO_ONLY_WAV_DC_ELF)))
$(eval $(call RUN_RULE,$(AUDIO_ONLY_MP3_DC_RUN),$(AUDIO_ONLY_MP3_DC_ELF)))
$(eval $(call RUN_RULE,$(PICKING_OUT_NOTES_DC_RUN),$(PICKING_OUT_NOTES_DC_ELF)))
$(eval $(call RUN_RULE,$(WAVEFORM_DC_RUN),$(WAVEFORM_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_DC_RUN),$(SOUND_ENVELOPE_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_3D_DC_RUN),$(SOUND_ENVELOPE_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_RUN),$(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_ELF)))
$(eval $(call RUN_RULE,$(WAVEFORM_TERRAIN_3D_DC_RUN),$(WAVEFORM_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(FFT_TERRAIN_3D_DC_RUN),$(FFT_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(FFT_BANDS_TERRAIN_3D_DC_RUN),$(FFT_BANDS_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(GLDC_MESH_DRAW_MODE_DC_RUN),$(GLDC_MESH_DRAW_MODE_DC_ELF)))

$(DC_ELFS): TARGET_CPPFLAGS := $(DC_CPPFLAGS)
$(DC_ELFS): TARGET_CFLAGS := $(DC_BASE_CFLAGS) $(FFT_CFLAGS)
$(DC_ELFS): TARGET_LDLIBS := $(DC_WAV_LDLIBS)
$(DC_ELFS): TARGET_LDFLAGS := $(DC_LDFLAGS)
$(DC_ELFS): EXTRA_OBJS :=
$(DC_ELFS): ASSETS :=

$(COOL_DC_ELF): SRC := $(COOL_DC_SRC)
$(COOL_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV)
$(COOL_DC_ELF): $(COOL_DC_SRC)

$(SH4ZAM_BUTTERFLY_ELF): SRC := $(SH4ZAM_BUTTERFLY_SRC)
$(SH4ZAM_BUTTERFLY_ELF): ASSETS := $(RES_COUNTRY_WAV)
$(SH4ZAM_BUTTERFLY_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(SH4ZAM_BUTTERFLY_ELF): TARGET_LDLIBS := $(DC_WAV_LDLIBS)
$(SH4ZAM_BUTTERFLY_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(SH4ZAM_BUTTERFLY_ELF): $(SH4ZAM_BUTTERFLY_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(FFTW_DC_ELF): SRC := $(FFTW_DC_SRC)
$(FFTW_DC_ELF): ASSETS := $(RES_COUNTRY_WAV)
$(FFTW_DC_ELF): TARGET_CPPFLAGS := $(DC_CPPFLAGS) -I$(FFTW_1997_DIR)
$(FFTW_DC_ELF): EXTRA_OBJS := $(FFTW_1997_OBJS)
$(FFTW_DC_ELF): $(FFTW_DC_SRC) $(FFTW_1997_OBJS)

$(AUDIO_ONLY_WAV_DC_ELF): SRC := $(AUDIO_ONLY_WAV_SRC)
$(AUDIO_ONLY_WAV_DC_ELF): ASSETS := $(RES_COUNTRY_WAV)
$(AUDIO_ONLY_WAV_DC_ELF): TARGET_CFLAGS := $(DC_BASE_CFLAGS)
$(AUDIO_ONLY_WAV_DC_ELF): TARGET_LDLIBS := $(DC_LDLIBS)
$(AUDIO_ONLY_WAV_DC_ELF): $(AUDIO_ONLY_WAV_SRC)

$(AUDIO_ONLY_MP3_DC_ELF): SRC := $(AUDIO_ONLY_MP3_SRC)
$(AUDIO_ONLY_MP3_DC_ELF): ASSETS := $(RES_COUNTRY_MP3)
$(AUDIO_ONLY_MP3_DC_ELF): TARGET_CFLAGS := $(DC_BASE_CFLAGS)
$(AUDIO_ONLY_MP3_DC_ELF): TARGET_LDLIBS := $(DC_LDLIBS)
$(AUDIO_ONLY_MP3_DC_ELF): $(AUDIO_ONLY_MP3_SRC)

$(PICKING_OUT_NOTES_DC_ELF): SRC := $(PICKING_OUT_NOTES_DC_SRC)
$(PICKING_OUT_NOTES_DC_ELF): ASSETS := $(RES_ELECTRONEBULAE_WAV)
$(PICKING_OUT_NOTES_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(PICKING_OUT_NOTES_DC_ELF): TARGET_LDLIBS := $(DC_LDLIBS)
$(PICKING_OUT_NOTES_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(PICKING_OUT_NOTES_DC_ELF): $(PICKING_OUT_NOTES_DC_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(WAVEFORM_DC_ELF): SRC := $(WAVEFORM_DC_SRC)
$(WAVEFORM_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV)
$(WAVEFORM_DC_ELF): $(WAVEFORM_DC_SRC)

$(SOUND_ENVELOPE_DC_ELF): SRC := $(SOUND_ENVELOPE_DC_SRC)
$(SOUND_ENVELOPE_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV)
$(SOUND_ENVELOPE_DC_ELF): $(SOUND_ENVELOPE_DC_SRC)

$(SOUND_ENVELOPE_3D_DC_ELF): SRC := $(SOUND_ENVELOPE_3D_DC_SRC)
$(SOUND_ENVELOPE_3D_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV)
$(SOUND_ENVELOPE_3D_DC_ELF): $(SOUND_ENVELOPE_3D_DC_SRC)

$(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_ELF): SRC := $(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_SRC)
$(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV)
$(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_ELF): $(SOUND_ENVELOPE_3D_AUDIO_CADENCE_DC_SRC)

$(WAVEFORM_TERRAIN_3D_DC_ELF): SRC := $(WAVEFORM_TERRAIN_3D_DC_SRC)
$(WAVEFORM_TERRAIN_3D_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV) $(RES_FONT_ASSETS)
$(WAVEFORM_TERRAIN_3D_DC_ELF): $(WAVEFORM_TERRAIN_3D_DC_SRC)

$(FFT_TERRAIN_3D_DC_ELF): SRC := $(FFT_TERRAIN_3D_DC_SRC)
$(FFT_TERRAIN_3D_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV) $(RES_FONT_ASSETS)
$(FFT_TERRAIN_3D_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(FFT_TERRAIN_3D_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(FFT_TERRAIN_3D_DC_ELF): $(FFT_TERRAIN_3D_DC_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(FFT_BANDS_TERRAIN_3D_DC_ELF): SRC := $(FFT_BANDS_TERRAIN_3D_DC_SRC)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): ASSETS := $(RES_EXPERIMENT_WAV) $(RES_ELECTRONEBULAE_WAV) $(RES_FONT_ASSETS)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): $(FFT_BANDS_TERRAIN_3D_DC_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(GLDC_MESH_DRAW_MODE_DC_ELF): SRC := $(GLDC_MESH_DRAW_MODE_DC_SRC)
$(GLDC_MESH_DRAW_MODE_DC_ELF): TARGET_CFLAGS := $(DC_BASE_CFLAGS)
$(GLDC_MESH_DRAW_MODE_DC_ELF): TARGET_LDLIBS := $(DC_LDLIBS)
$(GLDC_MESH_DRAW_MODE_DC_ELF): $(GLDC_MESH_DRAW_MODE_DC_SRC)

$(DESKTOP_PROGS) $(DC_RUNS): | $(BIN_DIR)

$(DC_ELFS): $(RAYLIB_DC_LIB)
	rm -rf $(@D)/romdisk && mkdir -p $(@D)/romdisk
	for asset in $(ASSETS); do cp -f $$asset $(@D)/romdisk/; done
	$(call BUILD_ROMDISK,$(@D),$(@D)/romdisk)
	$(KOS_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -c $(SRC) -o $(basename $@).o
	$(DC_LD) $(TARGET_LDFLAGS) -o $@ $(basename $@).o $(EXTRA_OBJS) $(@D)/romdisk.o $(TARGET_LDLIBS)
	rm -rf $(@D)/romdisk

$(SH4ZAM_COMPLEX_OBJ): $(SH4ZAM_COMPLEX_SRC)
	mkdir -p $(@D)
	$(KOS_CC) $(SH4ZAM_CPPFLAGS) -DPLATFORM_DREAMCAST $(DC_BASE_CFLAGS) -I$(abspath $(GLDC_DIR))/include -c $< -o $@

$(BUILD_DIR)/dc/fftw_dc/fftw_1997_%.o: $(FFTW_1997_DIR)/%.c
	mkdir -p $(@D)
	$(KOS_CC) -I$(FFTW_1997_DIR) $(DC_BASE_CFLAGS) $(FFT_CFLAGS) -c $< -o $@

$(COOL_GL33_PROG): $(GL33_SRC) $(RAYLIB_GL33_LIB)
	$(CC) $(GL33_CPPFLAGS) $(BASE_CFLAGS) $< $(RAYLIB_GL33_LIB) $(DESKTOP_LDLIBS) -o $@

$(COOL_GL11_PROG): $(GL11_SRC) $(RAYLIB_GL11_LIB)
	$(CC) $(GL11_CPPFLAGS) $(BASE_CFLAGS) $< $(RAYLIB_GL11_LIB) $(DESKTOP_LDLIBS) -o $@

$(FFTW_GL11_PROG): $(FFTW_GL11_SRC) $(FFTW_1997_SRCS) $(RAYLIB_GL11_LIB)
	$(CC) $(GL11_CPPFLAGS) -I$(FFTW_1997_DIR) $(BASE_CFLAGS) $(FFT_CFLAGS) $(FFTW_GL11_SRC) $(FFTW_1997_SRCS) $(RAYLIB_GL11_LIB) $(DESKTOP_LDLIBS) -o $@

$(SOUND_ENVELOPE_GL33_PROG): $(SOUND_ENVELOPE_GL33_SRC) $(RAYLIB_GL33_LIB)
	$(CC) $(GL33_CPPFLAGS) $(BASE_CFLAGS) $< $(RAYLIB_GL33_LIB) $(DESKTOP_LDLIBS) -o $@

gldc-lib:
	@test -f $(GLDC_DIR)/CMakeLists.txt || { echo "GLdc checkout missing at $(GLDC_DIR). Initialize the submodule first."; exit 1; }
	cd $(GLDC_DIR) && cmake --fresh -S . -B $(abspath $(GLDC_BUILD_DIR)) -DCMAKE_TOOLCHAIN_FILE=$(GLDC_TOOLCHAIN) -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
	cmake --build $(GLDC_BUILD_DIR) --target GL --parallel
	@test -f $(GLDC_LIB)

$(RAYLIB_GL33_LIB):
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) clean
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) PLATFORM=PLATFORM_DESKTOP
	mkdir -p $(RAYLIB_GL33_DIR)
	cp -f $(RAYLIB_DESKTOP_SRC_DIR)/libraylib.a $(RAYLIB_DESKTOP_SRC_DIR)/raylib.h $(RAYLIB_DESKTOP_SRC_DIR)/raymath.h $(RAYLIB_DESKTOP_SRC_DIR)/rlgl.h $(RAYLIB_GL33_DIR)/

$(RAYLIB_GL11_LIB):
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) clean
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) PLATFORM=PLATFORM_DESKTOP GRAPHICS=GRAPHICS_API_OPENGL_11
	mkdir -p $(RAYLIB_GL11_DIR)
	cp -f $(RAYLIB_DESKTOP_SRC_DIR)/libraylib.a $(RAYLIB_DESKTOP_SRC_DIR)/raylib.h $(RAYLIB_DESKTOP_SRC_DIR)/raymath.h $(RAYLIB_DESKTOP_SRC_DIR)/rlgl.h $(RAYLIB_GL11_DIR)/

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

fmt:
	$(FMT) -style=file:$(FMT_STYLE) -i $(FMT_SRCS)

$(BIN_DIR):
	mkdir -p $@

clean-all:
	rm -rf $(BUILD_DIR) $(DESKTOP_PROGS) $(DC_RUNS) logs/*
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC_DIR) clean || true
	$(MAKE) -C $(RAYLIB_DC_SRC_DIR) clean || true
