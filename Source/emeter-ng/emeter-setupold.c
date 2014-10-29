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
//  File: emeter-setup.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-setup.c,v 1.44 2009/04/27 09:46:32 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 setup routines for e-meters.
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
#include <stdint.h>
#include <string.h>
#if !defined(__MSP430__)
#include <stdio.h>
#include <stdlib.h>
#endif
#if defined(__GNUC__)
#include <signal.h>
#endif
#include <io.h>
#include <emeter-toolkit.h>

#include "emeter-structs.h"

#if defined(MESH_NET_SUPPORT)
#include "mesh_structure.h"
#endif

#if defined(__MSP430_HAS_SD16_2__)  ||  defined(__MSP430_HAS_SD16_3__)

#define SD16CONF0_FUDGE     0x70
#define SD16CONF1_FUDGE     0x48    //0x38

/*
 * Analog front-end initialization routine.
 *
 * Configures the sigma-delta ADC module as analog front-end for
 * a tamper-resistant meter using a current transformer and a
 * shunt as current sensors (see configuration of channel 0 and 1).
 */
static __inline__ void init_analog_front_end_normal(void)
{
    /*
     * First it makes sure that the Embedded Signal Processing is
     * disabled, otherwise it will not be possible to modify the
     * SD16 registers.
     */
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    /*
     * The general configuration of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD16SC;
    #endif

    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON;  /* Use internal reference */

    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;          /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;        /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;

        #if defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;    /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;     /* Set oversampling ratio to 256 (default) */
    SD16PRE_NEUTRAL = 0;
        #endif

    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_256 | SD16DF | SD16SC | SD16IE;      /* Set oversampling ratio to 256 (default) */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
    /*
     * \note
     * Please note, the oversampling ratio should be the same
     * for all channels. Default is 256.
     */
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
        #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
        #endif
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD16SC;
        #endif
    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON   /* Use internal reference */
            | SD16LP;
    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;                  /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;      /* Set oversampling ratio to 32 */
    SD16PRE_LIVE = 0;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;            /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;   /* Set oversampling ratio to 32 */
    SD16PRE_NEUTRAL = 0;
    #endif
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                          /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_32 | SD16DF | SD16SNGL | SD16IE;             /* Set oversampling ratio to 32 */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
}
    #endif

static __inline__ void disable_analog_front_end(void)
{
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD16CTL = 0;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
}
#endif

#if defined(__MSP430_HAS_SD24_A2__)  ||  defined(__MSP430_HAS_SD24_A3__)

#define SD24CONF0_FUDGE     0x70
#define SD24CONF1_FUDGE     0x48    //0x38

/*
 * Analog front-end initialization routine.
 *
 * Configures the sigma-delta ADC module as analog front-end for
 * a tamper-resistant meter using a current transformer and a
 * shunt as current sensors (see configuration of channel 0 and 1).
 */
static __inline__ void init_analog_front_end_normal(void)
{
    /*
     * The general configuration of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    SD16CCTL_VOLTAGE &= ~SD24SC;
    SD16CCTL_LIVE &= ~SD24SC;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD24SC;
    #endif

#if MCLK_DEF == 16
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_4   /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD24REFON;  /* Use internal reference */
#endif
#if MCLK_DEF == 8
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD24REFON;  /* Use internal reference */
#endif

    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;          /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD24OSR_256 | SD24DF | SD24GRP | SD24IE;        /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;

        #if defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;    /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD24OSR_256 | SD24DF | SD24GRP | SD24IE;     /* Set oversampling ratio to 256 (default) */
    SD16PRE_NEUTRAL = 0;
        #endif

    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE = SD24INCH_0 | VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD24OSR_256 | SD24DF | SD24SC | SD24IE;      /* Set oversampling ratio to 256 (default) */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD24CONF1_FUDGE)
    SD24CONF1 = SD24CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
    /*
     * \note
     * Please note, the oversampling ratio should be the same
     * for all channels. Default is 256.
     */
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
        #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
        #endif
    SD24CCTL_VOLTAGE &= ~SD24SC;
    SD24CCTL_LIVE &= ~SD24SC;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD24CCTL_NEUTRAL &= ~SD24SC;
        #endif
#if MCLK_DEF == 16
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_3   /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD24REFON   /* Use internal reference */
            | SD24LP;
#endif
#if MCLK_DEF == 8
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD24REFON   /* Use internal reference */
            | SD24LP;
#endif
    SD24INCTL_LIVE = SD24INCH_CURRENT | CURRENT_LIVE_GAIN;                  /* Set gain for channel 0 (I1) */
    SD24CCTL_LIVE = SD24OSR_32 | SD24DF | SD24SNGL | SD24GRP | SD24IE;      /* Set oversampling ratio to 32 */
    SD24PRE_LIVE = 0;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD24INCTL_NEUTRAL = SD24INCH_CURRENT | CURRENT_NEUTRAL_GAIN;            /* Set gain for channel 1 (I2) */
    SD24CCTL_NEUTRAL = SD24OSR_32 | SD24DF | SD24SNGL | SD24GRP | SD24IE;   /* Set oversampling ratio to 32 */
    SD24PRE_NEUTRAL = 0;
    #endif
    SD24INCTL_VOLTAGE = SD24INCH_0 | VOLTAGE_GAIN;                          /* Set gain for channel 2 (V) */
    SD24CCTL_VOLTAGE = SD24OSR_32 | SD24DF | SD24SNGL | SD24IE;             /* Set oversampling ratio to 32 */
    SD24PRE_VOLTAGE = 0;
    #if defined(SD24CONF0_FUDGE)
    SD24CONF0 = SD24CONF0_FUDGE;
    #endif
    #if defined(SD24CONF1_FUDGE)
    SD24CONF1 = SD24CONF1_FUDGE;
    #endif
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
}
    #endif

static __inline__ void disable_analog_front_end(void)
{
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD24CTL = 0;
    #if defined(SD24CONF0_FUDGE)
    SD24CONF0 = SD24CONF0_FUDGE;
    #endif
    #if defined(SD24CONF1_FUDGE)
    SD24CONF1 = SD24CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
}
#endif

#if defined(__MSP430_HAS_SD16_A3__)  ||  defined(__MSP430_HAS_SD16_A4__)
#error TBD
#endif

#if defined (__MSP430_HAS_SD24_B__)
//#define SD16CONF0_FUDGE     0x70
//#define SD16CONF1_FUDGE     0x48    //0x38

/*
 * Analog front-end initialization routine.
 *
 * Configures the sigma-delta ADC module as analog front-end for
 * a tamper-resistant meter using a current transformer and a
 * shunt as current sensors (see configuration of channel 0 and 1).
 */
static __inline__ void init_analog_front_end_normal(void)
{
    /*
     * First it makes sure that the Embedded Signal Processing is
     * disabled, otherwise it will not be possible to modify the
     * SD16 registers.
     */
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    /*
     * The general configuration of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    REFCTL0 = REFMSTR + REFVSEL_1 + REFON;         /* Enabale Reference = 2.0V */

    //SD16CCTL_VOLTAGE &= ~SD24SC;
    //SD16CCTL_LIVE &= ~SD24SC;
    SD24BCTL1 &= ~SD24GRP0SC;                                        /* SD16SC */
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
//    SD16CCTL_NEUTRAL &= ~SD24SC;
    #endif

#if MCLK_DEF == 16
    SD24BCTL0 = SD24SSEL__SMCLK  /* Clock is SMCLK */
            | SD24PDIV_3    /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD24DIV0
            | SD24REFS;     /* Use internal reference */
#endif
#if MCLK_DEF == 8
    SD24BCTL0 = SD24SSEL__SMCLK  /* Clock is SMCLK */
            | SD24PDIV_3    /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD24REFS;     /* Use internal reference */
#endif

    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;          /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD24DF_1 | SD24ALGN | SD24SCS_4;                /* Set oversampling ratio to 256 (default) */
    SD16BOSR_LIVE = 256 - 1;                                        /* Set oversampling ratio to 256 (default) */
///    SD16BIE |= SD16IE_LIVE;
    SD16PRE_LIVE = 0;

        #if defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;    /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD24DF_1 | SD24ALGN | SD24SCS_4;             /* Set oversampling ratio to 256 (default) */
    SD16BOSR_NEUTRAL = 256 -1;                                      /* Set oversampling ratio to 256 (default) */
///    SD16BIE |= SD16IE_NEUTRAL;
    SD16PRE_NEUTRAL = 0;
        #endif

    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE = SD24INCH_0 | VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD24DF_1 | SD24ALGN | SD24SCS_4;             /* Set oversampling ratio to 256 (default) */
    SD16BOSR_VOLTAGE = 256 -1;                                      /* Set oversampling ratio to 256 (default) */
    SD24BCTL1 |= SD24GRP0SC;                                        /* SD16SC */

#ifdef with_DMA
  extern int16_t dma_adc_buffer[6];

  SD24BCTL1_H = SD24DMA1_H;

  DMACTL0 = DMA0TSEL__SD24IFG;
  DMA0SA = (unsigned short) &SD24BMEML0;
  DMA0DA = (unsigned short) dma_adc_buffer;
  DMA0SZ = 6;
  DMA0CTL = DMASWDW + DMASRCINCR_3 + DMADSTINCR_3 + DMADT_5 + DMAEN + DMAIE; // word, inc dest, repeated block, Enable, Enable IE
#else
    SET_SD16IE_VOLTAGE;
#endif
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
    /*
     * \note
     * Please note, the oversampling ratio should be the same
     * for all channels. Default is 256.
     */
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
        #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
        #endif
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD16SC;
        #endif
    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON   /* Use internal reference */
            | SD16LP;
    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;                  /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;      /* Set oversampling ratio to 32 */
    SD16PRE_LIVE = 0;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;            /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;   /* Set oversampling ratio to 32 */
    SD16PRE_NEUTRAL = 0;
    #endif
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                          /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_32 | SD16DF | SD16SNGL | SD16IE;             /* Set oversampling ratio to 32 */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
}
    #endif

