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
//  File: emeter-foreground.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-foreground.c,v 1.29 2008/01/08 07:42:20 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 foreground (non-interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt. 
// 
//    Foreground process includes:
//    -Using timer tick to wait
//    -Calculating the power per channel
//    -Determine if current channel needs scaling.
//    -Determine if needs to be in low power modes.
//    -Compensate reference from temperature sensor
//
#include <stdint.h>
#include <stdlib.h>
#if defined(__GNUC__)
#include <signal.h>
#endif
#include <math.h>
#include <io.h>
#include <emeter-toolkit.h>
#define __MAIN_PROGRAM__
#include "emeter.h"
#include "emeter-structs.h"

uint8_t total_energy_pulse_remaining_time;
int32_t total_active_power;
int32_t total_active_power_counter;
uint32_t total_consumed_energy;

uint8_t total_reactive_energy_pulse_remaining_time;
int32_t total_reactive_power;
int32_t total_reactive_power_counter;
uint32_t total_consumed_reactive_energy;

#if 0 //CUSTOM_LCD_SUPPORT
/* Keep the toolkit library happy */
const int lcd_cells = LCD_CELLS;
const int lcd_pos_base = LCD_POS_BASE;
const int lcd_pos_step = LCD_POS_STEP;
#endif

uint16_t temperature;

int32_t rtc_correction;

/* Meter status flag bits. */
uint16_t meter_status;

/* Current operating mode - normal, limp, power down, etc. */
int8_t operating_mode;

/* Persistence check counters for anti-tamper measures. */
int8_t current_reversed;
int8_t current_unbalanced;

/* The main per-phase working parameter structure */
struct phase_parms_s chan1;

/* The main per-phase non-volatile parameter structure */
__infomem__ const struct info_mem_s nv_parms =
{
    {
    {
        0xFFFF,
            {
                {
                    {DEFAULT_I_RMS_SCALE_FACTOR_A},
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR},
                    0,
                    {DEFAULT_P_SCALE_FACTOR_A_LOW, DEFAULT_P_SCALE_FACTOR_A_HIGH},
                    DEFAULT_I_DC_ESTIMATE << 16,
                    {DEFAULT_BASE_PHASE_A_CORRECTION_LOW, DEFAULT_BASE_PHASE_A_CORRECTION_HIGH},
                },
                DEFAULT_V_RMS_SCALE_FACTOR_A,
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR,
                DEFAULT_V_DC_ESTIMATE << 16,
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
            },
        25,
        DEFAULT_TEMPERATURE_OFFSET,
        DEFAULT_TEMPERATURE_SCALING,
        0,
        {
            0,
            0,
            0,
            0,
            0,
            0
        },
        0,
        "",
        "",
        ""
    }
    }
};

#if !defined(__IAR_SYSTEMS_ICC__)
static __inline__ long labs(long __x);
static __inline__ long labs(long __x)
{
    return (__x < 0) ? -__x : __x;
}
#endif



void rtc_bumper(void)
{
    int i;

    i = bump_rtc();
    /* And now, a series of optional routines to get actions to take
       place at various intervals. Remember, we are in an interrupt
       routine. Do not do anything very complex here. If a complex action
       is needed set a flag in a simple routine and do the main work in
       the non-interrupt code's main loop. */
}

static void correct_rtc(void)
{
    int32_t temp;

    /* Correct the RTC to allow for basic error in the crystal, and
       temperature dependant changes. This is called every two seconds,
       so it must accumulate two seconds worth of error at the current
       temperature. */
    if (nv_parms.seg_a.s.temperature_offset)
    {
        temp = temperature - nv_parms.seg_a.s.temperature_offset;
        temp *= nv_parms.seg_a.s.temperature_scaling;
        temp >>= 16;

        /* The temperature is now in degrees C. */
        /* Subtract the centre point of the crystal curve. */
        temp -= 25;
        /* Do the parabolic curve calculation, to find the current ppm of
           error due to temperature, and then the scaled RTC correction
           value for 2 seconds at this temperature. */
        temp = temp*temp*(2589L*4295L >> 5);
        temp >>= 11;
        temp = -temp;
    }
    else
    {
        temp = 0;
    }
    /* Allow for the basic manufacturing tolerance error of the crystal, found
       at calibration time. */
    temp += nv_parms.seg_a.s.crystal_base_correction;
    if (rtc_correction >= 0)
    {
        rtc_correction += temp;
        if (rtc_correction < 0)
        {
            rtc_correction -= 0x80000000;
            /* We need to add an extra second to the RTC */
            rtc_bumper();
        }
    }
    else
    {
        rtc_correction += temp;
        if (rtc_correction >= 0)
        {
            rtc_correction += 0x80000000;
            /* We need to drop a second from the RTC */
            meter_status |= SKIP_A_SECOND;
        }
    }
}

