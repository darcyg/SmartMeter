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
//  $Id: emeter-foreground.c,v 1.43 2009/04/27 09:05:26 a0754793 Exp $
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
#if !defined(__MSP430__)
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#if defined(__GNUC__)
#include <signal.h>
#endif
#include <math.h>
#include <io.h>
#include <emeter-toolkit.h>
#define __MAIN_PROGRAM__

#include "emeter-structs.h"
#if defined(MESH_NET_SUPPORT)
#include "mesh_structure.h"
#endif

int32_t xxx;
int32_t yyy;

#if !defined(__IAR_SYSTEMS_ICC__)
static __inline__ long labs(long __x);

static __inline__ long labs(long __x)
{
    return (__x < 0) ? -__x : __x;
}
#endif

  void set_phase_correction(struct phase_correction_s *s, int correction)
  {
      correction += 128;
  ////#if defined(__HAS_SD_ADC__)
  ////    s->step = (correction >> 8) + I_HISTORY_STEPS*2 + 1;
  ////#else
      s->step = I_HISTORY_STEPS + (correction >> 8);
  ////#endif
      correction = 127 - ((correction & 0xFF) >> 1);
      s->fir_beta = fir_coeffs[correction][0];
      s->fir_gain = fir_coeffs[correction][1];
  }

#if defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
static void set_phase_gain_correction(struct phase_correction_s *s, int correction, int gain)
{
    correction += 128;
    s->step = I_HISTORY_STEPS + (correction >> 8);
    correction = 127 - ((correction & 0xFF) >> 1);
    s->fir_beta = fir_coeffs[correction][0];
    s->fir_gain = Q1_15_mul(gain, fir_coeffs[correction][1]);
}
#endif

#if defined(__HAS_SD_ADC__)
void set_sd16_phase_correction(struct phase_correction_sd16_s *s, int ph, int correction)
{
  #if defined(__MSP430_HAS_SD24_B__)
    static unsigned short volatile * const sd16_locations[NUM_CURRENT_CHANNELS] =
  #else
    static unsigned char volatile * const sd16_locations[NUM_CURRENT_CHANNELS] =
  #endif      
    {
    #if defined(SINGLE_PHASE)
        &SD16PRE_LIVE,
    #else
        &SD16PRE_CURRENT_1,
        &SD16PRE_CURRENT_2,
        &SD16PRE_CURRENT_3,
    #endif
    #if defined(NEUTRAL_MONITOR_SUPPORT)
        &SD16PRE_NEUTRAL
    #endif
    };
    uint8_t bump;

    /* Only try to nudge the converter's timing when in normal operating mode. */
    if (operating_mode == OPERATING_MODE_NORMAL)
    {
      // [HOTFIX RO] Take the larger size of the preload register into account
      //  if ((bump = s->sd16_preloaded_offset - (correction & 0xFF)))
            if ((bump = s->sd16_preloaded_offset - (correction & 0xFF)))
      // [END HOTFIX RO]        
            *sd16_locations[ph] = bump;
    }
    /* Always store the required correction. */
    s->step = I_HISTORY_STEPS - (correction >> 8);
////    s->step = (correction >> 8) - I_HISTORY_STEPS + 1;
    s->sd16_preloaded_offset = correction;
}
#endif

#if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(POWER_BALANCE_DETECTION_SUPPORT)
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
#endif

