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
//  File: basic_display.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-basic-display.c,v 1.28 2009/04/27 06:21:22 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 simple LCD display routines for e-meters
//
#include <stdint.h>
#include <stdlib.h>
#include <io.h>
#include <emeter-toolkit.h>

#include "emeter-structs.h"

#if defined(MULTI_RATE_SUPPORT)
#include "emeter-multirate.h"
#endif

#if defined(__MSP430__)  &&  defined(CUSTOM_LCD_SUPPORT)
#include <lcd-segments.h>
/* Define hex digits and the minus sign to match the allocation of segments we are using. */
const lcd_cell_t lcd_digit_table[] =
{
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
    CHAR_A,
    CHAR_B,
    CHAR_C,
    CHAR_D,
    CHAR_E,
    CHAR_F
};

const lcd_cell_t char_minus = CHAR_MINUS;

const int lcd_cells = LCD_CELLS;
const int lcd_pos_base = LCD_POS_BASE;
const int lcd_pos_step = LCD_POS_STEP;
#endif

#if defined(MULTI_RATE_SUPPORT)
uint8_t info_step;
uint8_t info_substep;
#endif

#if defined(__MSP430__)  &&  defined(BASIC_LCD_SUPPORT)
#include <lcd-segments.h>
/* Define hex digits and the minus sign to match the allocation of segments we are using. */
const lcd_cell_t lcd_digit_table[] =
{
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
    CHAR_A,
    CHAR_B,
    CHAR_C,
    CHAR_D,
    CHAR_E,
    CHAR_F
};

    #if defined(USE_STARBURST)
const lcd_cell_t lcd_alpha_table[] =
{
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
    CHAR_Z
};
    #endif

    #if defined(ASCII_LCD)
static const lcd_cell_t lcd_ascii_table[] =
{
    CHAR_SPACE,
    CHAR_SPACE,         //exclamation
    CHAR_DOUBLEQUOTE,
    CHAR_SPACE,         //hash
    CHAR_DOLLAR,
    CHAR_SPACE,         //percent
    CHAR_SPACE,         //ampersand
    CHAR_QUOTE,
    CHAR_LEFTBRACKET,
    CHAR_RIGHTBRACKET,
    CHAR_ASTERISK,
    CHAR_PLUS,
    CHAR_SPACE,         //comma
    CHAR_MINUS,
    CHAR_SPACE,         //full stop
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
    CHAR_SPACE,         //colon
    CHAR_SPACE,         //semi-colon
    CHAR_LT,
    CHAR_EQUALS,
    CHAR_GT,
    CHAR_QUESTION,
    CHAR_SPACE,         //at sign
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
    CHAR_C,
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
    CHAR_LEFTBRACKET,
    CHAR_VERTICALBAR,
    CHAR_RIGHTBRACKET,
    CHAR_SPACE,         //squiggle
    CHAR_SPACE          //delete
};

void lcd_text(char *s, int pos)
{
    int x;

    if (lcd_pos_step < 0)
        pos = -pos;
    if (abs(lcd_pos_step) > 1)
        pos <<= 1;
    pos = lcd_pos_base + pos;
    while (*s)
    {
        x = lcd_ascii_table[*s++ - 0x20];
        LCDMEM[pos] = x & 0xFF;
        pos += (lcd_pos_step >> 1);
        LCDMEM[pos] = x >> 8;
        pos += (lcd_pos_step >> 1);
    }
}
    #endif

const lcd_cell_t char_minus = CHAR_MINUS;

const int lcd_cells = LCD_CELLS;
const int lcd_pos_base = LCD_POS_BASE;
const int lcd_pos_step = LCD_POS_STEP;

static const lcd_cell_t lcd_high[TEXT_MESSAGE_LENGTH] =
{
    CHAR_H,
    CHAR_i,
    CHAR_g,
    CHAR_h,
    CHAR_SPACE,
    CHAR_SPACE,
    #if TEXT_MESSAGE_LENGTH == 7
    CHAR_SPACE
    #endif
};

static const lcd_cell_t lcd_startup[TEXT_MESSAGE_LENGTH] =
{
    #if defined(USE_STARBURST)
    CHAR_S,
    CHAR_T,
    CHAR_A,
    CHAR_R,
    CHAR_T,
    CHAR_SPACE,
    #else
    CHAR_S,
    CHAR_t,
    CHAR_a,
    CHAR_r,
    CHAR_t,
    CHAR_SPACE,
        #if TEXT_MESSAGE_LENGTH == 7
    CHAR_SPACE
        #endif
    #endif
};

