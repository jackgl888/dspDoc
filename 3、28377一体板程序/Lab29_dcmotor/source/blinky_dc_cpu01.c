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

void GPIO_Setup(void);

#define DCA_ON     GpioDataRegs.GPCSET.bit.GPIO66=1  //自定义直流电机的两相GPIO口
#define DCA_OFF    GpioDataRegs.GPCCLEAR.bit.GPIO66=1
#define DCB_ON     GpioDataRegs.GPESET.bit.GPIO133=1
#define DCB_OFF    GpioDataRegs.GPECLEAR.bit.GPIO133=1

unsigned char direct_dc=0;//直流电机转动方向标志，1正向，0反向


void main(void)
{
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

// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
    InitGpio(); // Skipped for this example
    GPIO_Setup();
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

// Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
    DCA_ON;
    DELAY_US(10);
    DCB_OFF;
    DELAY_US(10);
    switch(direct_dc)
    {
      case 1: DCA_ON;DCB_OFF;break;
      case 0: DCA_OFF;DCB_ON;break;
    }

    for(; ;)
    {
     }
}

void GPIO_Setup()
{
    GPIO_SetupPinMux(66, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(66, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(133, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(133, GPIO_OUTPUT, GPIO_PUSHPULL);

}
