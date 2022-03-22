
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_freqcal.h"    // Example specific include file
Uint32 t1,t2,t3,t4,T1,T2;
void EPwmSetup();

interrupt void ISRCap2(void);

void InitECap2ture(void)
{
   ECap2Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
   ECap2Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
   ECap2Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
   ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

   //
   // Configure peripheral registers
   //
   ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;   // One-shot
   ECap2Regs.ECCTL2.bit.STOP_WRAP = 3;     // Stop at 4 events
   ECap2Regs.ECCTL1.bit.CAP1POL = 1;       // Falling edge
   ECap2Regs.ECCTL1.bit.CAP2POL = 0;       // Rising edge
   ECap2Regs.ECCTL1.bit.CAP3POL = 1;       // Falling edge
   ECap2Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
   ECap2Regs.ECCTL1.bit.CTRRST1 = 0;       // Difference operation
   ECap2Regs.ECCTL1.bit.CTRRST2 = 0;       // Difference operation
   ECap2Regs.ECCTL1.bit.CTRRST3 = 0;       // Difference operation
   ECap2Regs.ECCTL1.bit.CTRRST4 = 0;       // Difference operation
   ECap2Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
   ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
   ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

   ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
   ECap2Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
   ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads
   ECap2Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = __interrupt
}
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
// This function is f
   InitEPwmGpio();
   InitECap2Gpio(21);
   GPIO_SetupPinOptions(21, GPIO_INPUT, GPIO_ASYNC);
//   InitECap1Gpio(22);
//   GPIO_SetupPinOptions(22, GPIO_INPUT, GPIO_ASYNC);


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

   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable. ECAP2_INT = &ISRCap2;
//   PieVectTable. ECAP3_INT = &ISRCap3;
//   PieVectTable.TINT0 = &ISRTimer0;
//   PieVectTable.ADCINT = &adc_isr;
   //PieVectTable.XINT13 = &cpu_timer1_isr;
   //PieVectTable.TINT2 = &cpu_timer2_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers
// Step 4. Initialize all the Device Peripherals:
// Example specific ePWM setup.  This function is found
// in Example_EPwmSetup.c
   InitECap2ture();
   EPwmSetup();
   // Enable CPU INT4 which is connected to ECAP1-4 INT:
   //
      IER |= M_INT4;

   //
   // Enable eCAP INTn in the PIE: Group 3 __interrupt 1-6
   //
    //  PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
      PieCtrlRegs.PIEIER4.bit.INTx2 = 1;
//      PieCtrlRegs.PIEIER4.bit.INTx3 = 1;

   //
   // Enable global Interrupts and higher priority real-time debug events:
   //
      EINT;   // Enable Global __interrupt INTM
      ERTM;   // Enable Global realtime __interrupt DBGM

    for(;;)
    {
    }

}

interrupt void ISRCap2(void)
{


    t1= ECap2Regs.CAP1;
    t2= ECap2Regs.CAP2;
    t3= ECap2Regs.CAP3;
    t4= ECap2Regs.CAP4;
    T1=t2-t1;T2=t4-t3;

    ECap2Regs.ECCLR.bit.CEVT4 = 1;
    ECap2Regs.ECCLR.bit.INT = 1;
    ECap2Regs.ECCTL2.bit.REARM = 1;

   // Acknowledge this interrupt to receive more interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    ECap2Regs.ECCLR.all=0xFFFF;//clare all flag

}