static const lcd_cell_t lcd_no_power[TEXT_MESSAGE_LENGTH] =
{
    CHAR_b,
    CHAR_l,
    CHAR_SPACE,
    CHAR_o,
    CHAR_u,
    CHAR_t,
    #if TEXT_MESSAGE_LENGTH == 7
    CHAR_SPACE
    #endif
};

static const lcd_cell_t lcd_4v2_power[TEXT_MESSAGE_LENGTH] =
{
    CHAR_SPACE,
    CHAR_SPACE,
    CHAR_SPACE,
    CHAR_4,
    #if defined(USE_STARBURST)
    CHAR_V,
    #else
    CHAR_U,
    #endif
    CHAR_2,
    #if TEXT_MESSAGE_LENGTH == 7
    CHAR_SPACE
    #endif
};

static const lcd_cell_t lcd_normal_power[TEXT_MESSAGE_LENGTH] =
{
    CHAR_SPACE,
    CHAR_SPACE,
    CHAR_SPACE,
    CHAR_8,
    #if defined(USE_STARBURST)
    CHAR_V,
    #else
    CHAR_U,
    #endif
    CHAR_4,
    #if TEXT_MESSAGE_LENGTH == 7
    CHAR_SPACE
    #endif
};

    #if defined(__MSP430__)
enum
{
    DISPLAY_ITEM_SELECT_RESTART = -6,
    DISPLAY_ITEM_SELECT_TOTAL = -5,
    DISPLAY_ITEM_SELECT_NEUTRAL = -4,
    DISPLAY_ITEM_SELECT_PHASE_3 = -3,
    DISPLAY_ITEM_SELECT_PHASE_2 = -2,
    DISPLAY_ITEM_SELECT_PHASE_1 = -1,
    DISPLAY_ITEM_ACTIVE_ENERGY,
    DISPLAY_ITEM_ACTIVE_POWER,
    #if defined(MAINS_FREQUENCY_SUPPORT)
    DISPLAY_ITEM_MAINS_FREQUENCY,
    #endif
    #if defined(IRMS_SUPPORT)
    DISPLAY_ITEM_CURRENT,
    #endif
    #if defined(VRMS_SUPPORT)
    DISPLAY_ITEM_VOLTAGE,
    #endif
    #if defined(POWER_FACTOR_SUPPORT)
    DISPLAY_ITEM_POWER_FACTOR,
    #endif
    #if defined(REACTIVE_POWER_SUPPORT)
    DISPLAY_ITEM_REACTIVE_POWER,
    DISPLAY_ITEM_REACTIVE_ENERGY,
    #endif
    #if defined(APPARENT_POWER_SUPPORT)
    DISPLAY_ITEM_VA,
    #endif
    #if defined(RTC_SUPPORT)
    DISPLAY_ITEM_DATE,
    DISPLAY_ITEM_TIME,
    #endif
    #if defined(TEMPERATURE_SUPPORT)
    DISPLAY_ITEM_TEMPERATURE,
    #endif
    #if defined(MULTI_RATE_SUPPORT)
    DISPLAY_ITEM_CURRENT_TARIFF
    #endif
};

enum
{
        #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
            #if !defined(TWO_LINE_LCD)
    DISPLAY_STAGE_PHASE_ACTIVE_POWER,
            #endif
    DISPLAY_STAGE_PHASE_ENERGY,
        #endif
        #if defined(REACTIVE_POWER_SUPPORT)
    DISPLAY_STAGE_REACTIVE_POWER,
        #endif
        #if defined(APPARENT_POWER_SUPPORT)
    DISPLAY_STAGE_VA,
        #endif
        #if defined(POWER_FACTOR_SUPPORT)
    DISPLAY_STAGE_POWER_FACTOR,
        #endif
        #if defined(VRMS_SUPPORT)
    DISPLAY_STAGE_VOLTAGE,
        #endif
        #if defined(IRMS_SUPPORT)
    DISPLAY_STAGE_CURRENT,
        #endif
        #if defined(MAINS_FREQUENCY_SUPPORT)
    DISPLAY_STAGE_MAINS_FREQUENCY,
        #endif
        #if !defined(SINGLE_PHASE)
    DISPLAY_STAGE_PHASE_LAST,
        #endif
        #if !defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(IRMS_SUPPORT)
    DISPLAY_STAGE_NEUTRAL_CURRENT,
        #endif
        #if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
    DISPLAY_STAGE_TOTAL_ACTIVE_POWER,
            #if !defined(TWO_LINE_LCD)
    DISPLAY_STAGE_TOTAL_ACTIVE_ENERGY,
            #endif
        #endif
        #if defined(RTC_SUPPORT)
    DISPLAY_STAGE_DATE,
            #if !defined(DEDICATED_TIME_FIELD)  &&  !defined(TWO_LINE_LCD)
    DISPLAY_STAGE_TIME,
            #endif
        #endif
        #if defined(TEMPERATURE_SUPPORT)
    DISPLAY_STAGE_TEMPERATURE,
        #endif
        #if defined(MULTI_RATE_SUPPORT)
    DISPLAY_STAGE_CURRENT_TARIFF,
        #endif
    DISPLAY_STAGE_LAST
};

        #if !defined(USE_STARBURST)