#if defined(IO_EXPANDER_SUPPORT)
/* This routine supports the use of a device like the 74HC595 to expand the number of
   output bits available on the lower pin count MSP430s. */
void set_io_expander(int what, int which)
{
    static uint8_t io_state = 0;
    int i;
    int j;

    if (what < 0)
        io_state &= ~which;
    else if (what > 0)
        io_state |= which;
    else
        io_state = which;
    /* Pump the data into the shift register */
    for (i = 8, j = io_state;  i > 0;  i--)
    {
        P1OUT &= ~BIT4;
        if ((j & 0x80))
            P1OUT |= BIT7;
        else
            P1OUT &= ~BIT7;
        P1OUT |= BIT4;
        j <<= 1;
    }
    /* Clock the data into the output register */
    P1OUT &= ~BIT6;
    P1OUT |= BIT6;
}
#endif

void set_phase_correction(struct phase_correction_s *s, int correction)
{
    correction += 128;
    s->step = I_HISTORY_STEPS - (correction >> 8);
    s->fir_beta = fir_coeffs[127 - ((correction & 0xFF) >> 1)][0];
    s->fir_gain = fir_coeffs[127 - ((correction & 0xFF) >> 1)][1];
}


int set_initial_sd16_phase_correction(struct phase_correction_s *s, int correction)
{
    s->step = I_HISTORY_STEPS - (correction >> 8);
    return correction & 0xFF;
}



int32_t voltage(void)
{
    int16_t i;
    int32_t x;

    /* Calculate the RMS voltage in 10mV increments. Return -1 for overrange
       (i.e. ADC clip). */
    if ((phase->status & V_OVERRANGE))
        return -1;
    x = div_sh48(phase->V_sq_accum_logged, 26 - 2*ADC_BITS, phase->sample_count_logged);
    if (operating_mode == OPERATING_MODE_LIMP)
        i = phase_nv->V_rms_limp_scale_factor;
    else
        i = phase_nv->V_rms_scale_factor;
    x = isqrt32(x);
    x = (x >> 12)*i;
    x >>= 14;
    return x;
}

int32_t current(void)
{
    int32_t x;

    /* Calculate the RMS current in 1mA increments. Return -1 for overrange
       (i.e. ADC clip). A side effect of this routine is it updates the dynamic
       phase correction settings, based on the newly calculated current. */
    /* We always have to work out the properly scaled current from both leads, in
       order to work out the FIR coeffs for the next block. */
    if ((phase->status & I_OVERRANGE))
    {
        x = -1;
        phase->current.I_rms = x;
    }
    else
    {
    #if ADC_BITS == 16
        x = div48(phase->current.I_sq_accum_logged[0], phase->sample_count_logged);
    #else
        x = div_sh48(phase->current.I_sq_accum_logged[0], 32 - 2*ADC_BITS, phase->sample_count_logged);
    #endif
        x = isqrt32(x) - phase_nv->current.ac_offset;
        
        if (operating_mode == OPERATING_MODE_LIMP)
            x = (x >> 12)*phase_nv->current.I_rms_limp_scale_factor[0];
        else
            x = (x >> 12)*phase_nv->current.I_rms_scale_factor[0];
        x >>= 14;
        phase->current.I_rms = x;
    }
    if (operating_mode == OPERATING_MODE_LIMP)
    {
        /* We need to work out for ourselves which is the relevant current
           to use. */
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)  &&  POWER_BALANCE_DETECTION_SUPPORT
        x = test_phase_balance(x, y, PHASE_UNBALANCED_THRESHOLD_CURRENT);
          /* In limp mode we have no way to determine if the phase is reversed,
             so just say it is not. */
        phase->status &= ~PHASE_REVERSED;
        #endif
    }
    else
    {
        /* The power calculation has provided us which is the appropriate
           current to use. */
    }
    return  x;
}


int32_t fredx;
int32_t fredy;

