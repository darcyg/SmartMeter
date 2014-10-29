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
//  File: lcd-segments.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: lcd-segments.h,v 1.12 2009/02/26 08:09:59 a0754793 Exp $
//
//--------------------------------------------------------------------------

#if !defined(_LCD_SEGMENTS_H_)
#define _LCD_SEGMENTS_H_

/* SEG_a to SEG_h should have been defined to match the segments of the LCD in use before
   this file is included. Otherwise.... */

#if !defined(SEG_a)  ||  !defined(SEG_b)  ||  !defined(SEG_c)  ||  !defined(SEG_d)  ||  !defined(SEG_e)  ||  !defined(SEG_f)  ||  !defined(SEG_g)  ||  !defined(SEG_h)
#error The LCD segments have not been defined
#endif

/* First blank, and the hex codes, which all display quite
   well on a 7-segment display. */
#if !defined(CHAR_SPACE)
#define CHAR_SPACE  0
#endif
#if !defined(CHAR_ALL)
#define CHAR_ALL            (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_f|SEG_g|SEG_h)
#endif
#define CHAR_0              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_f)
#define CHAR_1              (lcd_cell_t) (SEG_b|SEG_c)
#if defined(USE_STARBURST)
#define CHAR_2              (lcd_cell_t) (SEG_a|SEG_d|SEG_e|SEG_1|SEG_9)
#define CHAR_3              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_3)
#else
#define CHAR_2              (lcd_cell_t) (SEG_a|SEG_b|SEG_d|SEG_e|SEG_g)
#define CHAR_3              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_g)
#endif
#define CHAR_4              (lcd_cell_t) (SEG_b|SEG_c|SEG_f|SEG_g)
#define CHAR_5              (lcd_cell_t) (SEG_a|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_6              (lcd_cell_t) (SEG_a|SEG_c|SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_7              (lcd_cell_t) (SEG_a|SEG_b|SEG_c)
#define CHAR_8              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_9              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_A              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_e|SEG_f|SEG_g)
#if defined(USE_STARBURST)
#define CHAR_B              (lcd_cell_t) (SEG_a|SEG_d|SEG_e|SEG_f|SEG_9|SEG_1|SEG_5)
#else
#define CHAR_B              (lcd_cell_t) (SEG_c|SEG_d|SEG_e|SEG_f|SEG_g)
#endif
#define CHAR_C              (lcd_cell_t) (SEG_a|SEG_d|SEG_e|SEG_f)
#if defined(USE_STARBURST)
#define CHAR_D              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_6|SEG_12)
#define CHAR_E              (lcd_cell_t) (SEG_a|SEG_d|SEG_e|SEG_f|SEG_9)
#define CHAR_F              (lcd_cell_t) (SEG_a|SEG_e|SEG_f|SEG_9)
#else
#define CHAR_D              (lcd_cell_t) (SEG_b|SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_E              (lcd_cell_t) (SEG_a|SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_F              (lcd_cell_t) (SEG_a|SEG_e|SEG_f|SEG_g)
#endif

