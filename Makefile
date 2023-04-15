
CROSS_COMPILER:=arm-none-eabi-

CC:=$(CROSS_COMPILER)gcc
AS:=$(CROSS_COMPILER)gcc
LD:=$(CROSS_COMPILER)gcc
OBJCOPY:=$(CROSS_COMPILER)objcopy
OBJDUMP:=$(CROSS_COMPILER)objdump
SIZE:=$(CROSS_COMPILER)size

.DEFAULT_GOAL=all

VERSION:=Git$(shell git describe --always --dirty --tags --abbrev=6)
TIME=$(shell date "+%Y_%m_%d_%H_%M_%S")

out-dir:=out
inc-dirs:=
obj-dirs:=

target:=$(out-dir)/driver_practice_$(VERSION)_$(TIME)

elf-file:=$(target).elf
bin-file:=$(target).bin
dump-file:=$(target).dump
map-file:=$(target).map
ver-file:=src/main/version.h

ifeq ($(V),)
Q:=@
else
Q:=
endif



src-y:=
asm-y:=

defs:=

CFLAGS:= -O1 -ffunction-sections -fdata-sections --specs=nano.specs -fno-common -fno-exceptions -Werror -Wall -Wextra
LDFLAGS:=-Wl,-Map=$(map-file) -Wl,--gc-sections --specs=nano.specs
ASFLAGS:= -O1

LDFLAGS+=-Wl,--wrap=__aeabi_unwind_cpp_pr0
LDFLAGS+=-Wl,--wrap=putchar
LDFLAGS+=-Wl,--wrap=puts
LDFLAGS+=-Wl,--wrap=printf


-include .config
include $(shell find . -name *.mk)

-include $(shell find $(out-dir) -name *.d 2>/dev/null)

ASFLAGS+=$(CFLAGS)
CFLAGS+=$(addprefix -I,$(inc-dirs)) $(addprefix -D,$(defs))

obj-files:=$(addprefix $(out-dir)/,$(src-y:%.c=%.o))
obj-files+=$(addprefix $(out-dir)/,$(patsubst %.s,%.o,$(filter %.s,$(asm-y))))
obj-files+=$(addprefix $(out-dir)/,$(patsubst %.S,%.o,$(filter %.S,$(asm-y))))
obj-dirs:=$(sort $(dir $(obj-files)))

all:$(elf-file) $(bin-file) $(dump-file)
	@git checkout $(ver-file)
.PHONY:all

$(ver-file):
	@sed -i "s/unknown/$(VERSION)-$(TIME)/g" $(ver-file)
PHONY+=$(ver-file)

$(elf-file): $(obj-files)
	@echo linking $@
	$Q $(CC) $(LDFLAGS) $^  -o $@
	$Q $(SIZE) -d   $@

$(bin-file):$(elf-file)
	@echo objcopy...
	$Q $(OBJCOPY) -O binary $< $@

$(dump-file):$(elf-file)
	@echo objdump...
	$Q $(OBJDUMP)  -f -h -D -l -S  $<  > $@

$(out-dir)/%.o:%.S
	@if [ ! -e $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo building $<
	$Q $(CC) $(ASFLAGS) -MMD -c $< -o $@

$(out-dir)/%.o:%.s
	@if [ ! -e $(dir $@) ] ; then mkdir -p $(dir $@) ;fi
	@echo building $<
	$Q $(CC) $(ASFLAGS) -MMD -c $< -o $@

$(out-dir)/%.o:%.c
	@if [ ! -e $(dir $@) ] ;then mkdir -p $(dir $@); fi
	@echo building $<
	$Q $(CC) $(CFLAGS) -MMD -c $< -o $@


clean:
	rm -rf out
PHONY+=clean

.PHONY:$(PHONY)