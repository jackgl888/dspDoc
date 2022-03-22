//###########################################################################
// FILE:   blinky_cpu01.c
// TITLE:  LED Blink Example for F2837xD.
//
//! \addtogroup dual_example_list
//! <h1> Blinky </h1>
//!
//! Dual Core Blinky Example.  This example demonstrates how to implement
//! and run a standalone application on both cores.
//
//###########################################################################
// $TI Release: F2837xD Support Library v190 $
// $Release Date: Mon Feb  1 16:51:57 CST 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2837xD_Ipc_drivers.h"

Uint16    *ExRamStart = (Uint16 *)0x300000;
extern void InitEmif1(void);
Uint16  ErrCount = 0;

void main(void)
{
    Uint16 i=0;
 // Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xD_SysCtrl.c file.
   InitSysCtrl();

#ifdef _STANDALONE
#ifdef _FLASH
// Send boot command to allow the CPU2 application to begin execution
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
// Send boot command to allow the CPU2 application to begin execution
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);
#endif
#endif

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
#ifdef _FLASH
   InitFlash();
#endif

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
    DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xD_PieCtrl.c file.
    InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
    InitPieVectTable();

    InitEmif1();

    //////////////////////////////////////////////

    for(i = 0; i < 0xFFFF; i++)
    {
        *(ExRamStart + i) = 0x5555;
        if(*(ExRamStart + i) != 0x5555)
        {
           while(1);
        }
    }
    asm(" ESTOP0");
    for(i = 0; i < 0xFFFF; i++)
    {
       *(ExRamStart + i) = 0xAAAA;
       if(*(ExRamStart + i) != 0xAAAA)
       {
          while(1);
       }
    }
    asm(" ESTOP0");
    for(i = 0; i< 0xFFFF; i++)
    {
       *(ExRamStart + i) = i;
       if(*(ExRamStart + i) != i)
       {
          while(1);
       }
       if(*ExRamStart == 0x4000)
       {
          while(1);
       }
    }

    asm(" ESTOP0");
    while (1);
}

