HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = PlaydateNative.pdx

# Locate the SDK
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)

######
# IMPORTANT: You must add your source folders to VPATH for make to find them
# ex: VPATH += src1:src2
######

VPATH += :./Playdate

# List C source files here
SRC = main.cpp Playdate.cpp

# List all user directories here
UINCDIR = ./

# List user asm files
UASRC = 

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

