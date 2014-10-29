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
//  File: emeter-background.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-background.c,v 1.32 2007/11/26 07:06:27 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 background (interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt. 
//
//    The background process deals with the input samples.
//    These are first stored into buffers.
//    The buffered samples are processed as follows:
//    -Voltage and current signals are converted to DC-less AC signals
//    -The current signal is phase compensated
//    -Voltage and current are signed multiplied to give power.
//    -Power samples are accumulated. The accumulated power samples are averaged (in foreground.c)
//     after a number of voltage cycles has been detected.   
//
#include <stdint.h>
#include <stdlib.h>
#if defined(__GNUC__)
#include <signal.h>
#endif
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter.h"
#include "emeter-structs.h"

#if !defined(NULL)
#define NULL    (void *) 0
#endif

int16_t samples_per_second;


static uint8_t debounce_key_1;
static int16_t key_timer_1;
uint8_t key_states;

uint16_t battery_countdown;




/* This keypad debounce code provides for 1 to 4 keys, with debounce + long
   press detect, of debounce + auto-repeat on long press selectable for each
   key. Definitions in emeter.h control this. A long press means >2s.
   Auto-repeat means holding the key >1s starts repeats at 3 per second. */
static __inline__ int keypad_debounce(void)
{
    int kick_foreground;
    
    kick_foreground = FALSE;
    switch (debounce(&debounce_key_1, sense_key_1_up()))
    {
    case DEBOUNCE_JUST_RELEASED:
        key_timer_1 = 0;
        break;
    case DEBOUNCE_JUST_HIT:
        /* Start an initial 1s timeout for repeats */
        key_timer_1 = samples_per_second;
        key_states |= KEY_1_DOWN;
        kick_foreground = TRUE;
        break;
    case DEBOUNCE_HIT:
        if (key_timer_1  &&  --key_timer_1 == 0)
        {
            /* Start a 1/3s timeout for repeats */
            if (operating_mode == OPERATING_MODE_LIMP)
                key_timer_1 = 273;
            else
                key_timer_1 = 1092;
            key_states |= KEY_1_REPEAT_DOWN;
            kick_foreground = TRUE;
        }
        break;
    }
    return  kick_foreground;
}

static void __inline__ log_parameters(void)
{
#define i 0

    /**/ P3OUT ^= 0x02;

    /* Take a snapshot of various values for logging purposes; tell the
       foreground to deal with them; and clear the working values ready
       for the next analysis period. */
    if (phase->V_endstops <= 0)
        phase->status |= V_OVERRANGE;
    else
        phase->status &= ~V_OVERRANGE;
    phase->V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    transfer48(phase->V_sq_accum_logged, phase->V_sq_accum);

    if (phase->current.I_endstops <= 0)
        phase->status |= I_OVERRANGE;
    else
        phase->status &= ~I_OVERRANGE;
    phase->current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
    if (phase->neutral.I_endstops <= 0)
        phase->status |= I_NEUTRAL_OVERRANGE;
    else
        phase->status &= ~I_NEUTRAL_OVERRANGE;
    phase->neutral.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;

    phase->current.sample_count_logged = phase->current.sample_count;
    phase->current.sample_count = 0;
    phase->neutral.sample_count_logged = phase->neutral.sample_count;
    phase->neutral.sample_count = 0;
    {
        transfer48(phase->current.I_sq_accum_logged[i], phase->current.I_sq_accum[i]);
        transfer48(phase->current.P_accum_logged[i], phase->current.P_accum[i]);

        transfer48(phase->neutral.I_sq_accum_logged[i], phase->neutral.I_sq_accum[i]);
        transfer48(phase->neutral.P_accum_logged[i], phase->neutral.P_accum[i]);
    }
    phase->sample_count_logged = phase->sample_count;
    phase->sample_count = 0;
    
    /* Tell the foreground there are things to process. */
    phase->status |= NEW_LOG;
#undef i
}



