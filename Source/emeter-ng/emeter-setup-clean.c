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
//  $Id: emeter-setup.c,v 1.26 2008/01/08 07:26:54 a0754793 Exp $
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
#if defined(__GNUC__)
#include <signal.h>
#endif
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter.h"
#include "emeter-structs.h"


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
    ESPCTL &= ~ESPEN;

    /*
     * Then the general configurations of the analog front-end are done
     * that apply to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;

    SD16CTL = 0x800
            | SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON;  /* Use internal reference */
    SD16INCTL_LIVE = SD16INCH_0 | CURRENT_LIVE_GAIN;            /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;    /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;


    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;              /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_256 | SD16DF | SD16SC | SD16IE;  /* Set oversampling ratio to 256 (default) */
    SD16PRE_VOLTAGE = 0;

    SD16CONF0 = SD16CONF0_FUDGE;                /* Tweaks recommended by Freising */
    SD16CONF1 = SD16CONF1_FUDGE;

    chan1.current.sd16_preloaded_offset = 0;
    /*
     * \note 
     * Please note, the oversampling ratio should be the same 
     * for all channels. Default is 256.
     */
}

static __inline__ void init_analog_front_end_limp(void)
{
    ESPCTL &= ~ESPEN;
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
    SD16CTL = 0x800
            | SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON   /* Use internal reference */
            | SD16LP;
    SD16INCTL_LIVE = SD16INCH_0 | CURRENT_LIVE_GAIN;                        /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;      /* Set oversampling ratio to 32 */
    SD16PRE_LIVE = 0;
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_32 | SD16DF | SD16SNGL | SD16IE;     /* Set oversampling ratio to 32 */
    SD16PRE_VOLTAGE = 0;
    SD16CONF0 = SD16CONF0_FUDGE;                                    /* Tweaks recommended by Freising */
    SD16CONF1 = SD16CONF1_FUDGE;

    chan1.current.sd16_preloaded_offset = 0;
}

static __inline__ void disable_analog_front_end(void)
{
    ESPCTL &= ~ESPEN;

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = 0;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;


    SD16CTL = 0;
    SD16CONF0 = SD16CONF0_FUDGE;
    SD16CONF1 = SD16CONF1_FUDGE;

    chan1.current.sd16_preloaded_offset = 0;
}