static __inline__ void disable_analog_front_end(void)
{
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD24BCTL0 = 0;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
}
#endif

#if defined(__MSP430_HAS_SD16_A6__)  ||  defined(__MSP430_HAS_SD16_A7__)
static __inline__ void init_analog_front_end_normal(void)
{
    int i;

    /*
     * The general configurations of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    #if defined(SINGLE_PHASE)
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
    #else
    SD16CCTL_VOLTAGE_1 &= ~SD16SC;
    SD16CCTL_VOLTAGE_2 &= ~SD16SC;
    SD16CCTL_VOLTAGE_3 &= ~SD16SC;
    SD16CCTL_CURRENT_1 &= ~SD16SC;
    SD16CCTL_CURRENT_2 &= ~SD16SC;
    SD16CCTL_CURRENT_3 &= ~SD16SC;
    #endif
    #if defined(__MSP430_HAS_SD16_A7__)
    SD16CCTL_NEUTRAL &= ~SD16SC;
    #endif

    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16XDIV_2  /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD16REFON;  /* Use internal reference */

    #if defined(SINGLE_PHASE)
    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 1 */
    SD16CCTL_LIVE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;

    #else
    SD16INCTL_CURRENT_1 = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 1 */
    SD16CCTL_CURRENT_1 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_CURRENT_1 = 0;

    SD16INCTL_CURRENT_2 = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 2 */
    SD16CCTL_CURRENT_2 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_CURRENT_2 = 0;

    SD16INCTL_CURRENT_3 = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 3 */
    SD16CCTL_CURRENT_3 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_CURRENT_3 = 0;
    #endif

    #if defined(__MSP430_HAS_SD16_A7__)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;        /* Set gain for channel neutral */
    SD16CCTL_NEUTRAL = SD16OSR_256 | SD16DF | SD16GRP; // | SD16IE;         /* Set oversampling ratio to 256 (default) */
    SD16PRE_NEUTRAL = 0;
    #endif

    /* Configure analog front-end channel 2 - Voltage */
    #if defined(SINGLE_PHASE)
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    #else
    SD16INCTL_VOLTAGE_1 = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE_1 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;
    SD16PRE_VOLTAGE_1 = 0;

    SD16INCTL_VOLTAGE_2 = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE_2 = SD16OSR_256 | SD16DF | SD16GRP; // | SD16IE;
    SD16PRE_VOLTAGE_2 = 0;

    SD16INCTL_VOLTAGE_3 = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE_3 = SD16OSR_256 | SD16DF | SD16GRP; // | SD16IE;
    SD16PRE_VOLTAGE_3 = 0;
    #endif

    #if defined(__MSP430_HAS_SD16_A3__)
    SD16CCTL2 |= SD16SC;
    #elif defined(__MSP430_HAS_SD16_A4__)
    SD16CCTL3 |= SD16SC;
    #elif defined(__MSP430_HAS_SD16_A6__)
    SD16CCTL5 |= SD16SC;
    #elif defined(__MSP430_HAS_SD16_A7__)
    SD16CCTL6 |= SD16SC;
    #endif

    #if defined(SINGLE_PHASE)
        chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #else
    for (i = 0;  i < NUM_PHASES;  i++)
        chan[i].metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #endif
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
}
    #endif

static __inline__ void disable_analog_front_end(void)
{
    int i;
    #if defined(SINGLE_PHASE)

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #else
    SD16INCTL_VOLTAGE_1 = 0;
    SD16CCTL_VOLTAGE_1 = 0;
    SD16PRE_VOLTAGE_1 = 0;

    SD16INCTL_VOLTAGE_2 = 0;
    SD16CCTL_VOLTAGE_2 = 0;
    SD16PRE_VOLTAGE_2 = 0;

    SD16INCTL_VOLTAGE_3 = 0;
    SD16CCTL_VOLTAGE_3 = 0;
    SD16PRE_VOLTAGE_3 = 0;

    SD16INCTL_CURRENT_1 = 0;
    SD16CCTL_CURRENT_1 = 0;
    SD16PRE_CURRENT_1 = 0;

    SD16INCTL_CURRENT_2 = 0;
    SD16CCTL_CURRENT_2 = 0;
    SD16PRE_CURRENT_2 = 0;

    SD16INCTL_CURRENT_3 = 0;
    SD16CCTL_CURRENT_3 = 0;
    SD16PRE_CURRENT_3 = 0;
    #endif
    #if defined(__MSP430_HAS_SD16_A7__)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD16CTL = 0;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if defined(SINGLE_PHASE)
        chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #else
    for (i = 0;  i < NUM_PHASES;  i++)
        chan[i].metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #endif
}
#endif

