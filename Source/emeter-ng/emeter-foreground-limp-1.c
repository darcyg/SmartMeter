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
        {
            DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL,
            DEFAULT_I_RMS_LIMP_SCALE_FACTOR_NEUTRAL,
            0,
            DEFAULT_P_SCALE_FACTOR_NEUTRAL,
            DEFAULT_I_DC_ESTIMATE << 16,
            DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION,
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

static int32_t test_phase_balance(int32_t live_signal, int32_t neutral_signal, int threshold)
{
    int permitted_imbalance_fraction;

    /* This may be testing between two currents, or between two powers. I normal mode it
       is testing between two power readings. In limp mode it is testing between two
       current readings. */
    /* See which one is bigger, with some tolerance built in. If the signal measured
       from the neutral is more than 6.25% or 12.5% (options) different from the signal
       measured from the live there is something wrong (maybe fraudulent tampering, or
       just something faulty). In this case we use the current measured from the
       channel with the higher signal. When the channel is reasonably balanced, we use
       the signal from the live lead. If neither signal is above the threshold we use
       a more relaxed measure of imbalance (say 25% or even 50%), to allow for the lower
       accuracy of these small signals (remember, we need the test to work well under
       transient conditions, so the threshold needs to be far more relaxed than the
       basic measurement accuracy would indicate). Assessments are persistence checked
       to avoid transient conditions causing a false change of imbalance status. */
    if (live_signal <= threshold  &&  neutral_signal <= threshold)
        permitted_imbalance_fraction = RELAXED_IMBALANCE_FRACTION;
    else
        permitted_imbalance_fraction = PERMITTED_IMBALANCE_FRACTION;
    /* We have a signal strong enough for proper assessment. */
    if ((phase->status & PHASE_UNBALANCED))
    {
        /* We are looking for the restoration of balance between the
           live and neutral. */
        if ((live_signal - (live_signal >> permitted_imbalance_fraction)) < neutral_signal
            &&
            neutral_signal < (live_signal + (live_signal >> permitted_imbalance_fraction)))
        {
            /* Things might be balanced, but persistence check to be sure. */
            if (--current_unbalanced <= -PHASE_UNBALANCED_PERSISTENCE_CHECK)
            {
                /* Things look genuinely balanced. */
                phase->status &= ~(PHASE_UNBALANCED | CURRENT_FROM_NEUTRAL);
                current_unbalanced = 0;
            }
        }
        else
        {
            current_unbalanced = 0;
            /* The imbalanced might have swapped around - check. */
            /* Here we just choose the greater signal each block, as we have
                already confirmed (i.e. debounced) the imbalance condition. */
            if (neutral_signal > live_signal)
                phase->status |= CURRENT_FROM_NEUTRAL;
            else
                phase->status &= ~CURRENT_FROM_NEUTRAL;
        }
    }
    else
    {
        /* We are looking for the live and neutral becoming unbalanced. */
        if ((live_signal - (live_signal >> permitted_imbalance_fraction)) > neutral_signal
            ||
            neutral_signal > (live_signal + (live_signal >> permitted_imbalance_fraction)))
        {
            /* Things might be unbalanced, but persistence check to be sure. */
            if (++current_unbalanced >= PHASE_UNBALANCED_PERSISTENCE_CHECK)
            {
                /* Things look genuinely unbalanced. */
                current_unbalanced = 0;
                phase->status |= PHASE_UNBALANCED;
                if (neutral_signal > live_signal)
                    phase->status |= CURRENT_FROM_NEUTRAL;
                else
                    phase->status &= ~CURRENT_FROM_NEUTRAL;
            }
        }
        else
        {
            current_unbalanced = 0;
        }
    }
    /* Clear the unbalanced detection, so we don't display unbalanced. This should eliminate
       flashing of the LED if the are transient conditions causing false indications of
       imbalance. */
    if (live_signal <= threshold  &&  neutral_signal <= threshold)
        phase->status &= ~PHASE_UNBALANCED;
    if ((phase->status & CURRENT_FROM_NEUTRAL))
        return  neutral_signal;
    return  live_signal;
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
    int32_t y;

    /* Calculate the RMS current in 1mA increments. Return -1 for overrange
       (i.e. ADC clip). A side effect of this routine is it updates the dynamic
       phase correction settings, based on the newly calculated current. */
    /* We always have to work out the properly scaled current from both leads, in
       order to work out the FIR coeffs for the next block. */
    if ((phase->status & I_NEUTRAL_OVERRANGE))
    {
        y = -1;
        phase->neutral.I_rms = y;
    }
    else
    {

        y = div48(phase->neutral.I_sq_accum_logged[0], phase->sample_count_logged);

        y = isqrt32(y) - nv_parms.seg_a.s.neutral.ac_offset;

        y = (y >> 12)*nv_parms.seg_a.s.neutral.I_rms_limp_scale_factor[0];

        y >>= 14;
        phase->neutral.I_rms = y;
    }
    if ((phase->status & I_OVERRANGE))
    {
        x = -1;
        phase->current.I_rms = x;
    }
    else
    {

        x = div48(phase->current.I_sq_accum_logged[0], phase->sample_count_logged);

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
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(POWER_BALANCE_DETECTION_SUPPORT)
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
        if ((phase->status & CURRENT_FROM_NEUTRAL))
               x = y;
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
    
    switch_to_limp_mode();
    
    for (;;)
    {

            _BIS_SR(LPM0_bits);

            if ((phase->status & NEW_LOG))
            {
                /* The background activity has informed us that it is time to
                   perform a block processing operation. */
                phase->status &= ~NEW_LOG;

                    /* In limp mode we must assess estimated power from only the measured current. */
                    /* We cannot properly determine current reversal in this mode. Also, current
                       imbalance is really just a measure of which lead is still connected.
                       Just treat both the imbalance and reversal conditions as OK */
                    x = current();
                    phase->V_rms = voltage();
                    phase->I_rms = x;
                    x = x*MAINS_NOMINAL_VOLTAGE/10;



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

                //x /= 10;
                total_active_power = x;

                phase->active_power = x;
                phase->energy_counter += x*phase->sample_count_logged;
                while (phase->energy_counter > ENERGY_WATT_HOUR_THRESHOLD)
                {
                    phase->energy_counter -= ENERGY_WATT_HOUR_THRESHOLD;
                    phase->consumed_energy++;
                }
                
            update_display();    
            }


    }

}