#if defined(MAINS_FREQUENCY_SUPPORT)
#if defined(SINGLE_PHASE)
int16_t frequency(void)
#else
int16_t frequency(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
#endif
{
    int32_t x;
    //int step;
    //int offset;

    /* Calculate the mains frequency in 1/100Hz increments, based on the mains
       period assessment from the background activity. */
#if defined(LIMP_MODE_SUPPORT)
    if (operating_mode == OPERATING_MODE_LIMP)
    {
        /* In limp mode there is no voltage waveform, so we get the frequency from
           the current in the active lead. This may fail to measure frequency
           correctly for very low currents, and very distorted current waveforms. */
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        if ((phase->status & CURRENT_FROM_NEUTRAL))
            x = phase->metrology.neutral.mains_period;
        else
    #endif
            x = phase->metrology.current.mains_period;
    }
    else
    {
#endif
        /* Normally we get the mains frequency from the voltage. Voltage is always
           present, and is not subject to the same level of distortion as the current
           waveform with difficult loads. */
      
#if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
        /* We have a whole cycle period in the upper 16 bits, but we want the delay for 90 degrees, so we shift 2
           extra bits for that. */
        x = (phase->metrology.mains_period >> 18);
    #if defined(__HAS_SD_ADC__)
        set_phase_correction(&phase->metrology.current.quadrature_correction[0], x);
        #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.current.quadrature_correction[1], x);
        #endif
    #else
        set_phase_correction(&phase->metrology.current.quadrature_correction[0], x + phase_nv->current.phase_correction[0]);
        #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.current.quadrature_correction[1], x + phase_nv->current.phase_correction[1]);
        #endif
    #endif
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        #if defined(__HAS_SD_ADC__)
        set_phase_correction(&phase->metrology.neutral.quadrature_correction[0], x);
            #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.neutral.quadrature_correction[1], x);
            #endif
        #else
        set_phase_correction(&phase->metrology.neutral.quadrature_correction[0], x + nv_parms.seg_a.s.neutral.phase_correction[0]);
            #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.neutral.quadrature_correction[1], x + nv_parms.seg_a.s.neutral.phase_correction[1]);
            #endif
        #endif
    #endif
#endif

        
        
        x = phase->metrology.mains_period;
#if defined(LIMP_MODE_SUPPORT)
    }
#endif
    x = (int32_t) SAMPLES_PER_10_SECONDS*256L*10L/(x >> 16);
    return  x;
}
#endif

#if defined(VRMS_SUPPORT)
    #if defined(SINGLE_PHASE)
int32_t voltage(void)
    #else
int32_t voltage(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
    #endif
{
    int16_t i;
    int32_t x;

    /* Calculate the RMS voltage in 10mV increments. Return -1 for overrange
       (i.e. ADC clip). */
    if ((phase->status & V_OVERRANGE))
        return -1;
    x = div_sh48(phase->metrology.dot_prod_logged.V_sq, 26 - 2*ADC_BITS, phase->metrology.dot_prod_logged.sample_count);
    #if defined(LIMP_MODE_SUPPORT)
    if (operating_mode == OPERATING_MODE_LIMP)
        i = phase_nv->V_rms_limp_scale_factor;
    else
    #endif
        i = phase_nv->V_rms_scale_factor;
    x = isqrt32(x);
    x = (x >> 12)*i;
    x >>= 14;
    return x;
}
#endif

#if defined(IRMS_SUPPORT)
    #if defined(SINGLE_PHASE)
rms_current_t current(void)
    #else
rms_current_t current(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv, int ph)
#endif
{
    int16_t i;
    rms_current_t x;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    rms_current_t y;
    #endif
    #if defined(TWENTYFOUR_BIT)
    int64_t tmp;
    #else
    int32_t tmp;
    #endif

    /* Calculate the RMS current in 1mA increments. Return -1 for overrange
       (i.e. ADC clip). A side effect of this routine is it updates the dynamic
       phase correction settings, based on the newly calculated current. */
    /* We always have to work out the properly scaled current from both leads, in
       order to work out the FIR coeffs for the next block. */
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    if ((phase->status & I_NEUTRAL_OVERRANGE))
    {
        y = (rms_current_t) ~0;
    }
    else
    {
        test64=tmp = div_ac_current(phase->metrology.neutral.dot_prod_logged.I_sq, phase->metrology.neutral.dot_prod_logged.sample_count);
        if (tmp < phase_nv->current.ac_offset)
        {
            y = 0;
        }
        else
        {
            tmp -= phase_nv->neutral.ac_offset;
        #if defined(TWENTYFOUR_BIT)
            y = isqrt64(tmp) >> 26;
        #else
            y = isqrt32(tmp) >> 2;
        #endif
        #if defined(LIMP_MODE_SUPPORT)
            if (operating_mode == OPERATING_MODE_LIMP)
                i = phase_nv->neutral.I_rms_limp_scale_factor;
            else
        #endif
                i = phase_nv->neutral.I_rms_scale_factor;
            test32=y = mul48u_32_16(y, i);
        }
    }
        #if defined(PER_SENSOR_PRECALCULATED_PARAMETER_SUPPORT)
    phase->metrology.neutral.readings.I_rms = y;
        #endif
        #if defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
    dynamic_phase_correction_neutral(phase, phase_nv, ch);
        #endif
    #endif

    if ((phase->status & I_OVERRANGE))
    {
        x = (rms_current_t) ~0;
    }
    else
    {
      tmp = div_ac_current(phase->metrology.current.dot_prod_logged.I_sq, phase->metrology.current.dot_prod_logged.sample_count);
        if (tmp < phase_nv->current.ac_offset)
        {
            x = 0;
        }
        else
        {
tmp -= phase_nv->current.ac_offset;
    #if defined(TWENTYFOUR_BIT)
            
       x =  isqrt64(tmp) >> 26;
    #else
            x = isqrt32(tmp) >> 2;
    #endif
    #if defined(LIMP_MODE_SUPPORT)
            if (operating_mode == OPERATING_MODE_LIMP)
                i = phase_nv->current.I_rms_limp_scale_factor;
            else
    #endif
                i = phase_nv->current.I_rms_scale_factor;
            //Changed so can be consistent with F6736's display of parameters
           x = mul48u_32_16(x, i) >> 10;
        }
    }
        #if defined(PRECALCULATED_PARAMETER_SUPPORT)
            phase->metrology.current.I_rms = x;
        #endif
    #if defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
    dynamic_phase_correction(phase, phase_nv, ch);
    #endif
    #if defined(LIMP_MODE_SUPPORT)
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
    #endif
    {
        /* The power calculation has provided us which is the appropriate
           current to use. */
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        if ((phase->status & CURRENT_FROM_NEUTRAL))
               x = y;
    #endif
    }
    return  x;
}
#endif



#if !defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(IRMS_SUPPORT)
rms_current_t neutral_current(void)
{
    rms_current_t x;
    int16_t i;
    #if defined(TWENTYFOUR_BIT)
    int64_t tmp;
    #else
    int32_t tmp;
    #endif

    /* Calculate the RMS current in 1mA increments. Return -1 for overrange
       (i.e. ADC clip). A side effect of this routine is it updates the dynamic
       phase correction settings, based on the newly calculated current. */
    if ((neutral.status & I_OVERRANGE))
        return (rms_current_t) ~0;

    tmp = div_ac_current(neutral.metrology.I_sq_accum_logged, neutral.metrology.sample_count_logged);
    if (tmp < nv_parms.seg_a.s.neutral.ac_offset)
        return 0;

    #if defined(TWENTYFOUR_BIT)
    x = isqrt64(tmp - nv_parms.seg_a.s.neutral.ac_offset) >> 26;
    #else
    x = isqrt32(tmp - nv_parms.seg_a.s.neutral.ac_offset) >> 2;
    #endif
    #if defined(LIMP_MODE_SUPPORT)
    if (operating_mode == OPERATING_MODE_LIMP)
        i = nv_parms.seg_a.s.neutral.I_rms_limp_scale_factor;
    else
    #endif
        i = nv_parms.seg_a.s.neutral.I_rms_scale_factor;
    x = mul48u_32_16(x, i) >> 10;
    return x;
}
#endif

#if defined(SINGLE_PHASE)
power_t active_power(void)
#else
power_t active_power(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
#endif
{
    #if defined(TWENTYFOUR_BIT)
    int64_t x;
    #else
    int32_t x;
    #endif
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        #if defined(TWENTYFOUR_BIT)
    int64_t y;
        #else
    int32_t y;
        #endif
    #endif
    #if defined(PHASE_REVERSED_DETECTION_SUPPORT)
    int reversed;
    #endif

    if (phase->status & (V_OVERRANGE | I_OVERRANGE))
        return (power_t) ~0;

    /* We can only do real power assessment in full operating mode. */
    /* If we have neutral monitoring for a single phase meter, we need to measure
       both power levels, and decide between them. Issues to be assessed here are
       whether one or both leads show reverse power, and whether the power levels
       are balanced. */
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
    /* If we find a negative power level we may be genuinely feeding power to the grid,
       or we may be seeing a tamper condition. This is application dependant. */
    reversed = FALSE;
#endif

    x = div_ac_power(phase->metrology.current.dot_prod_logged.P_active, phase->metrology.current.dot_prod_logged.sample_count);
     x >>= 12;
    x = mul48_32_16(x, phase_nv->current.P_scale_factor[0]);
    #if defined(PRECALCULATED_PARAMETER_SUPPORT)
        phase->metrology.current.active_power = x;
    #endif
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
    if (x < 0)
    {
    #if defined(PHASE_REVERSED_IS_TAMPERING)
        x = -x;
    #endif
        phase->status |= I_REVERSED;
        if (x > PHASE_REVERSED_THRESHOLD_POWER)
            reversed = TRUE;
    }
    else
    {
        phase->status &= ~I_REVERSED;   
    }
#endif
#if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    y = div_ac_power(phase->metrology.neutral.dot_prod_logged.P_active, phase->metrology.neutral.dot_prod_logged.sample_count);
    y >>= 12;
    y = mul48_32_16(y, phase_nv->neutral.P_scale_factor);
    #if defined(PER_SENSOR_PRECALCULATED_PARAMETER_SUPPORT)
    phase->metrology.neutral.readings.active_power = y;
    #endif
    #if defined(PHASE_REVERSED_DETECTION_SUPPORT)
    if (y < 0)
    {
    #if defined(PHASE_REVERSED_IS_TAMPERING)
        y = -y;
    #endif
        phase->status |= I_NEUTRAL_REVERSED;
        if (y > PHASE_REVERSED_THRESHOLD_POWER)
            reversed = TRUE;
    }
    else
    {
        phase->status &= ~I_NEUTRAL_REVERSED;  
    }
    #endif
    #if defined(POWER_BALANCE_DETECTION_SUPPORT)
        #if defined(SINGLE_PHASE)
    x = test_phase_balance(x, y, PHASE_UNBALANCED_THRESHOLD_POWER);
        #else
    x = test_phase_balance(phase, x, y, PHASE_UNBALANCED_THRESHOLD_POWER);
        #endif
    if ((phase->status & PHASE_UNBALANCED))
    {
        /* When the phase is unbalanced we only look for reversed current in the 
           lead with the higher current. If we do not impose this restriction, coupling
           through a parasitic CT power supply transformer can cause the reverse condition
           to be raised incorrectly. If there is no parasitic supply this test is probably
           a waste of time. */
        if ((phase->status & CURRENT_FROM_NEUTRAL))
            reversed = phase->status & I_NEUTRAL_REVERSED;
        else
            reversed = phase->status & I_REVERSED;
    }
    #endif
#endif
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
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
#endif
    return x;
}

#if defined(REACTIVE_POWER_SUPPORT)  &&  defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
    #if defined(SINGLE_PHASE)
power_t reactive_power(void)
    #else
power_t reactive_power(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
    #endif
{
    #if defined(TWENTYFOUR_BIT)
    int64_t x;
    #else
    int32_t x;
    #endif
    int16_t i;

    if (phase->status & (V_OVERRANGE | I_OVERRANGE))
        return (power_t) ~0;

    /* We can only do real power assessment in full operating mode. */
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    /* If we have neutral monitoring for a single phase meter, we need to use whichever
       channel has been selected by the anti-tamper validation scheme. */
    if ((phase->status & CURRENT_FROM_NEUTRAL))
    {
        x = div_ac_power(phase->metrology.neutral.dot_prod_logged.P_reactive, phase->metrology.neutral.dot_prod_logged.sample_count);
        i = Q1_15_mul(phase_nv->neutral.P_scale_factor[0], phase->metrology.neutral.quadrature_correction[0].fir_gain);
    }
    else
    #endif
    {
        x = div_ac_power(phase->metrology.current.dot_prod_logged.P_reactive, phase->metrology.current.dot_prod_logged.sample_count);
        i = Q1_15_mul(phase_nv->current.P_scale_factor[0], phase->metrology.current.quadrature_correction[0].fir_gain);
    }
    x >>= 12;
    x = mul48_32_16(x, i);
    x <<= 2;
    #if defined(PER_SENSOR_PRECALCULATED_PARAMETER_SUPPORT)
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    phase->metrology.neutral.readings.reactive_power = x;
        #endif
    phase->metrology.current.readings.reactive_power = x;
    #endif
    #if defined(PHASE_REVERSED_DETECTION_SUPPORT)
    if (x < 0)
    {
        #if defined(PHASE_REVERSED_IS_TAMPERING)
        x = -x;
        #endif
    }
    #endif
    return  x;
}
#endif

#if defined(APPARENT_POWER_SUPPORT)
    #if defined(SINGLE_PHASE)
int32_t apparent_power(void)
    #else
int32_t apparent_power(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
    #endif
{
    #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
    int32_t p;
    int32_t x;
    int shift;

    /* Calculate apparent (VA) power in 0.01W increments */
    p = labs(phase->readings.active_power);
    x = labs(phase->readings.reactive_power);
    /* Justify for optimal accuracy */
    shift = 0;
    while ((p & 0xFFFFC000)  ||  (x & 0xFFFFC000))
    {
        shift++;
        p >>= 1;
        x >>= 1;
    }
    x = isqrt32(p*p + x*x);
    x >>= (16 - shift);
    #else
    int16_t i;
    int32_t x;
    int32_t y;

    /* Calculate VA power in 0.01W increments */
    x = isqrt32(div_ac_voltage(phase->metrology.dot_prod_logged.V_sq, phase->metrology.dot_prod_logged.sample_count));
        #if defined(LIMP_MODE_SUPPORT)
    if (operating_mode == OPERATING_MODE_LIMP)
        i = phase_nv->V_rms_limp_scale_factor;
    else
        #endif
        i = phase_nv->V_rms_scale_factor;
    x = (x >> 12)*i;
    x >>= 14;

        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    if ((phase->status & CURRENT_FROM_NEUTRAL))
    {
            #if defined(TWENTYFOUR_BIT)
        y = isqrt64(div_ac_current(phase->metrology.neutral.dot_prod_logged.I_sq, phase->metrology.dot_prod_logged.sample_count)
                  - phase_nv->neutral.ac_offset) >> 36;
            #else
        y = isqrt32(div_ac_current(phase->metrology.neutral.dot_prod_logged.I_sq, phase->metrology.dot_prod_logged.sample_count)
                  - phase_nv->neutral.ac_offset) >> 12;
            #endif
            #if defined(LIMP_MODE_SUPPORT)
        if (operating_mode == OPERATING_MODE_LIMP)
            y *= phase_nv->neutral.I_rms_limp_scale_factor;
        else
            #endif
            y *= phase_nv->neutral.I_rms_scale_factor;
        y >>= 14;
    }
    else
    {
        #endif
        #if defined(TWENTYFOUR_BIT)
        y = isqrt64(div_ac_current(phase->metrology.current.dot_prod_logged.I_sq, phase->metrology.current.dot_prod_logged.sample_count)
                  - phase_nv->neutral.ac_offset) >> 36;
        #else
        y = isqrt32(div_ac_current(phase->metrology.current.dot_prod_logged.I_sq, phase->metrology.current.dot_prod_logged.sample_count)
                  - phase_nv->neutral.ac_offset) >> 12;
        #endif
        #if defined(LIMP_MODE_SUPPORT)
        if (operating_mode == OPERATING_MODE_LIMP)
            y *= phase_nv->current.I_rms_limp_scale_factor;
        else
        #endif
            y *= phase_nv->current.I_rms_scale_factor;
        y >>= 14;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    }
        #endif
    x *= y;
    x /= 1000;
    #endif
    return x;
}
#endif

#if defined(POWER_FACTOR_SUPPORT)
int16_t power_factor()
{
    int32_t p;
    int32_t x;

    p = labs(phase->readings.active_power);
    #if defined(APPARENT_POWER_SUPPORT)
      x = labs(phase->readings.apparent_power);
    #endif
    if (p  &&  x)
    {
        /* Justify for optimal accuracy */
        while ((p & 0x40000000) == 0  &&  (x & 0x40000000) == 0)
        {
            p <<= 1;
            x <<= 1;
        }
        x >>= 16;
        p /= x;
        p *= 10000;
        p >>= 16;
        /* Don't let a little imprecision cause strange answers */
        if (p > 10000)
            p = 10000;
    }
    else
    {
        p = 0;
    }
    /* Use a negative PF to indicate an inductive load */
    {
        if (phase->metrology.current.leading < 0)
            p = -p;
    }
    return p;
}
#endif
