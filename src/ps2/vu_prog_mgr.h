
/* ================================================================================================
 * -*- C -*-
 * File: vu_prog_mgr.h
 * Author: Guilherme R. Lampert
 * Created on: 01/02/16
 * Brief: Vector Unit microprogram management.
 *
 * This source code is released under the GNU GPL v2 license.
 * Check the accompanying LICENSE file for details.
 * ================================================================================================ */

#ifndef PS2_VU_PROG_MGR_H
#define PS2_VU_PROG_MGR_H

#include <tamtypes.h>
#include "ps2/defs_ps2.h"
#include "ps2/dma_mgr.h"

// VU programs generated by dvp-as are always placed
// in the ".vudata" section of the object file.
#define VU_DATA_SECTION __attribute__((section(".vudata")))

/*
==============================================================

Basic information about a Vector Unit microprogram:

==============================================================
*/

typedef enum
{
    VU0_MICROPROGRAM, // Currently unused and unsupported.
    VU1_MICROPROGRAM  // Used for all 3D rendering done by Quake2.
} ps2_vu_prog_type_t;

typedef struct
{
    // Pointers to the static memory from vudata.
    const u64 * code_start_ptr;
    const u64 * code_end_ptr;

    // Code size in 64bits double-words.
    int dw_code_size;

    // Offset in 64bits words where the program is loaded in VU memory.
    int dw_vu_mem_offset;

    // Currently has to be = VU1_MICROPROGRAM.
    ps2_vu_prog_type_t prog_type;

    // Used by the upload DMA chain.
    int upload_ptr;
} ps2_vu_prog_t;

//
// Easier way of declaring a ps2_vu_prog_t
// instance referencing the external code.
//
#define DECLARE_VU_MICROPROGRAM(prog_name)             \
    extern const u64 prog_name##_CodeStart VU_DATA_SECTION; \
    extern const u64 prog_name##_CodeEnd   VU_DATA_SECTION; \
    ps2_vu_prog_t prog_name PS2_ALIGN(16) = {          \
        &prog_name##_CodeStart,                        \
        &prog_name##_CodeEnd,                          \
        0, 0, 0, 0                                     \
    }

/*
==============================================================

VIF macros:

==============================================================
*/

#define VIF_S_32  0
#define VIF_S_16  1
#define VIF_S_8   2
#define VIF_V2_32 4
#define VIF_V2_16 5
#define VIF_V2_8  6
#define VIF_V3_32 8
#define VIF_V3_16 9
#define VIF_V3_8  0xA
#define VIF_V4_32 0xC
#define VIF_V4_16 0xD
#define VIF_V4_8  0xE
#define VIF_V4_5  0xF

#define VIF_NOP                       ((u64)(0))
#define VIF_MSCALL(instructionNumber) ((u64)((0x14 << 24) | (instructionNumber)))
#define VIF_MSCNT                     ((u64)(0x17 << 24))
#define VIF_FLUSH                     ((u64)(0x11 << 24))
#define VIF_FLUSH_E                   ((u64)(0x10 << 24))
#define VIF_FLUSH_A                   ((u64)(0x13 << 24))
#define VIF_DIRECT(quadCount)         ((u64)((0x50 << 24) | (quadCount)))
#define VIF_MPG(numOf64Codes, addr)   ((u64)((0x4A << 24) | ((numOf64Codes) << 16) | (addr)))
#define VIF_STCYCL(WL, CL)            ((u64)((0x1 << 24) | ((WL) << 8) | (CL)))
#define VIF_OFFSET(offset)            ((u64)((0x2 << 24) | (offset)))
#define VIF_BASE(base)                ((u64)((0x3 << 24) | (base)))
#define VIF_ITOP(val)                 ((u64)((0x4 << 24) | (val)))
#define VIF_STMOD(mode)               ((u64)((0x5 << 24) | (mode & 0x3)))
#define VIF_STROW                     ((u64)(0x30 << 24))
#define VIF_STCOL                     ((u64)(0x31 << 24))
#define VIF_STMASK                    ((u64)(0x20 << 24))
#define VIF_MSK_PATH3(mask)           ((u64)(0x6 << 24) | (mask))

#define VIF_UNPACK(type, quadCount, addr)                 ((u64)(((0x60 | (u32)(type)) << 24) | ((quadCount) << 16) | (addr)))
#define VIF_UNPACK_TO_TOP(type, quadCount, addr)          ((u64)UNPACK(type, quadCount, addr) | (1 << 15))
#define VIF_UNPACK_UNSIGNED(type, quadCount, addr)        ((u64)UNPACK(type, quadCount, addr) | (1 << 14))
#define VIF_UNPACK_TO_TOP_UNSIGNED(type, quadCount, addr) ((u64)UNPACK_TO_TOP(type, quadCount, addr) | (1 << 14))

/*
==============================================================

Vector Unit microprogram manager:

==============================================================
*/

enum
{
    MAX_ACTIVE_VU1_PROGS   = 4,
    MAX_VU1_PROGMEM_DWORDS = 4096
};

typedef struct
{
    //
    // Current microprograms in the Vector Unit memory.
    // We allow up to four programs at any one time on the VU1.
    //
    // There are 2048 QWs in VU1 micro memory so you can have quite a few
    // microprograms loaded at once, depending on the size of each.
    //
    const ps2_vu_prog_t * active_vu1_progs[MAX_ACTIVE_VU1_PROGS];

    // Number of double-words (int64s) used so far in the VU1 for program memory.
    // We have a total of 2048 16bytes quadwords, so 4096 8bytes double-words.
    // This is just used for local runtime validation.
    int dw_vu1_progmem_used;
} ps2_vu_prog_manager_t;

// Call this on startup to properly initialize a VU program manager instance.
void VU_ProgManagerInit(ps2_vu_prog_manager_t * mgr);

// Sets all entries in active_vu1_progs[] to NULL and used mem to zero.
void VU_ProgManagerClearActiveProgs(ps2_vu_prog_manager_t * mgr);

// Set up a VU microprogram previously declared with DECLARE_VU_MICROPROGRAM().
// You must call this at least once before uploading the program.
void VU_InitMicroprogram(ps2_vif_dma_obj_t dma_obj, ps2_vu_prog_t * prog,
                         ps2_vu_prog_type_t type, int dw_start_offset);

// Send the program to the VU memory.
// 'index' is between 0 and MAX_ACTIVE_VU1_PROGS - 1.
// This function will do nothing if the program is already current
// at 'index', but you can force upload anyways if 'force' is nonzero.
void VU_UploadMicroprogram(ps2_vu_prog_manager_t * mgr, ps2_vif_dma_obj_t dma_obj,
                           const ps2_vu_prog_t * prog, int index, int force);

#endif // PS2_VU_PROG_MGR_H
