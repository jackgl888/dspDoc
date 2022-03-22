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

#define PWM_CLK   2e4
#define SP        TB_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200A              // Up-down cnt, timebase = SYSCLKOUT

void EPwmSetup()
{
	EPwm1Regs.TBSTS.all=0; //时基状态寄存器：计数器没有达到最大值；没有同步事件
	EPwm1Regs.TBPHS.all=0;//时基相位寄存器
	EPwm1Regs.TBCTR=0;//时基计数寄存器

	EPwm1Regs.CMPCTL.all=0x50;        // 立即装载模式 for CMPA and CMPB，
	EPwm1Regs.CMPA.bit.CMPA=SP/2;      //0.5占空比
	EPwm1Regs.CMPB.all=0;

	EPwm1Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and 向上计数
	                                  // EPWMxA = 0 when CTR=CMPA and 向下计数
	EPwm1Regs.AQCTLB.all=0;     //动作控制寄存器B 不动作
	EPwm1Regs.AQSFRC.all=0;  //动作软件强制寄存器 不动作
	EPwm1Regs.AQCSFRC.all=0;//软件强制无效

	EPwm1Regs.DBCTL.all=0xb;          // EPWMxB is inverted 死区控制寄存器 使能双边沿延时，PWMB翻转
	EPwm1Regs.DBRED.bit.DBRED=0;  //死区上升沿延时寄存器
	EPwm1Regs.DBFED.bit.DBFED=0;  //死区下降沿延时寄存器

	EPwm1Regs.TZSEL.all=0;  //
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


	EPwm2Regs.TBSTS.all=0;
	EPwm2Regs.TBPHS.all=0;
	EPwm2Regs.TBCTR=0;

	EPwm2Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm2Regs.CMPA.bit.CMPA=SP/2;
	EPwm2Regs.CMPB.all=0;

	EPwm2Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm2Regs.AQCTLB.all=0;
	EPwm2Regs.AQSFRC.all=0;
	EPwm2Regs.AQCSFRC.all=0;

	EPwm2Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm2Regs.DBRED.bit.DBRED=0;
	EPwm2Regs.DBFED.bit.DBFED=0;

	EPwm2Regs.TZSEL.all=0;
	EPwm2Regs.TZCTL.all=0;
	EPwm2Regs.TZEINT.all=0;
	EPwm2Regs.TZFLG.all=0;
	EPwm2Regs.TZCLR.all=0;
	EPwm2Regs.TZFRC.all=0;

	EPwm2Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm2Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm2Regs.ETFLG.all=0;
	EPwm2Regs.ETCLR.all=0;
	EPwm2Regs.ETFRC.all=0;

	EPwm2Regs.PCCTL.all=0;

	EPwm2Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm2Regs.TBPRD=SP;

	EPwm2Regs.TBSTS.all=0;
	EPwm2Regs.TBPHS.all=0;
	EPwm2Regs.TBCTR=0;

	EPwm2Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm2Regs.CMPA.bit.CMPA=SP/2;
	EPwm2Regs.CMPB.all=0;

	EPwm2Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm2Regs.AQCTLB.all=0;
	EPwm2Regs.AQSFRC.all=0;
	EPwm2Regs.AQCSFRC.all=0;

	EPwm2Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm2Regs.DBRED.bit.DBRED=0;
	EPwm2Regs.DBFED.bit.DBFED=0;

	EPwm2Regs.TZSEL.all=0;
	EPwm2Regs.TZCTL.all=0;
	EPwm2Regs.TZEINT.all=0;
	EPwm2Regs.TZFLG.all=0;
	EPwm2Regs.TZCLR.all=0;
	EPwm2Regs.TZFRC.all=0;

	EPwm2Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm2Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm2Regs.ETFLG.all=0;
	EPwm2Regs.ETCLR.all=0;
	EPwm2Regs.ETFRC.all=0;

	EPwm2Regs.PCCTL.all=0;

	EPwm2Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm2Regs.TBPRD=SP;


	EPwm3Regs.TBSTS.all=0;
	EPwm3Regs.TBPHS.all=0;
	EPwm3Regs.TBCTR=0;

	EPwm3Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm3Regs.CMPA.bit.CMPA=SP/2;
	EPwm3Regs.CMPB.all=0;

	EPwm3Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm3Regs.AQCTLB.all=0;
	EPwm3Regs.AQSFRC.all=0;
	EPwm3Regs.AQCSFRC.all=0;

	EPwm3Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm3Regs.DBRED.bit.DBRED=0;
	EPwm3Regs.DBFED.bit.DBFED=0;

	EPwm3Regs.TZSEL.all=0;
	EPwm3Regs.TZCTL.all=0;
	EPwm3Regs.TZEINT.all=0;
	EPwm3Regs.TZFLG.all=0;
	EPwm3Regs.TZCLR.all=0;
	EPwm3Regs.TZFRC.all=0;

	EPwm3Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm3Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm3Regs.ETFLG.all=0;
	EPwm3Regs.ETCLR.all=0;
	EPwm3Regs.ETFRC.all=0;

	EPwm3Regs.PCCTL.all=0;

	EPwm3Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm3Regs.TBPRD=SP;


	EPwm4Regs.TBSTS.all=0;
	EPwm4Regs.TBPHS.all=0;
	EPwm4Regs.TBCTR=0;

	EPwm4Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm4Regs.CMPA.bit.CMPA=SP/2;
	EPwm4Regs.CMPB.all=0;

	EPwm4Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm4Regs.AQCTLB.all=0;
	EPwm4Regs.AQSFRC.all=0;
	EPwm4Regs.AQCSFRC.all=0;

	EPwm4Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm4Regs.DBRED.bit.DBRED=0;
	EPwm4Regs.DBFED.bit.DBFED=0;

	EPwm4Regs.TZSEL.all=0;
	EPwm4Regs.TZCTL.all=0;
	EPwm4Regs.TZEINT.all=0;
	EPwm4Regs.TZFLG.all=0;
	EPwm4Regs.TZCLR.all=0;
	EPwm4Regs.TZFRC.all=0;

	EPwm4Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm4Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm4Regs.ETFLG.all=0;
	EPwm4Regs.ETCLR.all=0;
	EPwm4Regs.ETFRC.all=0;

	EPwm4Regs.PCCTL.all=0;

	EPwm4Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm4Regs.TBPRD=SP;


	EPwm5Regs.TBSTS.all=0;
	EPwm5Regs.TBPHS.all=0;
	EPwm5Regs.TBCTR=0;

	EPwm5Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm5Regs.CMPA.bit.CMPA=SP/2;
	EPwm5Regs.CMPB.all=0;

	EPwm5Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm5Regs.AQCTLB.all=0;
	EPwm5Regs.AQSFRC.all=0;
	EPwm5Regs.AQCSFRC.all=0;

	EPwm5Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm5Regs.DBRED.bit.DBRED=0;
	EPwm5Regs.DBFED.bit.DBFED=0;

	EPwm5Regs.TZSEL.all=0;
	EPwm5Regs.TZCTL.all=0;
	EPwm5Regs.TZEINT.all=0;
	EPwm5Regs.TZFLG.all=0;
	EPwm5Regs.TZCLR.all=0;
	EPwm5Regs.TZFRC.all=0;

	EPwm5Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm5Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm5Regs.ETFLG.all=0;
	EPwm5Regs.ETCLR.all=0;
	EPwm5Regs.ETFRC.all=0;

	EPwm5Regs.PCCTL.all=0;

	EPwm5Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm5Regs.TBPRD=SP;


	EPwm6Regs.TBSTS.all=0;
	EPwm6Regs.TBPHS.all=0;
	EPwm6Regs.TBCTR=0;

	EPwm6Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm6Regs.CMPA.bit.CMPA=SP/2;
	EPwm6Regs.CMPB.all=0;

	EPwm6Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm6Regs.AQCTLB.all=0;
	EPwm6Regs.AQSFRC.all=0;
	EPwm6Regs.AQCSFRC.all=0;

	EPwm6Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm6Regs.DBRED.bit.DBRED=0;
	EPwm6Regs.DBFED.bit.DBFED=0;

	EPwm6Regs.TZSEL.all=0;
	EPwm6Regs.TZCTL.all=0;
	EPwm6Regs.TZEINT.all=0;
	EPwm6Regs.TZFLG.all=0;
	EPwm6Regs.TZCLR.all=0;
	EPwm6Regs.TZFRC.all=0;

	EPwm6Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm6Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm6Regs.ETFLG.all=0;
	EPwm6Regs.ETCLR.all=0;
	EPwm6Regs.ETFRC.all=0;

	EPwm6Regs.PCCTL.all=0;

	EPwm6Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm6Regs.TBPRD=SP;


	EPwm7Regs.TBSTS.all=0;
	EPwm7Regs.TBPHS.all=0;
	EPwm7Regs.TBCTR=0;

	EPwm7Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm7Regs.CMPA.bit.CMPA=SP/2;
	EPwm7Regs.CMPB.all=0;

	EPwm7Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm7Regs.AQCTLB.all=0;
	EPwm7Regs.AQSFRC.all=0;
	EPwm7Regs.AQCSFRC.all=0;

	EPwm7Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm7Regs.DBRED.bit.DBRED=0;
	EPwm7Regs.DBFED.bit.DBFED=0;

	EPwm7Regs.TZSEL.all=0;
	EPwm7Regs.TZCTL.all=0;
	EPwm7Regs.TZEINT.all=0;
	EPwm7Regs.TZFLG.all=0;
	EPwm7Regs.TZCLR.all=0;
	EPwm7Regs.TZFRC.all=0;

	EPwm7Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm7Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm7Regs.ETFLG.all=0;
	EPwm7Regs.ETCLR.all=0;
	EPwm7Regs.ETFRC.all=0;

	EPwm7Regs.PCCTL.all=0;

	EPwm7Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm7Regs.TBPRD=SP;


	EPwm8Regs.TBSTS.all=0;
	EPwm8Regs.TBPHS.all=0;
	EPwm8Regs.TBCTR=0;

	EPwm8Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm8Regs.CMPA.bit.CMPA=SP/2;
	EPwm8Regs.CMPB.all=0;

	EPwm8Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm8Regs.AQCTLB.all=0;
	EPwm8Regs.AQSFRC.all=0;
	EPwm8Regs.AQCSFRC.all=0;

	EPwm8Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm8Regs.DBRED.bit.DBRED=0;
	EPwm8Regs.DBFED.bit.DBFED=0;

	EPwm8Regs.TZSEL.all=0;
	EPwm8Regs.TZCTL.all=0;
	EPwm8Regs.TZEINT.all=0;
	EPwm8Regs.TZFLG.all=0;
	EPwm8Regs.TZCLR.all=0;
	EPwm8Regs.TZFRC.all=0;

	EPwm8Regs.ETSEL.all=9;            // Interrupt when TBCTR = 0x0000
	EPwm8Regs.ETPS.all=1;	          // Interrupt on first event
	EPwm8Regs.ETFLG.all=0;
	EPwm8Regs.ETCLR.all=0;
	EPwm8Regs.ETFRC.all=0;

	EPwm8Regs.PCCTL.all=0;

	EPwm8Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm8Regs.TBPRD=SP;

}

