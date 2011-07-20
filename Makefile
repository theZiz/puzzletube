CFLAGS = -O3
#==PC==
CPP = gcc -g -march=native -DX86CPU
SDL = `sdl-config --cflags`
#==Consoles==
#==GP2X/WIZ==
#  REALGP2X activates gp2xspecific optimizations, which the wiz doesn't need
#	 e.g. modells with fewer polygons
#ORIGINALFW = -static -lsmpeg -lstdc++ -lSDL -lfreetype -lpng -lpthread -lz -ljpeg -lm -s
#CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DREALGP2X
#SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
#INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
#LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
#==Caanoo==
#CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO
#SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
#LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
#==Dingoo==
#CPP = /opt/mipsel-linux-uclibc/usr/bin/mipsel-linux-gcc -DDINGOO
#SDL = -I/opt/mipsel-linux-uclibc/usr/mipsel-linux-uclibc/sys-include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/mipsel-linux-uclibc/usr/mipsel-linux-uclibc/sys-include
#LIB = -L/opt/mipsel-linux-uclibc/usr/lib -Wl,-rpath=/opt/mipsel-linux-uclibc/usr/lib
#==Pandora==
#CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DARMCPU -DPANDORA
#SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
#INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
#LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib -lpnd

all: puzzletube

puzzletube: puzzletube.c particle.o game.o music.o menu.o stars.o settings.o lettering.o splashscreen.o
	$(CPP) $(CFLAGS) puzzletube.c particle.o game.o music.o menu.o stars.o settings.o lettering.o splashscreen.o ../sparrow3d/3dengine.o ../sparrow3d/graphicstuff.o ../sparrow3d/meshloader.o ../sparrow3d/graphicstuff-asm.o $(SDL) $(INCLUDE) $(LIB) -lSDL_ttf -lSDL_mixer -lSDL_image -lSDL -lm  $(ORIGINALFW) -o puzzletube

particle.o: particle.c particle.h
	$(CPP) $(CFLAGS) -c particle.c $(SDL) $(INCLUDE)

game.o: game.c game.h
	$(CPP) $(CFLAGS) -c game.c $(SDL) $(INCLUDE)

music.o: music.c music.h
	$(CPP) $(CFLAGS) -c music.c $(SDL) $(INCLUDE)

menu.o: menu.c menu.h
	$(CPP) $(CFLAGS) -c menu.c $(SDL) $(INCLUDE)

stars.o: stars.c stars.h
	$(CPP) $(CFLAGS) -c stars.c $(SDL) $(INCLUDE)

settings.o: settings.c settings.h
	$(CPP) $(CFLAGS) -c settings.c $(SDL) $(INCLUDE)

lettering.o: lettering.c lettering.h
	$(CPP) $(CFLAGS) -c lettering.c $(SDL) $(INCLUDE)

splashscreen.o: splashscreen.c splashscreen.h
	$(CPP) $(CFLAGS) -c splashscreen.c $(SDL) $(INCLUDE)

clean:
	rm *.o
	rm puzzletube
