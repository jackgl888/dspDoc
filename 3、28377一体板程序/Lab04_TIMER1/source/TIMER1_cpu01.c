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
__interrupt void cpu_timer1_isr(void);

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
    // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.TIMER1_INT = &cpu_timer1_isr;
    EDIS;
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

        // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
        // 200MHz CPU Freq, 1 second Period (in uSeconds)
    ConfigCpuTimer(&CpuTimer1, 200, 1000000);

        // To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
        // of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in F2837xS_cputimervars.h), the
        // below settings must also be updated.
        // CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
        CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
        // CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

        // Step 5. User specific code, enable interrupts:

        // Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
        // which is connected to CPU-Timer 1, and CPU int 14, which is connected
        // to CPU-Timer 2:
        //IER |= M_INT1;
       IER |= M_INT13;
      //  IER |= M_INT14;

        // Enable TINT0 in the PIE: Group 1 interrupt 7
    //PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
// Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
    while(1)
    {

    }
}

void GPIO_Setup()
{
    GPIO_SetupPinMux(0, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(0, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(1, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(1, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(2, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(2, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(3, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(3, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(4, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(4, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(5, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(5, GPIO_OUTPUT, GPIO_PUSHPULL);
}

__interrupt void cpu_timer1_isr(void)
{
   CpuTimer1.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

   GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO4 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO5 = 1;
}
