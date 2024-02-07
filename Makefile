
#
# !!! NOTE: Header files are completely ignored !!!
# If you change a header file, either make clean or
# change the accompanying source files to force a rebuild!
#

# ---------------------------------------------------------
#  Quake 2 source files:
# ---------------------------------------------------------

#
# All C source files used by the game and engine:
#
SRC_FILES = \
	ps2/builtin/backtile.c  \
	ps2/builtin/conback.c   \
	ps2/builtin/conchars.c  \
	ps2/builtin/help.c      \
	ps2/builtin/inventory.c \
	ps2/builtin/palette.c   \
	ps2/debug_print.c       \
	ps2/main_ps2.c          \
	ps2/math_funcs.c        \
	ps2/mem_alloc.c         \
	ps2/model_load.c        \
	ps2/net_ps2.c           \
	ps2/ref_ps2.c           \
	ps2/sys_ps2.c           \
	ps2/tex_image.c         \
	ps2/view_draw.c         \
	ps2/vec_mat.c           \
	ps2/vid_ps2.c           \
	ps2/vu1.c               \
	client/cl_cin.c         \
	client/cl_ents.c        \
	client/cl_fx.c          \
	client/cl_input.c       \
	client/cl_inv.c         \
	client/cl_main.c        \
	client/cl_newfx.c       \
	client/cl_parse.c       \
	client/cl_pred.c        \
	client/cl_scrn.c        \
	client/cl_tent.c        \
	client/cl_view.c        \
	client/console.c        \
	client/keys.c           \
	client/menu.c           \
	client/qmenu.c          \
	client/snd_dma.c        \
	client/snd_mem.c        \
	client/snd_mix.c        \
	common/cmd.c            \
	common/cmodel.c         \
	common/common.c         \
	common/crc.c            \
	common/cvar.c           \
	common/filesys.c        \
	common/md4.c            \
	common/net_chan.c       \
	common/pmove.c          \
	game/g_ai.c             \
	game/g_chase.c          \
	game/g_cmds.c           \
	game/g_combat.c         \
	game/g_func.c           \
	game/g_items.c          \
	game/g_main.c           \
	game/g_misc.c           \
	game/g_monster.c        \
	game/g_phys.c           \
	game/g_save.c           \
	game/g_spawn.c          \
	game/g_svcmds.c         \
	game/g_target.c         \
	game/g_trigger.c        \
	game/g_turret.c         \
	game/g_utils.c          \
	game/g_weapon.c         \
	game/m_actor.c          \
	game/m_berserk.c        \
	game/m_boss2.c          \
	game/m_boss3.c          \
	game/m_boss31.c         \
	game/m_boss32.c         \
	game/m_brain.c          \
	game/m_chick.c          \
	game/m_flash.c          \
	game/m_flipper.c        \
	game/m_float.c          \
	game/m_flyer.c          \
	game/m_gladiator.c      \
	game/m_gunner.c         \
	game/m_hover.c          \
	game/m_infantry.c       \
	game/m_insane.c         \
	game/m_medic.c          \
	game/m_move.c           \
	game/m_mutant.c         \
	game/m_parasite.c       \
	game/m_soldier.c        \
	game/m_supertank.c      \
	game/m_tank.c           \
	game/p_client.c         \
	game/p_hud.c            \
	game/p_trail.c          \
	game/p_view.c           \
	game/p_weapon.c         \
	game/q_shared.c         \
	null/cd_null.c          \
	null/in_null.c          \
	null/snddma_null.c      \
	server/sv_ccmds.c       \
	server/sv_ents.c        \
	server/sv_game.c        \
	server/sv_init.c        \
	server/sv_main.c        \
	server/sv_send.c        \
	server/sv_user.c        \
	server/sv_world.c

#
# IOP/IRX modules pulled from the PS2DEV SDK:
#IRX_PATH  = $(PS2SDK)/iop/irx
#IRX_FILES = usbd.irx

#
# VCL/VU microprograms:
#
VSM_FILES = src/ps2/vu1progs/color_triangles_clip_tris.vsm

# ---------------------------------------------------------
#  Libs from the PS2DEV SDK:
# ---------------------------------------------------------

EE_LIBS =    \
    -lcdvd   \
	-ldma     \
	-lgraph   \
	-ldraw    \
	-lpatches \
	-lpacket2 \
	-lm      \
	-lc       \
	-lkernel

# ---------------------------------------------------------
#  Make flags/macros:
# ---------------------------------------------------------

#
# Where to find the source files and where to dump the
# intermediate object files and the final ELF executable.
#
SRC_DIR        = src
OUTPUT_DIR     = build
FS_DIR		   = fs
IOP_OUTPUT_DIR = irx
VU_OUTPUT_DIR  = vu

#
# Name of the binary executable (QPS2) and
# object (.o) filenames derived from source filenames.
#
BIN_TARGET = $(FS_DIR)/QPS2.ELF
OBJ_FILES  = $(addprefix $(OUTPUT_DIR)/$(SRC_DIR)/, $(patsubst %.c, %.o, $(SRC_FILES)))

#
# The IRX IOProcessor modules we embed:
#
#IOP_MODULES = $(addprefix $(OUTPUT_DIR)/$(IOP_OUTPUT_DIR)/, $(patsubst %.irx, %.o, $(IRX_FILES)))

#
# The VU microprograms:
#
VU_PROGS = $(addprefix $(OUTPUT_DIR)/$(VU_OUTPUT_DIR)/, $(patsubst %.vsm, %.o, $(VSM_FILES)))