void system_setup(void)
{

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
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
    #else
    IE1 |= WDTIE;               /* Enable the WDT interrupt */
    #endif

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

    P1OUT = P1OUT_INIT;
    P1DIR = P1DIR_INIT;
    P1SEL = P1SEL_INIT;

    P2OUT = P2OUT_INIT;
    P2DIR = P2DIR_INIT;
    P2SEL = P2SEL_INIT;

    P3OUT = P3OUT_INIT;
    P3DIR = P3DIR_INIT;
    P3SEL = P3SEL_INIT;

    P4OUT = P4OUT_INIT;
    P4DIR = P4DIR_INIT;
    P4SEL = P4SEL_INIT;

    P5OUT = P5OUT_INIT;
    P5DIR = P5DIR_INIT;
    P5SEL = P5SEL_INIT;

    P6OUT = P6OUT_INIT;
    P6DIR = P6DIR_INIT;
    P6SEL = P6SEL_INIT;

    #if defined(P7OUT_INIT)
    P7OUT = P7OUT_INIT;
    #endif
    #if defined(P7DIR_INIT)
    P7DIR = P7DIR_INIT;
    #endif
    #if defined(P7SEL_INIT)
    P7SEL = P7SEL_INIT;
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

    #if defined(P9OUT_INIT)
    P9OUT = P9OUT_INIT;
    #endif
    #if defined(P9DIR_INIT)
    P9DIR = P9DIR_INIT;
    #endif
    #if defined(P9SEL_INIT)
    P9SEL = P9SEL_INIT;
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

    #if defined(IO_EXPANDER_SUPPORT)
    set_io_expander(0, 0);
    #endif
    
    display_startup_message();

    #if defined(__MSP430_HAS_DCO__)
    /* Set up the DCO clock */
    BCSCTL1 |= (RSEL0 | RSEL1 | RSEL2); /* Select the highest nominal freq */
    BCSCTL2 |= DCOR;                    /* Select the external clock control resistor pin */
    DCOCTL = 0xFF;                      /* Select the highest speed. */
    #endif
    #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    FLL_CTL0 |= XCAP10PF;               /* Configure load caps */
        #if defined(XT2OFF)
    FLL_CTL1 |= XT2OFF;
        #endif
    SCFI0 = FLLD_1;                     /* Freq = 1.024MHz */
    SCFQCTL = 32 - 1;
    FLL_CTL0 |= DCOPLUS;
    /* There seems no benefit in waiting for the FLL to settle at this point. */
    #endif

    samples_per_second = LIMP_SAMPLES_PER_10_SECONDS/10;

    #if defined(__MSP430_HAS_TA3__)  &&  defined(__MSP430_HAS_ADC12__)
    /* Use timer A to control the ADC sampling interval, and the ADC on/off timing. */
    /* TACCR0 determines the sample period */
    TAR = 0;
    TACCR0 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 1;
    TACCR1 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 3;
    TACCR2 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 4;
    TACCTL0 = 0;
    TACCTL1 = OUTMOD_3;
    TACTL = TACLR | MC_1 | TASSEL_1;
    #endif

    #if defined(__MSP430_HAS_TA3__)  &&  defined(__MSP430_HAS_SD16_3__)
    /* Use timer A to control the ADC sampling interval in limp mode. */
    /* CCR0 determines the sample period - 1024Hz */
    TAR = 0;
    TACCR0 = 32 - 1;
    TACCTL0 = 0;
    TACCTL1 = OUTMOD_3;
    TACTL = TACLR | MC_1 | TASSEL_1;
    #endif


    disable_analog_front_end();


        #if defined(__MSP430_HAS_UART0__)
    /* Configure UART0 */
    UCTL0 = CHAR;                       /* 8-bit character */
    /* 9600 Rx doesn't work very well from 32768Hz. Use the fast clock. */
    UTCTL0 = SSEL1;                     /* UCLK = SMCLK */
    UBR00 = 0x69;
    UBR10 = 0x3;
    UMCTL0 = 0x77;

    U0ME |= (UTXE0 | URXE0);            /* Enable USART0 TXD/RXD */
    U0IE |= URXIE0;
    /* If you do not initially kick the Tx port the TXEPT bit is not set. */
    TXBUF0 = 0;
        #endif
    
        #if defined(__MSP430_HAS_UART1__)
    /* Configure UART1 */
    //UCTL1 = PENA | PEV;               /* 7-bit + even parity character */
    UCTL1 = CHAR;                       /* 8-bit character */
    //UCTL1 = PENA | PEV | CHAR;        /* 8-bit + even parity character */
            #if USART1_BAUD_RATE == 9600
    /* 9600 Rx doesn't work very well from 32768Hz. Use the fast clock. */
    UTCTL1 = SSEL1;                     /* UCLK = SMCLK */
    UBR01 = 0x69;
    UBR11 = 0x3;
    UMCTL1 = 0x77;
            #elif USART1_BAUD_RATE == 4800
    UTCTL1 = SSEL0;                     /* UCLK = ACLK */
    UBR01 = 6;                          /* 32k/4800 - 6.8266 */
    UBR11 = 0;
    UMCTL1 = 0x6F;
            #elif USART1_BAUD_RATE == 2400
    UTCTL1 = SSEL0;                     /* UCLK = ACLK */
    UBR01 = 13;                         /* 32k/2400 - 13.653 */
    UBR11 = 0;
    UMCTL1 = 0x6B;
            #else
    UTCTL1 = SSEL0;                     /* UCLK = ACLK */
    UBR01 = 27;                         /* 32k/1200 - 27.307 */
    UBR11 = 0;
    UMCTL1 = 0x03;
            #endif

    ME2 |= (UTXE1 | URXE1);            /* Enable USART1 TXD/RXD */
    IE2 |= URXIE1;
    /* If you do not initially kick the Tx port, the TXEPT bit is not set. */
    TXBUF1 = 0;
        #endif

    /* Configure the IR receiver control line - we need to be able to power it down
       in limp mode, to conserve current. */
    disable_ir_receiver();

    meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_earthed_indicator();
    clr_reverse_current_indicator();
    clr_total_energy_pulse_indicator();
    clr_total_reactive_energy_pulse_indicator();
    clr_energy_pulse_indicator();

        /* Prime the DC estimates for quick settling */
        phase->current.I_dc_estimate[0] = phase_nv->current.initial_dc_estimate;
        phase->current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        phase->current.I_dc_estimate[1] = phase_nv->current.initial_dc_estimate;
        phase->V_dc_estimate[0] = phase_nv->initial_v_dc_estimate;
        phase->V_dc_estimate[1] = phase_nv->initial_v_limp_dc_estimate;
        phase->V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;

    temperature = 0;
    if (!check_rtc_sumcheck())
    {
        rtc.year = 7;
        rtc.month = 6;
        rtc.day = 11;
        rtc.hour = 12;
        rtc.minute = 0;
        rtc.second = 0;
        set_rtc_sumcheck();
    }
    custom_initialisation();

    _EINT();

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
        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    /* Speed up the clock to 8.388608MHz */
    SCFI0 = FN_3 | FLLD_4;
    SCFQCTL = 64 - 1;
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
}

void switch_to_normal_mode(void)
{
    /* Switch to full speed, full power mode */

    meter_status |= STATUS_PHASE_VOLTAGE_OK;
    set_normal_indicator();


        #if defined(__MSP430_HAS_TA3__)
    /* Disable the TIMER_A0 interrupt */
    TACTL = 0;
    TACCTL0 = 0;
        #endif
    _DINT();
    init_analog_front_end_normal();

    samples_per_second = SAMPLES_PER_10_SECONDS/10;

    _EINT();


    
    enable_ir_receiver();

    operating_mode = OPERATING_MODE_NORMAL;
}

void switch_to_limp_mode(void)
{
    /* Switch to minimum consumption, current measurement only mode */

    meter_status &= ~(STATUS_REVERSED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_reverse_current_indicator();


            #if defined(__MSP430_HAS_TA3__)
    /* Enable the TIMER_A0 interrupt */
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
            #endif
    _DINT();
    init_analog_front_end_limp();

    samples_per_second = LIMP_SAMPLES_PER_10_SECONDS/10;

    disable_ir_receiver();

    _EINT();

    operating_mode = OPERATING_MODE_LIMP;
}


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
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
    P2SEL &= ~BIT0;
    _EINT();
    return  period;
}
#endif
