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
void SEG_DATA(unsigned char d);
interrupt void xint1_isr(void);
interrupt void xint2_isr(void);
interrupt void xint3_isr(void);
interrupt void xint4_isr(void);
void ExintInit();

#define   SBIT3     GpioDataRegs.GPBDAT.bit.GPIO58
#define   SBIT2     GpioDataRegs.GPBDAT.bit.GPIO59
#define   SBIT1     GpioDataRegs.GPBDAT.bit.GPIO60
#define   SBIT0     GpioDataRegs.GPBDAT.bit.GPIO61

unsigned char const table[]={//共阴极0~f数码管编码
0xfc,0x60,0xda,0xf2,     //0~3
0x66,0xb6,0xbe,0xe0,    //4~7
0xfe,0xf6
};

Uint16 InterruptCount1 = 0,InterruptCount2 = 0,InterruptCount3 = 0,InterruptCount4 = 0;
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
    ExintInit();
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
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.XINT1_INT = &xint1_isr;
    PieVectTable.XINT2_INT = &xint2_isr;
    PieVectTable.XINT3_INT = &xint3_isr;
    PieVectTable.XINT4_INT = &xint4_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

    // Step 4. User specific code, enable interrupts:

    // Enable XINT1 and XINT2 in the PIE: Group 1 interrupt 4 & 5
    // Enable INT1 which is connected to WAKEINT:
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Group 1 INT4
    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Group 1 INT5
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          // Enable PIE Group 12 INT1
    PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // Enable PIE Group 12 INT2
    IER |= M_INT1;
    IER |= M_INT12;

// Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
    SBIT0=0;DELAY_US(1);
    SBIT1=0;DELAY_US(1);
    SBIT2=0;DELAY_US(1);
    SBIT3=0;DELAY_US(1);

    SEG_DATA(table[0]);
    while(1)
    {

        SEG_DATA(table[InterruptCount2]);
        SBIT0=0;DELAY_US(1);
        SBIT2=1;DELAY_US(1);
        SBIT1=1;DELAY_US(1);
        SBIT3=1;
        DELAY_US(100);
        SBIT0=1;
        SEG_DATA(0);

        SEG_DATA(table[InterruptCount1]);
        SBIT1=0;DELAY_US(1);
        SBIT2=1;DELAY_US(1);
        SBIT3=1;DELAY_US(1);
        SBIT0=1;
        DELAY_US(100);
        SBIT1=1;
        SEG_DATA(0);

        SEG_DATA(table[InterruptCount4]);
        SBIT2=0;DELAY_US(1);
        SBIT1=1; DELAY_US(1);
        SBIT3=1;DELAY_US(1);
        SBIT0=1;
        DELAY_US(100);
        SBIT2=1;
        SEG_DATA(0);

        SEG_DATA(table[InterruptCount3]);
        SBIT3=0;DELAY_US(1);
        SBIT1=1;DELAY_US(1);
        SBIT2=1;DELAY_US(1);
        SBIT0=1;
        DELAY_US(100);
        SBIT3=1;
        SEG_DATA(0);
    }
}

void GPIO_Setup()
{
    GPIO_SetupPinMux(58, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(58, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(59, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(59, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(60, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(60, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(61, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(61, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(77, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(77, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(78, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(78, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(79, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(79, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(80, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(80, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(81, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(81, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(82, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(82, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(83, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(83, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(85, GPIO_OUTPUT, GPIO_PUSHPULL);
}

void ExintInit()
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 1;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 1;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 1;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 1;        // XINT1 Synch to SYSCLKOUT only
    EDIS;
    GPIO_SetupXINT1Gpio(12);    // GPIO12 is XINT1
    XintRegs.XINT1CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1
    GPIO_SetupXINT2Gpio(13);    // GPIO13 is XINT2
    XintRegs.XINT2CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT2CR.bit.ENABLE = 1;        // Enable XINT1
    GPIO_SetupXINT3Gpio(14);    // GPIO14 is XINT3
    XintRegs.XINT3CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT3CR.bit.ENABLE = 1;        // Enable XINT1
    GPIO_SetupXINT4Gpio(15);
    XintRegs.XINT4CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT4CR.bit.ENABLE = 1;        // Enable XINT1
}

void SEG_DATA(unsigned char d)
{
    Uint32 dat1 = 0,dat2 = 0;
     dat1 = d;
     dat1 <<= 13;
     dat2 = d;
     dat2 <<= 13;
     dat2 &= 0x100000;
     dat2 <<= 1;
     dat1 |= dat2;
     GpioDataRegs.GPCDAT.all = dat1;
}

interrupt void xint1_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO12 == 0) InterruptCount1++;
    if(InterruptCount1 > 9) InterruptCount1 = 0;

}

interrupt void xint2_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO13 == 0) InterruptCount2++;
    if(InterruptCount2 > 9) InterruptCount2 = 0;

}

interrupt void xint3_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO14 == 0) InterruptCount3++;
    if(InterruptCount3 > 9) InterruptCount3 = 0;

}

interrupt void xint4_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO15 == 0) InterruptCount4++;
    if(InterruptCount4 > 9) InterruptCount4 = 0;

}
