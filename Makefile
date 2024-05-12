#COMPILER OPTIONS
CC = gcc

INCLUDE_FLAGS = -Iinclude/ -Idrivers/user/libphx/include/
CFLAGS = -Wall -Wno-unused -O6 -m64 -std=gnu99 -D_PHX_LINUX $(INCLUDE_FLAGS) 
LFLAGS = -L/usr/local/lib -Ldrivers/user/libphx -lphx -lpfw -lm -lpthread -lrt

#DEPENDANCIES
COMDEP  = Makefile $(wildcard ./src/*.h) drivers/kernel/phxdrv/picc_dio.h

#FILES
TARGETDIR   = bin
TARGET      = $(TARGETDIR)/watchdog
SOURCE      = $(wildcard ./src/*.c)
OBJECT      = $(patsubst %.c,%.o,$(SOURCE))


#WATCHDOG
$(TARGET): $(OBJECT) $(TARGETDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECT) $(LFLAGS) 

$(TARGETDIR):
	mkdir -p $(TARGETDIR)


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

#REMOVE *~ files
remove_backups:
	rm -f ./*~ */*~ */*/*~