int16_t fred;
int16_t fred2;

/*---------------------------------------------------------------------------
  This is the main interrupt routine where the main signal processing is done
  ---------------------------------------------------------------------------*/
ISR(SD16, adc_interrupt)
{
    int16_t V_sample;
    int16_t corrected;
    int16_t I_live_sample;
    int16_t I_neutral_sample;
#define i 0
    static int16_t adc_buffer[3];
    int adc_ptr;
    

    /**/ P3OUT &= ~0x04;
    if (!(SD16CCTL_VOLTAGE & SD16IFG))
    {
        /* We do not have a complete set of samples yet, but we may need to pick
           up some current values at this time */
        if ((SD16CCTL_NEUTRAL & SD16IFG))
        {
            adc_buffer[2] = SD16MEM_NEUTRAL;
            SD16CCTL_NEUTRAL &= ~SD16IFG;
        }
        if ((SD16CCTL_LIVE & SD16IFG))
        {
            adc_buffer[1] = SD16MEM_LIVE;
            SD16CCTL_LIVE &= ~SD16IFG;
        }
        return;
    }

    /* Filter away the DC bias.
    
       Do the phase lag compensation. Use a simple FIR approach,
       and absorb the non-unity gain of the filter in the overall
       current/power scaling later on. This is OK for the small
       phase shifts we expect to get. It would cause dynamic
       range problems for larger shifts. Note the some of this
       phase shift is due to the operation of the ADC itself. It
       performs sequential conversions of its 8 inputs, so there is
       some time delay between sampling of the various sensors.
    
       Accumulate power for each of the channels. These will
       be divided by the number of samples at the end of the
       measurement cycles, resulting in an average power
       value for each source.

       If RMS voltage and/or current readings are required, calculate the
       dot products needed to evaluate these. */


    /* Voltage is available */
    adc_buffer[0] = SD16MEM_VOLTAGE;
    SD16CCTL_VOLTAGE &= ~SD16IFG;
    /* Pick up any current samples which may have occurred a little before the
       voltage sample, but not those which may have occurred just after the
       voltage sample. */
    if (phase->neutral.sd16_preloaded_offset < 128  &&  (SD16CCTL_NEUTRAL & SD16IFG))
    {
        adc_buffer[2] = SD16MEM_NEUTRAL;
        SD16CCTL_NEUTRAL &= ~SD16IFG;
    }
    if (phase->current.sd16_preloaded_offset < 128  &&  (SD16CCTL_LIVE & SD16IFG))
    {
        adc_buffer[1] = SD16MEM_LIVE;
        SD16CCTL_LIVE &= ~SD16IFG;
    }
    /* We have a complete set of samples. Process them. */
    adc_ptr = -1;


    kick_watchdog();

        V_sample = adc_buffer[++adc_ptr];
        if ((V_sample >= ADC_MAX  ||  V_sample <= ADC_MIN)  &&  phase->V_endstops)
            phase->V_endstops--;
        V_sample = dc_filter(&phase->V_dc_estimate[(operating_mode == OPERATING_MODE_NORMAL)  ?  0  :  1], V_sample);
        accum48(phase->V_sq_accum, imul16(V_sample, V_sample));
        /* We need to save the history of the voltage signal if we are performing phase correction, and/or
           measuring the quadrature shifted power (to obtain an accurate measure of one form of the reactive power). */
        phase->V_history[(int) phase->V_history_index] = V_sample;
        I_live_sample = -dc_filter(&phase->current.I_dc_estimate[0], phase->current.I_history[0][0]);
        corrected = adc_buffer[++adc_ptr];
        if ((corrected >= ADC_MAX  ||  corrected <= ADC_MIN)  &&  phase->current.I_endstops)
            phase->current.I_endstops--;
        phase->current.I_history[0][0] = phase->current.I_history[0][1];
        phase->current.I_history[0][I_HISTORY_STEPS - 1] = corrected;
        accum48(phase->current.I_sq_accum[use_stage], imul16(I_live_sample, I_live_sample));
        if (operating_mode == OPERATING_MODE_NORMAL)
        {
            /* Perform phase shift compensation, to allow for the time
               between ADC samplings, internal phase shifts in CTs, etc.
               This uses a 1 tap FIR (basically an interpolator/extrapolator) */
    #if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
            corrected = Q1_15_mul(phase->V_history[(phase->V_history_index - phase->current.in_phase_correction[use_stage].step - 1) & V_HISTORY_MASK], phase->current.in_phase_correction[use_stage].fir_beta)
                      + phase->V_history[(phase->V_history_index - phase->current.in_phase_correction[use_stage].step) & V_HISTORY_MASK];
    #else
            corrected = phase->V_history[(phase->V_history_index - phase->current.in_phase_correction[use_stage].step) & V_HISTORY_MASK];
    #endif
            accum48(phase->current.P_accum[use_stage], imul16(corrected, I_live_sample));
        }
        ++phase->current.sample_count;

        I_neutral_sample = dc_filter(&phase->neutral.I_dc_estimate, phase->neutral.I_history[0][0]);
        corrected = adc_buffer[++adc_ptr];
        if ((corrected >= ADC_MAX  ||  corrected <= ADC_MIN)  &&  phase->neutral.I_endstops)
            phase->neutral.I_endstops--;
        phase->neutral.I_history[0][0] = phase->neutral.I_history[0][1];
        phase->neutral.I_history[0][I_HISTORY_STEPS - 1] = corrected;
        accum48(phase->neutral.I_sq_accum[0], imul16(I_neutral_sample, I_neutral_sample));
        if (operating_mode == OPERATING_MODE_NORMAL)
        {
        #if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
            corrected = Q1_15_mulq(phase->V_history[(phase->V_history_index - phase->neutral.in_phase_correction[use_stage].step - 1) & V_HISTORY_MASK], phase->neutral.in_phase_correction[use_stage].fir_beta)
                      + phase->V_history[(phase->V_history_index - phase->neutral.in_phase_correction[use_stage].step) & V_HISTORY_MASK];
        #else
            corrected = phase->V_history[(phase->V_history_index - phase->neutral.in_phase_correction[use_stage].step) & V_HISTORY_MASK];
        #endif
            accum48(phase->neutral.P_accum[0], imul16(corrected, I_neutral_sample));
        }
        ++phase->neutral.sample_count[0];
        phase->V_history_index = (phase->V_history_index + 1) & V_HISTORY_MASK;
        ++phase->sample_count;

        /* We now play the last measurement interval's power level, evaluated
           in the foreground, through this measurement interval. In this way
           we can evenly pace the pulsing of the LED. The only error produced
           by this is the ambiguity in the number of samples per measurement.
           This should not exceed 1 or 2 in over 4000. */
        if (operating_mode == OPERATING_MODE_NORMAL)
        {
            if ((phase->active_power_counter += phase->active_power) >= PHASE_ENERGY_PULSE_THRESHOLD)
            {
                phase->active_power_counter -= PHASE_ENERGY_PULSE_THRESHOLD;
                ++phase->consumed_energy;
                /* Ideally we want to log the energy each kWh unit, but doing
                   it with a mask here is good enough and faster. */
                if ((phase->consumed_energy & 0x3FF) == 0)
                    phase->status |= ENERGY_LOGABLE;
                /* Pulse the LED. Long pulses may not be reliable, as at full
                   power we may be pulsing several times per second. People may
                   check the meter's calibration with an instrument that counts
                   the pulsing rate, so it is important the pulses are clear,
                   distinct, and exactly at the rate of one per
                   1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
                set_energy_pulse_indicator();
                phase->energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
            }
            if (phase->energy_pulse_remaining_time  &&  --phase->energy_pulse_remaining_time == 0)
            {
                clr_energy_pulse_indicator();
            }
        }
        else
        {
            if ((phase->active_power_counter += LIMP_SAMPLING_RATIO*phase->active_power) >= PHASE_ENERGY_PULSE_THRESHOLD)
            {
                phase->active_power_counter -= PHASE_ENERGY_PULSE_THRESHOLD;
                ++phase->consumed_energy;
                if ((phase->consumed_energy & 0x3FF) == 0)
                    phase->status |= ENERGY_LOGABLE;
                set_energy_pulse_indicator();
                phase->energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
            }
            if (phase->energy_pulse_remaining_time  &&  (phase->energy_pulse_remaining_time -= LIMP_SAMPLING_RATIO) == 0)
            {
                clr_energy_pulse_indicator();
            }
        }
        /* Do the power cycle start detection */
        /* There is no hysteresis used here, but since the signal is
           changing rapidly at the zero crossings, and is always of
           large amplitude, miscounting cycles due to general noise
           should not occur. Spikes are another matter. A large spike
           could cause the power cycles to be miscounted, but does not
           matter very much. The cycle counting is not critical to power
           or energy measurement. */
        if (abs(V_sample - phase->last_V_sample) <= phase->since_last*MAX_PER_SAMPLE_VOLTAGE_SLEW)
        {
            /* This doesn't look like a spike - do mains cycle detection, and
               estimate the precise mains period */
            if (V_sample < 0)
            {
                /* Log the sign of the signal */
                phase->status &= ~V_POS;
            }
            else
            {
                if (!(phase->status & V_POS))
                {
                    /* See if a sufficiently long measurement interval has been
                       recorded, and catch the start of the next cycle. We do not
                       really care how many cycles there are, as long as the block
                       is a reasonable length. Setting a minimum of 1 second is
                       better than counting cycles, as it is not affected by noise
                       spikes. Synchronising to a whole number of cycles reduces
                       block to block jitter, though it doesn't affect the long
                       term accuracy of the measurements. */
                    if (phase->sample_count >= samples_per_second)
                    {
                        log_parameters();
                        /* The foreground may be conserving power (e.g. in limp mode), so we
                           need to kick it. */
                        _BIC_SR_IRQ(LPM0_bits);
                    }
                }
                /* Log the sign of the signal */
                phase->status |= V_POS;
            }
            phase->since_last = 0;
            phase->last_V_sample = V_sample;
        }
        phase->since_last++;

        if (phase->sample_count >= samples_per_second + 200)
        {
            /* We don't seem to be detecting the end of a mains cycle, so force
               the end of processing block condition. */
            log_parameters();
            /* The foreground may be conserving power (e.g. in limp mode), so we
               need to kick it. */
            _BIC_SR_IRQ(LPM0_bits);
        }
        if (I_live_sample < 0)
        {
            /* Log the sign of the signal */
            phase->status &= ~I_POS;
        }
        else
        {
            if (!(phase->status & I_POS))
            {
                /* A negative to positive transition has occurred. Trust it
                   blindly as a genuine zero crossing/start of cycle, even
                   though it might really be due to a noise spike. */
            }
            /* Log the sign of the signal */
            phase->status |= I_POS;
        }
        if (I_neutral_sample < 0)
        {
            /* Log the sign of the signal */
            phase->status &= ~I_NEUTRAL_POS;
        }
        else
        {
            if (!(phase->status & I_NEUTRAL_POS))
            {
                /* A negative to positive transition has occurred. Trust it
                   blindly as a genuine zero crossing/start of cycle, even
                   though it might really be due to a noise spike. */
            }
            /* Log the sign of the signal */
            phase->status |= I_NEUTRAL_POS;
        }


    /* We now play the last measurement interval's power level, evaluated
       in the foreground, through this measurement interval. In this way
       we can evenly pace the pulsing of the LED. The only error produced
       by this is the ambiguity in the number of samples per measurement.
       This should not exceed 1 or 2 in over 4000. */
    if (operating_mode == OPERATING_MODE_NORMAL)
    {
    
        if ((total_active_power_counter += total_active_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        {
            total_active_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
                ++total_consumed_energy;
                /* Ideally we want to log the energy each kWh unit, but doing
                   it with a mask here is good enough and faster. */
                if ((total_consumed_energy & 0x3FF) == 0)
                    phase->status |= ENERGY_LOGABLE;
                /* Pulse the LED. Long pulses may not be reliable, as at full
                   power we may be pulsing many times per second. People may
                   check the meter's calibration with an instrument that counts
                   the pulsing rate, so it is important the pulses are clear,
                   distinct, and exactly at the rate of one per
                   1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
                set_total_energy_pulse_indicator();
                total_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
                custom_energy_pulse_start();
        }
        if (total_energy_pulse_remaining_time  &&  --total_energy_pulse_remaining_time == 0)
        {
            clr_total_energy_pulse_indicator();
            custom_energy_pulse_end();
        }

        if ((total_reactive_power_counter += total_reactive_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        {
            total_reactive_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
                ++total_consumed_reactive_energy;
                /* Ideally we want to log the energy each kWh unit, but doing
                   it with a mask here is good enough and faster. */
                if ((total_consumed_reactive_energy & 0x3FF) == 0)
                    phase->status |= ENERGY_LOGABLE;
                /* Pulse the LED. Long pulses may not be reliable, as at full
                   power we may be pulsing many times per second. People may
                   check the meter's calibration with an instrument that counts
                   the pulsing rate, so it is important the pulses are clear,
                   distinct, and exactly at the rate of one per
                   1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
                set_total_reactive_energy_pulse_indicator();
                total_reactive_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
        }
        if (total_reactive_energy_pulse_remaining_time  &&  --total_reactive_energy_pulse_remaining_time == 0)
        {
            clr_total_reactive_energy_pulse_indicator();
        }

    }
    else
    {
        if ((total_active_power_counter += LIMP_SAMPLING_RATIO*total_active_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
        {
            total_active_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
                ++total_consumed_energy;
                if ((total_consumed_energy & 0x3FF) == 0)
                    phase->status |= ENERGY_LOGABLE;
                custom_energy_pulse_start();
                set_total_energy_pulse_indicator();
                total_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
                set_total_reactive_energy_pulse_indicator();
                total_reactive_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
        }
        if (total_energy_pulse_remaining_time  &&  (total_energy_pulse_remaining_time -= LIMP_SAMPLING_RATIO) == 0)
        {
            clr_total_energy_pulse_indicator();
            custom_energy_pulse_end();
        }
    }

    custom_adc_interrupt();

#if defined(x__MSP430__)
    if (keypad_debounce())
        _BIC_SR_IRQ(LPM0_bits);

    /* Update the inter-character timeout for receiving IR messages */
    if (char_timeout_1107)
        char_timeout_1107--;

#endif

    /* There may be some current samples available, which we need to pick up */
    if ((SD16CCTL_NEUTRAL & SD16IFG))
    {
        adc_buffer[2] = SD16MEM_NEUTRAL;
        SD16CCTL_NEUTRAL &= ~SD16IFG;
    }
    if ((SD16CCTL_LIVE & SD16IFG))
    {
        adc_buffer[1] = SD16MEM_LIVE;
        SD16CCTL_LIVE &= ~SD16IFG;
    }
    /**/ P3OUT |= 0x04;
}


/* Interrupt to trigger the SD16 ADC in limp mode */
ISR(TIMERA0, limp_trigger_interrupt)
{
    {
        /* Trigger the ADC to perform a single conversion from all inputs. */
        SD16PRE_NEUTRAL = 0;
        SD16PRE_LIVE = 0;
        SD16PRE_VOLTAGE = 0;
        SD16CCTL_VOLTAGE |= SD16SC;
    }
}