#if defined(__MSP430__)
void system_setup(void)
{
    #if !defined(SINGLE_PHASE)
    struct phase_parms_s *phase;
    struct phase_nv_parms_s const *phase_nv;
    int j;
    #endif

    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;

    #if defined(__MSP430_HAS_DCO__)
    /* Set up the DCO clock */
    BCSCTL1 |= (RSEL0 | RSEL1 | RSEL2); /* Select the highest nominal freq */
    BCSCTL2 |= DCOR;                    /* Select the external clock control resistor pin */
    DCOCTL = 0xFF;                      /* Select the highest speed. */
    #endif
    #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    FLL_CTL0 |= OSCCAP_3;               /* Configure load caps */
        #if defined(XT2OFF)
    FLL_CTL1 |= XT2OFF;
        #endif
    SCFI0 = SCFI0_LOW;
    SCFQCTL = SCFQCTL_LOW;
    FLL_CTL0 |= DCOPLUS;
    /* There seems no benefit in waiting for the FLL to settle at this point. */
    #endif

    #if defined(__MSP430_HAS_UCS__)
#include "hal_UCS.h"
#include "hal_PMM.h"
    #if defined (__MSP430_HAS_AUX_SUPPLY__)
    
 
         AUX3CHCTL = AUXCHKEY | AUXCHC_1 | AUXCHV_1 /*| AUXCHEN*/;  // Enable Charger for AUX3 to enable RTC
        
    #endif
#if MCLK_DEF > 8
    SetVCore(3);      // Set VCore to level 0
#endif
    SetVCore(3);  
    LFXT_Start(XT1DRIVE_3);

    Init_FLL_Settle(MCLK_DEF*8388608/8/1000, MCLK_DEF*32768*32/32768);
    /* There seems no benefit in waiting for the FLL to settle at this point. */
    #endif
   
    #if defined (__MSP430_HAS_AUX_SUPPLY__)
        PMMCTL0_H = PMMPW_H;    
        SVSMHCTL|=SVSMHRRL_4;

        AUXCTL0 = AUXKEY;
        AUXCTL1 |= AUX2MD;
        AUXCTL2 |=AUX0LVL_6+AUX1LVL_5;
        PMMCTL0_H = 0;    
    #endif
        
    

    #if defined(__MSP430_HAS_RTC_C__)
#ifdef LOCKBAK
   while(BAKCTL & LOCKBAK){                // unlock Backup Subsystem
      BAKCTL &= ~LOCKBAK;
    }
#endif
    /* Basic timer setup */
    /* Set ticker to 32768/(256*256) */
   RTCPS0CTL = RT0IP_7;                    //  / 256
   RTCPS1CTL = RT1IP_6 + RT1PSIE;          //  /128 / 256 = / 32768 -> sec interval

    /* We want a real watchdog function, but it doesn't have to be fast. */
    /* Use the longest timer - 1s */
        #if defined(USE_WATCHDOG)
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
        #else
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        #endif
    #else
    #if defined(__MSP430_HAS_BT__)  ||  defined(__MSP430_HAS_BT_RTC__)
    /* Basic timer setup */
    /* Set ticker to 32768/(256*256) */
        #if defined(__MSP430_HAS_BT__)
    BTCTL = BT_fLCD_DIV64 | BT_fCLK2_DIV128 | BT_fCLK2_ACLK_DIV256;
        #else
    BTCTL = BT_fCLK2_DIV128 | BT_fCLK2_ACLK_DIV256;
        #endif
    /* Enable the 1 second counter interrupt */
    IE2 |= BTIE;

    /* We want a real watchdog function, but it doesn't have to be fast. */
    /* Use the longest timer - 1s */
        #if defined(USE_WATCHDOG)
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
        #else
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        #endif
    #else
    IE1 |= WDTIE;               /* Enable the WDT interrupt */
    #endif
    #endif

    #if (defined(BASIC_LCD_SUPPORT)  ||  defined(CUSTOM_LCD_SUPPORT))
    LCDinit();
    custom_lcd_init();
        #if defined(LCD_INIT)  &&  defined(__MSP430_HAS_LCD4__)
    LCDCTL = LCD_INIT;
        #endif
        #if defined(LCDACTL_INIT)  &&  defined(__MSP430_HAS_LCD_A__)
    LCDACTL = LCDACTL_INIT;
    LCDAPCTL0 = LCDAPCTL0_INIT;
    LCDAPCTL1 = LCDAPCTL1_INIT;
    LCDAVCTL0 = LCDAVCTL0_INIT;
    LCDAVCTL1 = LCDAVCTL1_INIT;
        #endif
        #if defined(LCDCCTL_INIT)  &&  defined(__MSP430_HAS_LCD_C__)
    LCDCCTL0 = LCDCCTL0_INIT;
    LCDCCTL1 = LCDCCTL1_INIT;
    LCDCPCTL0 = LCDCPCTL0_INIT;
    LCDCPCTL1 = LCDCPCTL1_INIT;
    LCDCVCTL0 = LCDCVCTL0_INIT;
    LCDCVCTL1 = LCDCVCTL1_INIT;
        #endif
    #endif

    #if defined(P1OUT_INIT)
    P1OUT = P1OUT_INIT;
    #endif
    #if defined(P1DIR_INIT)
    P1DIR = P1DIR_INIT;
    #endif
    #if defined(P1SEL_INIT)
    P1SEL = P1SEL_INIT;
    #endif
    #if defined(P1REN_INIT)
    P1REN = P1REN_INIT;
    #endif
    #if defined(P1MAP01_INIT)
    P1MAP01 = P1MAP01_INIT;
    #endif

    #if defined(P2OUT_INIT)
    P2OUT = P2OUT_INIT;
    #endif
    #if defined(P2DIR_INIT)
    P2DIR = P2DIR_INIT;
    #endif
    #if defined(P2SEL_INIT)
    P2SEL = P2SEL_INIT;
    #endif
    #if defined(P2REN_INIT)
    P2REN = P2REN_INIT;
    #endif

    #if defined(P3OUT_INIT)
    P3OUT = P3OUT_INIT;
    #endif
    #if defined(P3DIR_INIT)
    P3DIR = P3DIR_INIT;
    #endif
    #if defined(P3SEL_INIT)
    P3SEL = P3SEL_INIT;
    #endif
    #if defined(P3REN_INIT)
    P3REN = P3REN_INIT;
    #endif

    #if defined(P4OUT_INIT)
    P4OUT = P4OUT_INIT;
    #endif
    #if defined(P4DIR_INIT)
    P4DIR = P4DIR_INIT;
    #endif
    #if defined(P4SEL_INIT)
    P4SEL = P4SEL_INIT;
    #endif
    #if defined(P4REN_INIT)
    P4REN = P4REN_INIT;
    #endif

    #if defined(P5OUT_INIT)
    P5OUT = P5OUT_INIT;
    #endif
    #if defined(P5DIR_INIT)
    P5DIR = P5DIR_INIT;
    #endif
    #if defined(P5SEL_INIT)
    P5SEL = P5SEL_INIT;
    #endif
    #if defined(P5REN_INIT)
    P5REN = P5REN_INIT;
    #endif

    #if defined(P6OUT_INIT)
    P6OUT = P6OUT_INIT;
    #endif
    #if defined(P6DIR_INIT)
    P6DIR = P6DIR_INIT;
    #endif
    #if defined(P6SEL_INIT)
    P6SEL = P6SEL_INIT;
    #endif
    #if defined(P6REN_INIT)
    P6REN = P6REN_INIT;
    #endif

    #if defined(P7OUT_INIT)
    P7OUT = P7OUT_INIT;
    #endif
    #if defined(P7DIR_INIT)
    P7DIR = P7DIR_INIT;
    #endif
    #if defined(P7SEL_INIT)
    P7SEL = P7SEL_INIT;
    #endif
    #if defined(P7REN_INIT)
    P7REN = P7REN_INIT;
    #endif

    #if defined(P8OUT_INIT)
    P8OUT = P8OUT_INIT;
    #endif
    #if defined(P8DIR_INIT)
    P8DIR = P8DIR_INIT;
    #endif
    #if defined(P8SEL_INIT)
    P8SEL = P8SEL_INIT;
    #endif
    #if defined(P8REN_INIT)
    P8REN = P8REN_INIT;
    #endif

    #if defined(P9OUT_INIT)
    P9OUT = P9OUT_INIT;
    #endif
    #if defined(P9DIR_INIT)
    P9DIR = P9DIR_INIT;
    #endif
    #if defined(P9SEL_INIT)
    P9SEL = P9SEL_INIT;
    #endif
    #if defined(P9REN_INIT)
    P9REN = P9REN_INIT;
    #endif

    #if defined(P10OUT_INIT)
    P10OUT = P10OUT_INIT;
    #endif
    #if defined(P10DIR_INIT)
    P10DIR = P10DIR_INIT;
    #endif
    #if defined(P10SEL_INIT)
    P10SEL = P10SEL_INIT;
    #endif
    #if defined(P10REN_INIT)
    P10REN = P10REN_INIT;
    #endif

    #if defined(PJOUT_INIT)
    PJOUT = PJOUT_INIT;
    #endif
    #if defined(PJDIR_INIT)
    PJDIR = PJDIR_INIT;
    #endif
    #if defined(PJSEL_INIT)
    PJSEL = PJSEL_INIT;
    #endif
    #if defined(PJREN_INIT)
    PJREN = PJREN_INIT;
    #endif

    #if defined(IO_EXPANDER_SUPPORT)
    set_io_expander(0, 0);
    #endif

    #if defined(BASIC_LCD_SUPPORT)
    display_startup_message();
    #endif

    #if defined(LIMP_MODE_SUPPORT)
    samples_per_second = LIMP_SAMPLES_PER_10_SECONDS/10;
    #else
    samples_per_second = SAMPLES_PER_10_SECONDS/10;
    #endif

    #if defined(__MSP430_HAS_TA3__)  &&  defined(__MSP430_HAS_ADC12__)
    /* Use timer A to control the ADC sampling interval, and the ADC on/off timing. */
    /* TACCR0 determines the sample period */
    TAR = 0;
        #if defined(LIMP_MODE_SUPPORT)
    TACCR0 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 1;
    TACCR1 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 3;
    TACCR2 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 4;
        #else
    TACCR0 = SAMPLE_PERIOD - 1;
    TACCR1 = SAMPLE_PERIOD - 3;
    TACCR2 = SAMPLE_PERIOD - 4;
        #endif
    TACCTL0 = 0;
    TACCTL1 = OUTMOD_3;
        #if defined(FINE_ENERGY_PULSE_TIMING_SUPPORT)
    TACCTL2 = OUTMOD_0 | OUT;           /* Turn off the pulse LED */
        #endif
    TACTL = TACLR | MC_1 | TASSEL_1;
    #endif

    #if defined(__MSP430_HAS_TA3__)  &&  defined(__HAS_SD_ADC__)
    /* Use timer A to control the ADC sampling interval in limp mode. */
    /* CCR0 determines the sample period - 1024Hz */
    TAR = 0;
    TACCR0 = 32 - 1;
    TACCTL0 = 0;
    TACCTL1 = OUTMOD_3;
    TACTL = TACLR | MC_1 | TASSEL_1;
    #endif

    #if defined(PWM_DITHERING_SUPPORT)
    /* Configure timer B to generate a dithering signal, to be added to the     */
    /* analogue signals, something like this.                                   */
    /*                                                                          */
    /*   -----------------                100n                                  */
    /*                    |    __47k_     ||    __1M__       __4K7__            */
    /*                TB1 |->-|______|-- -||---|______|->---|_______|--< Signal */
    /*                    |             | ||              |                     */
    /*                    |           -----               |                     */
    /*                    |           -----2n2            |                     */
    /*                    |             |                 |                     */
    /*                    |            GND                |                     */
    /*                    |                               |                     */
    /*                A0  |-<-----------------------------                      */
    /*                    |                                                     */
    P2SEL |= BIT2;                      /* Select P2.2 for PWM output */
    P2DIR |= BIT2;                      /* Select P2.2 for PWM output */

    TBR = 0;
    TBCCR0 = PWM_FREQ - 1;              /* load period register */
    TBCCR1 = PWM_MID_POINT;             /* start with 50% PWM duty cycle */
    TBCCR2 = PWM_FREQ - 1;
    TBCCTL0 = OUTMOD_4;                 /* set outmode 4 for toggle */
    TBCCTL1 = OUTMOD_6 | CLLD_1;        /* set outmode 6 Toggle/set */
                                        /* load new CCR1 when TBR = 0 */
    TBCCTL2 = OUTMOD_4;
    TBCTL = TBCLR | MC_1 | TBSSEL_2;    /* start TIMER_B up mode, SMCLK as input clock */
    #endif

    #if defined(__MSP430_HAS_ADC12__)
    /* Set up the ADC12 */
    /* Disable conversion while changing the settings */
    ADC12CTL0 = 0;
        #if defined(SINGLE_PHASE)
    ADC12MCTL0 = SREF_0 | AGND_INPUT;
    ADC12MCTL1 = SREF_0 | NEUTRAL_CURRENT_INPUT;
    ADC12MCTL2 = SREF_0 | AGND_INPUT;
    ADC12MCTL3 = SREF_0 | LIVE_CURRENT_INPUT;
    ADC12MCTL4 = SREF_0 | AGND_INPUT;
            #if defined(BATTERY_MONITOR_SUPPORT)
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT;
    ADC12MCTL6 = EOS | SREF_0 | BATTERY_INPUT;
            #else
    ADC12MCTL5 = EOS | SREF_0 | VOLTAGE_INPUT;
            #endif
        #else
            #if GAIN_STAGES > 1
    ADC12MCTL0 = SREF_0 | LIVE_LOW_CURRENT_INPUT_1;
    ADC12MCTL1 = SREF_0 | LIVE_HIGH_CURRENT_INPUT_1;
    ADC12MCTL2 = SREF_0 | VOLTAGE_INPUT_1;
    ADC12MCTL3 = SREF_0 | LIVE_LOW_CURRENT_INPUT_2;
    ADC12MCTL4 = SREF_0 | LIVE_HIGH_CURRENT_INPUT_2;
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT_2;
    ADC12MCTL6 = SREF_0 | LIVE_LOW_CURRENT_INPUT_3;
    ADC12MCTL7 = SREF_0 | LIVE_HIGH_CURRENT_INPUT_3;
                #if defined(NEUTRAL_CURRENT_INPUT)
    ADC12MCTL8 = SREF_0 | VOLTAGE_INPUT_3;
    ADC12MCTL9 = EOS | SREF_0 | NEUTRAL_CURRENT_INPUT;
                #else
    ADC12MCTL8 = EOS | SREF_0 | NEUTRAL_CURRENT_INPUT;
                #endif
            #else
    ADC12MCTL0 = SREF_0 | LIVE_CURRENT_INPUT_1;
    ADC12MCTL1 = SREF_0 | AGND_INPUT;
    ADC12MCTL2 = SREF_0 | VOLTAGE_INPUT_1;
    ADC12MCTL3 = SREF_0 | LIVE_CURRENT_INPUT_2;
    ADC12MCTL4 = SREF_0 | AGND_INPUT;
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT_2;
    ADC12MCTL6 = SREF_0 | LIVE_CURRENT_INPUT_3;
    ADC12MCTL7 = SREF_0 | AGND_INPUT;
                #if defined(NEUTRAL_CURRENT_INPUT)
    ADC12MCTL8 = SREF_0 | VOLTAGE_INPUT_3;
    ADC12MCTL9 = EOS | SREF_0 | NEUTRAL_CURRENT_INPUT;
                #else
    ADC12MCTL8 = EOS | SREF_0 | VOLTAGE_INPUT_3;
                #endif
            #endif
            #if defined(BATTERY_MONITOR_SUPPORT)
    /* Battery sensing control pin */
    P3DIR &= ~(BIT1);
    P3OUT |= (BIT1);
            #endif
        #endif

    /* Later, we will program the ADC for the following. For now
       we just get the basic configuration done, and wait until
       we are sure there is enough power to do more. */
    /* Sample & hold time 0 (for low ADC channels) */
    /* Sample & hold time 1 (for high ADC channels) */
    /* Multiple sample & hold off */
    /* Reference voltage is switched on at 2.5V */
    /* ADC12 module is switched on */
    /* Interrupt at the end of every ADC conversion */
    ADC12CTL0 = MSC;

    /* First conv. result is stored in ADC12MEM0 */
    /* ADC12SC bit triggers sample & hold */
    /* ISSH trigger is NOT inverted */
    /* Sample pulse is generated by sampling timer */
    /* Clock Source: TIMER_A OUT 1 */
    /* Clock divider: 1 */
    /* Sequence of channels conversion */
    ADC12CTL1 = SHS_1 | CONSEQ_1 | SHP | ADC12SSEL_0;
    #endif
    #if defined( __HAS_SD_ADC__)
    disable_analog_front_end();
    #endif

    #if defined(POWER_UP_BY_SUPPLY_SENSING)
      #if defined(__MSP430_HAS_COMPA__)
    /* Set up comparator A to monitor a drooping voltage within the
       e-meter's main power supply. This is an early warning of power
       fail, so we can get to low power mode before we start burning the
       battery. */
    CACTL1 = CAREF_1;
    CACTL2 = P2CA1 | CAF;
    P1SEL |= BIT7;
    CAPD |= BIT7;
      #endif
      #if defined(__MSP430_HAS_ADC10_A__)
    /* Set up comparator to monitor a drooping voltage within the
       e-meter's main power supply. This is an early warning of power
       fail, so we can get to low power mode before we start burning the
       battery. */
    CACTL1 = CAREF_1;
    CACTL2 = P2CA1 | CAF;
    P1SEL |= BIT7;
    CAPD |= BIT7;
      #endif
    #endif

    #if defined(IEC1107_SUPPORT)  ||  defined(IEC62056_21_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT)  ||  defined(SERIAL_CALIBRATION_REF_SUPPORT)
        #if defined(__MSP430_HAS_UART0__)  &&  defined(UART0_BAUD_RATE)
    /* Configure UART0 */
    UCTL0 = CHAR;                       /* 8-bit character */
            #if UART0_BAUD_RATE == 460800
    /* 460800 Rx doesn't work very well from 32768Hz. Use the fast clock. */
    UTCTL0 = SSEL1;                     /* UCLK = SMCLK */
    UBR00 = 18;
    UBR10 = 0;
    UMCTL0 = 0x11;
            #elif UART0_BAUD_RATE == 9600
    /* 9600 Rx doesn't work very well from 32768Hz. Use the fast clock. */
    UTCTL0 = SSEL1;                     /* UCLK = SMCLK */
    UBR10 = 0x3;
    UBR00 = 0x69;
    UMCTL0 = 0x77;
            #elif UART0_BAUD_RATE == 4800
    UTCTL0 = SSEL0;                     /* UCLK = ACLK */
    UBR10 = 0;
    UBR00 = 6;                          /* 32k/4800 - 6.8266 */
    UMCTL0 = 0x6F;
            #elif UART0_BAUD_RATE == 2400
    UTCTL0 = SSEL0;                     /* UCLK = ACLK */
    UBR10 = 0;
    UBR00 = 13;                         /* 32k/2400 - 13.653 */
    UMCTL0 = 0x6B;
            #else
    UTCTL0 = SSEL0;                     /* UCLK = ACLK */
    UBR10 = 0;
    UBR00 = 27;                         /* 32k/1200 - 27.307 */
    UMCTL0 = 0x03;
            #endif

            #if defined(SERIAL_CALIBRATION_REF_SUPPORT)
    U0ME |= (UTXE0 | URXE0);            /* Enable USART0 TXD/RXD */
    U0IE |= URXIE0;
            #elif defined(SERIAL_CALIBRATION_SUPPORT)
    U0ME |= URXE0;                      /* Enable only USART0 RXD */
    U0IE |= URXIE0;
            #else
    U0ME |= UTXE0;                      /* Enable only USART0 TXD */
            #endif
    /* If you do not initially kick the Tx port the TXEPT bit is not set. */
    TXBUF0 = 0;
        #endif

        #if defined(__MSP430_HAS_UART1__)  &&  defined(UART1_BAUD_RATE)
    /* Configure UART1 */
    //UCTL1 = PENA | PEV;               /* 7-bit + even parity character */
    UCTL1 = CHAR;                       /* 8-bit character */
    //UCTL1 = PENA | PEV | CHAR;        /* 8-bit + even parity character */
            #if UART1_BAUD_RATE == 9600
    /* 9600 Rx doesn't work very well from 32768Hz. Use the fast clock. */
    UTCTL1 = SSEL1;                     /* UCLK = SMCLK */
    UBR11 = 0x3;
    UBR01 = 0x69;
    UMCTL1 = 0x77;
            #elif UART1_BAUD_RATE == 4800
    UTCTL1 = SSEL0;                     /* UCLK = ACLK */
    UBR11 = 0;
    UBR01 = 6;                          /* 32k/4800 - 6.8266 */
    UMCTL1 = 0x6F;
            #elif UART1_BAUD_RATE == 2400
    UTCTL1 = SSEL0;                     /* UCLK = ACLK */
    UBR11 = 0;
    UBR01 = 13;                         /* 32k/2400 - 13.653 */
    UMCTL1 = 0x6B;
            #else
    UTCTL1 = SSEL0;                     /* UCLK = ACLK */
    UBR11 = 0;
    UBR01 = 27;                         /* 32k/1200 - 27.307 */
    UMCTL1 = 0x03;
            #endif

    ME2 |= (UTXE1 | URXE1);            /* Enable USART1 TXD/RXD */
    IE2 |= URXIE1;
    /* If you do not initially kick the Tx port, the TXEPT bit is not set. */
    TXBUF1 = 0;
        #endif

        #if defined(__MSP430_HAS_USCI_AB0__)  &&  defined(UART0_BAUD_RATE)
    /* Configure USCI0 UART */
    //UCTL1 = PENA | PEV;               /* 7-bit + even parity character */
    UCA0CTL0 = 0;                       /* 8-bit character */
    //UCTL1 = PENA | PEV | CHAR;        /* 8-bit + even parity character */
            #if UART0_BAUD_RATE == 9600
    /* 9600 Rx doesn't work very well from 32768Hz. Use the fast clock. */
#if 0
    UCA0CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 0x3;
    UCA0MCTL = UCBRS_3;
#else
    UCA0CTL1 |= UCSSEL1;                     /* UCLK = SMCLK */
    UCA0BR1 = 0x06;
    UCA0BR0 = 0xD3;
    UCA0MCTL = UCBRS_5;
#endif
            #elif UART0_BAUD_RATE == 4800
    UCA0CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 0x6;
    UCA0MCTL = UCBRS_7;
            #elif UART0_BAUD_RATE == 2400
    UCA0CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 0xD;
    UCA0MCTL = UCBRS_6;
            #else
    UCA0CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 0x1B;
    UCA0MCTL = UCBRS_2;
            #endif

    UCA0CTL1 &= ~UCSWRST;
    UC0IE |= UCA0RXIE;
        #endif

        #if defined(__MSP430_HAS_EUSCI_A0__)  &&  defined(UART0_BAUD_RATE)
    /* Configure USCI0 UART */

    UCA0CTL0 = 0;                       /* 8-bit character */
            #if UART0_BAUD_RATE == 9600
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 0x3;
#ifdef _ZEBU_
    UCA0MCTLW_H = 0x25;
#else
    UCA0MCTLW_H = 0x92;
#endif
            #elif UART0_BAUD_RATE == 4800
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
            #elif UART0_BAUD_RATE == 2400
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
    UCA0BR1 = 0x0;
    UCA0BR0 = 13;
#ifdef _ZEBU_
    UCA0MCTLW_H = 0x25;
#else
    UCA0MCTLW_H = 0xB6;
#endif
            #else
    UCA0CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
            #endif

    UCA0CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
        #endif


        #if defined(__MSP430_HAS_EUSCI_A1__)  &&  defined(UART1_BAUD_RATE)
    /* Configure USCI0 UART */

    UCA1CTL0 = 0;                       /* 8-bit character */
            #if UART1_BAUD_RATE == 9600
    UCA1CTL1 = UCSSEL__ACLK;                     /* UCLK = ACLK */
    UCA1BR1 = 0x0;
    UCA1BR0 = 0x3;
#ifdef _ZEBU_
    UCA1MCTLW_H = 0x25;
#else
    UCA1MCTLW_H = 0x92;
#endif
            #elif UART1_BAUD_RATE == 4800
    UCA1CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
            #elif UART1_BAUD_RATE == 2400
    UCA1CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
    UCA1BR1 = 0x0;
    UCA1BR0 = 13;
#ifdef _ZEBU_
    UCA1MCTLW_H = 0x25;
#else
    UCA1MCTLW_H = 0xB6;
#endif
            #else
    UCA1CTL1 |= UCSSEL__ACLK;                     /* UCLK = ACLK */
            #endif

    UCA1CTL1 &= ~UCSWRST;
    UCA1IE |= UCRXIE;
        #endif

    #if defined(__MSP430_HAS_USCI_AB1__)  &&  defined(UART1_BAUD_RATE)
    /* Configure USCI1 UART */
    //UCTL1 = PENA | PEV;               /* 7-bit + even parity character */
    UCA1CTL0 = 0;                       /* 8-bit character */
    //UCTL1 = PENA | PEV | CHAR;        /* 8-bit + even parity character */
            #if UART1_BAUD_RATE == 9600
    /* 9600 Rx doesn't work very well from 32768Hz. Use the fast clock. */
#if 0
    UCA1CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA1BR1 = 0x0;
    UCA1BR0 = 0x3;
    UCA1MCTL = UCBRS_3;
#else
    UCA1CTL1 |= UCSSEL1;                     /* UCLK = SMCLK */
    UCA1BR1 = 0x06;
    UCA1BR0 = 0xD3;
    UCA1MCTL = UCBRS_5;
#endif
            #elif UART1_BAUD_RATE == 4800
    UCA1CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA1BR1 = 0x0;
    UCA1BR0 = 0x6;
    UCA1MCTL = UCBRS_7;
            #elif UART1_BAUD_RATE == 2400
    UCA1CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA1BR1 = 0x0;
    UCA1BR0 = 0xD;
    UCA1MCTL = UCBRS_6;
            #else
    UCA1CTL1 |= UCSSEL0;                     /* UCLK = ACLK */
    UCA1BR1 = 0x0;
    UCA1BR0 = 0x1B;
    UCA1MCTL = UCBRS_2;
            #endif

    UCA1CTL1 &= ~UCSWRST;
    UC1IE |= UCA1RXIE;
        #endif

    #if defined(IEC1107_SUPPORT)
    /* Configure the IR receiver control line - we need to be able to power it down
       in limp mode, to conserve current. */
    disable_ir_receiver();
        #endif
    #endif

  #if defined(IHD430_SUPPORT) 
    UCA2CTL1 |= UCSWRST; 
    UCA2CTL1 |= UCSSEL_2;                     // SMCLK
    UCA2BRW = 9;                           // 16 MHz 115200
    //UCA2BR1 = 0x00;                         //16 8MHz 115200
    UCA2MCTLW = 0xB511;                          
    P2SEL0 |= BIT2+BIT3;
    UCA2CTL1 &= ~UCSWRST;  
  #endif
    
    meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_earthed_indicator();
    clr_reverse_current_indicator();
    #if defined(total_active_energy_pulse_end)
    total_active_energy_pulse_end();
    #endif
    #if defined(total_reactive_energy_pulse_end)
    total_reactive_energy_pulse_end();
    #endif
    #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
        #if !defined(SINGLE_PHASE)
    phase_1_active_energy_pulse_end();
    phase_2_active_energy_pulse_end();
    phase_3_active_energy_pulse_end();
        #else
    phase_active_energy_pulse_end();
        #endif
    #endif

    #if !defined(ESP_SUPPORT)
        #if !defined(SINGLE_PHASE)
    phase = chan;
    phase_nv = nv_parms.seg_a.s.chan;
    for (j = 0;  j < NUM_PHASES;  j++)
    {
        #endif
      /* Prime the DC estimates for quick settling */
        phase->metrology.current.I_dc_estimate[0] = phase_nv->current.initial_dc_estimate;
        phase->metrology.current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        phase->metrology.neutral.I_dc_estimate[0] = phase_nv->current.initial_dc_estimate;
        phase->metrology.neutral.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        #endif
        #if GAIN_STAGES > 1
        phase->metrology.current.I_dc_estimate[1] = phase_nv->current.initial_dc_estimate;
            #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        phase->metrology.neutral.I_dc_estimate[1] = phase_nv->current.initial_dc_estimate;
            #endif
        #endif
        #if defined(LIMP_MODE_SUPPORT)
        phase->metrology.V_dc_estimate[0] = phase_nv->initial_v_dc_estimate;
        phase->metrology.V_dc_estimate[1] = phase_nv->initial_v_limp_dc_estimate;
        #else
        phase->metrology.V_dc_estimate = phase_nv->initial_v_dc_estimate;
        #endif
        phase->metrology.V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        #if defined(MAINS_FREQUENCY_SUPPORT)
        phase->metrology.mains_period = ((SAMPLES_PER_10_SECONDS*6554)/MAINS_NOMINAL_FREQUENCY) << 8;
        #endif
        #if !defined(SINGLE_PHASE)
        phase++;
        phase_nv++;
    }
        #endif
    #endif
    #if !defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        neutral.metrology.I_dc_estimate[0] = nv_parms.seg_a.s.neutral.initial_dc_estimate;
        neutral.metrology.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    #endif

    #if defined(TEMPERATURE_SUPPORT)
    temperature = 0;
    #endif
    #if defined(RTC_SUPPORT)
    if (!check_rtc_sumcheck())
    {
        rtc.year = 0x10;
        rtc.month = 0x06;
        rtc.day = 0x11;
        rtc.hour = 0x12;
        rtc.minute = 0x0;
        rtc.second = 0x0;
        set_rtc_sumcheck();
    #if defined (__MSP430_HAS_RTC_C__)
        RTCCTL0_H = RTCKEY_H;                  // Unlock RTC
        if (RTCCTL0 & RTCOFIFG){  // init if RTCOFIFG is set
          RTCCTL13 = RTCBCD+RTCHOLD+RTCMODE+RTCTEV_0; // Init RTC
          RTCCTL0_L &= ~RTCOFIFG_L;   // Clear Flag
          RTCSEC   = rtc.second;
          RTCMIN   = rtc.minute;
          RTCHOUR  = rtc.hour;
        //    RTCDOW   = rtc.DayOfWeek;
          RTCDAY   = rtc.day;
          RTCMON   = rtc.month;
          RTCYEAR  = rtc.year + 0x2000;
          RTCCTL13 &= ~RTCHOLD;                   // Enable RTC
        }
        else
        {
          RTCCTL13 = RTCBCD+RTCMODE+RTCTEV_0; // Init RTC
        }
        RTCCTL0_H = 0;   // LOCK RTC
    #endif
    }
    #endif
    #if defined(EXTERNAL_EEPROM_SUPPORT)
/*
    external_eeprom_init();

    { volatile int j= 0;
      volatile int k = 0;
      volatile uint8_t buf[128] = {10,11,12,13,14,15,16,17,18};
      iicEEPROM_write(j, (void*)buf, 128);
      k = iicEEPROM_read(j, (void*)buf, 128);
    }
*/
    #endif
    custom_initialisation();

    _EINT();

    #if defined(POWER_DOWN_SUPPORT)
    /* Now go to lower power mode, until we know we should do otherwise */
    switch_to_powerfail_mode();
    #else
        #if defined(__MSP430_HAS_SVS__)
    /* Before we go to high speed we need to make sure the supply voltage is
       adequate. If there is an SVS we can use that. There should be no wait
       at this point, since we should only have been woken up if the supply
       is healthy. However, it seems better to be cautious. */
    SVSCTL |= (SVSON | 0x60);
    /* Wait for adequate voltage to run at full speed */
    while ((SVSCTL & SVSOP))
        /* dummy loop */;
    /* The voltage should now be OK to run the CPU at full speed. Now it should
       be OK to use the SVS as a reset source. */
    SVSCTL |= PORON;
        #endif

    /* Take control of the EEPROM signals. */
#if defined(EXTERNAL_EEPROM_SUPPORT)
    enable_eeprom_port();
#endif
        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    /* Speed up the clock to 8.388608MHz */
    SCFI0 = SCFI0_HIGH;
    SCFQCTL = SCFQCTL_HIGH;
#if 0
    {
        int i;
        for (i = 0xFFFF;  i;  i--);
            _NOP();
        _BIS_SR(SCG0);
    }
    SCFQCTL |= SCFQ_M;
    SCFI0 &= ~0x03;
    SCFI1 &= ~0x07;
    SCFI1 += 8;
#endif
    /* There seems no benefit in waiting for the FLL to settle at this point. */
        #endif

        #if defined(__MSP430_HAS_TA3__)  &&  defined(__MSP430_HAS_ADC12__)
    /* Enable the TIMER_A0 interrupt */
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
        #endif

    kick_watchdog();
    switch_to_normal_mode();
    #endif
    #if defined(MESH_NET_SUPPORT)
    meshnet_init();
    #endif
}
#else
void system_setup(void)
{
    #if !defined(SINGLE_PHASE)
    struct phase_parms_s *phase;
    int j;
    #endif

    #if !defined(SINGLE_PHASE)
    phase = chan;
    for (j = 0;  j < NUM_PHASES;  j++)
    {
    #endif
        /* Prime the DC estimates for quick settling */
    #if defined(NEUTRAL_MONITOR_SUPPORT)
        phase->metrology.neutral.I_dc_estimate = phase_nv->current.initial_dc_estimate;
    #endif
        phase->metrology.current.I_dc_estimate = phase_nv->current.initial_dc_estimate;
        phase->metrology.V_dc_estimate[0] = phase_nv->initial_v_dc_estimate;
        //phase->metrology.V_dc_estimate[1] = phase_nv->initial_v_limp_dc_estimate;
    #if defined(MAINS_FREQUENCY_SUPPORT)
        phase->mains_period = ((SAMPLES_PER_10_SECONDS*6554)/MAINS_NOMINAL_FREQUENCY) << 8;
    #endif
    #if !defined(SINGLE_PHASE)
        phase++;
    }
    #endif

    samples_per_second = SAMPLES_PER_10_SECONDS/10;
    #if defined(TEMPERATURE_SUPPORT)
    temperature = 0;
    #endif
}
#endif

