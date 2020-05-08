# project name (generate executable with this name)
#TARGET   = ${CROSS_COMPILE}libgurux.serial.java.so
TARGET   = test

CROSS    = arm-openstlinux_weston-linux-gnueabi-
CC       = g++

# compiling flags here
#CFLAGS   = -I/home/slee/work/l5cha/jdk1.8.0_231/include/ -I/home/slee/work/l5cha/jdk1.8.0_231/include/linux/ -O3 -Wall -fPIC -Iinclude -I../cis-3.3_charset-Linux-4.19-armv7ve_gcc_32bit/include -I../cis-3.3_charset-Linux-4.19-armv7ve_gcc_32bit/include/cc -I../cis-3.3_charset-Linux-4.19-armv7ve_gcc_32bit/include/ce -I/home/slee/work/common/inc 
#CC       = $(CROSS)gcc -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/sdb1/common/stm32mp1/2.6-openstlinux-4.19-thud-mp1-19-10-09/sysroots/cortexa7t2hf-neon-vfpv4-openstlinux_weston-linux-gnueabi
#CC = gcc
LINKER   = $(CC) -o


# linking flags here
LFLAGS   = -pthread

# change these to set the proper directories where each files shoould be

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin


SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)

OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
#	@$(rm) $(OBJDIR)/$(OBJECTS)
	@$(rm) $(OBJECTS)
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Cleanup complete!" 
	@echo $(OBJECTS) $(BINDIR)/$(TARGET)

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
