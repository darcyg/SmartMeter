//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY, 
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS 
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR 
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE. 
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET 
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY 
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR 
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL, 
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY 
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT 
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM. 
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF 
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS 
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF 
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S 
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF 
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS 
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted 
//  by Texas Instruments is distributed as "freeware".  You may, 
//  only under TI's copyright in the Program, use and modify the 
//  Program without any charge or restriction.  You may 
//  distribute to third parties, provided that you transfer a 
//  copy of this license to the third party and the third party 
//  agrees to these terms by its first use of the Program. You 
//  must reproduce the copyright notice and any other legend of 
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains 
//  copyrighted material, trade secrets and other TI proprietary 
//  information and is protected by copyright laws, 
//  international copyright treaties, and trade secret laws, as 
//  well as other intellectual property laws.  To protect TI's 
//  rights in the Program, you agree not to decompile, reverse 
//  engineer, disassemble or otherwise translate any object code 
//  versions of the Program to a human-readable form.  You agree 
//  that in no event will you alter, remove or destroy any 
//  copyright notice included in the Program.  TI reserves all 
//  rights not specifically granted under this license. Except 
//  as specifically provided herein, nothing in this agreement 
//  shall be construed as conferring by implication, estoppel, 
//  or otherwise, upon you, any license or other right under any 
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//    File: emeter-phase-corrections.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-phase-corrections.c,v 1.4 2009/01/08 08:30:37 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 e-meter dynamic phase correction parameters. These are modified during
//  calibration, so they must be on a separate page/pages of flash memory from
//  the program itself, so the program is not corrupted when the flash is erased.
//
#include <stdint.h>
#include <io.h>
#include <emeter-toolkit.h>

#include "emeter-structs.h"

#if defined(PHASE_CORRECTION_SUPPORT)  &&  defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)

#if NUM_CURRENT_CHANNELS == 1
#define FREQ_PHASE_DEFS 0
#define FREQ_GAIN_DEFS 0
#define TAB_DEFS DEFAULT_PHASE_CORRECTION1, DEFAULT_GAIN_CORRECTION1
#elif NUM_CURRENT_CHANNELS == 2
#define FREQ_PHASE_DEFS 0
#define FREQ_GAIN_DEFS 0
#define TAB_DEFS {DEFAULT_PHASE_CORRECTION1, DEFAULT_GAIN_CORRECTION1}, {DEFAULT_PHASE_CORRECTION2, DEFAULT_GAIN_CORRECTION2}
#elif NUM_CURRENT_CHANNELS == 3
#define FREQ_PHASE_DEFS 0
#define FREQ_GAIN_DEFS 0
#define TAB_DEFS {DEFAULT_PHASE_CORRECTION1, DEFAULT_GAIN_CORRECTION1}, {DEFAULT_PHASE_CORRECTION2, DEFAULT_GAIN_CORRECTION2}, {DEFAULT_PHASE_CORRECTION3, DEFAULT_GAIN_CORRECTION3}
#else
#define FREQ_PHASE_DEFS 0
#define FREQ_GAIN_DEFS 0
#define TAB_DEFS {DEFAULT_PHASE_CORRECTION1, DEFAULT_GAIN_CORRECTION1}, {DEFAULT_PHASE_CORRECTION2, DEFAULT_GAIN_CORRECTION2}, {DEFAULT_PHASE_CORRECTION3, DEFAULT_GAIN_CORRECTION3}, {DEFAULT_PHASE_CORRECTION4, DEFAULT_GAIN_CORRECTION4}
#endif

const int16_t __erasablemem__ frequency_phase_factor[NUM_CURRENT_CHANNELS] =
{
    FREQ_PHASE_DEFS
};

const int16_t __erasablemem__ frequency_gain_factor[NUM_CURRENT_CHANNELS] =
{
    FREQ_GAIN_DEFS
};

const int8_t __erasablemem__ phase_corrections[77][NUM_CURRENT_CHANNELS][2] =
{
    {TAB_DEFS},  // 0.00A
    {TAB_DEFS},  // 0.10A
    {TAB_DEFS},  // 0.20A
    {TAB_DEFS},  // 0.30A
    {TAB_DEFS},  // 0.40A
    {TAB_DEFS},  // 0.50A
    {TAB_DEFS},  // 0.60A
    {TAB_DEFS},  // 0.70A
    {TAB_DEFS},  // 0.80A
    {TAB_DEFS},  // 0.90A
    {TAB_DEFS},  // 1.00A
    {TAB_DEFS},  // 1.10A
    {TAB_DEFS},  // 1.20A
    {TAB_DEFS},  // 1.30A
    {TAB_DEFS},  // 1.40A
    {TAB_DEFS},  // 1.50A
    {TAB_DEFS},  // 1.60A
    {TAB_DEFS},  // 1.70A
    {TAB_DEFS},  // 1.80A
    {TAB_DEFS},  // 1.90A
    {TAB_DEFS},  // 2.00A
    {TAB_DEFS},  // 2.10A
    {TAB_DEFS},  // 2.20A
    {TAB_DEFS},  // 2.30A
    {TAB_DEFS},  // 2.40A
    {TAB_DEFS},  // 2.50A
    {TAB_DEFS},  // 2.60A
    {TAB_DEFS},  // 2.70A
    {TAB_DEFS},  // 2.80A
    {TAB_DEFS},  // 2.90A
    {TAB_DEFS},  // 3.00A
    {TAB_DEFS},  // 3.10A
    {TAB_DEFS},  // 3.20A
    {TAB_DEFS},  // 3.30A
    {TAB_DEFS},  // 3.40A
    {TAB_DEFS},  // 3.50A
    {TAB_DEFS},  // 3.60A
    {TAB_DEFS},  // 3.70A
    {TAB_DEFS},  // 3.80A
    {TAB_DEFS},  // 3.90A
    {TAB_DEFS},  // 4.00A
    {TAB_DEFS},  // 4.10A
    {TAB_DEFS},  // 4.20A
    {TAB_DEFS},  // 4.30A
    {TAB_DEFS},  // 4.40A
    {TAB_DEFS},  // 4.50A
    {TAB_DEFS},  // 4.60A
    {TAB_DEFS},  // 4.70A
    {TAB_DEFS},  // 4.80A
    {TAB_DEFS},  // 4.90A
    {TAB_DEFS},  // 5.00A    new step
    {TAB_DEFS},  // 6.00A
    {TAB_DEFS},  // 7.00A
    {TAB_DEFS},  // 8.00A
    {TAB_DEFS},  // 9.00A
    {TAB_DEFS},  // 10.00A
    {TAB_DEFS},  // 11.00A
    {TAB_DEFS},  // 12.00A
    {TAB_DEFS},  // 13.00A
    {TAB_DEFS},  // 14.00A
    {TAB_DEFS},  // 15.00A
    {TAB_DEFS},  // 16.00A
    {TAB_DEFS},  // 17.00A
    {TAB_DEFS},  // 18.00A
    {TAB_DEFS},  // 19.00A
    {TAB_DEFS},  // 20.00A
    {TAB_DEFS},  // 21.00A
    {TAB_DEFS},  // 22.00A
    {TAB_DEFS},  // 23.00A
    {TAB_DEFS},  // 24.00A
    {TAB_DEFS},  // 25.00A    new step
    {TAB_DEFS},  // 30.00A
    {TAB_DEFS},  // 35.00A
    {TAB_DEFS},  // 40.00A
    {TAB_DEFS},  // 45.00A
    {TAB_DEFS},  // 50.00A
    {TAB_DEFS}   // 55.00A
};
#endif