#define LCDcharsx LCDchars
        #else
void LCDcharsx(const lcd_cell_t *s, int pos, int len)
{
    if (lcd_pos_step < 0)
        pos = -pos;
    if (abs(lcd_pos_step) > 1)
        pos <<= 1;
    pos = lcd_pos_base + pos;
    for (  ;  len > 0;  --len)
    {
        LCDMEM[pos] = *s & 0xFF;
        pos += (lcd_pos_step >> 1);
        LCDMEM[pos] = *s >> 8;
        s++;
        pos += (lcd_pos_step >> 1);
    }
}
        #endif

void display_power_fail_message(void)
{
    LCDchar(CHAR_SPACE, 1);
    LCDcharsx(lcd_no_power, INFO_POSITION, INFO_CHARS);
}

void display_startup_message(void)
{
    LCDchar(CHAR_SPACE, 1);
    LCDcharsx(lcd_startup, INFO_POSITION, INFO_CHARS);
}

void display_power_4v2_message(void)
{
    LCDchar(CHAR_SPACE, 1);
    LCDcharsx(lcd_4v2_power, INFO_POSITION, INFO_CHARS);
}

void display_power_normal_message(void)
{
    LCDchar(CHAR_SPACE, 1);
    LCDcharsx(lcd_normal_power, INFO_POSITION, INFO_CHARS);
}

static void LCDicon(int pos, int on)
{
    LCDmodify_char(1 << (pos >> 5), pos & 0x1F, on);
}

void display_clear_periphery(void)
{
    /* Clear all the symbols around the display, which we are not using */
    custom_lcd_clear_periphery();
}

void display_clear_line_1(void)
{
    int i;

    /* Clear the digits */
    for (i = FIRST_ROW_START;  i < FIRST_ROW_START + FIRST_ROW_CHARS;  i++)
        LCDchar(CHAR_SPACE, i);
    /* Clear the associated tags */
    custom_lcd_clear_line_1_tags();
    display_clear_periphery();
}

        #if defined(TWO_LINE_LCD)
void display_clear_line_2(void)
{
    int i;
    
    /* Clear the digits */
    for (i = SECOND_ROW_START;  i < SECOND_ROW_START + SECOND_ROW_CHARS;  i++)
        LCDchar(CHAR_SPACE, i);
    /* Clear the associated tags */
    custom_lcd_clear_line_2_tags();
    display_clear_periphery();
}
        #else
#define display_clear_line_2() /**/
        #endif

        #if !defined(SINGLE_PHASE)  &&  defined(ICON_PHASE_A)  &&  defined(ICON_PHASE_B)  &&  defined(ICON_PHASE_C)
void display_phase_icon(int ph)
{
    int i;
    static const uint8_t phase_icons[NUM_PHASES] =
    {
        ICON_PHASE_A,
        ICON_PHASE_B,
        ICON_PHASE_C
    };

    for (i = 0;  i < 3;  i++)
        LCDicon(phase_icons[i], i == ph);
}
        #else
#define display_phase_icon(x) /**/
        #endif

static void LCDoverrange(void)
{
    LCDcharsx(lcd_high, INFO_POSITION, INFO_CHARS);
}

        #if defined(MAINS_FREQUENCY_SUPPORT)
