#COMPILER OPTIONS
CC = gcc

INCLUDE_FLAGS := -Iinclude/ -Idrivers/user/libphx/include/ -Ilibraries/
CFLAGS := -Wall -Wno-unused -O6 -m64 -std=gnu99 -D_PHX_LINUX -DPICC_DIO_ENABLE $(INCLUDE_FLAGS) $(CFLAGS)
LDFLAGS := -L/usr/local/lib -Ldrivers/user/libphx -lphx -lpfw -lm -lpthread -lrt $(LDFLAGS)

#DEPENDANCIES
COMDEP  := Makefile $(wildcard ./include/*.h) $(wildcard ./libraries/*/*.h) drivers/kernel/phxdrv/picc_dio.h

#FILES
TARGETDIR   = bin
TARGET      = $(TARGETDIR)/watchdog
SOURCE      = $(wildcard ./src/*.c)
OBJECT      = $(patsubst %.c,%.o,$(SOURCE))
LIBBMP      = libraries/libbmp/libbmp.a
DATADIR     = data

#WATCHDOG
$(TARGET): $(OBJECT) $(TARGETDIR) $(DATADIR) $(LIBBMP)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECT) $(LIBBMP) $(LDFLAGS) 

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

$(DATADIR):
	mkdir -p $(DATADIR)

$(LIBBMP):
	cd libraries/libbmp && make && cd ../..

#USERSPACE OBJECTS
%.o: %.c  $(COMDEP)
	$(CC) $(CFLAGS) -o $@ -c $<

#DRIVERS
drivers: phxdrv

phxdrv:
	make -C drivers/phxdrv
	sudo make -C drivers/phxdrv install 

#CLEAN
clean:
	rm -f ./src/*.o $(TARGET)

spotless: clean
	rm -f $(TARGETDIR)/*
	rm -f $(DATADIR)/*
	cd libraries/libbmp && make clean && cd ../..

#REMOVE *~ files
remove_backups:
	rm -f ./*~ */*~ */*/*~