#if defined(__MSP430__)
void switch_to_normal_mode(void)
{
    /* Switch to full speed, full power mode */

    meter_status |= STATUS_PHASE_VOLTAGE_OK;
    set_normal_indicator();

    #if defined(__MSP430_HAS_ADC12__)
    /* Change the ADC reference to Vref+ */
    _DINT();

    /* Must disable conversion while reprogramming the ADC */
    ADC12CTL0 &= ~ENC;

    /* Turn the Vref and ADC on. */
    ADC12CTL0 = REFON | MSC | REF2_5V | ADC12ON | SHT0_2 | SHT1_2;

        #if defined(SINGLE_PHASE)
    ADC12MCTL0 = SREF_1 | AGND_INPUT;
    ADC12MCTL1 = SREF_1 | NEUTRAL_CURRENT_INPUT;
    ADC12MCTL2 = SREF_1 | AGND_INPUT;
    ADC12MCTL3 = SREF_1 | LIVE_CURRENT_INPUT;
    ADC12MCTL4 = SREF_1 | AGND_INPUT;
            #if defined(TEMPERATURE_SUPPORT) | defined(BATTERY_MONITOR_SUPPORT)
    ADC12MCTL5 = SREF_1 | VOLTAGE_INPUT;
            #else
    ADC12MCTL5 = EOS | SREF_1 | VOLTAGE_INPUT;
            #endif
            #if defined(BATTERY_MONITOR_SUPPORT)
                #if defined(TEMPERATURE_SUPPORT)
    ADC12MCTL6 = SREF_1 | BATTERY_INPUT;
                #else
    ADC12MCTL6 = EOS | SREF_1 | BATTERY_INPUT;
                #endif
            #endif
            #if defined(TEMPERATURE_SUPPORT)
    ADC12MCTL7 = EOS | SREF_1 | INCH_10;    /* Temperature */
            #endif
        #else
            #if GAIN_STAGES > 1
    ADC12MCTL0 = SREF_1 | LIVE_LOW_CURRENT_INPUT_1;
    ADC12MCTL1 = SREF_1 | LIVE_HIGH_CURRENT_INPUT_1;
    ADC12MCTL2 = SREF_1 | VOLTAGE_INPUT_1;
    ADC12MCTL3 = SREF_1 | LIVE_LOW_CURRENT_INPUT_2;
    ADC12MCTL4 = SREF_1 | LIVE_HIGH_CURRENT_INPUT_2;
    ADC12MCTL5 = SREF_1 | VOLTAGE_INPUT_2;
    ADC12MCTL6 = SREF_1 | LIVE_LOW_CURRENT_INPUT_3;
    ADC12MCTL7 = SREF_1 | LIVE_HIGH_CURRENT_INPUT_3;
    ADC12MCTL8 = SREF_1 | VOLTAGE_INPUT_3;
                #if defined(NEUTRAL_CURRENT_INPUT)
    ADC12MCTL9 = SREF_0 | NEUTRAL_CURRENT_INPUT;
    ADC12MCTL10 = EOS | SREF_1 | INCH_10;   /* Temperature */
                #else
    ADC12MCTL9 = EOS | SREF_1 | INCH_10;    /* Temperature */
                #endif
            #else
    ADC12MCTL0 = SREF_1 | LIVE_CURRENT_INPUT_1;
    ADC12MCTL1 = SREF_1 | AGND_INPUT;
    ADC12MCTL2 = SREF_1 | VOLTAGE_INPUT_1;
    ADC12MCTL3 = SREF_1 | LIVE_CURRENT_INPUT_2;
    ADC12MCTL4 = SREF_1 | AGND_INPUT;
    ADC12MCTL5 = SREF_1 | VOLTAGE_INPUT_2;
    ADC12MCTL6 = SREF_1 | LIVE_CURRENT_INPUT_3;
    ADC12MCTL7 = SREF_1 | AGND_INPUT;
    ADC12MCTL8 = SREF_1 | VOLTAGE_INPUT_3;
                #if defined(NEUTRAL_CURRENT_INPUT)
    ADC12MCTL9 = SREF_0 | NEUTRAL_CURRENT_INPUT;
    ADC12MCTL10 = EOS | SREF_1 | INCH_10;   /* Temperature */
                #else
    ADC12MCTL9 = EOS | SREF_1 | INCH_10;    /* Temperature */
                #endif
            #endif
        #endif
    ADC12CTL0 |= ENC;

        #if defined(__MSP430_HAS_TA3__)
    /* Switch to the normal sampling rate. */
    TAR = 0;
    TACCR0 = SAMPLE_PERIOD - 1;
    TACCR1 = SAMPLE_PERIOD - 3;
    TACCR2 = SAMPLE_PERIOD - 4;
    TACCTL0 = CCIE;
    TACCTL1 = OUTMOD_3;
    TACTL = TACLR | MC_1 | TASSEL_1;
    /* Disable the interrupt routine which re-enables the ADC */
    TACCTL2 = 0;
        #endif
    #endif

    #if defined(__HAS_SD_ADC__)
        #if defined(__MSP430_HAS_TA3__)
    /* Disable the TIMER_A0 interrupt */
    TACTL = 0;
    TACCTL0 = 0;
        #endif
    _DINT();
    init_analog_front_end_normal();
    #endif

    samples_per_second = SAMPLES_PER_10_SECONDS/10;

    _EINT();

    #if defined(IEC62056_21_SUPPORT)
    /* Configure the USART and 38kHz output bits */
    P2SEL |= (BIT5 | BIT4 | BIT3);
    P2DIR |= (BIT4 | BIT3);

    /* Configure the bit that powers the 38kHz receiver */
    P1DIR |= (BIT5);
    P1OUT |= (BIT5);

        #if defined(INFRA_RED_38K_FROM_TIMER_A)
    /* Program TA0 to output a 38kHz clock, based on the core frequency */
    TAR = 0;
    TACCR0 = IR_38K_DIVISOR - 1;        /* Load period register */
    TACCTL0 = OUTMOD_4;                 /* Set outmode 4 for toggle */
    TACTL = TACLR | MC_1 | TASSEL_2;    /* Start TIMER_B up mode, SMCLK as input clock */
        #endif
        #if defined(INFRA_RED_38K_FROM_TIMER_B)
    /* Program TB2 to output a 38kHz clock, based on the core frequency */
    TBR = 0;
    TBCCR0 = IR_38K_DIVISOR - 1;        /* Load period register */
    TBCCR2 = IR_38K_DIVISOR - 10;
    TBCCTL2 = OUTMOD_4;                 /* Set outmode 4 for toggle */
    TBCTL = TBCLR | MC_1 | TBSSEL_2;    /* Start TIMER_B up mode, SMCLK as input clock */
        #endif
    #endif

    #if defined(PWM_DITHERING_SUPPORT)
    /* Enable dithering, by enabling Timer B */
    TBR = 0;
    TBCTL = TBCLR | MC_1 | TBSSEL_2;
    #endif

    #if defined(DAC12_DITHERING_SUPPORT)
    DAC12_0CTL &= ~ENC;
    DAC12_1CTL &= ~ENC;
    DAC12_0CTL =
    DAC12_1CTL = DAC12CALON | DAC12IR | DAC12AMP_7 | DAC12LSEL_2;
    DAC12_0CTL |= ENC;
    DAC12_1CTL |= ENC;
    DAC12_0DAT = 0x800;
    DAC12_1DAT = 0;
    #endif

    #if defined(IEC1107_SUPPORT)
    enable_ir_receiver();
    #endif

    operating_mode = OPERATING_MODE_NORMAL;
}

    #if defined(LIMP_MODE_SUPPORT)
