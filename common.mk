######################################################################################
# GNU GCC ARM Embeded Toolchain base directories and binaries
######################################################################################
GCC_BASE = 
GCC_BIN  = $(GCC_BASE)bin/
GCC_LIB  = $(GCC_BASE)arm-none-eabi/lib/
GCC_INC  = $(GCC_BASE)arm-none-eabi/include/

AS       = arm-none-eabi-as
CC       = arm-none-eabi-gcc
CPP      = arm-none-eabi-g++
LD       = arm-none-eabi-gcc
AR       = arm-none-eabi-ar
OBJCOPY  = arm-none-eabi-objcopy
OBJDUMP  = arm-none-eabi-objdump

######################################################################################
# Shell command
######################################################################################
ifeq ($(OS),Windows_NT)
SHELL				= sh.exe
ECHO				= echo.exe
RM					= rm.exe
MKDIR				= mkdir.exe
TOUCH				= touch.exe
DOXYGEN				= doxygen.exe
OPENOCD				= openocd.exe
else
SHELL				= bash
ECHO				= echo
RM					= rm
MKDIR				= mkdir
TOUCH				= touch
DOXYGEN				= doxygen
OPENOCD				= openocd
endif

######################################################################################
# Custom options for cortex-m and cortex-r processors 
######################################################################################
CORTEX_M0PLUS_CC_FLAGS  = -mthumb -mcpu=cortex-m0plus
CORTEX_M0PLUS_LIB_PATH  = $(GCC_LIB)armv6-m
CORTEX_M0_CC_FLAGS      = -mthumb -mcpu=cortex-m0
CORTEX_M0_LIB_PATH      = $(GCC_LIB)armv6-m
CORTEX_M1_CC_FLAGS      = -mthumb -mcpu=cortex-m1
CORTEX_M1_LIB_PATH      = $(GCC_LIB)armv6-m
CORTEX_M3_CC_FLAGS      = -mthumb -mcpu=cortex-m3
CORTEX_M3_LIB_PATH      = $(GCC_LIB)armv7-m
CORTEX_M4_NOFP_CC_FLAGS = -mthumb -mcpu=cortex-m4
CORTEX_M4_NOFP_LIB_PATH = $(GCC_LIB)armv7e-m
CORTEX_M4_SWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16
CORTEX_M4_SWFP_LIB_PATH = $(GCC_LIB)armv7e-m/softfp
CORTEX_M4_HWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CORTEX_M4_HWFP_LIB_PATH = $(GCC_LIB)armv7e-m/fpu
CORTEX_R4_NOFP_CC_FLAGS = -mthumb -march=armv7-r
CORTEX_R4_NOFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb
CORTEX_R4_SWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=softfp -mfpu=vfpv3-d16
CORTEX_R4_SWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/softfp
CORTEX_R4_HWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=hard -mfpu=vfpv3-d16
CORTEX_R4_HWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/fpu
CORTEX_R5_NOFP_CC_FLAGS = -mthumb -march=armv7-r
CORTEX_R5_NOFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb
CORTEX_R5_SWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=softfp -mfpu=vfpv3-d16
CORTEX_R5_SWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/softfp
CORTEX_R5_HWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=hard -mfpu=vfpv3-d16
CORTEX_R5_HWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/fpu

######################################################################################
# Project Configuration ( from makfile )
#	PROJECT_NAME				= name of target to build
#	PROJECT_SOURCE				= all source file , including .c & .s
#	PROJECT_INC_PATHS
#	PROJECT_LIBRARY
#	PROJECT_DOXYGEN_CONFIG
#	PROJECT_OPENOCD_CONFIG
######################################################################################
TARGET				= $(PROJECT_NAME)
SOURCE_FOR_S		= $(foreach s_source_file, $(PROJECT_SOURCE),$(filter %.s, $(s_source_file)))
SOURCE_FOR_C		= $(foreach c_source_file, $(PROJECT_SOURCE),$(filter %.c, $(c_source_file)))

DEPENDS_S			= $(addprefix obj/,$(SOURCE_FOR_S:.s=.s.d))
DEPENDS_C			= $(addprefix obj/,$(SOURCE_FOR_C:.c=.c.d))