#
# Global #defines, C-flags and include paths:
#
PS2_GLOBAL_DEFS = -D_EE -DGAME_HARD_LINKED -DPS2_QUAKE
PS2_CFLAGS      = $(PS2_GLOBAL_DEFS) -O0 -G0 -Wformat=2
PS2_INCS        = -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(SRC_DIR)

# Linker flags
EE_LDFLAGS := -L$(PS2SDK)/ee/lib $(EE_LDFLAGS)

#
# Custom GCC for the PS2 EE CPU (based on GCC v3):
#
PS2_CC = mips64r5900el-ps2-elf-gcc

#
# Vector Unit assembler:
#
PS2_VU_DVP = dvp-as

#
# The same structure of src/ is recreated in build/, so we need mkdir.
#
MKDIR_CMD = mkdir -p

# ---------------------------------------------------------
#  Define VERBOSE somewhere before this to get the
#  full make command printed to stdout.
#  If VERBOSE is NOT defined you can also:
#   - Define QUIET_W_CFLAGS to print the CFLAGS
# ---------------------------------------------------------
ifndef VERBOSE
  QUIET = @
  # Colored text if the terminal supports ANSI color codes:
  #COLOR_ALT1    = "\033[36;1m"
  #COLOR_ALT2    = "\033[35;1m"
  #COLOR_DEFAULT = "\033[0;1m"
  COLOR_ALT1    = ""
  COLOR_ALT2    = ""
  COLOR_DEFAULT = ""
  ifdef QUIET_W_CFLAGS
    ECHO_COMPILING = @echo $(COLOR_ALT1)"-> Compiling "$(PS2_CFLAGS) $(COLOR_ALT2)$<$(COLOR_ALT1)" ..."$(COLOR_DEFAULT)
  else # !QUIET_W_CFLAGS
    ECHO_COMPILING = @echo $(COLOR_ALT1)"-> Compiling "$(COLOR_ALT2)$<$(COLOR_ALT1)" ..."$(COLOR_DEFAULT)
  endif # QUIET_W_CFLAGS
  ECHO_LINKING  = @echo $(COLOR_ALT1)"-> Linking ..."$(COLOR_DEFAULT)
  ECHO_CLEANING = @echo $(COLOR_ALT1)"-> Cleaning ..."$(COLOR_DEFAULT)
  ECHO_BUILDING_IOP_MODS   = @echo $(COLOR_ALT1)"-> Assembling IOP module '$(notdir $*).irx' ..."$(COLOR_DEFAULT)
  ECHO_ASSEMBLING_VU_PROGS = @echo $(COLOR_ALT1)"-> Assembling VU microprogram '$(notdir $*).vcl' ..."$(COLOR_DEFAULT)
endif # VERBOSE

# ---------------------------------------------------------
#  Make rules:
# ---------------------------------------------------------

.PHONY : all iso run clean clean_vu


all: iso
#	$(QUIET) ee-strip --strip-all $(BIN_TARGET)

iso: $(BIN_TARGET)
	mkisofs -o ./build/qps2.iso ./fs

#
# C source files => OBJ files:
#
$(BIN_TARGET): $(OBJ_FILES) $(IOP_MODULES) $(VU_PROGS)
	$(ECHO_LINKING)
	$(PS2_CC) -T$(PS2SDK)/ee/startup/linkfile -o $(BIN_TARGET) $(OBJ_FILES) $(VU_PROGS) $(IOP_MODULES) $(EE_LDFLAGS) $(EE_LIBS)

$(OBJ_FILES): $(OUTPUT_DIR)/%.o: %.c
	$(ECHO_COMPILING)
	$(QUIET) $(MKDIR_CMD) $(dir $@)
	$(QUIET) $(PS2_CC) $(PS2_CFLAGS) $(PS2_INCS) -c $< -o $@

#
# IOP/IRX modules, compiled into the program:
#
#$(IOP_MODULES): $(OUTPUT_DIR)/$(IOP_OUTPUT_DIR)/%.o: $(OUTPUT_DIR)/$(IOP_OUTPUT_DIR)/%.c
#	$(ECHO_BUILDING_IOP_MODS)
#	$(QUIET) $(PS2_CC) $(PS2_CFLAGS) -c $< -o $@

#$(OUTPUT_DIR)/$(IOP_OUTPUT_DIR)/%.c:
#	$(QUIET) $(MKDIR_CMD) $(dir $@)
#	$(QUIET) bin2c $(IRX_PATH)/$*.irx $@ $*_irx

#
# VU microprograms:
#
$(VU_PROGS): $(OUTPUT_DIR)/$(VU_OUTPUT_DIR)/%.o: %.vsm
	$(ECHO_ASSEMBLING_VU_PROGS)
	$(QUIET) $(MKDIR_CMD) $(dir $@)
	$(QUIET) $(PS2_VU_DVP) $< -o $@

# ---------------------------------------------------------
#  Custom 'clean' rules:
# ---------------------------------------------------------

# Using 'find' because we need to recursively clear all subdirs.
clean:
	$(ECHO_CLEANING)
	$(QUIET) rm -f  $(BIN_TARGET)
	$(QUIET) rm -rf $(OUTPUT_DIR)/$(IOP_OUTPUT_DIR)
	$(QUIET) rm -rf $(OUTPUT_DIR)/$(VU_OUTPUT_DIR)
	$(QUIET) rm -f  $(INSTALL_PATH)/$(notdir $(BIN_TARGET))
	$(QUIET) find $(OUTPUT_DIR) -name "*.o" -type f -delete

# Just clears the VU code output directory.
clean_vu:
	$(ECHO_CLEANING)
	$(QUIET) rm -rf $(OUTPUT_DIR)/$(VU_OUTPUT_DIR)