static __inline__ void display_mains_frequency(int ph)
{
    int32_t x;

    /* Display mains frequency in 0.1Hz or 0.01Hz increments */
    x = current_mains_frequency(ph);
    LCDdecu32(x, FIRST_FREQUENCY_POSITION, FREQUENCY_DIGITS, FREQUENCY_RESOLUTION);
            #if defined(ICON_FREQUENCY_DECIMAL_POINT)
    LCDicon(ICON_FREQUENCY_DECIMAL_POINT, TRUE);
            #endif
            #if defined(ICON_HERTZ)
    LCDicon(ICON_HERTZ, TRUE);
            #endif
            #if defined(ICON_FREQUENCY)
    LCDicon(ICON_FREQUENCY, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_F, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(VRMS_SUPPORT)
static void display_vrms(int ph)
{
    int32_t x;

    /* Display RMS voltage in 0.1V or 0.01V increments */
    x = current_rms_voltage(ph);
    if (x < 0)
    {
        LCDoverrange();
    }
    else
    {
            #if defined(VOLTAGE_DISPLAY_DIVISOR)
        x /= VOLTAGE_DISPLAY_DIVISOR;
            #endif
        LCDdecu32(x, FIRST_VOLTAGE_POSITION, VOLTAGE_DIGITS, VOLTAGE_RESOLUTION);
            #if defined(ICON_VOLTAGE_DECIMAL_POINT)
        LCDicon(ICON_VOLTAGE_DECIMAL_POINT, TRUE);
            #endif
            #if defined(ICON_V)
    LCDicon(ICON_V, TRUE);
            #endif
    }
            #if defined(ICON_VOLTAGE)
    LCDicon(ICON_VOLTAGE, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
                #if defined(USE_STARBURST)
    LCDchar(CHAR_V, DISPLAY_TYPE_POSITION);
                #else
    LCDchar(CHAR_U, DISPLAY_TYPE_POSITION);
                #endif
            #endif
}
        #endif

        #if defined(IRMS_SUPPORT)
static __inline__ void display_irms(int ph)
{
    int32_t x;

    /* Display RMS current in 1mA increments */
    x = current_rms_current(ph);
    if (x < 0)
    {
        LCDoverrange();
    }
    else
    {
            #if defined(CURRENT_DISPLAY_DIVISOR)
        x /= CURRENT_DISPLAY_DIVISOR;
            #endif
        LCDdecu32(x, FIRST_CURRENT_POSITION, CURRENT_DIGITS, CURRENT_RESOLUTION);
            #if defined(ICON_CURRENT_DECIMAL_POINT)
        LCDicon(ICON_CURRENT_DECIMAL_POINT, TRUE);
            #endif
            #if defined(ICON_A)
    LCDicon(ICON_A, TRUE);
            #endif
    }
            #if defined(ICON_CURRENT)
    LCDicon(ICON_CURRENT, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_C, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)  ||  defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
static __inline__ void display_consumed_active_energy(int ph)
{
    int32_t x;

    //Display per phase or total consumed energy in 0.1kWh increments
    x = current_consumed_active_energy(ph);
            #if ACTIVE_ENERGY_RESOLUTION == 1
    x /= 10;
            #else
    x /= 100;
            #endif
    LCDdecu32(x, FIRST_ACTIVE_ENERGY_POSITION, ACTIVE_ENERGY_DIGITS, ACTIVE_ENERGY_RESOLUTION);
            #if defined(ICON_ACTIVE_ENERGY_DECIMAL_POINT)
    LCDicon(ICON_ACTIVE_ENERGY_DECIMAL_POINT, TRUE);
            #endif
            #if defined(ICON_kWH)
    LCDicon(ICON_kWH, TRUE);
            #elif defined(ICON_kW)  &&  defined(ICON_H_FOR_kW)
    LCDicon(ICON_kW, TRUE);
    LCDicon(ICON_H_FOR_kW, TRUE);
            #endif
            #if defined(ICON_ACTIVE_ENERGY)
    LCDicon(ICON_ACTIVE_ENERGY, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_E, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(REACTIVE_POWER_SUPPORT)
static __inline__ void display_consumed_reactive_energy(int ph)
{
    int32_t x;

    //Display per phase or total consumed energy in 0.1kWh increments
    x = current_consumed_reactive_energy(ph);
            #if REACTIVE_ENERGY_RESOLUTION == 1
    x /= 10;
            #else
    x /= 100;
            #endif
    LCDdecu32(x, FIRST_REACTIVE_ENERGY_POSITION, REACTIVE_ENERGY_DIGITS, REACTIVE_ENERGY_RESOLUTION);
            #if defined(ICON_ACTIVE_ENERGY_DECIMAL_POINT)
    LCDicon(ICON_REACTIVE_ENERGY_DECIMAL_POINT, TRUE);
            #endif
            #if defined(ICON_kvarH)
    LCDicon(ICON_kvarH, TRUE);
            #elif defined(ICON_kvar)  &&  defined(ICON_H_FOR_kvar)
    LCDicon(ICON_kvar, TRUE);
    LCDicon(ICON_H_FOR_kvar, TRUE);
            #endif
            #if defined(ICON_REACTIVE_ENERGY)
    LCDicon(ICON_REACTIVE_ENERGY, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_E, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)  ||  defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
static __inline__ void display_active_power(int ph)
{
    int32_t x;

    //Display per phase or total power in 0.01W increments
    x = current_active_power(ph);
            #if POWER_RESOLUTION == 1
    /* Needs scaling */
    x /= 10;
            #endif
    LCDdecu32(x, FIRST_ACTIVE_POWER_POSITION, ACTIVE_POWER_DIGITS, ACTIVE_POWER_RESOLUTION);
            #endif
            #if defined(ICON_ACTIVE_POWER_DECIMAL_POINT)
    LCDicon(ICON_ACTIVE_POWER_DECIMAL_POINT, TRUE);
            #endif
            #if defined(ICON_ACTIVE_POWER)
    LCDicon(ICON_ACTIVE_POWER, TRUE);
            #endif
            #if defined(ICON_kW)
    LCDicon(ICON_kW, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_P, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(REACTIVE_POWER_SUPPORT)
static __inline__ void display_reactive_power(int ph)
{
    int32_t x;

    //Display reactive power in 0.01W increments
    x = current_reactive_power(ph);
            #if POWER_RESOLUTION == 1
    /* Needs scaling */
    x /= 10;
            #endif
    LCDdecu32(x, FIRST_REACTIVE_POWER_POSITION, REACTIVE_POWER_DIGITS, REACTIVE_POWER_RESOLUTION);
    LCDicon(ICON_REACTIVE_POWER_DECIMAL_POINT, TRUE);
            #if defined(ICON_kvar)
    LCDicon(ICON_kvar, TRUE);
            #endif
            #if defined(ICON_REACTIVE_POWER)
    LCDicon(ICON_REACTIVE_POWER, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
                #if defined(USE_STARBURST)
    LCDchar(CHAR_R, DISPLAY_TYPE_POSITION);
                #else
    LCDchar(CHAR_r, DISPLAY_TYPE_POSITION);
                #endif
            #endif
}
        #endif

        #if defined(APPARENT_POWER_SUPPORT)
static __inline__ void display_apparent_power(int ph)
{
    int32_t x;

    //Display apparent (VA) power in 0.01W increments
    x = current_apparent_power(ph);
            #if POWER_RESOLUTION == 1
    /* Needs scaling */
    x /= 10;
            #endif
    LCDdecu32(x, FIRST_APPARENT_POWER_POSITION, APPARENT_POWER_DIGITS, APPARENT_POWER_RESOLUTION);
    LCDicon(ICON_APPARENT_POWER_DECIMAL_POINT, TRUE);
            #if defined(ICON_kVA)
    LCDicon(ICON_kVA, TRUE);
            #endif
            #if defined(ICON_APPARENT_POWER)
    LCDicon(ICON_APPARENT_POWER, TRUE);
            #elif defined(DISPLAY_TYPE_POSITION)
                #if defined(USE_STARBURST)
    LCDchar(CHAR_A, DISPLAY_TYPE_POSITION);
                #else
    LCDchar(CHAR_n, DISPLAY_TYPE_POSITION);
                #endif
            #endif
}
        #endif

        #if defined(IRMS_SUPPORT)  &&  defined(VRMS_SUPPORT)  &&  defined(POWER_FACTOR_SUPPORT)
static __inline__ void display_power_factor(int ph)
{
    int32_t x;

    x = current_power_factor(ph);
    if (x < 0)
    {
        LCDchar(CHAR_L, FIRST_POWER_FACTOR_POSITION);
        x = -x;
    }
    else
    {
        LCDchar(CHAR_C, FIRST_POWER_FACTOR_POSITION);
    }
    LCDdec16(x/10, FIRST_POWER_FACTOR_POSITION + 1, POWER_FACTOR_DIGITS - 2, POWER_FACTOR_RESOLUTION);
            #if defined(ICON_POWER_FACTOR_DECIMAL_POINT)
    LCDicon(ICON_POWER_FACTOR_DECIMAL_POINT, FALSE);
            #endif
            #if defined(ICON_COS_PHI)
    LCDicon(ICON_COS_PHI, TRUE);
            #elif  defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_F, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(RTC_SUPPORT)
static /*__inline__*/ void display_date(int year, int month, int day)
{
    LCDdecu16(year, YEAR_POSITION, 2, 1);
    LCDdecu16(month, MONTH_POSITION, 2, 1);
    LCDdecu16(day, DAY_POSITION, 2, 1);
    #if defined(ICON_DATE_COLON_1)
    LCDicon(ICON_DATE_COLON_1, TRUE);
    #endif
    #if defined(ICON_DATE_COLON_1A)
    LCDicon(ICON_DATE_COLON_1A, TRUE);
    #endif
    #if defined(ICON_DATE_COLON_2)
    LCDicon(ICON_DATE_COLON_2, TRUE);
    #endif
    #if defined(ICON_DATE_COLON_2A)
    LCDicon(ICON_DATE_COLON_2A, TRUE);
    #endif
}

static /*__inline__*/ void display_time(int hour, int minute, int second)
{
    LCDdecu16(hour, HOUR_POSITION, 2, 1);
    LCDdecu16(minute, MINUTE_POSITION, 2, 1);
                #if defined(SECONDS_POSITION)
    LCDdecu16(second, SECONDS_POSITION, 2, 1);
                #endif
                #if defined(ICON_TIME_COLON_1)
    LCDicon(ICON_TIME_COLON_1, TRUE);
                #endif
                #if defined(ICON_TIME_COLON_1A)
    LCDicon(ICON_TIME_COLON_1A, TRUE);
                #endif
                #if defined(ICON_TIME_COLON_2)
    LCDicon(ICON_TIME_COLON_2, TRUE);
                #endif
                #if defined(ICON_TIME_COLON_2A)
    LCDicon(ICON_TIME_COLON_2A, TRUE);
                #endif
}

static /*__inline__*/ void display_current_date(void)
{
            #if !defined(ICON_DATE)  &&  defined(DISPLAY_TYPE_POSITION)
                #if defined(USE_STARBURST)
    LCDchar(CHAR_D, DISPLAY_TYPE_POSITION);
                #else
    LCDchar(CHAR_d, DISPLAY_TYPE_POSITION);
                #endif
                #if FIRST_POSITION > 2
    LCDchar(CHAR_t, DISPLAY_TYPE_POSITION + 1);
                #endif
            #endif
            #if defined(ZAP_COLON_CELL)
    LCDchar(CHAR_SPACE, ZAP_COLON_CELL);
            #endif
    display_date(rtc.year, rtc.month, rtc.day);
            #if defined(ICON_DATE)
    LCDicon(ICON_DATE, TRUE);
            #endif
}

static __inline__ void display_current_time(void)
{
            #if defined(DEDICATED_TIME_FIELD)
    uint8_t x;

    /*TODO: This is fudged, as the time field has digits in
            reverse order from the main field. */
    LCDdecu16(rtc.hour, DEDICATED_TIME_FIELD + 2, 2, 1);
    LCDdecu16(rtc.minute, DEDICATED_TIME_FIELD, 2, 1);
    x = LCDMEM[12];
    LCDMEM[12] = LCDMEM[11];
    LCDMEM[11] = x;
    x = LCDMEM[14];
    LCDMEM[14] = LCDMEM[13];
    LCDMEM[13] = x;
                #if defined(ICON_TIME_FIELD_TIME)
    LCDicon(ICON_TIME_FIELD_TIME, TRUE);
                #endif
                #if defined(ICON_TIME_FIELD_COLON)
    LCDicon(ICON_TIME_FIELD_COLON, TRUE);
                #endif
            #elif defined(TWO_LINE_LCD)
                #if defined(ZAP_COLON_CELL)
    LCDchar(CHAR_SPACE, ZAP_COLON_CELL);
                #endif
    display_time(rtc.hour, rtc.minute, rtc.second);
                #if defined(ICON_TIME)
    LCDicon(ICON_TIME, TRUE);
                #endif
            #else
                #if !defined(ICON_TIME)  &&  defined(DISPLAY_TYPE_POSITION)
                    #if defined(USE_STARBURST)
    LCDchar(CHAR_T, DISPLAY_TYPE_POSITION);
                    #else
    LCDchar(CHAR_t, DISPLAY_TYPE_POSITION);
                    #endif
                    #if FIRST_POSITION > 2
    LCDchar(CHAR_i, DISPLAY_TYPE_POSITION + 1);
                    #endif
                #endif
                #if defined(ZAP_COLON_CELL)
    LCDchar(CHAR_SPACE, ZAP_COLON_CELL);
                #endif
    display_time(rtc.hour, rtc.minute, rtc.second);
                #if defined(ICON_TIME)
    LCDicon(ICON_TIME, TRUE);
                #endif
            #endif
}
        #endif

        #if defined(TEMPERATURE_SUPPORT)
static __inline__ void display_temperature(void)
{
    int32_t temp;

    /* Convert the temperature reading to degrees C */
    /* DegC = ((((int32_t) ADC_result - 1615)*704)/4095); */
    /* We filtered in a way that multiplied the temperature reading by 8 */
    /* Therefore, to get a result in 0.1 degree C steps we do this... */
    temp = temperature - nv_parms.seg_a.s.temperature_offset;
    temp *= nv_parms.seg_a.s.temperature_scaling;
    temp >>= 16;
    LCDdec32(temp, FIRST_TEMPERATURE_POSITION, TEMPERATURE_DIGITS, TEMPERATURE_RESOLUTION);
            #if !defined(ICON_TEMPERATURE)  &&  defined(DISPLAY_TYPE_POSITION)
    LCDchar(CHAR_C, DISPLAY_TYPE_POSITION);
            #endif
}
        #endif

        #if defined(MULTI_RATE_SUPPORT)
void display_current_tariff(void)
{
            #if !defined(ICON_DATE)  &&  defined(DISPLAY_TYPE_POSITION)
                #if defined(USE_STARBURST)
    LCDchar(CHAR_T, DISPLAY_TYPE_POSITION);
                #else
    LCDchar(CHAR_t, DISPLAY_TYPE_POSITION);
                #endif
    LCDchar(CHAR_a, DISPLAY_TYPE_POSITION + 1);
    LCDchar(CHAR_r, DISPLAY_TYPE_POSITION + 2);
    LCDchar(CHAR_r, DISPLAY_TYPE_POSITION + 3);
    LCDchar(CHAR_SPACE, DISPLAY_TYPE_POSITION + 4);
            #endif
    LCDdecu16(current_tariff + 1, DISPLAY_TYPE_POSITION + 5, 2, 0);
}

void display_tariff_holiday(void)
{
    int i;
    eeprom_holiday_t holiday;

    info_step = 0;
    for (i = info_step;  i < MULTIRATE_MAX_HOLIDAYS;  i++)
    {
        iicEEPROM_read(EEPROM_START_HOLIDAYS + i*sizeof(eeprom_holiday_t), (void *) &holiday, sizeof(holiday));
        if (holiday.year)
        {
            info_step = i;
            #if !defined(ICON_DATE)  &&  defined(DISPLAY_TYPE_POSITION)
                #if defined(USE_STARBURST)
            LCDchar(CHAR_D, DISPLAY_TYPE_POSITION);
                #else
            LCDchar(CHAR_d, DISPLAY_TYPE_POSITION);
                #endif
                #if FIRST_POSITION > 2
            LCDchar(CHAR_t, DISPLAY_TYPE_POSITION + 1);
                #endif
            #endif
            #if defined(ZAP_COLON_CELL)
            LCDchar(CHAR_SPACE, ZAP_COLON_CELL);
            #endif
            display_date(holiday.year, holiday.month, holiday.day);
            #if defined(ICON_DATE)
            LCDicon(ICON_DATE, TRUE);
            #endif
            return;
        }
    }
}
        #endif

void display_item(int item, int ph)
{
    switch (item)
    {
    case DISPLAY_ITEM_ACTIVE_ENERGY:
        display_consumed_active_energy(ph);
        break;
    case DISPLAY_ITEM_ACTIVE_POWER:
        display_active_power(ph);
        break;
    #if defined(REACTIVE_POWER_SUPPORT)
    case DISPLAY_ITEM_REACTIVE_POWER:
        display_reactive_power(ph);
        break;
    case DISPLAY_ITEM_REACTIVE_ENERGY:
        display_consumed_reactive_energy(ph);
        break;
    #endif
    #if defined(MAINS_FREQUENCY_SUPPORT)
    case DISPLAY_ITEM_MAINS_FREQUENCY:
        display_mains_frequency(ph);
        break;
    #endif
    #if defined(IRMS_SUPPORT)
    case DISPLAY_ITEM_CURRENT:
        display_irms(ph);
        break;
    #endif
    #if defined(VRMS_SUPPORT)
    case DISPLAY_ITEM_VOLTAGE:
        display_vrms(ph);
        break;
    #endif
    #if defined(POWER_FACTOR_SUPPORT)
    case DISPLAY_ITEM_POWER_FACTOR:
        display_power_factor(ph);
        break;
    #endif
    #if defined(APPARENT_POWER_SUPPORT)
    case DISPLAY_ITEM_VA:
        display_apparent_power(ph);
        break;
    #endif
    #if defined(RTC_SUPPORT)
    case DISPLAY_ITEM_DATE:
        display_current_date();
        break;
    case DISPLAY_ITEM_TIME:
        display_current_time();
        break;
    #endif
    #if defined(TEMPERATURE_SUPPORT)
    case DISPLAY_ITEM_TEMPERATURE:
        display_temperature();
        break;
    #endif
    #if defined(MULTI_RATE_SUPPORT)
    case DISPLAY_ITEM_CURRENT_TARIFF:
        display_current_tariff();
        break;
    #endif
    }
}

int display_step = 0;
int display_select = 0;

void update_display(void)
{
    static const int8_t display_steps[] =
    {
        /* The following display sequence table should be defined in the hardware specific
           header file. */
        DISPLAY_STEP_SEQUENCE
    };

    /* Deal with the next stage of the sequenced display */
    display_clear_line_1();
    display_clear_line_2();
    for (;;)
    {
        if (display_steps[display_step] < 0)
        {
            if (display_steps[display_step] == DISPLAY_ITEM_SELECT_RESTART)
            {
                display_step = 0;
                continue;
            }
            display_select = -1 - display_steps[display_step++];
            break;
        }
        display_phase_icon(display_select);
        display_item(display_steps[display_step++], display_select);
    }
    /* Now deal with things which are constantly displayed */
    #if defined(DEDICATED_TIME_FIELD)
    display_current_time();
    #endif
    #if defined(BATTERY_MONITOR_SUPPORT)  &&  defined(ICON_BATTERY)
    LCDicon(ICON_BATTERY, (meter_status & STATUS_BATTERY_OK));
    #endif
    #if defined(LIMP_MODE_SUPPORT)
    if (operating_mode == OPERATING_MODE_LIMP)
        LCDchar(CHAR_L, DISPLAY_TYPE_POSITION);
    #endif
}

    #if !defined(__GNUC__)
int16_t ram_exclusion_zone_start_;
int16_t ram_exclusion_zone_middle_;
int16_t ram_exclusion_zone_end_;
    #endif

#endif

#if defined(__MSP430__)
    #if defined(BASIC_KEYPAD_SUPPORT)
void keypad_handler(void)
{
    if ((key_states & KEY_1_DOWN))
    {
        update_display();
        key_states &= ~KEY_1_DOWN;
    }
    if ((key_states & KEY_1_REPEAT_DOWN))
    {
        update_display();
        key_states &= ~KEY_1_REPEAT_DOWN;
    }
}
    #endif
#endif

#if 0
    switch (info_section)
    {
    case 0:
        info_substep
        if (++info_step >= MULTIRATE_TARIFFS)
        {
            info_section++;
            info_step = 0;
        }
        break;
    case 1:
        if (++info_step >= MULTIRATE_MAX_HOLIDAYS)
        {
            info_section++;
            info_step = 0;
        }
        break;
    case 2:
        if (++info_step >= MULTIRATE_MAX_CUTOFF_DATES)
        {
            info_section++;
            info_step = 0;
        }
        break;
    case 3:
        if (++info_step >= MULTIRATE_HISTORIES)
        {
            info_section++;
            info_step = 0;
        }
        break;
    case 4:
        if (++info_step >= MULTIRATE_MAX_DAILY_PEAKS)
        {
            info_section++;
            info_step = 0;
        }
        break;
    }
#endif