#if defined(USE_STARBURST)
#define CHAR_DOUBLEQUOTE    (lcd_cell_t) (SEG_f|SEG_12)
#define CHAR_DOLLAR         (lcd_cell_t) (SEG_a|SEG_c|SEG_d|SEG_f|SEG_g|SEG_6|SEG_12)
#define CHAR_QUOTE          (lcd_cell_t) (SEG_f)
#define CHAR_ASTERISK       (lcd_cell_t) (SEG_1|SEG_3|SEG_5|SEG_6|SEG_7|SEG_9|SEG_11|SEG_12)
#define CHAR_PLUS           (lcd_cell_t) (SEG_3|SEG_6|SEG_9|SEG_12)
#define CHAR_MINUS          (lcd_cell_t) (SEG_g)
#define CHAR_SLASH          (lcd_cell_t) (SEG_1|SEG_7)
#define CHAR_LT             (lcd_cell_t) (SEG_1|SEG_5)
#define CHAR_EQUALS         (lcd_cell_t) (SEG_d|SEG_g)
#define CHAR_GT             (lcd_cell_t) (SEG_7|SEG_11)
#define CHAR_QUESTION       (lcd_cell_t) (SEG_a|SEG_f|SEG_1|SEG_6)
#define CHAR_LEFTBRACKET    (lcd_cell_t) (SEG_a|SEG_d|SEG_e|SEG_f)
#define CHAR_BACKSLASH      (lcd_cell_t) (SEG_5|SEG_11)
#define CHAR_RIGHTBRACKET   (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d)
#define CHAR_CARET          (lcd_cell_t) (SEG_5|SEG_7)
#define CHAR_UNDERSCORE     (lcd_cell_t) (SEG_d)
#define CHAR_BACKQUOTE      (lcd_cell_t) (SEG_b)
#define CHAR_VERTICALBAR    (lcd_cell_t) (SEG_6|SEG_12)
#define CHAR_a              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_b              (lcd_cell_t) (SEG_c|SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_c              (lcd_cell_t) (SEG_d|SEG_e|SEG_g)
#define CHAR_d              (lcd_cell_t) (SEG_b|SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_e              (lcd_cell_t) (SEG_a|SEG_b|SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_f              (lcd_cell_t) (SEG_a|SEG_e|SEG_f|SEG_9)               //same as F
#define CHAR_G              (lcd_cell_t) (SEG_a|SEG_c|SEG_d|SEG_e|SEG_f|SEG_3)
#define CHAR_g              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_H              (lcd_cell_t) (SEG_b|SEG_c|SEG_e|SEG_f|SEG_g)
#define CHAR_h              (lcd_cell_t) (SEG_c|SEG_e|SEG_f|SEG_g)
#define CHAR_I              (lcd_cell_t) (SEG_a|SEG_d|SEG_6|SEG_12)
#define CHAR_i              (lcd_cell_t) (SEG_c)
#define CHAR_J              (lcd_cell_t) (SEG_a|SEG_e|SEG_7|SEG_12)
#define CHAR_j              (lcd_cell_t) (SEG_e|SEG_7|SEG_12)
#define CHAR_K              (lcd_cell_t) (SEG_e|SEG_f|SEG_1|SEG_5|SEG_9)
#define CHAR_k              (lcd_cell_t) (SEG_e|SEG_f|SEG_1|SEG_5|SEG_9)         //same as K
#define CHAR_L              (lcd_cell_t) (SEG_d|SEG_e|SEG_f)
#define CHAR_l              (lcd_cell_t) (SEG_b|SEG_c)
#define CHAR_M              (lcd_cell_t) (SEG_b|SEG_c|SEG_e|SEG_f|SEG_1|SEG_11)
#define CHAR_m              (lcd_cell_t) (SEG_c|SEG_e|SEG_g|SEG_6)
#define CHAR_N              (lcd_cell_t) (SEG_b|SEG_c|SEG_e|SEG_f|SEG_5|SEG_11)
#define CHAR_n              (lcd_cell_t) (SEG_c|SEG_e|SEG_g)
#define CHAR_O              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_f)
#define CHAR_o              (lcd_cell_t) (SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_P              (lcd_cell_t) (SEG_a|SEG_b|SEG_e|SEG_f|SEG_g)
#define CHAR_p              (lcd_cell_t) (SEG_a|SEG_e|SEG_f|SEG_1|SEG_9)
#define CHAR_Q              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_f|SEG_5)
#define CHAR_q              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_f|SEG_g)
#define CHAR_R              (lcd_cell_t) (SEG_a|SEG_b|SEG_e|SEG_f|SEG_3|SEG_5|SEG_9)
#define CHAR_r              (lcd_cell_t) (SEG_e|SEG_g)
#define CHAR_S              (lcd_cell_t) (SEG_a|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_s              (lcd_cell_t) (SEG_d|SEG_5|SEG_3)
#define CHAR_T              (lcd_cell_t) (SEG_a|SEG_6|SEG_12)
#define CHAR_t              (lcd_cell_t) (SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_U              (lcd_cell_t) (SEG_b|SEG_c|SEG_d|SEG_e|SEG_f)
#define CHAR_u              (lcd_cell_t) (SEG_c|SEG_d|SEG_e)
#define CHAR_V              (lcd_cell_t) (SEG_e|SEG_f|SEG_1|SEG_7)
#define CHAR_v              (lcd_cell_t) (SEG_e|SEG_7)
#define CHAR_W              (lcd_cell_t) (SEG_b|SEG_c|SEG_e|SEG_f|SEG_5|SEG_7)
#define CHAR_w              (lcd_cell_t) (SEG_c|SEG_d|SEG_e|SEG_6)
#define CHAR_X              (lcd_cell_t) (SEG_1|SEG_5|SEG_7|SEG_11)
#define CHAR_x              (lcd_cell_t) (SEG_1|SEG_5|SEG_7|SEG_11)              //same as X
#define CHAR_Y              (lcd_cell_t) (SEG_1|SEG_6|SEG_11)
#define CHAR_y              (lcd_cell_t) (SEG_b|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_Z              (lcd_cell_t) (SEG_a|SEG_d|SEG_1|SEG_7)
#define CHAR_z              (lcd_cell_t) (SEG_d|SEG_9|SEG_7)
#else
/* Now, a few other symbols - some look good, and some do not. Some
   also look the same as a digit. Beware! */
