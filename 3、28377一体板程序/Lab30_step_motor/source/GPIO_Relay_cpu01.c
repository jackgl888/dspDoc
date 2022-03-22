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


#define StepA_ON   GpioDataRegs.GPASET.bit.GPIO16=1; //自定义步进机的四相GPIO口
#define StepA_OFF  GpioDataRegs.GPACLEAR.bit.GPIO16=1;
#define StepB_ON   GpioDataRegs.GPASET.bit.GPIO17=1;
#define StepB_OFF  GpioDataRegs.GPACLEAR.bit.GPIO17=1;
#define StepC_ON   GpioDataRegs.GPASET.bit.GPIO18=1;
#define StepC_OFF  GpioDataRegs.GPACLEAR.bit.GPIO18=1;
#define StepD_ON   GpioDataRegs.GPASET.bit.GPIO19=1;
#define StepD_OFF  GpioDataRegs.GPACLEAR.bit.GPIO19=1;

#define time      5000

#define PowerDwMotor()   CpuTimer0Regs.TCR.bit.TSS = 1;    //停止电机
#define PowerOnMotor()   CpuTimer0Regs.TCR.bit.TSS = 0;   //启动电机

void GPIO_Setup(void);
__interrupt void cpu_timer0_isr(void);

unsigned char sa=1; //步进机四相通电分配顺序

unsigned char direct_st=1;//步进机转动方向标志direct_st，1正向，0反向

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
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    EDIS;
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

        // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
        // 200MHz CPU Freq, 1 second Period (in uSeconds)
    ConfigCpuTimer(&CpuTimer0, 200, time);

        // To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
        // of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in F2837xS_cputimervars.h), the
        // below settings must also be updated.
    CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
       // CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
       // CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

        // Step 5. User specific code, enable interrupts:

        // Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
        // which is connected to CPU-Timer 1, and CPU int 14, which is connected
        // to CPU-Timer 2:
    IER |= M_INT1;
       // IER |= M_INT13;
      //  IER |= M_INT14;
    PieCtrlRegs.PIECTRL.bit.ENPIE=1;
        // Enable TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
// Enable global Interrupts and higher priority real-time debug events:
        EINT;  // Enable Global interrupt INTM
        ERTM;  // Enable Global realtime interrupt DBGM

        StepA_ON;  //步进机IO初始化
        DELAY_US(10);
        StepB_ON;
        DELAY_US(10);
        StepC_ON;
        DELAY_US(10);
        StepD_ON;
        DELAY_US(10);
        PowerOnMotor();
// Step 6. IDLE loop. Just sit and loop forever (optional):
    while(1)
    {

    }
}

void GPIO_Setup()
{
    GPIO_SetupPinMux(16, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(16, GPIO_OUTPUT, GPIO_INVERT);
    GPIO_SetupPinMux(17, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(17, GPIO_OUTPUT, GPIO_INVERT);
    GPIO_SetupPinMux(18, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(18, GPIO_OUTPUT, GPIO_INVERT);
    GPIO_SetupPinMux(19, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(19, GPIO_OUTPUT, GPIO_INVERT);
}

__interrupt void cpu_timer0_isr(void)
{
   CpuTimer0.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
   CpuTimer0Regs.TCR.bit.TIF=1; //
   CpuTimer0Regs.TCR.bit.TRB=1;

   switch(sa)
   {
      case 1: StepA_OFF; StepB_ON;  StepC_ON;  StepD_ON; break;//分配顺序1
      case 2: StepA_OFF; StepB_OFF; StepC_ON;  StepD_ON; break;//分配顺序2
      case 3: StepA_ON;  StepB_OFF; StepC_ON;  StepD_ON; break;//分配顺序3
      case 4: StepA_ON;  StepB_OFF; StepC_OFF; StepD_ON; break;//分配顺序4
      case 5: StepA_ON;  StepB_ON;  StepC_OFF; StepD_ON; break;//分配顺序5
      case 6: StepA_ON;  StepB_ON;  StepC_OFF; StepD_OFF;break;//分配顺序6
      case 7: StepA_ON;  StepB_ON;  StepC_ON;  StepD_OFF;break;//分配顺序7
      case 8: StepA_OFF; StepB_ON;  StepC_ON;  StepD_OFF;break;//分配顺序8
   }

   switch(direct_st) //判断步进机转动方向
   {
      case 1: if(sa==8) //正向
                 sa=1;
                else
                 sa++;  break;
      case 0: if(sa==1) //反向
                 sa=8;
                else
                 sa--;  break;
   }

}
