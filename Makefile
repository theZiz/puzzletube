#==PC==
CPP = gcc -g -march=native -DX86CPU
SDL = `sdl-config --cflags`
#==Consoles==
#==GP2X/WIZ==
#  REALGP2X activates gp2xspecific optimizations, which the wiz doesn't need
#	 e.g. modells with fewer polygons
#ORIGINALFW = -static -lpng -lpthread -lz -ljpeg -lm -s
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
#SDL = -I/opt/mipsel-linux-uclibc/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/mipsel-linux-uclibc/usr/include
#LIB = -L/opt/mipsel-linux-uclibc/usr/lib -Wl,-rpath=/opt/mipsel-linux-uclibc/usr/lib
#==Pandora==
#CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DARMCPU -DPANDORA
#SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
#INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
#LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib -lpnd

all: puzzletube

puzzletube: puzzletube.c
	$(CPP) -O3 puzzletube.c ../3dengine/3dengine.o ../3dengine/graphicstuff.o ../3dengine/meshloader.o ../3dengine/graphicstuff-asm.o $(SDL) $(INCLUDE) $(LIB) -lSDL_image -lSDL -lm $(ORIGINALFW) -o puzzletube

puzzletube4.5: puzzletube.c
	gcc-4.5 -fforward-propagate -finline -ftoplevel-reorder -ftree-copy-prop -ftree-sink -fomit-frame-pointer -fauto-inc-dec -fcprop-registers -fdce -fdefer-pop -fdelayed-branch -fdse -fguess-branch-probability -fif-conversion2 -fif-conversion -fipa-pure-const -fipa-reference -fmerge-constants -fsplit-wide-types -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copyrename -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre -ftree-phiprop -ftree-sra -ftree-pta -ftree-ter -funit-at-a-time puzzletube.c ../3dengine/3dengine.o ../3dengine/graphicstuff.o ../3dengine/meshloader.o ../3dengine/graphicstuff-asm.o $(SDL) $(INCLUDE) $(LIB) -lSDL_image -lSDL -lm $(ORIGINALFW) -o puzzletube

clean:
	rm puzzletube
	rm *.o