#define CHAR_a              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_b              (lcd_cell_t) (SEG_c|SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_c              (lcd_cell_t) (SEG_d|SEG_e|SEG_g)
#define CHAR_d              (lcd_cell_t) (SEG_b|SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_g              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_H              (lcd_cell_t) (SEG_b|SEG_c|SEG_e|SEG_f|SEG_g)
#define CHAR_h              (lcd_cell_t) (SEG_c|SEG_e|SEG_f|SEG_g)
#define CHAR_i              (lcd_cell_t) (SEG_c)
#define CHAR_L              (lcd_cell_t) (SEG_d|SEG_e|SEG_f)
#define CHAR_l              (lcd_cell_t) (SEG_b|SEG_c)
#define CHAR_n              (lcd_cell_t) (SEG_c|SEG_e|SEG_g)
#define CHAR_O              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_d|SEG_e|SEG_f)
#define CHAR_o              (lcd_cell_t) (SEG_c|SEG_d|SEG_e|SEG_g)
#define CHAR_P              (lcd_cell_t) (SEG_a|SEG_b|SEG_e|SEG_f|SEG_g)
#define CHAR_q              (lcd_cell_t) (SEG_a|SEG_b|SEG_c|SEG_f|SEG_g)
#define CHAR_r              (lcd_cell_t) (SEG_e|SEG_g)
#define CHAR_S              (lcd_cell_t) (SEG_a|SEG_c|SEG_d|SEG_f|SEG_g)
#define CHAR_t              (lcd_cell_t) (SEG_d|SEG_e|SEG_f|SEG_g)
#define CHAR_u              (lcd_cell_t) (SEG_c|SEG_d|SEG_e)
#define CHAR_U              (lcd_cell_t) (SEG_b|SEG_c|SEG_d|SEG_e|SEG_f)
#define CHAR_MINUS          (lcd_cell_t) (SEG_g)
#define CHAR_EQUALS         (lcd_cell_t) (SEG_d|SEG_g)
#endif

#endif

#if 0
uint16_t ASCII_table[] =
{
    0,
    CHAR_DOUBLEQUOTE,
    0,
    CHAR_DOLLAR,
    0,
    0,
    CHAR_QUOTE,
    0,
    0,
    CHAR_ASTERISK,
    CHAR_PLUS,
    0,
    CHAR_MINUS,
    0,
    CHAR_SLASH,
    CHAR_0,
    CHAR_1,
    CHAR_2,
    CHAR_3,
    CHAR_4,
    CHAR_5,
    CHAR_6,
    CHAR_7,
    CHAR_8,
    CHAR_9,
    0,
    0,
    CHAR_LT,
    CHAR_EQUALS,
    CHAR_GT,
    CHAR_QUESTION,
    0,
    CHAR_A,
    CHAR_B,
    CHAR_C,
    CHAR_D,
    CHAR_E,
    CHAR_F,
    CHAR_G,
    CHAR_H,
    CHAR_I,
    CHAR_J,
    CHAR_K,
    CHAR_L,
    CHAR_M,
    CHAR_N,
    CHAR_O,
    CHAR_P,
    CHAR_Q,
    CHAR_R,
    CHAR_S,
    CHAR_T,
    CHAR_U,
    CHAR_V,
    CHAR_W,
    CHAR_X,
    CHAR_Y,
    CHAR_Z,
    CHAR_LEFTBRACKET,
    CHAR_BACKSLASH,
    CHAR_RIGHTBRACKET,
    CHAR_CARET,
    CHAR_UNDERSCORE,
    CHAR_BACKQUOTE,
    CHAR_a,
    CHAR_b,
    CHAR_c,
    CHAR_d,
    CHAR_e,
    CHAR_f,
    CHAR_g,
    CHAR_h,
    CHAR_i,
    CHAR_j,
    CHAR_k,
    CHAR_l,
    CHAR_m,
    CHAR_n,
    CHAR_o,
    CHAR_p,
    CHAR_q,
    CHAR_r,
    CHAR_s,
    CHAR_t,
    CHAR_u,
    CHAR_v,
    CHAR_w,
    CHAR_x,
    CHAR_y,
    CHAR_z,
    0,   
    CHAR_VERTICALBAR,
    0,
    0,
    0
};
#endif
