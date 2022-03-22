//###########################################################################
//
// FILE:   F2837xD_I2C.c
//
// TITLE:  F2837xD I2C Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2837xD Support Library v190 $
// $Release Date: Mon Feb  1 16:51:57 CST 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F2837xD_device.h"     // F2837xD Headerfile Include File
#include "F2837xD_Examples.h"   // F2837xD Examples Include File


//---------------------------------------------------------------------------
// Example: I2cAGpioConfig(), I2cBGpioConfig()
//---------------------------------------------------------------------------
// These functions configures GPIO pins to function as I2C pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.
//

#ifdef CPU1
//************************************************************************************
// 'I2caDataClkPin' should be assign with one of the possible I2C_A SDA - SDL GPIO pin
// Use defined Macros from "F2837xD_I2c_defines.h" for  assignment
//************************************************************************************
void I2CA_Init(void)
{
   // Initialize I2C
    InitI2CGpio();
    I2caRegs.I2CMDR.bit.IRS =0;
//   I2caRegs.I2CSAR.all = 0x0050;      // Slave address - EEPROM control code

   I2caRegs.I2CPSC.all = 9;         // Prescaler - need 7-12 Mhz on module clk
   I2caRegs.I2CCLKL = 90;           // NOTE: must be non zero
   I2caRegs.I2CCLKH = 90;           // NOTE: must be non zero
   I2caRegs.I2CIER.all = 0x00;      // Enable SCD & ARDY __interrupts

   I2caRegs.I2CIER.all = 0x24;
   I2caRegs.I2CFFTX.all = 0x0000;
   I2caRegs.I2CMDR.bit.IRS = 1;
//   I2caRegs.I2CMDR.all = 0x0020;  // Take I2C out of reset
                                    // Stop I2C when suspended

  // I2caRegs.I2CFFTX.all = 0x6000; // Enable FIFO mode and TXFIFO
   //I2caRegs.I2CFFRX.all = 0x2040; // Enable RXFIFO, clear RXFFINT,

   return;
}

