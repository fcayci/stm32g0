TARGET = standalone
SRCS = main.c

LINKER_SCRIPT = stm32.ld

OBJDIR = Debug

OBJS := $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))

CFLAGS  = -mcpu=cortex-m0plus -mthumb # processor setup
CFLAGS += -O0 # optimization is off
CFLAGS += -std=gnu11 # use GNU 11 standard

CFLAGS += -fno-common
CFLAGS += -Wall # turn on warnings
CFLAGS += -Wextra # extra warnings

LDFLAGS += -mcpu=cortex-m0plus -mthumb # processor setup
LDFLAGS += -nostdlib # dont use startup or default libs
#LDFLAGS += --specs=nosys.specs
LDFLAGS += -T$(LINKER_SCRIPT)
#LDFLAGS += -lm

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

$(OBJDIR)/%.o: %.c
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
	@st-flash write $(OBJDIR)/$(TARGET).bin 0x8000000

clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR)/

.PHONY: all build size clean burn debug
