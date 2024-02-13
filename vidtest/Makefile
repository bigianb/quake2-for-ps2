
# ------------

DEBUG ?= 0

EE_BIN = vidtest.elf
EE_SRC_DIR = src/
EE_OBJS_DIR = obj/
EE_ASM_DIR = asm/

MAPFILE = vidtest.map
EE_LDFLAGS += -Wl,-Map,$(MAPFILE)

EE_LIBS = -lpad -lgs -ldma -lc

ifeq ($(DEBUG),1)
  EE_CFLAGS += -D__DEBUG -g
else
  EE_CFLAGS += -O2
endif

MAIN_OBJS = main.o

# Generate .d files to track header file dependencies of each object file
EE_CFLAGS += -MMD -MP
EE_OBJS += $(MAIN_OBJS)
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)
EE_DEPS = $($(filter %.o,$(EE_OBJS)):%.o=%.d)

.PHONY: all release debug rebuild

all: $(EE_BIN)


debug:
	$(MAKE) DEBUG=1 all

clean:
	echo "Cleaning..."
	rm -fr $(MAPFILE) $(EE_BIN) $(EE_OBJS_DIR) $(EE_ASM_DIR)

rebuild: clean all

$(EE_ASM_DIR):
	@mkdir -p $@

$(EE_OBJS_DIR):
	@mkdir -p $@

$(EE_OBJS_DIR)%.o: $(EE_SRC_DIR)%.c | $(EE_OBJS_DIR)
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)%.o: $(EE_ASM_DIR)%.c | $(EE_OBJS_DIR)
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

ifndef PS2SDK
ps2sdk-not-setup:
	@echo "PS2SDK is not setup. Please setup PS2SDK before building this project"
endif

ifdef PS2SDK
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
endif

-include $(EE_DEPS)
