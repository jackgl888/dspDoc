
//###########################################################################
//
// FILE:   SFO_V8.H
//
// TITLE:  Scale Factor Optimizer Library V8 Interface Header
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  1.0 | 23 01 2014  | KP   | Initial release
//###########################################################################
// $TI Release: F2837xS Support Library v190 $
// $Release Date: Mon Feb  1 16:59:09 CST 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//============================================================================
// Description:		This header provides the function call interface
// 			        for the scale factor optimizer
//============================================================================


//============================================================================
// Multiple include Guard
//============================================================================
#ifndef _SFO_V8_H
#define _SFO_V8_H

//============================================================================
// C++ namespace
//============================================================================
#ifdef __cplusplus
extern "C" {
#endif

//============================================================================
// USER MUST UPDATE THIS CONSTANT FOR NUMBER OF HRPWM CHANNELS USED
//============================================================================
#define PWM_CH	9	// Equal # of HRPWM modules PLUS 1
                    // i.e. PWM_CH is 9 for 8 channels, 7 for 6 channels, etc.
//============================================================================
// Function prototypes for MEP SFO
//============================================================================

int SFO(void);             // SFO V8  Calibration Function
//============================================================================
// Useful Defines when Using SFO Functions
//============================================================================
#define SFO_INCOMPLETE      0
#define SFO_COMPLETE        1
#define SFO_ERROR           2

//============================================================================
// Multiple include Guard
//============================================================================
#endif // End: Multiple include Guard

//============================================================================
// C++ namespace
//============================================================================
#ifdef __cplusplus
}
#endif /* extern "C" */
