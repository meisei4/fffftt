.DEFAULT_GOAL := help

BUILD_DIR := build
BIN_DIR := bin
SRC_DIR := src
RES_DIR := $(SRC_DIR)/resources

SH4ZAM_DIR := sh4zam
GLDC_DIR := GLdc
RAYLIB_DC_SRC_DIR := raylib_dc/src

GLDC_BUILD_DIR := $(BUILD_DIR)/dc/gldc
GLDC_LIB := $(GLDC_BUILD_DIR)/libGL.a
GLDC_TOOLCHAIN := $(KOS_BASE)/utils/cmake/kallistios.toolchain.cmake

RAYLIB_DC_DIR := $(BUILD_DIR)/dc/raylib

RAYLIB_DC_LIB := $(RAYLIB_DC_DIR)/libraylib.a

AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES ?=
DC_LD ?= kos-cc

FFT_CFLAGS := -fno-fast-math -ffp-contract=off
FFT_CFLAGS += -DFFT_ENABLE_PROFILE
DC_BASE_CFLAGS := $(filter-out -I$(KOS_PORTS)/include,$(KOS_CFLAGS)) -std=gnu2x
DC_KOS_CFLAGS := $(filter-out -I$(KOS_PORTS)/include,$(KOS_CFLAGS)) -I$(abspath $(GLDC_DIR))/include

