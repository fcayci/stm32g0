TARGET = asm
SRCS = asm.s

LINKER_SCRIPT = stm32.ld

OBJDIR = Debug

OBJS := $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.s=.o)))

CFLAGS  = -mcpu=cortex-m0plus -mthumb # processor setup
CFLAGS += -mfloat-abi=soft # soft fp
CFLAGS += -g3 # enable full debug info
#CFLAGS += --specs=nano.specs

LDFLAGS += -mcpu=cortex-m0plus -mthumb # processor setup
LDFLAGS += -mfloat-abi=soft # soft fp
LDFLAGS += -nostdlib # dont use startup or default libs
#LDFLAGS += -Wl,--gc-sections
#LDFLAGS += -static
#LDFLAGS += --specs=nano.specs
LDFLAGS += -T$(LINKER_SCRIPT)
#LDFLAGS += -Wl,--start-group -lc -lm -Wl,--end-group

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
	$(CC) $(OBJS) $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(OBJDIR)
	@echo "Building" $<
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

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
	@st-flash write $(OBJDIR)/$(TARGET).bin 0x08000000

clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR)/

.PHONY: all build size clean burn debug