int32_t active_power(void)
{
    int32_t x;
    int32_t y;
    int16_t i;
    int reversed;

    /* We can only do real power assessment in full operating mode. */
    /* If we have neutral monitoring for a single phase meter, we need to measure
       both power levels, and decide between them. Issues to be assessed here are
       whether one or both leads show reverse power, and whether the power levels
       are balanced. */
    /* If we find a negative power level we may be genuinely feeding power to the grid,
       or we may be seeing a tamper condition. This is application dependant. */
    reversed = FALSE;

    x = div_sh48(phase->current.P_accum_logged[0], 27 - 2*ADC_BITS, phase->sample_count_logged);
    i = phase_nv->current.P_scale_factor[0];
#if defined(PHASE_CORRECTION_SUPPORT)  &&  (defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__))
    i = Q1_15_mul(i, phase->current.in_phase_correction[0].fir_gain);
#endif
    fredx = mul48(x, i);
    x = fredx;
    y = div_sh48(phase->current.P_accum_logged[1], 27 - 2*ADC_BITS, phase->sample_count_logged);
    i = phase_nv->current.P_scale_factor[1];
#if defined(PHASE_CORRECTION_SUPPORT)  &&  (defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__))
    i = Q1_15_mul(i, phase->current.in_phase_correction[1].fir_gain);
#endif
    fredy = mul48(y, i);
    x += fredy;
    phase->current.active_power = x;
    if (x < 0)
    {
        x = -x;
        phase->status |= I_REVERSED;
        if (x > PHASE_REVERSED_THRESHOLD_POWER)
            reversed = TRUE;
    }
    else
    {
        phase->status &= ~I_REVERSED;   
    }
    if ((phase->status & PHASE_REVERSED))
    {
        if (!reversed)
        {
            if (--current_reversed <= -PHASE_REVERSED_PERSISTENCE_CHECK)
            {
                phase->status &= ~PHASE_REVERSED;
                current_reversed = 0;
            }
        }
        else
        {
            current_reversed = 0;
        }
    }
    else
    {
        if (reversed)
        {
            if (++current_reversed >= PHASE_REVERSED_PERSISTENCE_CHECK)
            {
                phase->status |= PHASE_REVERSED;
                current_reversed = 0;
            }
        }
        else
        {
            current_reversed = 0;
        }
    }
    return  x;
}




