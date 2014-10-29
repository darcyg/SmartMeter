//--------------------------------------------------------------------------
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
//  File: emeter-asm.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-asm.h,v 1.6 2007/08/07 05:55:38 a0754793 Exp $
//
//--------------------------------------------------------------------------

// This file contains declarations to assist assembly language applications
// programming with the reference C language e-meter software.


// Offsets into a phase structure

// Offset to the pre-calculated metrics.
#define power_offset                        0       // 32 bit value
#define V_rms_offset                        4       // 16 bit value
#define I_rms_offset                        6       // 16 bit value
#define frequency_offset                    8       // 16 bit value
#define power_factor                        10      // 16 bit value
#define reactive_power_offset               12      // 32 bit value
#define va_power_offset                     16      // 32 bit value

// Offsets to internal data, which might be useful in a custom application.
#define V_sq_accum_logged_offset            32      // 48 bit value
#define live_P_accum_logged_offset          48      // 48 bit value
#define live_I_sq_accum_logged_offset       60      // 48 bit value
#define neutral_P_accum_logged_offset       88      // 48 bit value
#define neutral_I_sq_accum_logged_offset    100     // 48 bit value
#define sample_count_offset                 68      // 16 bit value

// Offsets into an rtc structure to access the various elements
#define year_offset                         0
#define month_offset                        1
#define day_offset                          2
#define hour_offset                         3
#define minute_offset                       4
#define second_offset                       5

// Values for the operating_mode
#define OPERATING_MODE_NORMAL               0
#define OPERATING_MODE_LIMP                 1
#define OPERATING_MODE_LCD_ONLY             2
#define OPERATING_MODE_POWERFAIL            3

// Bits in the key_states variable. Clear them once the keypress is recognised.
#define KEY_1_DOWN                          0x01
//#define KEY_1_LONG_DOWN                   0x02
#define KEY_1_REPEAT_DOWN                   0x02
//#define KEY_2_DOWN                        0x04
//#define KEY_2_LONG_DOWN                   0x08
//#define KEY_2_REPEAT_DOWN                 0x08
//#define KEY_3_DOWN                        0x10
//#define KEY_3_LONG_DOWN                   0x20
//#define KEY_3_REPEAT_DOWN                 0x20
//#define KEY_4_DOWN                        0x40
//#define KEY_4_LONG_DOWN                   0x80
//#define KEY_4_REPEAT_DOWN                 0x80

// The structure which contains the above data
    .extern     _chan1
    .extern     _operating_mode
    .extern     _key_states
    .extern     _calibrate_select_debounce

// Routines in the customer code which service the custom functions of the meter.
// These are called by the core meter code.

;----------------background-------------------
            .extern _sz_star_rtc_int        ;           ;// 1 second int// µ ± ±÷”
            .extern _sz_star_rtc_set                  ;// Set the RTC
            .extern _sz_star_eng_pulse     ;            ;//  1 energy pulse

            .extern     _sz_star_handle_keypress
            .extern     _sz_star_display_template
            .extern     _sz_star_setup
            .extern     _sz_star_power_down
            .extern     _sz_star_power_up

;----------------foreground-------------------          
            .extern _sz_star_initialisation  ;                   ;//  star initialization
            .extern _sz_star_mainloop_handler ;        ;// star main program
;-------------------------------------------  
            .extern _sz_star_LCDawaken_initialisation
            .extern _sz_star_serial_message_handler
