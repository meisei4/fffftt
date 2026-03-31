.DEFAULT_GOAL := help

BUILD_DIR := build
BIN_DIR := bin
SRC_DIR := src
FFTW_1997_DIR := fftw_1997
ROMDISK_DIR := $(SRC_DIR)/romdisk
############################ TOGGLES!!!!!!!!########################
############################ FLOAT PRECISION ####################### 
# This will not work as i expect, need to figure out whats happening
FFTW_PRECISION_CFLAGS :=
# FFTW_PRECISION_CFLAGS := -DFFTW_ENABLE_FLOAT

############################ NUMERIC OPTIMIZATION OFF ##############
# Faster? idk whats happening here tbh
FFTW_DC_CFLAGS_TEST :=
FFTW_DC_CFLAGS_TEST := -fno-fast-math -ffp-contract=off

############################ PERIOD ################################ 
ALT_AUDIO_DEVICE_PERIOD_FRAMES_KOS_CFLAGS :=
PERIOD_FRAMES := 1024 # Uncomment  below linefor 1024-period testing!
#ALT_AUDIO_DEVICE_PERIOD_FRAMES_KOS_CFLAGS := KOS_CFLAGS="$(KOS_CFLAGS) -DAUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=$(PERIOD_FRAMES)"

COMMON_SOURCE := $(SRC_DIR)/audio_spectrum_analyzer.c
GL33_SOURCE := $(SRC_DIR)/cool_gl33.c
FFTW_SOURCE := $(SRC_DIR)/fftw_gl11.c
FFTW_DC_SOURCE := $(SRC_DIR)/fftw_dc.c
GL11_SOURCE := $(SRC_DIR)/cool_gl11.c
DC_SOURCE := $(SRC_DIR)/cool_dc.c
SH4_SOURCE := $(SRC_DIR)/sh4zam_butterfly.c
SH4ZAM_COMPLEX_SOURCE := sh4zam/source/shz_complex.c
FFTW_1997_SOURCES := $(wildcard $(FFTW_1997_DIR)/*.c)

GL33_RAYLIB_DIR := $(BUILD_DIR)/gl33/raylib
GL11_RAYLIB_DIR := $(BUILD_DIR)/gl11/raylib
DC_RAYLIB_DIR := $(BUILD_DIR)/dc/raylib
SH4_RAYLIB_DIR := $(BUILD_DIR)/sh4/raylib
DC_TARGET_DIR := $(BUILD_DIR)/dc/cool_dc
DC_FFTW_TARGET_DIR := $(BUILD_DIR)/dc/fftw_dc
SH4_TARGET_DIR := $(BUILD_DIR)/sh4/sh4zam_butterfly

GL33_BIN := $(BIN_DIR)/cool_gl33
FFTW_BIN := $(BIN_DIR)/fftw_gl11
GL11_BIN := $(BIN_DIR)/cool_gl11
DC_LAUNCHER := $(BIN_DIR)/cool_dc
DC_FFTW_LAUNCHER := $(BIN_DIR)/fftw_dc
SH4_LAUNCHER := $(BIN_DIR)/sh4zam_butterfly
DC_TARGET := $(DC_TARGET_DIR)/cool_dc.elf
DC_FFTW_TARGET := $(DC_FFTW_TARGET_DIR)/fftw_dc.elf
SH4_TARGET := $(SH4_TARGET_DIR)/sh4zam_butterfly.elf
DC_FFTW_1997_OBJS := $(patsubst $(FFTW_1997_DIR)/%.c,$(DC_FFTW_TARGET_DIR)/fftw_1997_%.o,$(FFTW_1997_SOURCES))

RAYLIB_DESKTOP_SRC := raylib_desktop/src
RAYLIB_DC_SRC := raylib_dc/src

KOS_PORTS_INCLUDE := -I$(KOS_PORTS)/include
FMT ?= $(shell command -v clang-format 2>/dev/null || { [ -x /usr/bin/clang-format ] && echo /usr/bin/clang-format; } || { [ -x /opt/homebrew/bin/clang-format ] && echo /opt/homebrew/bin/clang-format; } || { [ -x /Library/Developer/CommandLineTools/usr/bin/clang-format ] && echo /Library/Developer/CommandLineTools/usr/bin/clang-format; } || echo clang-format)
FMT_STYLE ?= .clang-format
FMT_SOURCES := $(COMMON_SOURCE) $(GL33_SOURCE) $(FFTW_SOURCE) $(FFTW_DC_SOURCE) $(GL11_SOURCE) $(DC_SOURCE) $(SH4_SOURCE)

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

.PHONY: help all cool-gl33 fftw-gl11 cool-gl11 cool-dc fftw-dc sh4zam-butterfly fmt clean-all

help:
	$(error PLEASE DEFINE A TARGET: make cool-gl33 | make cool-gl11 | make fftw-gl11 | make cool-dc | make fftw-dc | make sh4zam-butterfly | make fmt | make clean-all)

all: help

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
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL33_RAYLIB_DIR) $(GL33_SOURCE) $(COMMON_SOURCE) $(GL33_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(GL33_BIN)

fftw-gl11: $(GL11_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR)
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL11_RAYLIB_DIR) -I$(FFTW_1997_DIR) $(FFTW_PRECISION_CFLAGS) $(FFTW_SOURCE) $(COMMON_SOURCE) $(FFTW_1997_SOURCES) $(GL11_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(FFTW_BIN)

cool-gl11: $(GL11_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR)
	$(CC) -std=c99 -O2 -Wall -Wextra -DPLATFORM_DESKTOP -I$(GL11_RAYLIB_DIR) $(GL11_SOURCE) $(COMMON_SOURCE) $(GL11_RAYLIB_DIR)/libraylib.a -lm $(DESKTOP_LIBS) -o $(GL11_BIN)

$(DC_RAYLIB_DIR)/libraylib.a:
	$(MAKE) -C $(RAYLIB_DC_SRC) clean
	$(MAKE) -C $(RAYLIB_DC_SRC) all PLATFORM=PLATFORM_DREAMCAST GRAPHICS=GRAPHICS_API_OPENGL_11 $(ALT_AUDIO_DEVICE_PERIOD_FRAMES_KOS_CFLAGS)
	mkdir -p $(DC_RAYLIB_DIR)
	cp -f $(RAYLIB_DC_SRC)/libraylib.a $(RAYLIB_DC_SRC)/raylib.h $(RAYLIB_DC_SRC)/raymath.h $(RAYLIB_DC_SRC)/rlgl.h $(DC_RAYLIB_DIR)/

$(SH4_RAYLIB_DIR)/libraylib.a: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(SH4_RAYLIB_DIR)
	cp -f $(DC_RAYLIB_DIR)/libraylib.a $(DC_RAYLIB_DIR)/raylib.h $(DC_RAYLIB_DIR)/raymath.h $(DC_RAYLIB_DIR)/rlgl.h $(SH4_RAYLIB_DIR)/


$(DC_FFTW_TARGET_DIR)/fftw_1997_%.o: $(FFTW_1997_DIR)/%.c
	mkdir -p $(DC_FFTW_TARGET_DIR)
	kos-cc -I$(FFTW_1997_DIR) $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(FFTW_DC_CFLAGS_TEST) $(FFTW_PRECISION_CFLAGS) -std=gnu2x -c $< -o $@

cool-dc: $(DC_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(DC_TARGET_DIR)
	$(KOS_GENROMFS) -f $(DC_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(DC_TARGET_DIR)/romdisk.img $(DC_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(DC_TARGET_DIR)/romdisk_tmp.o -c $(DC_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(DC_TARGET_DIR)/romdisk.o -r $(DC_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) -std=gnu2x -c $(COMMON_SOURCE) -o $(DC_TARGET_DIR)/common.o
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) -std=gnu2x -c $(DC_SOURCE) -o $(DC_TARGET_DIR)/dc.o
	kos-cc -o $(DC_TARGET) $(DC_TARGET_DIR)/dc.o $(DC_TARGET_DIR)/common.o $(DC_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a -lGL -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,build/dc/cool_dc/cool_dc.elf,$(DC_LAUNCHER))
	rm -f $(DC_TARGET_DIR)/romdisk_tmp.c $(DC_TARGET_DIR)/romdisk_tmp.o

fftw-dc: $(DC_RAYLIB_DIR)/libraylib.a $(DC_FFTW_1997_OBJS)
	mkdir -p $(BIN_DIR) $(DC_FFTW_TARGET_DIR)
	$(KOS_GENROMFS) -f $(DC_FFTW_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(DC_FFTW_TARGET_DIR)/romdisk.img $(DC_FFTW_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(DC_FFTW_TARGET_DIR)/romdisk_tmp.o -c $(DC_FFTW_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(DC_FFTW_TARGET_DIR)/romdisk.o -r $(DC_FFTW_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(FFTW_1997_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(FFTW_DC_CFLAGS_TEST) -std=gnu2x -c $(COMMON_SOURCE) -o $(DC_FFTW_TARGET_DIR)/common.o
	kos-cc -I$(DC_RAYLIB_DIR) -I$(SRC_DIR) -I$(FFTW_1997_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) $(FFTW_DC_CFLAGS_TEST) $(FFTW_PRECISION_CFLAGS) -std=gnu2x -c $(FFTW_DC_SOURCE) -o $(DC_FFTW_TARGET_DIR)/fftw_dc.o
	kos-cc -o $(DC_FFTW_TARGET) $(DC_FFTW_TARGET_DIR)/fftw_dc.o $(DC_FFTW_TARGET_DIR)/common.o $(DC_FFTW_1997_OBJS) $(DC_FFTW_TARGET_DIR)/romdisk.o $(DC_RAYLIB_DIR)/libraylib.a -lGL -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,$(DC_FFTW_TARGET),$(DC_FFTW_LAUNCHER))
	rm -f $(DC_FFTW_TARGET_DIR)/romdisk_tmp.c $(DC_FFTW_TARGET_DIR)/romdisk_tmp.o

sh4zam-butterfly: $(SH4_RAYLIB_DIR)/libraylib.a
	mkdir -p $(BIN_DIR) $(SH4_TARGET_DIR)
	$(KOS_GENROMFS) -f $(SH4_TARGET_DIR)/romdisk.img -d $(ROMDISK_DIR) -v -x .gitignore -x .DS_Store -x Thumbs.db
	$(KOS_BASE)/utils/bin2c/bin2c $(SH4_TARGET_DIR)/romdisk.img $(SH4_TARGET_DIR)/romdisk_tmp.c romdisk
	$(KOS_CC) $(KOS_CFLAGS) -o $(SH4_TARGET_DIR)/romdisk_tmp.o -c $(SH4_TARGET_DIR)/romdisk_tmp.c
	$(KOS_CC) -o $(SH4_TARGET_DIR)/romdisk.o -r $(SH4_TARGET_DIR)/romdisk_tmp.o \
	  -L$(KOS_BASE)/lib/$(KOS_ARCH) -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
	  -L$(KOS_PORTS)/lib -Wl,--whole-archive -lromdiskbase
	kos-cc -iquote sh4zam/include -I$(SH4_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) -std=gnu2x -c $(COMMON_SOURCE) -o $(SH4_TARGET_DIR)/common.o
	kos-cc -iquote sh4zam/include -I$(SH4_RAYLIB_DIR) -I$(SRC_DIR) -I$(KOS_PORTS)/libwav/inst/include -DPLATFORM_DREAMCAST -DGRAPHICS_API_OPENGL_11 $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) -std=gnu2x -c $(SH4_SOURCE) -o $(SH4_TARGET_DIR)/sh4.o
	kos-cc -iquote sh4zam/include -DPLATFORM_DREAMCAST $(filter-out $(KOS_PORTS_INCLUDE),$(KOS_CFLAGS)) -std=gnu2x -c $(SH4ZAM_COMPLEX_SOURCE) -o $(SH4_TARGET_DIR)/shz_complex.o
	kos-cc -o $(SH4_TARGET) $(SH4_TARGET_DIR)/sh4.o $(SH4_TARGET_DIR)/common.o $(SH4_TARGET_DIR)/shz_complex.o $(SH4_TARGET_DIR)/romdisk.o $(SH4_RAYLIB_DIR)/libraylib.a -lGL -lkosutils -lwav -lm -lpthread
	$(call WRITE_BIN,build/sh4/sh4zam_butterfly/sh4zam_butterfly.elf,$(SH4_LAUNCHER))
	rm -f $(SH4_TARGET_DIR)/romdisk_tmp.c $(SH4_TARGET_DIR)/romdisk_tmp.o

fmt:
	@F="$(FMT)"; (command -v "$$F" >/dev/null 2>&1 || [ -x "$$F" ]) || { echo "clang-format not found. Install it or set FMT=path/to/clang-format"; exit 1; }
	@if [ -f "$(FMT_STYLE)" ]; then $(FMT) -style=file:$(FMT_STYLE) -i $(FMT_SOURCES); else $(FMT) -i $(FMT_SOURCES); fi

clean-all:
	rm -rf $(BUILD_DIR)
	rm -f $(GL33_BIN) $(FFTW_BIN) $(GL11_BIN) $(DC_LAUNCHER) $(DC_FFTW_LAUNCHER) $(SH4_LAUNCHER)
	rm -rf logs/*
	$(MAKE) -C $(RAYLIB_DESKTOP_SRC) clean || true
	$(MAKE) -C $(RAYLIB_DC_SRC) clean || true