void InitI2CGpio()
{
// For this example, only init the pins for the SCI-A port.
// These functions are found in the F2837xS_Gpio.c file.
//   GPIO_SetupPinMux(32, GPIO_MUX_CPU1, 1);
//   GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 1);
EALLOW;
    /* Enable internal pull-up for the selected I2C pins */
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;

    /* Set qualification for the selected I2C pins */
    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;

    /* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/

    GpioCtrlRegs.GPBGMUX1.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;//SDAA
    GpioCtrlRegs.GPBGMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;//SCLA

    EDIS;
}

Uint16 I2CA_WriteData(Uint16 slaveaddr,Uint16 addr,Uint16 len,unsigned char * str)
{
   Uint16 i;

   if (I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;
   }

   // Setup slave address
   I2caRegs.I2CSAR.all = slaveaddr;

   // Check if bus busy
   if (I2caRegs.I2CSTR.bit.BB == 1)
   {
      return I2C_BUS_BUSY_ERROR;
   }

   // Setup number of bytes to send
   // MsgBuffer + Address
   I2caRegs.I2CCNT = len+2;

   // Setup data to send
   I2caRegs.I2CDXR.all = addr>>8;
   I2caRegs.I2CMDR.all = 0x6E20;
   while(!I2C_xrdy());
   I2caRegs.I2CDXR.all = addr & 0xff;

   for (i=0; i<len; i++)

   {
      while(!I2C_xrdy());
      I2caRegs.I2CDXR.all = *(str+i);
      if (I2caRegs.I2CSTR.bit.NACK == 1)
      return    I2C_BUS_BUSY_ERROR;
   }

   // Send start as master transmitter

   return I2C_SUCCESS;
}
//////////////////////////////////////
Uint16 Writetoeepromstr(Uint16 addr,Uint16 len,unsigned char * str)
{
 Uint16 i=0,j=0;
 j=len/8;
 if(j==0)
 {
  for(i=0;i<1;i++)
  {
    I2CA_WriteData(0x50,addr+i,len,str+i);
    DELAY_US(10000);//必须等待5MS，EEPROM规定连续写间隔
  }
 }
 else
 {
     for(i=0;i<j;i++)
     {
      I2CA_WriteData(0x50,addr+i*8,8,str+i*8);
      DELAY_US(10000);//必须等待5MS，EEPROM规定连续写间隔
     }
  }
 return 1;
}

Uint16 I2CA_ReadData(Uint16 slaveaddr,Uint16 addr,Uint16 len,unsigned char * str)
{
   Uint16 i;
   if (I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;
   }

   I2caRegs.I2CSAR.all = slaveaddr;

   if (I2caRegs.I2CSTR.bit.BB == 1)
    {
         return I2C_BUS_BUSY_ERROR;
    }
      I2caRegs.I2CCNT = 2;
      I2caRegs.I2CDXR.all = addr >>8;
      I2caRegs.I2CMDR.all = 0x6620;         // Send data to setup RTC address
      while(!I2C_xrdy());
      I2caRegs.I2CDXR.all = addr&0xff;
   if (I2caRegs.I2CSTR.bit.NACK == 1)
   return   I2C_BUS_BUSY_ERROR;
   while(!I2C_xrdy());

   I2caRegs.I2CSAR.all = slaveaddr;
   I2caRegs.I2CCNT = len;
   I2caRegs.I2CMDR.all = 0x6C20;
   if (I2caRegs.I2CSTR.bit.NACK == 1)
        return  I2C_BUS_BUSY_ERROR;
   for(i=0;i<len;i++)
   {
      while(!I2C_rrdy());
      *(str + i) = I2caRegs.I2CDRR.all;
      if (I2caRegs.I2CSTR.bit.NACK == 1)
          return    I2C_BUS_BUSY_ERROR;
   }
   DELAY_US(200);
   return I2C_SUCCESS;
}


Uint16  I2C_xrdy()
{
    Uint16  t;
    t = I2caRegs.I2CSTR.bit.XRDY;
    return t;
}

Uint16  I2C_rrdy()
{
    Uint16  t;
    t = I2caRegs.I2CSTR.bit.RRDY;
    return t;
}

void ReadfromeepromStr(Uint16 addr,Uint16 len,unsigned char * str)
{
 Uint16 i=0,j=0;
 j=len/8;
 if(j==0)
 {
 for(i=0;i<1;i++)
 I2CA_ReadData(0x50,addr+i,len,str+i);
 }
 else
 {
 for(i=0;i<=j;i++)
 {
  I2CA_ReadData(0x50,addr+i*8,8,str+i*8);
   DELAY_US(20);
  }
 }

}


void I2cAGpioConfig(Uint16 I2caDataClkPin)
{
	EALLOW;

	switch(I2caDataClkPin)
	{
		case I2C_A_GPIO0_GPIO1:
			/* Enable internal pull-up for the selected I2C pins */
			GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;     // Enable pull-up for GPIO0 (SDAA)
			GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;     // Enable pull-up for GPIO1 (SDLA)
		
			/* Set qualification for the selected I2C pins */
			GpioCtrlRegs.GPAQSEL1.bit.GPIO0 = 3;   // Async/no qualification (I/ps sync
			GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 3;   // to SYSCLKOUT by default)

			/* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/
			GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 1;    // Configure GPIO0 for SDAA operation
			GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 2;    // Configure GPIO0 for SDAA operation
		
			GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 1;    // Configure GPIO1 for SDLA operation
			GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 2;    // Configure GPIO1 for SDLA operation
	
			break;
		
		
		case I2C_A_GPIO32_GPIO33:
			/* Enable internal pull-up for the selected I2C pins */
			GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;
			GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;
			
			/* Set qualification for the selected I2C pins */
			GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;
			GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;
			
			/* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/
			GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;
			GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;

			break;
			
		case I2C_A_GPIO42_GPIO43:
			/* Enable internal pull-up for the selected I2C pins */
			
			/* Set qualification for the selected I2C pins */
			
			/* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/
			break;
			
		case I2C_A_GPIO91_GPIO92:
			/* Enable internal pull-up for the selected I2C pins */
			GpioCtrlRegs.GPCPUD.bit.GPIO91 = 0;
			GpioCtrlRegs.GPCPUD.bit.GPIO92 = 0;
			
			/* Set qualification for the selected I2C pins */
	   		GpioCtrlRegs.GPCQSEL2.bit.GPIO91 = 3;
	   		GpioCtrlRegs.GPCQSEL2.bit.GPIO92 = 3;
			
			/* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/

	   		GpioCtrlRegs.GPCGMUX2.bit.GPIO91 = 1;	GpioCtrlRegs.GPCMUX2.bit.GPIO91 = 2;
	   		GpioCtrlRegs.GPCGMUX2.bit.GPIO92 = 1;	GpioCtrlRegs.GPCMUX2.bit.GPIO92 = 2;
			break;
			
		case I2C_A_GPIO63104_GPIO105:
			/* Enable internal pull-up for the selected I2C pins */
			
			/* Set qualification for the selected I2C pins */
			
			/* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/
			break;

		default:
			
			break;
			
	} // End of Switch
	EDIS;
} //////////// End of I2cAGpioConfig ///////////////////////////////////////

//************************************************************************************
// 'I2cbDataClkPin' should be assign with one of the possible I2C_B SDA - SDL GPIO pin
// Use defined Macros from "F2837xD_I2c_defines.h" for assignment
//************************************************************************************
void I2cBGpioConfig(Uint16 I2cbDataClkPin)
{
	EALLOW;

	switch(I2cbDataClkPin)
	{
		case I2C_B_GPIO2_GPIO3:
			/* Enable internal pull-up for the selected I2C pins */
			GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;     // Enable pull-up for GPIO0 (SDAB)
			GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;     // Enable pull-up for GPIO1 (SDLB)
		
			/* Set qualification for the selected I2C pins */
			GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 3;   // Async/no qualification (I/ps sync
			GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;   // to SYSCLKOUT by default)

			/* Configure which of the possible GPIO pins will be I2C_B pins using GPIO regs*/
			GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 1;    // Configure GPIO0 for SDAB operation
			GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 2;    // Configure GPIO0 for SDAB operation
		
			GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 1;    // Configure GPIO1 for SDLB operation
			GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 2;    // Configure GPIO1 for SDLB operation
			
			break;

		case I2C_B_GPIO134_GPIO35:
			/* Enable internal pull-up for the selected I2C pins */

			/* Set qualification for the selected I2C pins */

			/* Configure which of the possible GPIO pins will be I2C_B pins using GPIO regs*/
			break;

		case I2C_B_GPIO40_GPIO41:
			/* Enable internal pull-up for the selected I2C pins */

			/* Set qualification for the selected I2C pins */

			/* Configure which of the possible GPIO pins will be I2C_B pins using GPIO regs*/
			break;

		case I2C_B_GPIO66_GPIO69:
			/* Enable internal pull-up for the selected I2C pins */
			GpioCtrlRegs.GPCPUD.bit.GPIO66 = 0;	//SDAB
			GpioCtrlRegs.GPCPUD.bit.GPIO69 = 0;	//SCLB

			/* Set qualification for the selected I2C pins */
			GpioCtrlRegs.GPCQSEL1.bit.GPIO66 = 3;
			GpioCtrlRegs.GPCQSEL1.bit.GPIO69 = 3;

			/* Configure which of the possible GPIO pins will be I2C_B pins using GPIO regs*/
			GpioCtrlRegs.GPCGMUX1.bit.GPIO66 = 1;	//0x6
			GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 2;

			GpioCtrlRegs.GPCGMUX1.bit.GPIO69 = 1;	//0x6
			GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 2;
			break;

		default:
			break;

	}
	EDIS;
}
//////////// End of I2cBGpioConfig///////////////////////////////////////
#endif


//===========================================================================
// End of file.
//===========================================================================
