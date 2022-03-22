//###########################################################################
// FILE:    Example_2837xSI2c_eeprom.c
// TITLE:   I2C EEPROM Example
//
//! \addtogroup cpu01_example_list
//! <h1>I2C EEPROM Example (i2c_eeprom)</h1>
//!
//! This program will write 1-14 words to EEPROM and read them back.
//! The data written and the EEPROM address written to are contained
//! in the message structure, I2cMsgOut1. The data read back will be
//! contained in the message structure I2cMsgIn1.
//!
//! \b External \b Connections \n
//! - This program requires an external I2C EEPROM connected to
//!   the I2C bus at address 0x50.
////###########################################################################
// FILE:    Example_2837xDI2c_eeprom.c
// TITLE:   I2C EEPROM Example
//
//! \addtogroup cpu01_example_list
//! <h1>I2C EEPROM Example (i2c_eeprom)</h1>
//!
//! This program will write 1-14 words to EEPROM and read them back.
//! The data written and the EEPROM address written to are contained
//! in the message structure, I2cMsgOut1. The data read back will be
//! contained in the message structure I2cMsgIn1.
//!
//! \b External \b Connections \n
//! - This program requires an external I2C EEPROM connected to
//!   the I2C bus at address 0x50.
//
//###########################################################################
// $TI Release: F2837xD Support Library v190 $
// $Release Date: Mon Feb  1 16:51:57 CST 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2837xD_Ipc_drivers.h"


// Note: I2C Macros used in this example can be found in the
// F2837xD_I2C_defines.h file

// Prototype statements for functions found within this file.
void   I2CA_Init(void);

Uint16   dat1[]={0,0,0,0,0,0,0,0};
Uint16   dat[]={ 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};

Uint16 WriteData(Uint16 *Wdata, Uint16  RomAddress, Uint16  number)//从地址，数据要保存在设备中的地址
{
   Uint16 i;
   if (I2caRegs.I2CSTR.bit.BB == 1) //总线被占用
   {
      return I2C_BUS_BUSY_ERROR;
   }
   while(!I2C_xrdy());         //发送数据到设置的 EEPROM 地址
   I2caRegs.I2CSAR.all = 0x50;          //从地址
   I2caRegs.I2CCNT = number + 1;       //数据计数寄存器
   I2caRegs.I2CDXR.all = RomAddress;  //数据发送寄存器
   I2caRegs.I2CMDR.all = 0x6E20;     //配置模式：从机接收，发送起始信号，主机模式，发送，10位地址
   for (i=0; i<number; i++)
   {
      while(!I2C_xrdy());
      I2caRegs.I2CDXR.all = *Wdata; //数据发送寄存器
      Wdata++;
      if (I2caRegs.I2CSTR.bit.NACK == 1) //收到无应答信号
          return    I2C_BUS_BUSY_ERROR;
   }
   return I2C_SUCCESS;
}

Uint16 ReadData(Uint16  *RamAddr, Uint16    RomAddress, Uint16 number)
{
   Uint16  i,Temp;

   if (I2caRegs.I2CSTR.bit.BB == 1)      //总线被占用
   {
       return I2C_BUS_BUSY_ERROR;
   }
   while(!I2C_xrdy());
   I2caRegs.I2CSAR.all = 0x50;
   I2caRegs.I2CCNT = 1;
   I2caRegs.I2CDXR.all = RomAddress;
   I2caRegs.I2CMDR.all = 0x6620;
   if (I2caRegs.I2CSTR.bit.NACK == 1)
        return  I2C_BUS_BUSY_ERROR;
   DELAY_US(50);
   while(!I2C_xrdy());
   I2caRegs.I2CSAR.all = 0x50;
   I2caRegs.I2CCNT = number;
   I2caRegs.I2CMDR.all = 0x6C20;//主机模式，接收，7位
   if (I2caRegs.I2CSTR.bit.NACK == 1)//收到无应答信号，
        return  I2C_BUS_BUSY_ERROR;
   for(i=0;i<number;i++)
   {
      while(!I2C_rrdy());
      Temp = I2caRegs.I2CDRR.all;  //数据接收寄存器
      if (I2caRegs.I2CSTR.bit.NACK == 1)
          return    I2C_BUS_BUSY_ERROR;
      *RamAddr = Temp;
      RamAddr++;
   }
   return I2C_SUCCESS;
}

//Uint16  i;
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
    //    InitGpio(); // Skipped for this example
     //   GPIO_Setup();
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

// Step 4. Initialize the Device Peripherals:
   I2CA_Init();
   WriteData(dat,0x60,8);
   DELAY_US(10000);
   ReadData(dat1,0x60,8);
   // Application loop
  for(;;)
   {

   }   // end of for(;;)
}   // end of main






//===========================================================================
// No more.
//===========================================================================
