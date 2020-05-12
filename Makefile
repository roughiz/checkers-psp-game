PBP_PATH = Bundle

TARGET = main
OBJS = framebuffer.o graphics.o engine.o menu.o about.o checkers.o main.o 

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LIBDIR = 
LIBS = -lpsplibc -lstdc++ -lpspgu -lpsppower -lpsprtc -lpng -lz -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSPCheckers
PSP_EBOOT_ICON = $(PBP_PATH)/icon0.png
PSPSDK=$(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak

font.c : font.raw
	bin2c font.raw font.c font