void switch_to_limp_mode(void)
{
    /* Switch to minimum consumption, current measurement only mode */

    meter_status &= ~(STATUS_REVERSED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_reverse_current_indicator();

        #if defined(__MSP430_HAS_ADC12__)
    /* Change the ADC reference to Vcc */
    _DINT();

    /* Must disable conversion while reprogramming the ADC */
    ADC12CTL0 &= ~ENC;

    /* Turn the Vref off and ADC on */
    ADC12CTL0 = MSC | REF2_5V | ADC12ON | SHT0_2 | SHT1_2;

            #if defined(SINGLE_PHASE)
    ADC12MCTL0 = SREF_0 | AGND_INPUT;
    ADC12MCTL1 = SREF_0 | NEUTRAL_CURRENT_INPUT;
    ADC12MCTL2 = SREF_0 | AGND_INPUT;
    ADC12MCTL3 = SREF_0 | LIVE_CURRENT_INPUT;
    ADC12MCTL4 = SREF_0 | AGND_INPUT;
                #if defined(BATTERY_MONITOR_SUPPORT)
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT;
    ADC12MCTL6 = EOS | SREF_0 | BATTERY_INPUT;
                #else
    ADC12MCTL5 = EOS | SREF_0 | VOLTAGE_INPUT;
                #endif
    /* Skip the temperature, or the reference switches on! */
            #else
    ADC12MCTL0 = SREF_0 | LIVE_CURRENT_INPUT_1;
    ADC12MCTL1 = SREF_0 | AGND_INPUT;
    ADC12MCTL2 = SREF_0 | VOLTAGE_INPUT_1;
    ADC12MCTL3 = SREF_0 | LIVE_CURRENT_INPUT_2;
    ADC12MCTL4 = SREF_0 | AGND_INPUT;
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT_2;
    ADC12MCTL6 = SREF_0 | LIVE_CURRENT_INPUT_3;
    ADC12MCTL7 = SREF_0 | AGND_INPUT;
                #if defined(NEUTRAL_CURRENT_INPUT)
    ADC12MCTL8 = SREF_0 | VOLTAGE_INPUT_3;
    ADC12MCTL9 = EOS | SREF_0 | NEUTRAL_CURRENT_INPUT;
                #else
    ADC12MCTL8 = EOS | SREF_0 | VOLTAGE_INPUT_3;
                #endif
    /* Skip the temperature, or the reference switches on! */
            #endif
    ADC12CTL0 |= ENC;

            #if defined(__MSP430_HAS_TA3__)
    /* Switch to a lower sampling rate. */
    TAR = 0;
    TACCR0 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 1;
    TACCR1 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 3;
    TACCR2 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 4;
    TACCTL0 = CCIE;
    TACCTL1 = OUTMOD_3;
    TACTL = TACLR | MC_1 | TASSEL_1;
    /* Enable the interrupt routine which re-enables the ADC */
    TACCTL2 = CCIE;
            #endif
        #endif

        #if defined(__HAS_SD_ADC__)
            #if defined(__MSP430_HAS_TA3__)
    /* Enable the TIMER_A0 interrupt */
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
            #endif
    _DINT();
    init_analog_front_end_limp();
        #endif

    samples_per_second = LIMP_SAMPLES_PER_10_SECONDS/10;

        #if defined(IEC1107_SUPPORT)
    disable_ir_receiver();
        #endif

    _EINT();

        #if defined(PWM_DITHERING_SUPPORT)
    /* Disable dithering, by disabling Timer B */
    TBCTL = TBCLR | TBSSEL_2;
        #endif
    operating_mode = OPERATING_MODE_LIMP;
}
    #endif

    #if defined(POWER_DOWN_SUPPORT)
void switch_to_powerfail_mode(void)
{
    operating_mode = OPERATING_MODE_POWERFAIL;

    /* Note that a power down occured */
    meter_status |= POWER_DOWN;

    /* Turn off all the LEDs. */
    meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_reverse_current_indicator();
    clr_earthed_indicator();
        #if defined(total_active_energy_pulse_end)
    total_active_energy_pulse_end();
        #endif
        #if defined(total_reactive_energy_pulse_end)
    total_reactive_energy_pulse_end();
        #endif

    /* Make the EEPROM signals inputs, and rely on pullups. */
    disable_eeprom_port();

    /* Shut down the LCD */
    custom_lcd_sleep_handler();

        #if defined(__MSP430_HAS_TA3__)  &&  (defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_SD_ADC__))
    /* Disable the TIMER_A0 interrupt */
    TACTL = 0;
    TACCTL0 = 0;
    /* Disable the interrupt routine which re-enables the ADC */
    TACCTL2 = 0;
        #endif

        #if defined(__MSP430_HAS_ADC12__)
    /* Now the interrupts have stopped it should be safe to power
       down the ADC. */
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 &= ~(REFON | REF2_5V | ADC12ON);
        #endif
        #if defined(__HAS_SD_ADC__)
    disable_analog_front_end();
        #endif
        #if defined(PWM_DITHERING_SUPPORT)
    /* Disable dithering, by disabling Timer B */
    TBCTL = TBCLR | TBSSEL_2;
        #endif
        #if defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT)  ||  defined(SERIAL_CALIBRATION_REF_SUPPORT)
    /* Disable the serial port. */
        #if defined(__MSP430_HAS_UART0__)
    U0ME &= ~(UTXE0 | URXE0);
        #elif defined(__MSP430_HAS_USCI_AB0__)
        #elif defined(__MSP430_HAS_USCI_A0__)
        #elif defined(__MSP430_HAS_EUSCI_A0__)
        #endif
            #if defined(IEC1107_SUPPORT)
    disable_ir_receiver();
            #endif
        #endif

        #if defined(BATTERY_MONITOR_SUPPORT)
    /* Battery sensing control pin */
    P3DIR &= ~(BIT1);
    P3OUT |= (BIT1);
        #endif

        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    /* Slow the clock to 1MHz as quickly as possible. The FLL will not be active
       in LPM3, so switch it off now, and force the FLL's RC oscillator to
       about 1MHz. The exact frequency is not critical. */
    _BIS_SR(SCG0);                  /* switch off FLL locking */
    SCFI0 = FLLD_1;
    SCFQCTL = SCFI0_LOW | SCFQ_M;
    SCFI0 = 0x0;
    SCFI1 = 0x37;
        #endif
        #if defined(__MSP430_HAS_SVS__)
    /* At 1MHz it is safe to turn off the SVS, and rely on the brownout
       detector. Now the meter can survive on a very weak battery. */
    SVSCTL = 0;
        #endif
    custom_power_fail_handler();

    /* ******************** LOW POWER STATE ************************** */
    /* Go to LPM3 mode and exit only when power comes back on. The timer
       interrupt that ticks every second should be checking for power
       restored while we sit here. When it sees the unregulated supply
       at a healthy voltage, it will wake us up. */
    _BIS_SR(LPM3_bits);

    /* Waking up from power down mode */
        #if defined(__MSP430_HAS_SVS__)
    /* Before we go to high speed we need to make sure the supply voltage is
       adequate. If there is an SVS we can use that. There should be no wait
       at this point, since we should only have been woken up if the supply
       is healthy. However, it seems better to be cautious. */
    SVSCTL |= (SVSON | 0x60);
    /* Wait for adequate voltage to run at full speed */
    while ((SVSCTL & SVSOP))
        /* dummy loop */;
    /* The voltage should now be OK to run the CPU at full speed. Now it should
       be OK to use the SVS as a reset source. */
    SVSCTL |= PORON;
        #endif

        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    /* Speed up the clock to high speed. */
    SCFI0 = FN_3 | FLLD_4;
    SCFQCTL = SCFQCTL_HIGH;
    /* There seems no good reason to wait until the FLL has settled at this point. */
        #endif

    /* Take control of the EEPROM signals again. */
    enable_eeprom_port();

    /* Enable the serial port */
        #if defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT)  ||  defined(SERIAL_CALIBRATION_REF_SUPPORT)
        #if defined(__MSP430_HAS_UART0__)
            #if defined(SERIAL_CALIBRATION_REF_SUPPORT)
    U0ME |= (UTXE0 | URXE0);
            #elif defined(SERIAL_CALIBRATION_SUPPORT)
    U0ME |= URXE0;
            #else
    U0ME |= UTXE0;
            #endif
        #elif defined(__MSP430_HAS_USCI_AB0__)
        #elif defined(__MSP430_HAS_USCI_A0__)
        #elif defined(__MSP430_HAS_EUSCI_A0__)
        #endif
        #endif

        #if defined(__MSP430_HAS_TA3__)  &&  (defined(__MSP430_HAS_ADC12__)  || defined(__HAS_SD_ADC__))
    /* Enable the TIMER_A0 interrupt */
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
        #endif

    kick_watchdog();

        #if defined(LOSE_FRACTIONAL_PULSE_AT_POWER_ON)
            #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
//    phase->power_counter = 0;
            #endif
            #if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
//    total_power_counter = 0;
                #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
    extra_total_power_counter = 0;
                #endif
            #endif
            #if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
    total_reactive_power_counter = 0;
                #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
    extra_total_reactive_power_counter = 0;
                #endif
            #endif
        #endif

        #if defined(BATTERY_MONITOR_SUPPORT)
    if (battery_countdown)
    {
        battery_countdown = 1000;
        /* Battery sensing control pin */
        P3DIR |= (BIT1);
        P3OUT &= ~(BIT1);
    }
        #endif
    /* Come out of power down in limp mode, as we don't know
       if there is sufficent power available to driver the meter
       at full speed. We will soon switch to normal mode if a
       voltage signal is available. */
    /* Limp mode will fire up the ADC again. */
        #if defined(LIMP_MODE_SUPPORT)
    switch_to_limp_mode();
        #else
    switch_to_normal_mode();
        #endif
    custom_power_restore_handler();
}
    #endif
#else
void switch_to_normal_mode(void)
{
}

    #if defined(LIMP_MODE_SUPPORT)
void switch_to_limp_mode(void)
{
}
    #endif

    #if defined(POWER_DOWN_SUPPORT)
void switch_to_powerfail_mode(void)
{
}
    #endif
#endif

#if defined(CORRECTED_RTC_SUPPORT)  &&  defined(__MSP430_HAS_TA3__)
int32_t assess_rtc_speed(void)
{
    int32_t period;
    uint16_t this_capture;
    uint16_t last_capture;
    uint16_t step;
    int32_t counter;
    int limit;

    /* The fast clock should be an exact multiple of the crystal clock, once the FLL has
        settled. If we capture many cycles of an accurate external 32768Hz clock, using
        timer A (or B), we can measure the speed difference between the MSP430's crystal
        and the external clock in a reasonable time. */
    /* The SM clock should be running at 244*32768Hz at this time. */
    _DINT();
    /* Change timer A to running fast, and sampling the external 32768Hz reference. */
    P2SEL |= BIT0;
    TACCR0 = 0xFFFF;
    TACCTL0 = CAP | CCIS_0 | CM_1;
    TACCTL2 = CAP | CCIS_0 | CM_1 | SCS;
    TACTL = TACLR | MC_2 | TASSEL_2;    /* start TIMER_A up mode, SMCLK as input clock */
    period = 0;
    last_capture = TACCR2;
    limit = -1;
    TACCTL2 &= ~CCIFG;
    for (counter = 0;  counter < 32768*5 + 1;  counter++)
    {
        limit = 1000;
        while (!(TACCTL2 & CCIFG))
        {
            if (--limit <= 0)
                break;
        }
        if (limit <= 0)
            break;
        TACCTL2 &= ~CCIFG;
        this_capture = TACCR2;
        step = this_capture - last_capture;
        last_capture = this_capture;
        /* Skip the first sample, as it will be meaningless */
        if (counter)
        {
    #if 0
            if (step < (244 - 5)  ||  step > (244 + 5))
            {
                limit = -2;
                break;
            }
    #endif
            period += step;
        }
        kick_watchdog();
    }
    if (limit <= 0)
        period = limit;
    #if defined(__MSP430_HAS_ADC12__)
    /* Change timer A back to controlling the ADC sampling interval, and the ADC on/off timing. */
    /* CCR0 determines the sample period */
    TACCR0 = SAMPLE_PERIOD - 1;
    TACCR1 = SAMPLE_PERIOD - 3;
    TACCR2 = SAMPLE_PERIOD - 4;
    TACCTL1 = OUTMOD_3;
    #endif
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
    P2SEL &= ~BIT0;
    _EINT();
    return  period;
}
#endif

#if !defined(ESP_SUPPORT)
int align_hardware_with_calibration_data(void)
{
#if !defined(SINGLE_PHASE)
    int ch;
    static struct phase_parms_s *phase;
    static struct phase_nv_parms_s const *phase_nv;
#endif

    disable_analog_front_end();
    init_analog_front_end_normal();

    #if !defined(SINGLE_PHASE)
    for (ch = 0;  ch < NUM_PHASES;  ch++)
    {
        phase = &chan[ch];
        phase_nv = &nv_parms.seg_a.s.chan[ch];
    #endif
    #if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
        set_phase_correction(&phase->metrology.current.in_phase_correction[0], phase_nv->current.phase_correction[0]);
        #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.current.in_phase_correction[1], phase_nv->current.phase_correction[1]);
        #endif
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        set_phase_correction(&phase->neutral.in_phase_correction[0], nv_parms.seg_a.s.neutral.phase_correction[0]);
            #if GAIN_STAGES > 1
        set_phase_correction(&phase->neutral.in_phase_correction[1], nv_parms.seg_a.s.neutral.phase_correction[1]);
            #endif
        #endif
    #elif !defined(ESP_SUPPORT)
        #if defined(SINGLE_PHASE)
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], 0, phase_nv->current.phase_correction[0]);
            #if defined(NEUTRAL_MONITOR_SUPPORT)
        set_sd16_phase_correction(&phase->metrology.neutral.in_phase_correction[0], 1, nv_parms.seg_a.s.neutral.phase_correction[0]);
            #endif
        #else
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], ch, phase_nv->current.phase_correction[0]);
        #endif
    #endif
    #if !defined(SINGLE_PHASE)
    }
    #endif
    return 0;
}
#endif