OBJECT_S			= $(DEPENDS_S:.s.d=.o)
OBJECT_C			= $(DEPENDS_C:.c.d=.o)

LINKER_SCRIPT		= $(PROJECT_LINKER_SCRIPT)

INCLUDE_DIR			= -I$(TOP)/inc

PROGRAM				= bin/$(TARGET)

MAKEALL_DEPEND		=


######################################################################################
# System Configuration
######################################################################################
SYS_LIBRARIES		= -lc -lnosys
SYS_LD_FLAGS		= --specs=nano.specs 

CFLAGS				= $(MCU_CC_FLAGS) $(INCLUDE_DIR)  $(PROJECT_SYMBOLS) $(PROJECT_INC_PATHS) -g -fno-common -O3 -Wall -Werror -finline-functions -ffunction-sections -fdata-sections -fno-builtin
AFLAGS				= $(MCU_CC_FLAGS)
LFLAGS				= $(MCU_CC_FLAGS) -L$(MCU_LIB_PATH) $(SYS_LIBRARIES) $(SYS_LD_FLAGS) -Wl,--gc-sections -Wl,-Map,$(PROGRAM).map


###############################################################################
# Makefile execution
###############################################################################

.PHONY:    all clean depends force flash document dis openocd

all:$(TARGET)
$(TARGET):$(DEPENDS_S) $(DEPENDS_C) $(OBJECT_S) $(OBJECT_C) $(PROJECT_LIBRARY)
	@$(ECHO)
	@$(MKDIR) -p bin
	@$(ECHO) Linking...
	@$(LD) $(LFLAGS) -T$(LINKER_SCRIPT) -o $(TOP)/bin/$(TARGET).elf $(OBJECT_S) $(OBJECT_C) $(LDFLAGS) $(PROJECT_LIBRARY)
	@$(ECHO) Create HEX file...
	@$(OBJCOPY) -O ihex $(PROGRAM).elf $(PROGRAM).hex

dis:
	@$(ECHO) disassemble $(PROGRAM).dis
	@$(OBJDUMP) -d $(PROGRAM).elf > $(PROGRAM).dis

obj/%.o : obj/%.c.d $(MAKEALL_DEPEND)    
obj/%.o : obj/%.s.d $(MAKEALL_DEPEND)    

obj/%.o: %.c
	@$(MKDIR) -p obj
	@$(ECHO) c compile $<
	@$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: %.s
	@$(MKDIR) -p obj
	@$(ECHO) assemble $<
	@$(AS) $(AFLAGS) -o $@ $< > $(addsuffix .lst,$(basename $@))
    
obj/%.s.d : %.s $(MAKEALL_DEPEND)
	@$(MKDIR) -p obj
	@$(ECHO) s mkdep $<
	@$(CC) $(AFLAGS) -MM -MT $(patsubst %.s.d,%.o,$@) $< > $(addsuffix .d,$(basename $@))

obj/%.c.d : %.c $(MAKEALL_DEPEND)
	@$(MKDIR) -p obj
	@$(ECHO) c mkdep $<
	@$(CC) $(CFLAGS) -MM -MT $(patsubst %.c.d,%.o,$@) $< > $(addsuffix .d,$(basename $@))

clean:
	@$(ECHO) clean dependency and object ...
	@$(RM) -rf obj
	@$(ECHO) clean binary file ...
	@$(RM) -rf bin

flash:
	@$(ECHO)  
	@$(ECHO) Load file to flash
	@$(OPENOCD) -f $(PROJECT_OPENOCD_CONFIG) -c "init" -c "reset halt" -c "flash write_image erase $(TOP)/bin/$(TARGET).hex" -c "reset run" -c "shutdown" 2> /dev/null

document:
	@$(ECHO) Generating document ...
	@$(MKDIR) -p document
	@$(DOXYGEN) $(PROJECT_DOXYGEN_CONFIG)
	@$(ECHO) Done.

openocd:
	@$(OPENOCD) -f $(PROJECT_OPENOCD_CONFIG) 
	
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),flash)
ifneq ($(MAKECMDGOALS),document)
ifneq ($(MAKECMDGOALS),dis)
ifneq ($(MAKECMDGOALS),openocd)
sinclude $(DEPENDS_S) $(DEPENDS_C)
endif
endif
endif
endif
endif