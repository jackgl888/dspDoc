
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_freqcal.h"    // Example specific include file


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
//   InitEQep1Gpio();
   InitEPwmGpio();

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


// Step 4. Initialize all the Device Peripherals:
// Example specific ePWM setup.  This function is found
// in Example_EPwmSetup.c
   EPwmSetup();


   EINT;   // Enable Global __interrupt INTM
   ERTM;   // Enable Global realtime __interrupt DBGM


	for(;;)
	{
	}

}

//===========================================================================
// No more.
//===========================================================================

