//###########################################################################
//
// FILE:	Example_freqcal.c
//
// TITLE:	Frequency measurement using EQEP peripheral
//
// DESCRIPTION:
//
// This file includes the EQEP initialization and frequency calculation
// functions called by Eqep_freqcal.c.  The frequency calculation
// steps performed by FREQCAL_Calc() at SYSCLKOUT = 200 MHz are
// described below:
//
//
// 1. This program calculates: **freqhz_fr**
//    freqhz_fr or v = (x2-x1)/T                                                 - Equation 1
//
// If max/base freq = 10kHz:   10kHz = (x2-x1)/(2/100Hz)                         - Equation 2
//                      max (x2-x1) = 200 counts = freqScaler_fr
//		Note: T = 2/100Hz. 2 is from (x2-x1)/2 - because QPOSCNT counts 2 edges per cycle
//		                                         (rising and falling)
//
// If both sides of Equation 2 are divided by 10 kHz, then:
//                            1 = (x2-x1)/[10kHz*(2/100Hz)] where [10kHz* (2/100Hz)] = 200
//							Because (x2-x1) must be <200 (max),
//						    (x2-x1)/200 < 1 for all frequencies less than max
//							freq_fr = (x2-x1)/200 or (x2-x1)/[10kHz*(2/100Hz)]    - Equation 3
//
// To get back to original velocity equation, Equation 1, multiply Equation 3 by 10 kHz
//                           freqhz_fr (or velocity) = 10kHz*(x2-x1)/[10kHz*(2/100Hz)]
//						                           = (x2-x1)/(2/100Hz)           - Final equation
//
// 2. **min freq** = 1 count/(2/100Hz) = 50 Hz
//
// 3. **freqhz_pr**
//    freqhz_pr or v = X/(t2-t1)                                                 - Equation 4
//
// If max/base freq = 10kHz:  10kHz = (8/2)/T = 8/2T
//    where 8 = QCAPCTL [UPPS] (Unit timeout - once every 8 edges)
//		    2 = divide by 2 because QPOSCNT counts 2 edges per cycle (rising and falling)
//		    T = time in seconds
//            = t2-t1/(100MHz/128),  t2-t1= # of QCAPCLK cycles, and
//		                  1 QCAPCLK cycle = 1/(100MHz/128)
//										  = QCPRDLAT
//
// So: 10 kHz = 8(200MHz/128)/2(t2-t1)
//      t2-t1 = 8(200MHz/128)/(10kHz*2) = (200MHz/128)/(2*10kHz/8)  - Equation 5
//            = 625QCAPCLK cycles = maximum (t2-t1) = freqScaler_pr
//
// Divide both sides by (t2-t1), and:
//          1 = 625/(t2-t1) = [(200MHz/128)/(2*10kHz/8)]/(t2-t1)
//  Because (t2-t1) must be <625 (max).
//              625/(t2-t1) < 1 for all frequencies less than max
//    freq_pr = 625/(t2-t1) or [(200MHz/128)/(2*10kHz/8)]/(t2-t1)  - Equation 6
// Now within velocity limits, to get back to original velocity equation, 
// Equation 1, multiply Equation 6 by 10 kHz:
//  freqhz_fr (or velocity) = 10kHz*[200MHz/128)/(2*10kHz/8)]/(t2-t1)
//                          = (200MHz/128)*8/[2(t2-t1)]
//                          or 8/[2*(t2-t1)(QCPRDLAT)]        - Final Equation
//
//
// More detailed calculation results can be found in the Example_freqcal.xls
// spreadsheet included in the example folder.
//
//
// This file contains source for the freq calculation module.
//
//###########################################################################
// $TI Release: F2837xS Support Library v190 $
// $Release Date: Mon Feb  1 16:59:09 CST 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_freqcal.h"    // Example specific include file

void  FREQCAL_Init(void)
{
    #if (CPU_FRQ_200MHZ)
      EQep1Regs.QUPRD=200000000;            // Unit Timer for 100Hz at 150MHz SYSCLKOUT
    #endif
    #if (CPU_FRQ_100MHZ)
      EQep2Regs.QUPRD=100000000;            // Unit Timer for 100Hz at 100MHz SYSCLKOUT
    #endif

    EQep1Regs.QDECCTL.bit.QSRC=2;       // 向上计数模式 (freq. measurement)
    EQep1Regs.QDECCTL.bit.XCR=0;        // 2倍，上下边沿计数 (cnt falling and rising edges)
    EQep1Regs.QEPCTL.bit.FREE_SOFT=0;
    EQep1Regs.QEPCTL.bit.PCRM=00;       // 索引事件发生时复位
    EQep1Regs.QEPCTL.bit.UTE=1;         // QEP单位寄存器使能
    //EQep2Regs.QUTMR=0;
    //EQep2Regs.QUPRD=150000000; //周期值
    //EQep2Regs.QFLG.bit.UTO=0;
//  EQep2Regs.QFLG.bit.INT=0;
    //EQep2Regs.QCLR.bit.UTO=1; //清除定时中断标志位
    //EQep2Regs.QFRC.bit.UTO=1;
    EQep1Regs.QEINT.all=0x0800; //使能定时器的中断
    EQep1Regs.QEPCTL.bit.QCLM=1;        // 锁存单位时间事件
    EQep1Regs.QPOSMAX=0xffffffff;
    EQep1Regs.QEPCTL.bit.QPEN=1;        // QEP使能

    #if (CPU_FRQ_200MHZ)
      EQep1Regs.QCAPCTL.bit.UPPS=1;     // 单位位置事件分频数1/2
    #endif
    #if (CPU_FRQ_100MHZ)
      EQep1Regs.QCAPCTL.bit.UPPS=3;     // 单位位置事件分频数1/8 for unit position at 100MHz SYSCLKOUT
    #endif

    EQep1Regs.QCAPCTL.bit.CCPS=7;       // QEP捕捉时钟分频，为系统时钟的1/128
    EQep1Regs.QCAPCTL.bit.CEN=1;        // QEP 捕获功能使能
}

/*void  FREQCAL_Init(void)
{

	EQep1Regs.QUPRD=2000000;			// Unit Timer for 100Hz at 200 MHz SYSCLKOUT

	EQep1Regs.QDECCTL.bit.QSRC=2;		// Up count mode (freq. measurement)
	EQep1Regs.QDECCTL.bit.XCR=0;        // 2x resolution (cnt falling and rising edges)

	EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
	EQep1Regs.QEPCTL.bit.PCRM=00;		// QPOSCNT reset on index evnt
	EQep1Regs.QEPCTL.bit.UTE=1; 		// Unit Timer Enable
	EQep1Regs.QEPCTL.bit.QCLM=1; 		// Latch on unit time out
	EQep1Regs.QPOSMAX=0xffffffff;
	EQep1Regs.QEPCTL.bit.QPEN=1; 		// QEP enable
	EQep1Regs.QEINT.bit.UTO = 1;

	EQep1Regs.QCAPCTL.bit.UPPS=3;   	// 1/8 for unit position
	EQep1Regs.QCAPCTL.bit.CCPS=7;		// 1/128 for CAP clock
	EQep1Regs.QCAPCTL.bit.CEN=1; 		// QEP Capture Enable

}*/



