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

unsigned    int  *FlashStart = (unsigned  int *)0x100000;//flash的首地址

#define     TimeOutErr               1
#define     VerifyErr                2
#define     WriteOK                  0
#define     EraseErr                 3
#define     EraseOK                  0
#define     SectorSize               0x800
#define     BlockSize                0x8000

Uint16  ChipErase(void);
Uint16 FLASHTEST(Uint16 Length);
extern void InitEmif1(void);
Uint16  ErrCount = 0;

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

    ChipErase();
    asm(" ESTOP0");
    FLASHTEST(0xFFFF);
    asm(" ESTOP0");
    while (1);
}

Uint16  ChipErase(void)
{
     Uint16  Data;
     Uint32  TimeOut,i;
     *(FlashStart + 0x5555) = 0xAAAA;
     *(FlashStart + 0x2AAA) = 0x5555;
     *(FlashStart + 0x5555) = 0x8080;
     *(FlashStart + 0x5555) = 0xAAAA;
     *(FlashStart + 0x2AAA) = 0x5555;
     *(FlashStart + 0x5555) = 0x1010;
      i = 0;
      TimeOut = 0;
      while(i<5)
      {
          Data = *(FlashStart +  0x3FFFF);
          if  (Data == 0xFFFF)    i++;
          else    i=0;
          if ( ++TimeOut>0x1000000)   return (TimeOutErr);
      }
      for (i=0;i<0x40000;i++)
      {
          Data = *(FlashStart + i);
          if (Data !=0xFFFF)  return (EraseErr);
      }
      return  (EraseOK);

}
Uint16 FLASHTEST(Uint16 Length)
{
     Uint32  i,TimeOut;
     Uint16  Data1,Data2,j,t;
     for (i=0;i<Length;i++)  //写部分
     {
           *(FlashStart + 0x5555) = 0x00AA;
           *(FlashStart + 0x2AAA) = 0x0055;
           *(FlashStart + 0x5555) = 0x00A0;
           *(FlashStart +  i) =  i;
           TimeOut = 0;
           j=0;
           while(j<5)
           {
              Data1 = *(FlashStart +  i);
              Data2 = *(FlashStart +  i);
              if  (Data1 == Data2)    j++;
              else    j=0;
              if ( ++TimeOut>0x1000000)   return (TimeOutErr);//flash写自然数无响应
           }
      }

      for(i=0;i<Length;i++)
      {
           t =  *(FlashStart  +i);
           if(t!=i)
           return (EraseErr);
      }
      return  (WriteOK);

}

