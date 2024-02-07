
/* ================================================================================================
 * -*- C -*-
 * File: main_ps2.c
 * Author: Guilherme R. Lampert
 * Created on: 26/10/15
 * Brief: Main application entry point for QPS2.
 *
 * This source code is released under the GNU GPL v2 license.
 * Check the accompanying LICENSE file for details.
 * ================================================================================================ */

#include "common/q_common.h"

// An artificial argv[] param for Qcommon_Init:
static char * ps2_argv[] = { "QPS2.ELF", NULL };

/*
================
PS2 main():
================
*/
int main(void)
{
    FS_SetDefaultBasePath(""); 

    // PS2 main() takes no arguments so we
    // fake a default program name argv[].
    Qcommon_Init(1, ps2_argv);

    int time    = 0;
    int newtime = 0;
    int oldtime = Sys_Milliseconds();

    for (;;)
    {
        do
        {
            newtime = Sys_Milliseconds();
            time = newtime - oldtime;
        } while (time < 1);

        Qcommon_Frame(time);
        oldtime = newtime;
    }
    
    Sys_Quit();
}
