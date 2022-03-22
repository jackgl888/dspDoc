//###########################################################################
// FILE:    epwm_deadband_c28.c
// TITLE:   Check PWM Dead-Band
//
//! \addtogroup cpu01_example_list
//! <h1> EPWM dead band control (epwm_deadband)</h1>
//!
//! During the test, monitor ePWM1, ePWM2, and/or ePWM3 outputs
//! on a scope.
//!
//! - ePWM1A is on GPIO0
//! - ePWM1B is on GPIO1
//! - ePWM2A is on GPIO2
//! - ePWM2B is on GPIO3
//! - ePWM3A is on GPIO4
//! - ePWM3B is on GPIO5
//!
//! This example configures ePWM1, ePWM2 and ePWM3 for:
//! - Count up/down
//! - Deadband
//!
//! 3 Examples are included:
//! - ePWM1: Active low PWMs
//! - ePWM2: Active low complementary PWMs
//! - ePWM3: Active high complementary PWMs
//!
//! Each ePWM is configured to interrupt on the 3rd zero event.
//! When this happens the deadband is modified such that
//! 0 <= DB <= DB_MAX.  That is, the deadband will move up and
//! down between 0 and the maximum value.
//!
//! View the EPWM1A/B, EPWM2A/B and EPWM3A/B waveforms
//! via an oscilloscope
//
//
//###########################################################################
// $TI Release: F2837xS Support Library v180 $
// $Release Date: Fri Nov  6 16:27:58 CST 2015 $
// $Copyright: Copyright (C) 2014-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File

// Prototype statements for functions found within this file.
void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);
void InitEPwm4Example(void);
void InitEPwm5Example(void);
void InitEPwm6Example(void);
void InitEPwm7Example(void);
void InitEPwm8Example(void);
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);
__interrupt void epwm4_isr(void);
__interrupt void epwm5_isr(void);
__interrupt void epwm6_isr(void);
__interrupt void epwm7_isr(void);
__interrupt void epwm8_isr(void);

// Global variables used in this example
Uint32 EPwm1TimerIntCount;
Uint32 EPwm2TimerIntCount;
Uint32 EPwm3TimerIntCount;
Uint32 EPwm4TimerIntCount;
Uint32 EPwm5TimerIntCount;
Uint32 EPwm6TimerIntCount;
Uint32 EPwm7TimerIntCount;
Uint32 EPwm8TimerIntCount;
Uint16 EPwm1_DB_Direction;
Uint16 EPwm2_DB_Direction;
Uint16 EPwm3_DB_Direction;
Uint16 EPwm4_DB_Direction;
Uint16 EPwm5_DB_Direction;
Uint16 EPwm6_DB_Direction;
Uint16 EPwm7_DB_Direction;
Uint16 EPwm8_DB_Direction;

// Maximum Dead Band values
#define EPWM1_MAX_DB   0x03FF
#define EPWM2_MAX_DB   0x03FF
#define EPWM3_MAX_DB   0x03FF
#define EPWM4_MAX_DB   0x03FF
#define EPWM5_MAX_DB   0x03FF
#define EPWM6_MAX_DB   0x03FF
#define EPWM7_MAX_DB   0x03FF
#define EPWM8_MAX_DB   0x03FF

#define EPWM1_MIN_DB   0
#define EPWM2_MIN_DB   0
#define EPWM3_MIN_DB   0
#define EPWM4_MIN_DB   0
#define EPWM5_MIN_DB   0
#define EPWM6_MIN_DB   0
#define EPWM7_MIN_DB   0
#define EPWM8_MIN_DB   0

// To keep track of which way the Dead Band is moving
#define DB_UP   1
#define DB_DOWN 0

