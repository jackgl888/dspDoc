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
#define   LCD_DATA_BUS  (*((volatile  Uint16 *)0x380000))
#define   EN    GpioDataRegs.GPBDAT.bit.GPIO56
#define   RW    GpioDataRegs.GPBDAT.bit.GPIO57
#define   RS    GpioDataRegs.GPBDAT.bit.GPIO53
#define   uchar     unsigned char
extern uchar menu1[]={"  南京研旭欢迎你"};
extern uchar menu2[]={"祝你使用愉快"};
extern uchar menu3[]={"请进行你的操作"};
extern uchar menu4[]={"谢谢"};
void LCD_DATA(uchar d);
void LCDInit(void);
void Write_order(Uint16 order);
void Write_data(Uint16 data);
void delay(Uint16 t);
void configio(void);
void display(uchar *hz);
Uint16  ErrCount = 0;
extern void InitEmif1(void);
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
    InitEmif1();
    RS=0;
    delay(5);
    RW=0;
    delay(5);
    EN=0;
    LCDInit();
    delay(5);
    Write_order(0x01);
    delay(50);
    Write_order(0x80);
    delay(5);
    display(menu1);

    Write_order(0x91);
    delay(5);
    display(menu2);

    Write_order(0x89);
    delay(5);
    display(menu3);

    Write_order(0x99);
    delay(5);
    display(menu4);
    while(1)
    {
    }
}

void GPIO_Setup()
{
    GPIO_SetupPinMux(53, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(53, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(56, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(56, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(57, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(57, GPIO_OUTPUT, GPIO_PUSHPULL);
}

void LCD_DATA(uchar d)
{
  LCD_DATA_BUS = d;
}
void LCDInit(void)
{
  DELAY_US(5000);
  Write_order(0x30);// 功能设定：基本指令动作
  DELAY_US(50);
  Write_order(0x0c);//显示状态：开
  DELAY_US(50);
  Write_order(0x01);//清屏
  DELAY_US(60);
  Write_order(0x02);//地址归位：AC=0x00
  DELAY_US(40);
}

void Write_order(Uint16 order)
{
   RS=0;delay(1);
   RW=0;delay(1);
   EN=1;delay(1);
   LCD_DATA(order);
   //delay(1);
   EN=0;delay(1);
}

void Write_data(Uint16 data)
{
   RS=1;delay(1);
   RW=0;delay(1);
   EN=1;delay(1);
   LCD_DATA(data);
   //delay(1);
   EN=0;delay(1);
}

void delay(Uint16 t)
{
   Uint16 i;
   while(t--)
    {
      for(i=0;i<500;i++);
    }
}

void display(uchar *hz)
{
   while(*hz!='\0')
    {
       Write_data(*hz);
       hz++;
       DELAY_US(250);
    }
}
