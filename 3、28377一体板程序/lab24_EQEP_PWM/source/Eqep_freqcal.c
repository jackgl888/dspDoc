//###########################################################################
// FILE:	Eqep_freqcal.c
// TITLE:	Frequency measurement using EQEP peripheral
//
//! \addtogroup cpu01_example_list
//! <h1>Frequency measurement using EQEP peripheral (Eqep_freqcal)</h1>
//!
//!  This test will calculate the frequency and period of an input signal
//!  using eQEP module.
//!
//!  EPWM1A is configured to generate a frequency of 5 kHz.
//!  \sa Section on Frequency Calculation for more details on the frequency 
//!  calculation performed in this example.
//!
//!  In addition to the main example file, the following files must be included
//!  in this project:
//!  - \b Example_freqcal.c - includes all eQEP functions
//!  - \b Example_EPwmSetup.c - sets up EPWM1A for use with this example
//!  - \b Example_freqcal.h - includes initialization values for frequency 
//!    structure.
//!
//!  The configuration for this example is as follows
//!  - Maximum frequency is configured to 10KHz (BaseFreq)
//!  - Minimum frequency is assumed at 50Hz for capture pre-scalar selection
//!
//!  \b SPEED_FR: High Frequency Measurement is obtained by counting the 
//!  external input pulses for 10ms (unit timer set to 100Hz).
//!  \f[ SPEED\_FR = \frac{Count\ Delta}{10ms} \f]
//!
//!  \b SPEED_PR: Low Frequency Measurement is obtained by measuring time 
//!  period of input edges. Time measurement is averaged over 64 edges for 
//!  better results and capture unit performs the time measurement using 
//!  pre-scaled SYSCLK.
//!
//!  Note that pre-scaler for capture unit clock is selected such that
//!  capture timer does not overflow at the required minimum frequency.
//!  This example runs forever until the user stops it.
//!
//!  \b External \b Connections \n
//!  - Connect GPIO20/EQEP1A to GPIO0/EPWM1A
//!
//!  \b Watch \b Variables \n
//!  - \b freq.freqhz_fr - Frequency measurement using position counter/unit 
//!    time out
//!  - \b freq.freqhz_pr - Frequency measurement using capture unit
//
//###########################################################################
// $TI Release: F2837xS Support Library v190 $
// $Release Date: Mon Feb  1 16:59:09 CST 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################
/*
 * EQEP1A连接EPWM2A,EPWM2A频率50HZ-10KHZ,运行程序，观察变量
 * freqhz_pr：
 * freqhz_fr：
 *
 */
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_freqcal.h"    // Example specific include file

#define SYSCLK 200e6
#define CPP    128
void EPwmSetup(void);

unsigned long Freq=0;
unsigned long newp=0,oldp=0,tmp=0;
unsigned long Prd=0,freq=0;
//__interrupt void prdTick(void);
void prd_cal(void);
__interrupt void UTO_INT(void);

unsigned int a = 0;

//FREQCAL freq=FREQCAL_DEFAULTS;

void main(void)
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xS_SysCtrl.c file.
   InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the F2837xS_Gpio.c file and
// illustrates how to set the GPIO to its default state.
// InitGpio();  // Skipped for this example

// Only init the GPIO for EQep1 and EPwm1 in this case
// This function is found in F2837xS_EQep.c
   InitEQep1Gpio();
   InitEPwm1Gpio();

// Step 3. Clear all __interrupts and initialize PIE vector table:
// Disable CPU __interrupts
   DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE __interrupts disabled and flags
// are cleared.
// This function is found in the F2837xS_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU __interrupts and clear all CPU __interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the __interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xS_DefaultIsr.c.
// This function is found in F2837xS_PieVect.c.
   InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;  // This is needed to write to EALLOW protected registers
//   PieVectTable.EPWM2_INT= &prdTick;
   PieVectTable.EQEP1_INT= &UTO_INT;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// Example specific ePWM setup.  This function is found
// in Example_EPwmSetup.c
   EPwmSetup();

// Step 5. User specific code, enable __interrupts:
// Enable CPU INT1 which is connected to CPU-Timer 0:
   IER |= M_INT5;

// Enable TINT0 in the PIE: Group 3 __interrupt 1
   PieCtrlRegs.PIEIER5.bit.INTx1 = 1;

// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global __interrupt INTM
   ERTM;   // Enable Global realtime __interrupt DBGM

//   freq.init(&freq);         // Initializes eQEP for frequency calculation in
   FREQCAL_Init();                             // FREQCAL_Init(void)function in Example_EPwmSetup.c
	for(;;)
	{
        //prd_cal();
	}

}

__interrupt void UTO_INT(void)
{

    newp=EQep1Regs.QPOSLAT;
    if (newp>oldp)
    {
        tmp = newp - oldp;
    }                     // x2-x1 in v=(x2-x1)/T equation
    else if(newp<oldp)
    {
        tmp = (0xFFFFFFFF-oldp)+newp;
    }
    Freq=tmp/2;
    oldp=newp;
    //memcpy(f,Freq,4);
    //Send_Dat(f);
    prd_cal();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    EQep1Regs.QFLG.bit.INT=0;
    EQep1Regs.QFLG.bit.UTO=0;
    EQep1Regs.QCLR.bit.INT=1;
    EQep1Regs.QCLR.bit.UTO=1;
}

void prd_cal(void)
{
   if(EQep1Regs.QEPSTS.bit.UPEVNT==1)              // 单位位置事件发生
    {
        if(EQep1Regs.QEPSTS.bit.COEF==0)            // No Capture overflow
        {Prd=(unsigned long)EQep1Regs.QCPRDLAT;}
        //else                                      // Capture overflow, saturate the result
        //  Prd=0xFFFF;
        freq=SYSCLK/(Prd*CPP);
        EQep1Regs.QEPSTS.all=0x88;
    }
}



/*__interrupt void prdTick(void) // Interrupts once per ePWM period
{
   freq.calc(&freq); // Checks for event and calculates frequency in FREQCAL_Calc(FREQCAL *p)

                     // function in Example_EPwmSetup.c
   // Acknowledge this __interrupt to receive more __interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
   EPwm2Regs.ETCLR.bit.INT=1;
}
*/
