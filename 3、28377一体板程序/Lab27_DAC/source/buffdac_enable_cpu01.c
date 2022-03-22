//###########################################################################
// FILE:   buffdac_enable_cpu01.c
// TITLE:  Buffered DAC Enable Output Example for F2837xS.
//
//! \addtogroup cpu01_example_list
//! <h1> Buffered DAC Enable (buffdac_enable) </h1>
//!
//! This example generates a voltage on the buffered DAC output,
//! DACOUTA/ADCINA0 (HSEC Pin 9) and uses the default DAC reference setting of VDAC.
//!
//! When the DAC reference is set to VDAC, an external reference voltage
//! must be applied to the VDAC pin. This can accomplished by connecting a
//! jumper wire from 3.3V to ADCINB0 (HSEC pin 12).
//!
//
//###########################################################################
// $TI Release: F2837xS Support Library v180 $
// $Release Date: Fri Nov  6 16:27:58 CST 2015 $
// $Copyright: Copyright (C) 2014-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File

//initialize DAC pointer
volatile struct DAC_REGS* DAC_PTR[4] = {0x0,&DacaRegs,&DacbRegs,&DaccRegs};

//definitions for selecting DAC reference
#define REFERENCE_VDAC		0
#define REFERENCE_VREF		1

//definitions for DAC number
#define DACA				1
#define DACB				2
#define DACC				3

//specify configuration
#define REFERENCE			REFERENCE_VREF
#define DAC_NUM				DACA
Uint16 dacval = 2048;

//local functions
void configureDAC(Uint16 dac_num);

void main(void)
{
// Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xS_SysCtrl.c file.
	InitSysCtrl();

// Disable CPU interrupts
	DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags are cleared.
// This function is found in the F2837xS_PieCtrl.c file.
	InitPieCtrl();

// Clear all interrupts and initialize PIE vector table:
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

// Configure DAC
	configureDAC(DACA);
	configureDAC(DACB);
	configureDAC(DACC);

	while(1)
	{
		DAC_PTR[DACA]->DACVALS.all = 1365;//1V
		DELAY_US(2);

		DAC_PTR[DACB]->DACVALS.all = 2730;//2V
		DELAY_US(2);

		DAC_PTR[DACC]->DACVALS.all = 4095;//3V
		DELAY_US(2);

	}
}

void configureDAC(Uint16 dac_num)
{
	EALLOW;
	DAC_PTR[dac_num]->DACCTL.bit.DACREFSEL = REFERENCE;
	DAC_PTR[dac_num]->DACOUTEN.bit.DACOUTEN = 1;
	DAC_PTR[dac_num]->DACVALS.all = 0;
	DELAY_US(10); // Delay for buffered DAC to power up
	EDIS;
}
