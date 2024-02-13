#include <kernel.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <stdio.h>
#include <tamtypes.h>

#include <dma.h>
#include <libgs.h>
#include <libpad.h>

static void
loadModules(void)
{
    int ret;

    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
    if (ret < 0)
    {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }

    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
    if (ret < 0)
    {
        printf("sifLoadModule pad failed: %d\n", ret);
        SleepThread();
    }
}

static char padBuf[256] __attribute__((aligned(64)));
static char actAlign[6];
static int actuators;

static int waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1))
    {
        if (state != lastState)
        {
            padStateInt2String(state, stateString);
            printf("Please wait, pad(%d,%d) is in state %s\n",
                   port, slot, stateString);
        }
        lastState = state;
        state = padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1)
    {
        printf("Pad OK!\n");
    }
    return 0;
}

static int
initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    printf("The device has %d modes\n", modes);

    if (modes > 0)
    {
        printf("( ");
        for (i = 0; i < modes; i++)
        {
            printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
        }
        printf(")");
    }

    printf("It is currently using mode %d\n",
           padInfoMode(port, slot, PAD_MODECURID, 0));

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0)
    {
        printf("This is a digital controller?\n");
        return 1;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do
    {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes)
    {
        printf("This is no Dual Shock controller\n");
        return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0)
    {
        printf("This is no Dual Shock controller??\n");
        return 1;
    }

    printf("Enabling dual shock functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    waitPadReady(port, slot);
    printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);
    printf("# of actuators: %d\n", actuators);

    if (actuators != 0)
    {
        actAlign[0] = 0; // Enable small engine
        actAlign[1] = 1; // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);
        printf("padSetActAlign: %d\n",
               padSetActAlign(port, slot, actAlign));
    }
    else
    {
        printf("Did not find any actuators.\n");
    }

    waitPadReady(port, slot);

    return 1;
}

static void initPad(int port, int slot)
{
    SifInitRpc(0);
    loadModules();

    padInit(0);
    int ret = padPortOpen(port, slot, padBuf);
    if (ret == 0)
    {
        printf("padOpenPort failed: %d\n", ret);
        SleepThread();
    }

    if (!initializePad(port, slot))
    {
        printf("pad initalization failed!\n");
        SleepThread();
    }
}

static int readPad(int port, int slot, struct padButtonStatus * buttons)
{
    int ret = padGetState(port, slot);
    while ((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1))
    {
        if (ret == PAD_STATE_DISCONN)
        {
            printf("Pad(%d, %d) is disconnected\n", port, slot);
        }
        ret = padGetState(port, slot);
    }

    ret = padRead(port, slot, buttons); // port, slot, buttons
    return ret;
}

#define GS_CSR (volatile u64 *)0x12001000

#define GS_RESET() *GS_CSR = ((u64)(1) << 9)

