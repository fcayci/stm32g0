SRCS += ../include/system_stm32g0xx.c
SRCS += ../include/startup_stm32g031k8tx.s

OBJDIR = Debug

OBJS := $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))
OBJS := $(addprefix $(OBJDIR)/,$(notdir $(OBJS:.s=.o)))
vpath %.c $(sort $(dir $(SRCS)))
vpath %.s $(sort $(dir $(SRCS)))

INCLUDES += -I../include

CFLAGS += $(CDEFS) # project specific definitions
CFLAGS += -mcpu=cortex-m0plus -mthumb # processor setup
CFLAGS += -O0 # optimization is off
CFLAGS += -std=gnu11 # use GNU 11 standard

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 # generate debug info
# generate dependency info
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
endif

CFLAGS += -fno-common
CFLAGS += -Wall # turn on warnings
CFLAGS += -Wextra # extra warnings
CFLAGS += -pedantic # strict ISO warnings
CFLAGS += -Wmissing-include-dirs
CFLAGS += -Wsign-compare
CFLAGS += -Wcast-align
CFLAGS += -Wconversion # neg int const implicitly converted to uint
CFLAGS += -fsingle-precision-constant
CFLAGS += -fomit-frame-pointer # do not use fp if not needed
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += --specs=nano.specs

LDFLAGS += -mcpu=cortex-m0plus -mthumb # processor setup
#LDFLAGS += -nostartfiles # dont use standard start files
#LDFLAGS += -nodefaultlibs # dont use standard libraries
#LDFLAGS += -nostdlib # dont use startup or default libs
#LDFLAGS += --specs=nosys.specs
LDFLAGS += --specs=nano.specs
#LDFLAGS += --specs=rdimon.specs
LDFLAGS += -Wl,--gc-sections # linker garbage collector
LDFLAGS += -Wl,-Map=$(OBJDIR)/$(TARGET).map #generate map file
LDFLAGS += -Wl,--cref # add symbols to map file
LDFLAGS += -T$(LINKER_SCRIPT)
LDFLAGS += $(LIBINCLUDES)
LDFLAGS += $(LIBS)
LDFLAGS += -lc

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
DBG = $(CROSS_COMPILE)gdb

all: clean build size
	@echo "Successfully finished..."

build: $(TARGET).elf $(TARGET).bin $(TARGET).lst

$(TARGET).elf: $(OBJS)
	@echo "Linking..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	@echo "Building" $<
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.s
	@echo "Building" $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.hex: %.elf
	@$(OBJCOPY) -O ihex $(OBJDIR)/$< $(OBJDIR)/$@

%.bin: %.elf
	@$(OBJCOPY) -O binary $(OBJDIR)/$< $(OBJDIR)/$@

%.lst: %.elf
	@$(OBJDUMP) -x -S $(OBJDIR)/$(TARGET).elf > $(OBJDIR)/$@

size: $(TARGET).elf
	@$(SIZE) $(OBJDIR)/$(TARGET).elf

debug:
	@$(DBG) --eval-command="target extended-remote :4242" \
	 $(OBJDIR)/$(TARGET).elf

burn:
	@st-flash write $(OBJDIR)/$(TARGET).bin 0x8000000

clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR)/

.PHONY: all build size debug burn clean
