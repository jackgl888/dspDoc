//###########################################################################
//
// FILE:	Example_EpwmSetup.c
//
// TITLE:	Frequency measurement using EQEP peripheral
//
// DESCRIPTION:
//
// This file contains source for the ePWM initialization for the
// freq calculation module
//
//###########################################################################
// $TI Release: F2837xS Support Library v190 $
// $Release Date: Mon Feb  1 16:59:09 CST 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_freqcal.h"    // Example specific include file

#define TB_CLK   100e6

#define PWM_CLK   5e3
#define SP        TB_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200A              // Up-down cnt, timebase = SYSCLKOUT

void EPwmSetup()
{
	EPwm1Regs.TBSTS.all=0;
	EPwm1Regs.TBPHS.all=0;
	EPwm1Regs.TBCTR=0;

	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm1Regs.CMPA.bit.CMPA=SP/2;
	EPwm1Regs.CMPB.all=0;

	EPwm1Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm1Regs.AQCTLB.all=0;
	EPwm1Regs.AQSFRC.all=0;
	EPwm1Regs.AQCSFRC.all=0;

	EPwm1Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm1Regs.DBRED.bit.DBRED=0;
	EPwm1Regs.DBFED.bit.DBFED=0;

	EPwm1Regs.TZSEL.all=0;
	EPwm1Regs.TZCTL.all=0;
	EPwm1Regs.TZEINT.all=0;
	EPwm1Regs.TZFLG.all=0;
	EPwm1Regs.TZCLR.all=0;
	EPwm1Regs.TZFRC.all=0;

	EPwm1Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm1Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm1Regs.ETFLG.all=0;
	EPwm1Regs.ETCLR.all=0;
	EPwm1Regs.ETFRC.all=0;

	EPwm1Regs.PCCTL.all=0;

	EPwm1Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm1Regs.TBPRD=SP;
}

