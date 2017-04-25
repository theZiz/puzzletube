#==stuff linked to
DYNAMIC = -lSDL_ttf -lSDL_mixer -lSDL_image -lSDL -lm
#==global Flags. Even on the gp2x with 16 kb Cache, -O3 is much better then -Os
CFLAGS = -O3 -fsingle-precision-constant -fPIC
# Testtweaks: -fgcse-lm -fgcse-sm -fsched-spec-load -fmodulo-sched -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fgcse-las -fgcse-after-reload -fvariable-expansion-in-unroller -ftracer -fbranch-target-load-optimize
GENERAL_TWEAKS = -ffast-math
#==PC==
FLAGS = -g -DDESKTOP $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`

SPARROW_FOLDER = ../sparrow3d

SPARROW3D_LIB = libsparrow3d.so
SPARROWNET_LIB = libsparrowNet.so
SPARROWSOUND_LIB = libsparrowSound.so

ifdef TARGET
include $(SPARROW_FOLDER)/target-files/$(TARGET).mk
BUILD = ./build/$(TARGET)/puzzletube
SPARROW_LIB = $(SPARROW_FOLDER)/build/$(TARGET)/sparrow3d
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
SPARROW_LIB = $(SPARROW_FOLDER)
endif
LIB += -L$(SPARROW_LIB)
INCLUDE += -I$(SPARROW_FOLDER)
DYNAMIC += -lsparrow3d -lsparrowSound -lsparrowNet

CFLAGS += $(PARAMETER) $(FLAGS)

all: puzzletube
	@echo "=== Built for Target "$(TARGET)" ==="

targets:
	@echo "The targets are the same like for sparrow3d. :P"

puzzletube: puzzletube.c particle.o game.o music.o menu.o stars.o settings.o lettering.o splashscreen.o c4a.o makeBuildDir
	cp -u $(SPARROW_LIB)/$(SPARROW3D_LIB) $(BUILD)
	cp -u $(SPARROW_LIB)/$(SPARROWNET_LIB) $(BUILD)
	cp -u $(SPARROW_LIB)/$(SPARROWSOUND_LIB) $(BUILD)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< particle.o game.o music.o menu.o stars.o settings.o lettering.o splashscreen.o c4a.o $(SDL) $(INCLUDE) $(LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/$@$(SUFFIX)

makeBuildDir:
	 @if [ ! -d $(BUILD:/puzzletube=/) ]; then mkdir $(BUILD:/puzzletube=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

particle.o: particle.c particle.h
	$(CC) $(CFLAGS) -c particle.c $(SDL) $(INCLUDE)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< $(SDL) $(INCLUDE)

clean:
	rm -f *.o
	rm -f puzzletube$(SUFFIX)