void main(void)
{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xS_SysCtrl.c file.
    InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the F2837xS_Gpio.c file and
// illustrates how to set the GPIO to its default state.
//    InitGpio();

//allocate PWM1, PWM2 and PWM3 to CPU1
//    CpuSysRegs.CPUSEL0.bit.EPWM1 = 0;
//    CpuSysRegs.CPUSEL0.bit.EPWM2 = 0;
//    CpuSysRegs.CPUSEL0.bit.EPWM3 = 0;

// enable PWM1, PWM2 and PWM3
    CpuSysRegs.PCLKCR2.bit.EPWM1=1;
    CpuSysRegs.PCLKCR2.bit.EPWM2=1;
    CpuSysRegs.PCLKCR2.bit.EPWM3=1;
    CpuSysRegs.PCLKCR2.bit.EPWM4=1;
    CpuSysRegs.PCLKCR2.bit.EPWM5=1;
    CpuSysRegs.PCLKCR2.bit.EPWM6=1;
    CpuSysRegs.PCLKCR2.bit.EPWM7=1;
    CpuSysRegs.PCLKCR2.bit.EPWM8=1;

// For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
// These functions are in the F2837xS_EPwm.c file
    InitEPwm1Gpio();
	InitEPwm2Gpio();
	InitEPwm3Gpio();
    InitEPwm4Gpio();
    InitEPwm5Gpio();
    InitEPwm6Gpio();
//    InitEPwm7Gpio();
    InitEPwm8Gpio();

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
    DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xS_PieCtrl.c file.
    InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xS_DefaultIsr.c.
// This function is found in F2837xS_PieVect.c.
    InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
	EALLOW; // This is needed to write to EALLOW protected registers
	PieVectTable.EPWM1_INT = &epwm1_isr;
	PieVectTable.EPWM2_INT = &epwm2_isr;
	PieVectTable.EPWM3_INT = &epwm3_isr;
    PieVectTable.EPWM4_INT = &epwm4_isr;
    PieVectTable.EPWM5_INT = &epwm5_isr;
    PieVectTable.EPWM6_INT = &epwm6_isr;
    PieVectTable.EPWM7_INT = &epwm7_isr;
    PieVectTable.EPWM8_INT = &epwm8_isr;
	EDIS;   // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize the Device Peripherals:
// For this example, only initialize the ePWM

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC =0;

	EDIS;

	InitEPwm1Example();
	InitEPwm2Example();
	InitEPwm3Example();
    InitEPwm4Example();
    InitEPwm5Example();
    InitEPwm6Example();
    InitEPwm7Example();
    InitEPwm8Example();

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC =1;

	EDIS;

// Step 5. User specific code, enable interrupts:
// Initialize counters:
	EPwm1TimerIntCount = 0;
	EPwm2TimerIntCount = 0;
	EPwm3TimerIntCount = 0;
    EPwm4TimerIntCount = 0;
    EPwm5TimerIntCount = 0;
    EPwm6TimerIntCount = 0;
    EPwm7TimerIntCount = 0;
    EPwm8TimerIntCount = 0;

// Enable CPU INT3 which is connected to EPWM1-3 INT:
	IER |= M_INT3;

// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx3 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx4 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx5 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx6 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx7 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx8 = 1;


// Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
    for(;;)
    {
        asm ("          NOP");

    }
}

