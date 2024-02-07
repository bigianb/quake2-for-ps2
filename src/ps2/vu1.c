
/* ================================================================================================
 * -*- C -*-
 * File: vu1.c
 * Author: Guilherme R. Lampert
 * Created on: 17/01/16
 * Brief: Vector Unit 1 (VU1) microcode upload and VU management.
 *
 * This source code is released under the GNU GPL v2 license.
 * Check the accompanying LICENSE file for details.
 * ================================================================================================ */
#include <tamtypes.h>

#include "ps2/vu1.h"
#include "ps2/mem_alloc.h"
#include "ps2/defs_ps2.h"
#include "game/q_shared.h" // For qboolean and stuff...

#include <vif_registers.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>

void VU1_Init(void)
{
    dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);
}

// TODO: allocate once and just reset them when switching.
static packet2_t *buildingPacket = NULL;
static packet2_t *sendingPacket = NULL;

void VU1_Shutdown(void)
{
    if (buildingPacket){
        packet2_free(buildingPacket);
        buildingPacket = NULL;
    }
    if (sendingPacket){
        packet2_free(sendingPacket);
        sendingPacket = NULL;
    }
}

void VU1_UploadProg(void * vu1_code_start, void * vu1_code_end)
{
    // + 1 for end tag
	u32 packet_size = packet2_utils_get_packet_size_for_program(&vu1_code_start, &vu1_code_end) + 1; 
	packet2_t *packet2 = packet2_create(packet_size, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_vif_add_micro_program(packet2, 0, &vu1_code_start, &vu1_code_end);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

#define MAX_PACKET_SIZE_QW 1024 * 1000

void VU1_Begin(void)
{
    if (buildingPacket){
        packet2_free(buildingPacket);
        buildingPacket = NULL;
    }
    buildingPacket = packet2_create(MAX_PACKET_SIZE_QW, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
}

void VU1_End(int start)
{
    if (start >= 0)
    {
        packet2_utils_vu_add_start_program(buildingPacket, start);
	    packet2_utils_vu_add_end_tag(buildingPacket);
    }
    packet2_utils_vu_add_end_tag(buildingPacket);

    // Wait for previous transfer to complete if not yet:
    dma_channel_wait(DMA_CHANNEL_VIF1, 0);

    if (sendingPacket){
        packet2_free(sendingPacket);
        sendingPacket = NULL;
    }
    sendingPacket = buildingPacket;
    buildingPacket = NULL;
    dma_channel_send_packet2(sendingPacket, DMA_CHANNEL_VIF1, 1);
}

void VU1_ListAddBegin(int address)
{
    packet2_utils_vu_open_unpack(buildingPacket, address, 0);
}

void VU1_ListAddEnd(void)
{
    packet2_utils_vu_close_unpack(buildingPacket);
}

void VU1_ListData(int dest_address, void * data, int quad_size)
{
    packet2_utils_vu_add_unpack_data(buildingPacket, dest_address, data, quad_size, 0);
}

void VU1_ListAdd128(u64 v1, u64 v2)
{
    packet2_add_2x_s64(buildingPacket, v1, v2);
}

u64 * VU1_ListAddGIFTag(void)
{
    u64* pGifTag = (u64*)buildingPacket->next;
    VU1_ListAdd128(0, 0);

   // TODO: FIXME this is ugly
   return pGifTag;
}

void VU1_ListAdd32(u32 v)
{
    packet2_add_u32(buildingPacket, v);
}

void VU1_ListAddFloat(float v)
{
    packet2_add_float(buildingPacket, v);
}