void initVid_480P()
{
    GS_RESET();

    int width = 704;
    int height = 480;
    int StartX = 232;
    int StartXOffset = 0;
    int StartY = 35;
    int StartYOffset = 0;
    int DW = 1440;
    int DH = 480;
    int MagH = (DW / width) - 1;  // multiple of the screen width
    int MagV = (DH / height) - 1; // multiple of the screen height

    // Calculate the actual display width and height
    DW = (MagH + 1) * width;
    DH = (MagV + 1) * height;

    // Keep the framebuffer in the center of the screen
    StartX += (DW - ((MagH + 1) * width)) / 2;
    StartY += (DH - ((MagV + 1) * height)) / 2;

    printf("Screen Mode:\n");
    printf("------------\n");
    printf("Width : %d\n", width);
    printf("Height: %d\n", height);
    printf("StartX: %d\n", StartX);
    printf("StartY: %d\n", StartY);
    printf("MagH  : %d\n", MagH);
    printf("MagV  : %d\n", MagV);
    printf("DW    : %d\n", DW);
    printf("DH    : %d\n", DH);

    __asm__("sync.p; nop;");

    *GS_CSR = 0x00000000; // Clean CSR registers

    GsPutIMR(0x00007F00); // Masks all interrupts

    SetGsCrt(0, GS_MODE_DTV_480P, GS_FFMD_FRAME);
    DIntr();

    GS_SET_PMODE(0,     // Read Circuit 1
                 1,     // Read Circuit 2
                 0,     // Use ALP Register for Alpha Blending
                 1,     // Alpha Value of Read Circuit 2 for Output Selection
                 0,     // Blend Alpha with output of Read Circuit 2
                 0x80); // Alpha Value = 1.0

    GS_SET_DISPFB1(0,             // Frame Buffer Base Pointer (Address/2048)
                   width / 64,    // Buffer Width (Address/64)
                   GS_PIXMODE_32, // Pixel Storage Format
                   0,             // Upper Left X in Buffer
                   0);

    GS_SET_DISPFB2(0,             // Frame Buffer Base Pointer (Address/2048)
                   width / 64,    // Buffer Width (Address/64)
                   GS_PIXMODE_32, // Pixel Storage Format
                   0,             // Upper Left X in Buffer
                   0);            // Upper Left Y in Buffer

    GS_SET_DISPLAY1(
    StartX + StartXOffset, // X position in the display area (in VCK unit
    StartY + StartYOffset, // Y position in the display area (in Raster u
    MagH,                  // Horizontal Magnification
    MagV,                  // Vertical Magnification
    DW - 1,                // Display area width
    DH - 1);               // Display area height

    GS_SET_DISPLAY2(
    StartX + StartXOffset, // X position in the display area (in VCK unit
    StartY + StartYOffset, // Y position in the display area (in Raster u
    MagH,                  // Horizontal Magnification
    MagV,                  // Vertical Magnification
    DW - 1,                // Display area width
    DH - 1);               // Display area height

    GS_SET_BGCOLOR(0x00,  // Red
                   0x80,  // Green
                   0x80); // Blue

    EIntr();

    int offset_x = 2048 - width / 2;
    int offset_y = 2048 - height / 2;
    GsSetXYOffset1(offset_x << 4, offset_y << 4);
    GsSetScissor1(0, 0, width, height);
    int fbw = (width + 63) / 64;
    GsSetFrame1(0, fbw, GS_PIXMODE_32, 0);

    GsOverridePrimAttributes(GS_DISABLE, 0, 0, 0, 0, 0, 0, 0, 0);

    // Set transparency settings for context 1 (Refer to documentation on TEST and TEXA registers).
    // Alpha test = enabled, pass if >= alpha reference, alpha reference = 1, fail method = no update
    GsEnableAlphaTransparency1(GS_ENABLE, GS_ALPHA_GEQUAL, 0x01, GS_ALPHA_NO_UPDATE);
    // Enable global alpha blending
    GsEnableAlphaBlending1(GS_ENABLE);

    // set transparency settings for context 2 (Refer to documentation on TEST and TEXA registers).
    // Alpha test = enabled, pass if >= alpha reference, alpha reference = 1, fail method = no update
    GsEnableAlphaTransparency2(GS_ENABLE, GS_ALPHA_GEQUAL, 0x01, GS_ALPHA_NO_UPDATE);
    // Enable global alpha blending
    GsEnableAlphaBlending2(GS_ENABLE);
}

#define GIF_PACKET_MAX 10
static GS_GIF_PACKET packets[GIF_PACKET_MAX];
static GS_PACKET_TABLE giftable;

void drawSprite()
{
    QWORD * p = (QWORD *)UNCACHED_SEG(GsGifPacketsAlloc(&giftable, 5)); // Allocate 5 qword for 1 untextured sprite

    /*	For this GIF packet, the EOP flag is set to 1.
            Rightfully, it should only be set for only the final packet so that the GIF knows when it can safely switch paths,
            but to keep things simple, it's set to 1 for every packet.

            The packets are all in the PACKED format.	*/
    int xpos = 2048;
    int ypos = 2048;
    gs_setGIF_TAG(((GS_GIF_TAG *)&p[0]), 4, 1, 0, 0, GS_GIF_PACKED, 1, gif_rd_ad);
    gs_setR_PRIM(((GS_R_PRIM *)&p[1]), GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0);
    gs_setR_RGBAQ(((GS_R_RGBAQ *)&p[2]), 0xe0, 0, 0, 0x80, 0);
    gs_setR_XYZ2(((GS_R_XYZ *)&p[3]), xpos << 4, xpos << 4, 0x00000000);
    gs_setR_XYZ2(((GS_R_XYZ *)&p[4]), (xpos + 20) << 4, (ypos + 20) << 4, 0x00000000);
}

int main(int argc, char * argv[])
{
    giftable.packet_count = GIF_PACKET_MAX;
    giftable.packets = packets;

    dma_reset();

    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);

    struct padButtonStatus buttons;

    int port = 0;
    int slot = 0;

    initPad(port, slot);
    initVid_480P();
    u32 old_pad = 0;
    while (1)
    {

        int padret = readPad(port, slot, &buttons);
        if (padret != 0)
        {
            u32 paddata = 0xffff ^ buttons.btns;

            u32 new_pad = paddata & ~old_pad;
            old_pad = paddata;

            // Directions
            if (new_pad & PAD_LEFT)
            {
                printf("LEFT\n");
            }
            if (new_pad & PAD_DOWN)
            {
                printf("DOWN\n");
            }
            if (new_pad & PAD_RIGHT)
            {
                printf("RIGHT\n");
            }
            if (new_pad & PAD_UP)
            {
                printf("UP\n");
            }
        }

        GsGifPacketsClear(&giftable);
        drawSprite();
        GsDrawSync(0);
        GsVSync(0);
        GsGifPacketsExecute(&giftable, 1);
    }
    return 0;
}
