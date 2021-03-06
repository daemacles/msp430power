#!/bin/bash

#
# make_script.sh
#
# creates skeleton files for test cases
# 
# usage:
# ./make_script.sh test_name
#

BASENAME=$1

mkdir $BASENAME
cp template/main.c $BASENAME/$BASENAME.c

cat > $BASENAME/Makefile <<EOF
#
# Makefile for msp430
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced
# \$(TARGET).elf \$(TARGET).hex and \$(TARGET).txt nad \$(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
#

TARGET     = $BASENAME
#MCU        = msp430g2553
MCU        = msp430f2618
# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = $BASENAME.c
# Include are located in the Include directory
INCLUDES = -IInclude
# Add or subtract whatever MSPGCC flags you want. There are plenty more
ifndef OPT
OPT = -Os
endif
############################################################
CFLAGS   = -mmcu=\$(MCU) -g \$(OPT) -Wall -Wunused \$(INCLUDES)
ASFLAGS  = -mmcu=\$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=\$(MCU) -Wl,-Map=\$(TARGET).map
############################################################
CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv
############################################################

# the file which will include dependencies
DEPEND = \$(SOURCES:.c=.d)

# all the object files
OBJECTS = \$(SOURCES:.c=.o)

all: \$(TARGET).elf \$(TARGET).hex \$(TARGET).txt

\$(TARGET).elf: \$(OBJECTS)
	echo "Linking \$@"
	\$(CC) \$(OBJECTS) \$(LDFLAGS) \$(LIBS) -o \$@
	echo
	echo ">>>> Size of Firmware <<<<"
	\$(SIZE) \$(TARGET).elf
	echo

%.hex: %.elf
	\$(OBJCOPY) -O ihex \$< \$@

%.txt: %.hex
	\$(MAKETXT) -O \$@ -TITXT \$< -I

%.o: %.c
	echo "Compiling \$<"
	\$(CC) -c \$(CFLAGS) -o \$@ \$<

# rule for making assembler source listing, to see the code
%.lst: %.c
	\$(CC) -c \$(ASFLAGS) -Wa,-anlhd \$< > \$@

# include the dependencies unless we're going to clean, then forget about
# them.
ifneq (\$(MAKECMDGOALS), clean)
-include \$(DEPEND)
endif

# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
%.d: %.c
	echo "Generating dependencies \$@ from \$<"
	\$(CC) -M \${CFLAGS} \$< >\$@

acquire: \$(TARGET).elf
	python ../../scripts/power.py $BASENAME.dat
program: \$(TARGET).elf
	mspdebug rf2500 "prog \$(TARGET).elf"

.SILENT:
.PHONY:	clean

clean:
	-\$(RM) \$(OBJECTS)
	-\$(RM) \$(TARGET).elf
	-\$(RM) \$(TARGET).hex
	-\$(RM) \$(TARGET).txt
	-\$(RM) \$(TARGET).map
	-\$(RM) \$(SOURCES:.c=.lst)
	-\$(RM) \$(DEPEND)

EOF

echo "Don't forget to add $BASENAME to git..."
echo "Before programming, change to correct MSP device in the Makefile"
echo "Use 'make acquire' to gather data from o-scope"