__interrupt void epwm1_isr(void)
{
/*    if(EPwm1_DB_Direction == DB_UP)
    {
        if(EPwm1Regs.DBFED.bit.DBFED < EPWM1_MAX_DB)
        {
            EPwm1Regs.DBFED.bit.DBFED++;
            EPwm1Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm1_DB_Direction = DB_DOWN;
            EPwm1Regs.DBFED.bit.DBFED--;
            EPwm1Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm1Regs.DBFED.bit.DBFED == EPWM1_MIN_DB)
        {
            EPwm1_DB_Direction = DB_UP;
            EPwm1Regs.DBFED.bit.DBFED++;
            EPwm1Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm1Regs.DBFED.bit.DBFED--;
            EPwm1Regs.DBRED.bit.DBRED--;
        }
    }
    */
    EPwm1TimerIntCount++;

    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm2_isr(void)
{

    if(EPwm2_DB_Direction == DB_UP)
    {
        if(EPwm2Regs.DBFED.bit.DBFED < EPWM2_MAX_DB)
        {
            EPwm2Regs.DBFED.bit.DBFED++;
            EPwm2Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm2_DB_Direction = DB_DOWN;
            EPwm2Regs.DBFED.bit.DBFED--;
            EPwm2Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm2Regs.DBFED.bit.DBFED == EPWM2_MIN_DB)
        {
            EPwm2_DB_Direction = DB_UP;
            EPwm2Regs.DBFED.bit.DBFED++;
            EPwm2Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm2Regs.DBFED.bit.DBFED--;
            EPwm2Regs.DBRED.bit.DBRED--;
        }
    }

    EPwm2TimerIntCount++;

    // Clear INT flag for this timer
    EPwm2Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm3_isr(void)
{
    if(EPwm3_DB_Direction == DB_UP)
    {
        if(EPwm3Regs.DBFED.bit.DBFED < EPWM3_MAX_DB)
        {
            EPwm3Regs.DBFED.bit.DBFED++;
            EPwm3Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm3_DB_Direction = DB_DOWN;
            EPwm3Regs.DBFED.bit.DBFED--;
            EPwm3Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm3Regs.DBFED.bit.DBFED == EPWM3_MIN_DB)
        {
            EPwm3_DB_Direction = DB_UP;
            EPwm3Regs.DBFED.bit.DBFED++;
            EPwm3Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm3Regs.DBFED.bit.DBFED--;
            EPwm3Regs.DBRED.bit.DBRED--;
        }
    }

    EPwm3TimerIntCount++;

    // Clear INT flag for this timer
    EPwm3Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm4_isr(void)
{
    if(EPwm4_DB_Direction == DB_UP)
    {
        if(EPwm4Regs.DBFED.bit.DBFED < EPWM4_MAX_DB)
        {
            EPwm4Regs.DBFED.bit.DBFED++;
            EPwm4Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm4_DB_Direction = DB_DOWN;
            EPwm4Regs.DBFED.bit.DBFED--;
            EPwm4Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm4Regs.DBFED.bit.DBFED == EPWM4_MIN_DB)
        {
            EPwm4_DB_Direction = DB_UP;
            EPwm4Regs.DBFED.bit.DBFED++;
            EPwm4Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm4Regs.DBFED.bit.DBFED--;
            EPwm4Regs.DBRED.bit.DBRED--;
        }
    }

    EPwm4TimerIntCount++;

    // Clear INT flag for this timer
    EPwm4Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm5_isr(void)
{
    if(EPwm5_DB_Direction == DB_UP)
    {
        if(EPwm5Regs.DBFED.bit.DBFED < EPWM5_MAX_DB)
        {
            EPwm5Regs.DBFED.bit.DBFED++;
            EPwm5Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm5_DB_Direction = DB_DOWN;
            EPwm5Regs.DBFED.bit.DBFED--;
            EPwm5Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm5Regs.DBFED.bit.DBFED == EPWM5_MIN_DB)
        {
            EPwm5_DB_Direction = DB_UP;
            EPwm5Regs.DBFED.bit.DBFED++;
            EPwm5Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm5Regs.DBFED.bit.DBFED--;
            EPwm5Regs.DBRED.bit.DBRED--;
        }
    }

    EPwm5TimerIntCount++;

    // Clear INT flag for this timer
    EPwm5Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm6_isr(void)
{
    if(EPwm6_DB_Direction == DB_UP)
    {
        if(EPwm6Regs.DBFED.bit.DBFED < EPWM6_MAX_DB)
        {
            EPwm6Regs.DBFED.bit.DBFED++;
            EPwm6Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm6_DB_Direction = DB_DOWN;
            EPwm6Regs.DBFED.bit.DBFED--;
            EPwm6Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm6Regs.DBFED.bit.DBFED == EPWM6_MIN_DB)
        {
            EPwm6_DB_Direction = DB_UP;
            EPwm6Regs.DBFED.bit.DBFED++;
            EPwm6Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm6Regs.DBFED.bit.DBFED--;
            EPwm6Regs.DBRED.bit.DBRED--;
        }
    }
    EPwm6TimerIntCount++;

    // Clear INT flag for this timer
    EPwm6Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm7_isr(void)
{
    if(EPwm7_DB_Direction == DB_UP)
    {
        if(EPwm7Regs.DBFED.bit.DBFED < EPWM7_MAX_DB)
        {
            EPwm7Regs.DBFED.bit.DBFED++;
            EPwm7Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm7_DB_Direction = DB_DOWN;
            EPwm7Regs.DBFED.bit.DBFED--;
            EPwm7Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm7Regs.DBFED.bit.DBFED == EPWM7_MIN_DB)
        {
            EPwm7_DB_Direction = DB_UP;
            EPwm7Regs.DBFED.bit.DBFED++;
            EPwm7Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm7Regs.DBFED.bit.DBFED--;
            EPwm7Regs.DBRED.bit.DBRED--;
        }
    }
    EPwm7TimerIntCount++;

    // Clear INT flag for this timer
    EPwm7Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm8_isr(void)
{
    if(EPwm8_DB_Direction == DB_UP)
    {
        if(EPwm8Regs.DBFED.bit.DBFED < EPWM8_MAX_DB)
        {
            EPwm8Regs.DBFED.bit.DBFED++;
            EPwm8Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm8_DB_Direction = DB_DOWN;
            EPwm8Regs.DBFED.bit.DBFED--;
            EPwm8Regs.DBRED.bit.DBRED--;
        }
    }
    else
    {
        if(EPwm8Regs.DBFED.bit.DBFED == EPWM8_MIN_DB)
        {
            EPwm8_DB_Direction = DB_UP;
            EPwm8Regs.DBFED.bit.DBFED++;
            EPwm8Regs.DBRED.bit.DBRED++;
        }
        else
        {
            EPwm8Regs.DBFED.bit.DBFED--;
            EPwm8Regs.DBRED.bit.DBRED--;
        }
    }
    EPwm8TimerIntCount++;

    // Clear INT flag for this timer
    EPwm8Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

void InitEPwm1Example()
{
    EPwm1Regs.TBPRD = 6000;                       // Set timer period
    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                     // Clear counter

    // Setup TBCLK
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV4;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    // Setup compare
    EPwm1Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;         // Set PWM1A on Zero
    EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active Low PWMs - Setup Deadband
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_LO;
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm1Regs.DBRED.bit.DBRED = EPWM1_MIN_DB;
    EPwm1Regs.DBFED.bit.DBFED = EPWM1_MIN_DB;
    EPwm1_DB_Direction = DB_UP;

    // Interrupt where we will change the Deadband
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event

}

void InitEPwm2Example()
{

    EPwm2Regs.TBPRD = 6000;                       // Set timer period
    EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                     // Clear counter

    // Setup TBCLK
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV4;          // Slow just to observe on the
                                                   // scope

    // Setup compare
    EPwm2Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM2A on Zero
    EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM2A on Zero
    EPwm2Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active Low complementary PWMs - setup the deadband
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm2Regs.DBRED.bit.DBRED = EPWM2_MIN_DB;
    EPwm2Regs.DBFED.bit.DBFED = EPWM2_MIN_DB;
    EPwm2_DB_Direction = DB_UP;

    // Interrupt where we will modify the deadband
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    EPwm2Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

}

void InitEPwm3Example()
{
    EPwm3Regs.TBPRD = 6000;                        // Set timer period
    EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0
    EPwm3Regs.TBCTR = 0x0000;                      // Clear counter

    // Setup TBCLK
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV4;          // Slow so we can observe on
                                                   // the scope

    // Setup compare
    EPwm3Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM3A on Zero
    EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm3Regs.AQCTLB.bit.CAU = AQ_CLEAR;           // Set PWM3A on Zero
    EPwm3Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active high complementary PWMs - Setup the deadband
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm3Regs.DBRED.bit.DBRED = EPWM3_MIN_DB;
    EPwm3Regs.DBFED.bit.DBFED = EPWM3_MIN_DB;
    EPwm3_DB_Direction = DB_UP;

    // Interrupt where we will change the deadband
    EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;      // Select INT on Zero event
    EPwm3Regs.ETSEL.bit.INTEN = 1;                 // Enable INT
    EPwm3Regs.ETPS.bit.INTPRD = ET_3RD;            // Generate INT on 3rd event
}

void InitEPwm4Example()
{
    EPwm4Regs.TBPRD = 6000;                        // Set timer period
    EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0
    EPwm4Regs.TBCTR = 0x0000;                      // Clear counter

    // Setup TBCLK
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV4;          // Slow so we can observe on
                                                   // the scope

    // Setup compare
    EPwm4Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM3A on Zero
    EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm4Regs.AQCTLB.bit.CAU = AQ_CLEAR;           // Set PWM3A on Zero
    EPwm4Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active high complementary PWMs - Setup the deadband
    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm4Regs.DBRED.bit.DBRED = EPWM4_MIN_DB;
    EPwm4Regs.DBFED.bit.DBFED = EPWM4_MIN_DB;
    EPwm4_DB_Direction = DB_UP;

    // Interrupt where we will change the deadband
    EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;      // Select INT on Zero event
    EPwm4Regs.ETSEL.bit.INTEN = 1;                 // Enable INT
    EPwm4Regs.ETPS.bit.INTPRD = ET_3RD;            // Generate INT on 3rd event
}

void InitEPwm5Example()
{
    EPwm5Regs.TBPRD = 6000;                        // Set timer period
    EPwm5Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0
    EPwm5Regs.TBCTR = 0x0000;                      // Clear counter

    // Setup TBCLK
    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV4;          // Slow so we can observe on
                                                   // the scope

    // Setup compare
    EPwm5Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm5Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM3A on Zero
    EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm5Regs.AQCTLB.bit.CAU = AQ_CLEAR;           // Set PWM3A on Zero
    EPwm5Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active high complementary PWMs - Setup the deadband
    EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm5Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm5Regs.DBRED.bit.DBRED = EPWM5_MIN_DB;
    EPwm5Regs.DBFED.bit.DBFED = EPWM5_MIN_DB;
    EPwm5_DB_Direction = DB_UP;

    // Interrupt where we will change the deadband
    EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;      // Select INT on Zero event
    EPwm5Regs.ETSEL.bit.INTEN = 1;                 // Enable INT
    EPwm5Regs.ETPS.bit.INTPRD = ET_3RD;            // Generate INT on 3rd event
}

void InitEPwm6Example()
{
    EPwm6Regs.TBPRD = 6000;                       // Set timer period
    EPwm6Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm6Regs.TBCTR = 0x0000;                     // Clear counter

    // Setup TBCLK
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV4;

    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Load registers every ZERO
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    // Setup compare
    EPwm6Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;         // Set PWM1A on Zero
    EPwm6Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active Low PWMs - Setup Deadband
    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_LO;
    EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm6Regs.DBRED.bit.DBRED = EPWM6_MIN_DB;
    EPwm6Regs.DBFED.bit.DBFED = EPWM6_MIN_DB;
    EPwm6_DB_Direction = DB_UP;

    // Interrupt where we will change the Deadband
    EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm6Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm6Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event

}

void InitEPwm7Example()
{
    EPwm7Regs.TBPRD = 6000;                       // Set timer period
    EPwm7Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm7Regs.TBCTR = 0x0000;                     // Clear counter

    // Setup TBCLK
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV4;

    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    // Setup compare
    EPwm7Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm7Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm7Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm7Regs.AQCTLB.bit.CAU = AQ_CLEAR;         // Set PWM1A on Zero
    EPwm7Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active Low PWMs - Setup Deadband
    EPwm7Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_LO;
    EPwm7Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm7Regs.DBRED.bit.DBRED = EPWM7_MIN_DB;
    EPwm7Regs.DBFED.bit.DBFED = EPWM7_MIN_DB;
    EPwm7_DB_Direction = DB_UP;

    // Interrupt where we will change the Deadband
    EPwm7Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm7Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm7Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event

}

void InitEPwm8Example()
{
    EPwm8Regs.TBPRD = 6000;                       // Set timer period
    EPwm8Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm8Regs.TBCTR = 0x0000;                     // Clear counter

    // Setup TBCLK
    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm8Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV4;

    EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Load registers every ZERO
    EPwm8Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm8Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    // Setup compare
    EPwm8Regs.CMPA.bit.CMPA = 3000;

    // Set actions
    EPwm8Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm8Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm8Regs.AQCTLB.bit.CAU = AQ_CLEAR;         // Set PWM1A on Zero
    EPwm8Regs.AQCTLB.bit.CAD = AQ_SET;

    // Active Low PWMs - Setup Deadband
    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_LO;
    EPwm8Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm8Regs.DBRED.bit.DBRED = EPWM8_MIN_DB;
    EPwm8Regs.DBFED.bit.DBFED = EPWM8_MIN_DB;
    EPwm8_DB_Direction = DB_UP;

    // Interrupt where we will change the Deadband
    EPwm8Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm8Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm8Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event

}