#if defined(__AQCOMPILER__)  ||  defined(__IAR_SYSTEMS_ICC__)
void main(void)
#else
int main(int argc, char *argv[])
#endif
{
    static int32_t x;

    system_setup();
    
    #if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
        set_phase_correction(&phase->current.in_phase_correction[0], phase_nv->current.phase_correction[0]);
        set_phase_correction(&phase->current.in_phase_correction[1], phase_nv->current.phase_correction[1]);
    #else
        SD16PRE_LIVE = set_initial_sd16_phase_correction(&phase->current.in_phase_correction[0], phase_nv->current.phase_correction);
        SD16PRE_NEUTRAL = set_initial_sd16_phase_correction(&phase->neutral.in_phase_correction[0], phase_nv->neutral.phase_correction);
    #endif

    
    for (;;)
    {
        kick_watchdog();
            /* Unless we are in normal operating mode, we should wait to be
               woken by a significant event from the interrupt routines. */
            if (operating_mode != OPERATING_MODE_NORMAL)
                _BIS_SR(LPM0_bits);
            if (nv_parms.seg_a.s.meter_uncalibrated)
                enable_ir_receiver();
            if ((phase->status & NEW_LOG))
            {
                /* The background activity has informed us that it is time to
                   perform a block processing operation. */
                phase->status &= ~NEW_LOG;
                if (operating_mode == OPERATING_MODE_NORMAL)
                {
                    /* We can only do real power assessment in full operating mode */
                    x = active_power();
                    phase->I_rms = current();
                    phase->V_rms = voltage();
                }
                else if (operating_mode == OPERATING_MODE_LIMP)
                {
                    /* In limp mode we must assess estimated power from only the measured current. */
                    /* We cannot properly determine current reversal in this mode. Also, current
                       imbalance is really just a measure of which lead is still connected.
                       Just treat both the imbalance and reversal conditions as OK */
                    x = current();
                    phase->V_rms = voltage();
                    phase->I_rms = x;
                    x = x*MAINS_NOMINAL_VOLTAGE/10;
                }
                if (labs(x) < RESIDUAL_POWER_CUTOFF  ||  (phase->status & V_OVERRANGE))
                {
                    x = 0;
                    /* Avoid displaying a residual current, which is nothing more
                       than integrated noise. */
                    //phase->I_rms = 0;
                    /* Turn off the LEDs, regardless of the internal state of the
                       reverse and imbalance assessments. */
                    meter_status &= ~STATUS_REVERSED;
                    clr_reverse_current_indicator();
                    meter_status &= ~STATUS_EARTHED;
                    clr_earthed_indicator();
                }
                else
                {
                    if (operating_mode == OPERATING_MODE_NORMAL)
                    {
                        if ((phase->status & PHASE_REVERSED))
                        {
                            meter_status |= STATUS_REVERSED;
                            set_reverse_current_indicator();
                        }
                        else
                        {
                            meter_status &= ~STATUS_REVERSED;
                            clr_reverse_current_indicator();
                        }
                        if ((phase->status & PHASE_UNBALANCED))
                        {
                            meter_status |= STATUS_EARTHED;
                            set_earthed_indicator();
                        }
                        else
                        {
                            meter_status &= ~STATUS_EARTHED;
                            clr_earthed_indicator();
                        }
                    }
                    else
                    {
                        /* We cannot tell forward from reverse current in limp mode,
                           so just say it is not reversed. */
                        meter_status &= ~STATUS_REVERSED;
                        clr_reverse_current_indicator();
                        /* We are definitely in the unbalanced state, but only light
                           the LED if we have persistence checked, and the current
                           is sufficient to sustain operation. */
                        if ((phase->status & PHASE_UNBALANCED)  &&  phase->I_rms >= LIMP_MODE_MINIMUM_CURRENT)
                        {
                            meter_status |= STATUS_EARTHED;
                            set_earthed_indicator();
                        }
                        else
                        {
                            meter_status &= ~STATUS_EARTHED;
                            clr_earthed_indicator();
                        }
                        /* Only run the IR interface if we are sure there is enough power from the
                           supply to support the additional current drain. If we have not yet been
                           calibrated we had better keep the IR port running so we can complete the
                           calibration. */
                        if (phase->I_rms >= LIMP_MODE_MINIMUM_CURRENT_FOR_IR
                            ||
                            nv_parms.seg_a.s.meter_uncalibrated)
                        {
                            enable_ir_receiver();
                        }
                        else
                        {
                            disable_ir_receiver();
                        }
                    }
                }
                //x /= 10;
                total_active_power = x;
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)  &&  defined(PHASE_REVERSED_IS_GENERATION)
#endif
                phase->active_power = x;
                phase->energy_counter += x*phase->sample_count_logged;
                while (phase->energy_counter > ENERGY_WATT_HOUR_THRESHOLD)
                {
                    phase->energy_counter -= ENERGY_WATT_HOUR_THRESHOLD;
                    phase->consumed_energy++;
                }
            }
            /* The voltage channel DC estimate will never move very much when the
               meter is operating normally. If it does move, there must be some
               tampering, such as a diode between the grid and the meter. */
            if (operating_mode == OPERATING_MODE_NORMAL)
            {
                if (phase->V_rms < LIMP_MODE_VOLTAGE_THRESHOLD*100
                    ||
                    phase->V_dc_estimate[0] > UPPER_TAMPER_V_DC_ESTIMATE
                    ||
                    phase->V_dc_estimate[0] < LOWER_TAMPER_V_DC_ESTIMATE)
                {
                    switch_to_limp_mode();
                }
            }
            else if (operating_mode == OPERATING_MODE_LIMP)
            {
                if (phase->V_rms >= NORMAL_MODE_VOLTAGE_THRESHOLD*100
                    &&
                    phase->V_dc_estimate[1] <= UPPER_LIMP_TAMPER_V_DC_ESTIMATE
                    &&
                    phase->V_dc_estimate[1] >= LOWER_LIMP_TAMPER_V_DC_ESTIMATE)
                {
                    /* The LCD might need to be revived */
                    LCDawaken();
                    switch_to_normal_mode();
                }
            }



        /* Do display and other housekeeping here */
        if ((meter_status & TICKER))
        {
            /* Two seconds have passed */
            /* We have a 2 second tick */
            meter_status &= ~TICKER;
            /* Update the display, cycling through the phases */
            update_display();
            custom_2second_handler();
            correct_rtc();
        }
        custom_keypad_handler();
        custom_mainloop_handler();
    }
#if !defined(__AQCOMPILER__)  &&  !defined(__IAR_SYSTEMS_ICC__)
    return  0;
#endif
}