DC_CPPFLAGS := -I$(RAYLIB_DC_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -I$(abspath $(GLDC_DIR))/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11
DC_CPPFLAGS += $(if $(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),-DAUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=$(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),)
SH4ZAM_CPPFLAGS := -iquote $(SH4ZAM_DIR)/include

DC_LDFLAGS := $(KOS_LIB_PATHS) -L$(KOS_PORTS)/libwav/inst/lib
DC_LDLIBS := $(RAYLIB_DC_LIB) $(GLDC_LIB) -lkosutils -lm -lpthread
DC_WAV_LDLIBS := $(RAYLIB_DC_LIB) $(GLDC_LIB) -lkosutils -lwav -lm -lpthread

RES_KREUZSCHMERZEN_YOU_KNOW_WHY_WAV := $(RES_DIR)/kreuzschmerzen_you_know_why_22050hz_pcm16_mono.wav
RES_KREUZSCHMERZEN_RENT_DUE_WAV := $(RES_DIR)/kreuzschmerzen_rent_due_22050hz_pcm16_mono.wav
RES_EXPERIMENT_WAV := $(RES_DIR)/shadertoy_experiment_22050hz_pcm16_mono.wav
RES_ELECTRONEBULAE_WAV := $(RES_DIR)/shadertoy_electronebulae_22050hz_pcm16_mono.wav
RES_8BIT_WAV := $(RES_DIR)/shadertoy_8bit_22050hz_pcm16_mono.wav
RES_GEOMETRIC_PERSON_WAV := $(RES_DIR)/shadertoy_geometric_person_22050hz_pcm16_mono.wav
RES_TROPICAL_WAV := $(RES_DIR)/shadertoy_tropical_22050hz_pcm16_mono.wav
RES_XTRACK_WAV := $(RES_DIR)/shadertoy_xtrack_22050hz_pcm16_mono.wav
RES_FONT_ASSETS := $(RES_DIR)/vga_rom_f16_0px_TIGHT.fnt $(RES_DIR)/vga_rom_f16_0px_TIGHT.png

SH4ZAM_BUTTERFLY_SRC := $(SRC_DIR)/sh4zam_butterfly.c
PICKING_OUT_NOTES_DC_SRC := $(SRC_DIR)/picking_out_notes_dc.c
WAVEFORM_DC_SRC := $(SRC_DIR)/waveform_dc.c
SOUND_ENVELOPE_DC_SRC := $(SRC_DIR)/sound_envelope_dc.c
SOUND_ENVELOPE_3D_DC_SRC := $(SRC_DIR)/sound_envelope_3d_dc.c
WAVEFORM_TERRAIN_3D_DC_SRC := $(SRC_DIR)/waveform_terrain_3d_dc.c
FFT_TERRAIN_3D_DC_SRC := $(SRC_DIR)/fft_terrain_3d_dc.c
FFT_BANDS_TERRAIN_3D_DC_SRC := $(SRC_DIR)/fft_bands_terrain_3d_dc.c

SH4ZAM_COMPLEX_SRC := $(SH4ZAM_DIR)/source/shz_complex.c
SH4ZAM_COMPLEX_OBJ := $(BUILD_DIR)/dc/sh4zam/shz_complex.o

SH4ZAM_BUTTERFLY_ELF := $(BUILD_DIR)/dc/sh4zam_butterfly/sh4zam_butterfly.elf
PICKING_OUT_NOTES_DC_ELF := $(BUILD_DIR)/dc/picking_out_notes_dc/picking_out_notes_dc.elf
WAVEFORM_DC_ELF := $(BUILD_DIR)/dc/waveform_dc/waveform_dc.elf
SOUND_ENVELOPE_DC_ELF := $(BUILD_DIR)/dc/sound_envelope_dc/sound_envelope_dc.elf
SOUND_ENVELOPE_3D_DC_ELF := $(BUILD_DIR)/dc/sound_envelope_3d_dc/sound_envelope_3d_dc.elf
WAVEFORM_TERRAIN_3D_DC_ELF := $(BUILD_DIR)/dc/waveform_terrain_3d_dc/waveform_terrain_3d_dc.elf
FFT_TERRAIN_3D_DC_ELF := $(BUILD_DIR)/dc/fft_terrain_3d_dc/fft_terrain_3d_dc.elf
FFT_BANDS_TERRAIN_3D_DC_ELF := $(BUILD_DIR)/dc/fft_bands_terrain_3d_dc/fft_bands_terrain_3d_dc.elf

SH4ZAM_BUTTERFLY_RUN := $(BIN_DIR)/sh4zam_butterfly
PICKING_OUT_NOTES_DC_RUN := $(BIN_DIR)/picking_out_notes_dc
WAVEFORM_DC_RUN := $(BIN_DIR)/waveform_dc
SOUND_ENVELOPE_DC_RUN := $(BIN_DIR)/sound_envelope_dc
SOUND_ENVELOPE_3D_DC_RUN := $(BIN_DIR)/sound_envelope_3d_dc
WAVEFORM_TERRAIN_3D_DC_RUN := $(BIN_DIR)/waveform_terrain_3d_dc
FFT_TERRAIN_3D_DC_RUN := $(BIN_DIR)/fft_terrain_3d_dc
FFT_BANDS_TERRAIN_3D_DC_RUN := $(BIN_DIR)/fft_bands_terrain_3d_dc

DC_ELFS := $(SH4ZAM_BUTTERFLY_ELF) $(PICKING_OUT_NOTES_DC_ELF) $(WAVEFORM_DC_ELF) $(SOUND_ENVELOPE_DC_ELF) $(SOUND_ENVELOPE_3D_DC_ELF) $(WAVEFORM_TERRAIN_3D_DC_ELF) $(FFT_TERRAIN_3D_DC_ELF) $(FFT_BANDS_TERRAIN_3D_DC_ELF)
DC_RUNS := $(SH4ZAM_BUTTERFLY_RUN) $(PICKING_OUT_NOTES_DC_RUN) $(WAVEFORM_DC_RUN) $(SOUND_ENVELOPE_DC_RUN) $(SOUND_ENVELOPE_3D_DC_RUN) $(WAVEFORM_TERRAIN_3D_DC_RUN) $(FFT_TERRAIN_3D_DC_RUN) $(FFT_BANDS_TERRAIN_3D_DC_RUN)
PHONY_DC_TARGETS := sh4zam-butterfly picking-out-notes-dc waveform-dc sound-envelope-dc sound-envelope-3d-dc waveform-terrain-3d-dc fft-terrain-3d-dc fft-bands-terrain-3d-dc

FMT ?= $(shell command -v clang-format 2>/dev/null || xcrun --find clang-format 2>/dev/null || echo clang-format)
FMT_STYLE ?= ./.clang-format
FMT_SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*.h) $(wildcard $(RES_DIR)/*.glsl)

.PHONY: help all fmt clean-all gldc-lib $(PHONY_DC_TARGETS)

help:
	$(error PLEASE DEFINE A TARGET: make $(PHONY_DC_TARGETS) fmt clean-all)

all: help

sh4zam-butterfly: $(SH4ZAM_BUTTERFLY_RUN)
picking-out-notes-dc: $(PICKING_OUT_NOTES_DC_RUN)
waveform-dc: $(WAVEFORM_DC_RUN)
sound-envelope-dc: $(SOUND_ENVELOPE_DC_RUN)
sound-envelope-3d-dc: $(SOUND_ENVELOPE_3D_DC_RUN)
waveform-terrain-3d-dc: $(WAVEFORM_TERRAIN_3D_DC_RUN)
fft-terrain-3d-dc: $(FFT_TERRAIN_3D_DC_RUN)
fft-bands-terrain-3d-dc: $(FFT_BANDS_TERRAIN_3D_DC_RUN)

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

$(eval $(call RUN_RULE,$(SH4ZAM_BUTTERFLY_RUN),$(SH4ZAM_BUTTERFLY_ELF)))
$(eval $(call RUN_RULE,$(PICKING_OUT_NOTES_DC_RUN),$(PICKING_OUT_NOTES_DC_ELF)))
$(eval $(call RUN_RULE,$(WAVEFORM_DC_RUN),$(WAVEFORM_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_DC_RUN),$(SOUND_ENVELOPE_DC_ELF)))
$(eval $(call RUN_RULE,$(SOUND_ENVELOPE_3D_DC_RUN),$(SOUND_ENVELOPE_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(WAVEFORM_TERRAIN_3D_DC_RUN),$(WAVEFORM_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(FFT_TERRAIN_3D_DC_RUN),$(FFT_TERRAIN_3D_DC_ELF)))
$(eval $(call RUN_RULE,$(FFT_BANDS_TERRAIN_3D_DC_RUN),$(FFT_BANDS_TERRAIN_3D_DC_ELF)))

$(DC_ELFS): TARGET_CPPFLAGS := $(DC_CPPFLAGS)
$(DC_ELFS): TARGET_CFLAGS := $(DC_BASE_CFLAGS) $(FFT_CFLAGS)
$(DC_ELFS): TARGET_LDLIBS := $(DC_WAV_LDLIBS)
$(DC_ELFS): TARGET_LDFLAGS := $(DC_LDFLAGS)
$(DC_ELFS): EXTRA_OBJS :=
# $(DC_ELFS): ASSETS := $(RES_KREUZSCHMERZEN_YOU_KNOW_WHY_WAV) $(RES_KREUZSCHMERZEN_RENT_DUE_WAV) $(RES_FONT_ASSETS)
$(DC_ELFS): ASSETS := $(RES_EXPERIMENT_WAV) $(RES_ELECTRONEBULAE_WAV) $(RES_FONT_ASSETS)
# $(DC_ELFS): ASSETS += $(RES_8BIT_WAV) $(RES_GEOMETRIC_PERSON_WAV) $(RES_TROPICAL_WAV) $(RES_XTRACK_WAV)

$(SH4ZAM_BUTTERFLY_ELF): SRC := $(SH4ZAM_BUTTERFLY_SRC)
$(SH4ZAM_BUTTERFLY_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(SH4ZAM_BUTTERFLY_ELF): TARGET_LDLIBS := $(DC_WAV_LDLIBS)
$(SH4ZAM_BUTTERFLY_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(SH4ZAM_BUTTERFLY_ELF): $(SH4ZAM_BUTTERFLY_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(PICKING_OUT_NOTES_DC_ELF): SRC := $(PICKING_OUT_NOTES_DC_SRC)
$(PICKING_OUT_NOTES_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(PICKING_OUT_NOTES_DC_ELF): TARGET_LDLIBS := $(DC_LDLIBS)
$(PICKING_OUT_NOTES_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(PICKING_OUT_NOTES_DC_ELF): $(PICKING_OUT_NOTES_DC_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(WAVEFORM_DC_ELF): SRC := $(WAVEFORM_DC_SRC)
$(WAVEFORM_DC_ELF): $(WAVEFORM_DC_SRC)

$(SOUND_ENVELOPE_DC_ELF): SRC := $(SOUND_ENVELOPE_DC_SRC)
$(SOUND_ENVELOPE_DC_ELF): $(SOUND_ENVELOPE_DC_SRC)

$(SOUND_ENVELOPE_3D_DC_ELF): SRC := $(SOUND_ENVELOPE_3D_DC_SRC)
$(SOUND_ENVELOPE_3D_DC_ELF): $(SOUND_ENVELOPE_3D_DC_SRC)

$(WAVEFORM_TERRAIN_3D_DC_ELF): SRC := $(WAVEFORM_TERRAIN_3D_DC_SRC)
$(WAVEFORM_TERRAIN_3D_DC_ELF): $(WAVEFORM_TERRAIN_3D_DC_SRC)

$(FFT_TERRAIN_3D_DC_ELF): SRC := $(FFT_TERRAIN_3D_DC_SRC)
$(FFT_TERRAIN_3D_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(FFT_TERRAIN_3D_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(FFT_TERRAIN_3D_DC_ELF): $(FFT_TERRAIN_3D_DC_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(FFT_BANDS_TERRAIN_3D_DC_ELF): SRC := $(FFT_BANDS_TERRAIN_3D_DC_SRC)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): TARGET_CPPFLAGS := $(SH4ZAM_CPPFLAGS) $(DC_CPPFLAGS)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): EXTRA_OBJS := $(SH4ZAM_COMPLEX_OBJ)
$(FFT_BANDS_TERRAIN_3D_DC_ELF): $(FFT_BANDS_TERRAIN_3D_DC_SRC) $(SH4ZAM_COMPLEX_OBJ)

$(DC_RUNS): | $(BIN_DIR)

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

gldc-lib:
	@test -f $(GLDC_DIR)/CMakeLists.txt || { echo "GLdc checkout missing at $(GLDC_DIR). Initialize the submodule first."; exit 1; }
	cd $(GLDC_DIR) && cmake --fresh -S . -B $(abspath $(GLDC_BUILD_DIR)) -DCMAKE_TOOLCHAIN_FILE=$(GLDC_TOOLCHAIN) -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
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

fmt:
	$(FMT) -style=file:$(FMT_STYLE) -i $(FMT_SRCS)

$(BIN_DIR):
	mkdir -p $@

clean-all:
	rm -rf $(BUILD_DIR) $(DC_RUNS) logs/*
	$(MAKE) -C $(RAYLIB_DC_SRC_DIR) clean || true
