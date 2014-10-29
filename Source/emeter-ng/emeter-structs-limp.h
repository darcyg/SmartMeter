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
//  File: emeter-structs.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-structs.h,v 1.32 2008/01/08 07:53:48 a0754793 Exp $
//
//--------------------------------------------------------------------------

/*! \file */

/*! \page metering_technique Measurement techniques in the e-meter library

This e-meter library is designed to work with MSP430 devices using either the ADC12 or the SD16 ADC. It provides
a range of features to support most fully electronic e-meter requirements, from single phase to three phase.
It support the measurement of:

    - Active power
    - Reactive power
    - Apparent (VA) power
    - Power factor
    - RMS voltage
    - RMS current
    - Mains frequency

Most meters which are required to measure reactive and apparent power or energy are only required to give a general indication of load quality. For some applications, such as tri-vector meters in India, the required accuracy for active, reactive and apparent power is similar. This software can be built to offer good accuracy with lower complexity. It can also be built to give accuracy suitable for tri-vector meters, and supports the data gathering and storage features typical of these meters.
    
It supports a wide range of anti-tamper features;

    - Reverse current detection
    - Current imbalance (earthing) detection
    - Limp mode operation
    - Neutral current monitoring for 3-phase meters

\section multirate Multi-rate and tri-vector features

Multi-rate features vary considerably. This software provides sophisticated multi-rate features, for single and 3-phase meters. It does this will very modest flash and RAM usage. The software currently stores all schedules, and recorded usage, in an external EEPROM or FRAM. The schedules could be stored in a section of the MCU¡¦s flash, with a few changes to the code. 

This multi-rate software provides the following features:

    - Multi-rate tariff gathering of usage information
    - Assessment of the time and sizes of each day¡¦s peak demand.
    - Storing schedules and gathered information in an external serial EEPROM or FRAM.
    - Downloading schedules from a remote computer
    - Uploading the gathered information to a remote computer
    - Displaying the schedules and gathered information on the meter's LCD display

Most of the limits in the following description are set be definitions in the source code. If there is sufficient EEPROM space, changing these definitions will change the limits. This has little effect on code size or RAM usage. It does, of course, affect the size of EEPROM required.

    - The software provides for up to 8 tariffs. Usage information is gathered separately for each of these tariffs, between specified cutoff dates.
    - Up to 8 daily schedules may be configured. Each schedule may specify up to 8 time slots. Each time slot may be assigned to any of the tariffs. Typically only 3 schedules would be required ¡V weekdays, Saturdays and Sundays/public holidays.
    - Each weekday may be assigned to one of the tariff schedules.
    - A list of up to 32 public holidays may be configured. These override the normal weekday tariff patterns. Holidays may be assigned to any of the tariff schedules. 
    - Up to 12 billing cut-off dates may be configured. Between these dates, the meter accumulates the total energy used for each of the tariffs. The meter stores the last 12 sets of these values, to allow the meter to be read infrequently. Note the actual cutoff point is midnight on the day preceding the specified day.
    - The meter measures the consumed energy in five minute steps. It accumulates six consecutive five minute steps, to find the total energy consumed in the last 30 minutes. For each day, the meter determines the time and size of the peak 30 minute consumption value. This allows the time of peak demand to be found with reasonable accuracy. The last 31 daily peak times and amounts are stored.

\section metering_technique_strategy The measurement strategy

Traditional electricity meters simply measured the active energy consumed by the load. These days meters are increasingly required to provide additional information, for monitoring or charging purposes. The basic set of measurements which might be required are:
    - Active power and energy
    - Reactive power and energy
    - Apparent (VA) power and energy
    - RMS voltage
    - RMS current
    - Mains frequency
A key reason for requiring reactive energy measurement is to be able to charge with poor load quality at a punative rate. To take this further, some meters now also measure the actual harmonic structure of the current waveform, for the first few harmonics.

The active energy consumed in a period, T, is
\f[E_{active}=\int_0^T v(t)i(t) dt\f]
where v is the voltage across the load, and i is the current passing through it.
Traditional electro-mechanical electricity meters measure the active energy consumption by directly implementing this formula. It is possible to build a fully electronic meter, using analogue methods, which implements it. However, more accurate, more stable and cheaper solutions are easier to implement using digital methods. Precision analogue to digital converters (ADCs), which are very stable over time and temperature, can be integrated with digital signal processing and other functions to build a complete and very functional e-meter using just one IC.

To use digital methods to measure the active energy, the previous formula needs to be restated in a suitable discrete time form
\f[E_{active} = \Sigma_{k=0}^N{v_k}{i_k}\f]
where v is the voltage across the load, and i is the current passing through it, and N is the number of samples in the period T. Note that this summation is calculating the dot, or inner, product of the voltage and current sample vectors over the measurement period.

The RMS voltage and current must be calculated over some period, using the following formulae

\f[V_{rms}=(V{\cdot}V)=\Sigma_{k=0}^Nv_k^2\f]

\f[I_{rms}=(I{\cdot}I)=\Sigma_{k=0}^Ni_k^2\f]

Note again that these summations are calculating the dot, or inner, product of the voltage or current sample vectors, with themselves, over the measurement period.

There are various ways in which reactive and apparent power may be evaluated, based on the same voltage and current samples gathered for the active power measurement. Apparent power can be calculated simply as:

\f$P_{apparent}=V_{rms}I_{rms}\f$

and from the active and apparent powers, the reactive power may be evaluated as

\f$P_{reactive}=\sqrt{P_{apparent} - P_{active}}\f$

Whilst simple, the accuracy of this method, at small currents, is limited, especially when a shunt resistor sensor is used to measure the current. To see why, we must look at the implications of the preceeding formulae.

In an e-emeter, the voltage signal is always large. In many places it is specified as always being within a few percent of the nominal level. In some rural areas it may vary more, but the voltage will never drop to a very low level, unless the power actually fails. The current, on the other hand, varies between a cutoff point close to zero and full load. We need to achieve sufficient accuracy over a current range that may be up to 1000:1. When the current is very small, the percentage of each digitised sample which is noise from the electronics can be significant. If a shunt sensor and a high gain front end is used, that noise will typically be much larger than if a sensor with a large output, such as a CT, is used.

If the reactive power can be calculated from a \f$(V{\cdot}I)\f$ type of calculation, the substantial noise content of a small RMS current value could be avoided. One way to achieve this is to use an accurate 90 degree phase shift to rotate the voltage samples. Then, the reactive energy during a period, T, can be expressed as
    \f[E_{reactive}=\int_0^T v_{90}(t)i(t) dt\f]
    
A Hilbert transform could produce such a phase shift over a sufficiently wide frequency range, but would be computationally expensive. However, we can measure the mains frequency accurately, and obtain an accurate estimate for the number of samples of delay required to produce a precise 90 degree phase shift at the current mains frequency. We can implement this phase shift easily, by delaying for required whole number of samples and using a single pole FIR filter to shift by the fractional amount. This will only shift the fundamental frequency by 90 degrees, but this may not be a problem. the voltage waveform never contains significant harmonic energy. It is only the current waveform which is often rich in harmonics, due to the nature of the load.

Once precise values for the active and reactive energy are known, the apparent (VA) energy consumed in a period T is \f[E_{apparent}=\sqrt{E_{active}^2E_{reactive}^2}\f]

Another method which achieves similar results may be derived as follows:

    \f[Q=VIsin(\theta)\f]

    \f[Q_{+x}=\int{v(t)i(t+x)dt}=VIcos(\theta + 2\pi x/T)\f]

    \f[Q_{-x}=\int{v(t)i(t-x)dt}=VIcos(\theta - 2\pi x/T)\f]

    \f[Q_{-x} - Q_{+x}=VI(cos(\theta - 2\pi x/T) - cos(\theta + 2\pi x/T))\f]

A little trignometry can reduce this to

    \f[Q_{-x} - Q_{+x}=2VIsin(\theta)sin(2\pi x/T))\f]

The value Q, which we are trying to find, is buried in this equation as \f$VIsin(\theta)\f$, so

    \f[Q = \frac {Q_{-x} - Q_{+x}} {2sin(2\pi x/T)} = \frac{\int(V(t-x)I(t) - V(t)I(t-x))dt} {sin(2\pi x/T)}\f]

This requires more computation, but avoids the need to buffer many samples of the voltage signal. If RAM is very limited, this may be a solution.

Fully electronic electricity meters generally use one of two strategies for energy accumulation. Both start out by calculating the dot product of the voltage and current waveforms. One method uses a continuous integration technique, integrating every product of a V and I sample. This technique is used in many metering ASICs, as it suits their architectures well. The other method processes the accumulated data in timed blocks, typically around one second long. This fits the architectures of MCUs and programmable DSPs much better. This is the method used in the current software. Both methods are capable of similar accuracy, and response times. However, the character of their outputs is a little different. The output of the continuous method smoothly adjusts to step changes in the load. The output of the block method jumps in response to step changes in the load. However, the total energy accumulated through such as a step change is the same for both methods (assuming they are correctly implemented).

For a device using the ADC12, the meter samples each of the waveforms 3276.8 times per second. For a device using the SD16, the meter samples each of the waveforms 4096 times per second.

\section metering_technique_configuration The meter configuration

\subsection metering_technique_configuration_dithering Dithering with the ADC12

A typical meter must maintain good accuracy over a current range of 200:1 or more. If the gain is set such that maximum current is close to clipping in the ADC12, the waveform for the minimum current occupies only the first 3 or 4 bits of the converter's output. It might seem this is insufficient to allow accurate measurement. Some meters use current range switching to overcome this problem, but this has disadvantages - unless the ADC can monitor all the gain stages at once, the measurement process is paralysed for a period when range switching occurs; significant additional analogue circuitry is required, adding cost and increasing board size; additional calibration is required during production. The ADC12 is very linear, and the steps in its output characteristic are very evenly spaced. With such a good converter, dithering may be used to greatly extend its effect resolution. With correct dithering, MSP430's with an ADC12 converter can be used to build accurate single gain stage class 1 meters.

Dithering is the process of adding a controlled noise signal to a waveform to be quantised. Adding noise will reduce the accuracy of a single quantised sample. However, for most repetitive waveforms it can substantially increase the average accuracy over many samples, by providing a measure of interpolation between the steps of the converter. Depending on the application, the noise waveform might be additive white gaussian noise, triangular PDF noise, or a small triangular or sawtooth waveform. In the metering application a small triangular waveform is easy to generate. However, if the cycles of this waveform correlate with the mains waveform cycles, that correlation might degrade the effect of the dithering. By randomising the dithering a little, an effective dithering waveform which is also easy to produce at low cost is possible.

\section metering_technique_gathering Gathering the data

An interrupt routine is called each time the ADC conversion of a set of values (i.e. all the voltage and current values for single time sample) is complete. The meter software spends a considerable portion of its time within this routine, as it is where most of the calculations are performed.

\subsection metering_technique_gathering_phase_correction Correcting the phase relationship between the voltage and current

For meters using the ADC12, the timing relationship between individual ADC samples is fixed by the sequential sampling process within the ADC. The software must correct for the time delay between ADC samples, and the phase shifts in the external circuitry (particularly for CT sensors). An single stage FIR (an interpolator/extrapolator) is used to provide corrections of up to +-1/2 a waveform sample. Short sample buffers are used to extend the correction beyong 1/2 a sample. For meters using the SD16, the ADC hardware itself provides a facility to adjust the relative timing of the samples by up to 1 sample. This is used for the phase correction in these meters.

\subsection metering_technique_gathering_dc Dealing with DC in the signals

Any DC content in the voltage and current signals must be removed. A dot product is similar to correlation. If only one of the waveforms has some DC content, this does not correlate and has no effect on the output. However, if both have some DC content, this correlates and does affect the final result. For the power calculation it might be acceptable to remove DC from only one of the signals. However, calculating the RMS voltage and current requires calculating the dot product of each signal with itself. DC will, therefore, always corrupt these calculations. The software actually removes the DC from all the sampled waveforms.

The software uses an extremely low pass single pole IIR filter to estimate the DC content of the signals. The software maintains a 32 bit output from the filter. The filter damps so heavily that ripple and noise is confined to the lower 16 bits. The upper 16 bits is a clean and stable estimate of the DC content of the waveform. The filter takes several seconds to fully respond to a step change in the DC level. If the current waveform changes to a new and very distorted shape (which can be very distorted with complex electronic loads), the DC content of the waveform might show a step change. However, as noted earlier, the power reading will only be corrupted if the voltage waveform also has DC content. Load changes will not normally cause DC changes in the voltage waveform, so the power reading will always be accurate.

\subsection metering_technique_gathering_dot_products Calculating the dot products

The dot product calculations consist of multiplying two 12 bit (ADC12) or two 16 bit (SD16) numbers, and accumulating the results. With a sampling rate of 3276.8/s or 4096/s and an accumulation period around one second, 48 bit arithmetic is the smallest multiple of the CPU's native 16 bits that can be used. The samples are multiplied using 16x16=>32 bit arithmetic. The 32 bit values are them accumulated into 48 bit results. C compilers do not generally support 48 bit arithmetic. Some support 64 bit, but this is wasteful on a small MCU. Therefore some simple assembly language routines are used to support the handling of 48 bit values. The 48 bit values are stored in three element arrays of 16 bit integers.

Three dot products are evaluated for each measured channel - V.I, V.V and V.I. From these, the backend processing can calculate the active, reactive and VA power, the RMS voltage and the RMS current.

\subsection metering_technique_gathering_cycles Tracking the mains waveform cycles.

From a measurement accuracy point of view, it is satisfactory to calculate the consumed energy using fixed length blocks of samples. However, unless these align with a whole number of cycles of the mains waveform there will be some block to block jitter in the readings. The total accumulated energy would be correct, but this jitter has a bad psychological effect on customers. To avoid this, the software does not use fixed length blocks. It counts one second's worth of samples, and then looks for the next positive going zero crossing in the mains waveform. It ends the block at that point. By synchronising to whole cycles of the waveform, to within one sample, the block to block jitter is reduced to the point where noise dominates. This method is robust in the presence of large noise spikes on the mains, which might be mistaken for zero crossings. Such spikes can only affect the block timing a little, and cause minor jitter in the particular blocks where they occur. They cannot affect the overall accuracy, or cause short blocks which might flood the meter's back end processing.

When the end of a block of samples is declared, the currently accumulated dot products, and the exact number of gathered samples, N, is recorded, as follows:

    - The dot product for the RMS voltage \f$V{\cdot}V = \Sigma_{k=0}^Nv_k^2\f$
    - The dot product for the RMS current \f$I{\cdot}I = \Sigma_{k=0}^Ni_k^2\f$
    - The dot product for the active power \f$V{\cdot}I = \Sigma_{k=0}^N{v_k}{i_k}\f$
    - The dot product for the reactive power \f$V_{90}{\cdot}I = \Sigma_{k=0}^N{v_{90_k}}{i_k}\f$

A flag is also set to tell the back end processing that new data is available. The counts are cleared, and the accumulation process begins again.

To evaluate the mains frequency, the number of sample between positive zero crossings of the waveform is counted, and filtered using a single pole IIR filter. By using a 32 bit result register in the filter, and filtering the cycles lengths into the top 16 bits, the mains frequency is estimated to about 0.01Hz accuracy over many cycles of the mains waveform. To minimise the effects of noise spikes, and the mains power switching on and off, excessively long or short mains cycles are not counted.

We calculate the power factor from the power, voltage and current measurements. This technique does not indicate whether the
power factor is leading or lagging. To determine this, the software monitors the current waveform at each positive going zero crossing of the voltage waveform. If the current is above the voltage, the current must be leading the voltage. If the current is below the voltage waveform, the current must be lagging the voltage. To ensure stable assessment of lead vs lag, the software lightly integrates the decisions.

\subsection metering_technique_gathering_pulses Energy pulses

To satisfy customer requirements it is usually necessary for an e-meter to have an LED or LCD segment which flashes once for each small increment in consumed energy. An increment might typically be 1/1000kWh or 1/1600kWh. This has nothing to do with the actual measurement process of modern fully electronic meters. It is provided mostly for historic reasons. However, it must always give stable and accurate pulses - it is this pulse rate which will generally be used to evaluate the meter's performance. To provide sufficient timing accuracy in the pulses to avoid significant jitter, these pulses are generated within the ADC interrupt routine. The LED can, therefore, flash within 244us or 305us (depending on the ADC type) of the exact time that it should.

During each ADC interrupt, the power measured during the previous sampling block is accumulated pulse by pulse. The LED is flashed when the accumulated value reached the value of one pulse. The duration of the LED pulse is also controlled within the ADC interrupt routine to be a fixed number of ADC interrupts.

\section metering_technique_backend_processing Processing the blocks of gathered data

When the back end, non-interrupt, code sees the new data available flag set by the ADC interrupt code, it uses the gathered data to find the actual power, voltage and current.  These calculations have fairly relaxed timing constraints, since there is a full second within which to generate each set of answers.

From this information the following are calculated, using appropriate scaling factors determined during calibration:
    - RMS voltage is \f$scalingfactor\sqrt{\frac{V{\cdot}V}{N}}\f$
    - RMS current is \f$scalingfactor\sqrt{\frac{I{\cdot}I}{N}}\f$
    - Active power is \f$scalingfactor\frac{{V{\cdot}I}}{N}\f$
    - Reactive (imaginary) power is \f$scalingfactor\frac{{V_{90}{\cdot}I}}{N}\f$ or \f$scalingfactor\frac{\sqrt{(V{\cdot}V)(I{\cdot}I) - (V{\cdot}I)(V{\cdot}I)}}{N}\f$
    - Apparent (VA) power is \f$V_{rms}I_{rms}\f$
    - Power_factor [ie \f$\left|cos{\phi}\right|\f$] is \f$\frac{V{\cdot}I}{\sqrt{(V{\cdot}V)(I{\cdot}I)}}\f$
*/

#if !defined(_METER_STRUCTS_H_)
#define _METER_STRUCTS_H_

/* N.B.
   When built for devices using the ADC12 converter, this e-meter software uses a
   sampling rate of 3276.8/s. For a mains supply at exactly 50Hz, each sample is
   5.4932 degrees of phase from the last.
   When built for devices using the SD16 converter, this software uses a sampling
   rate of 4096.0/s. For a mains supply at exactly 50Hz, each sample is
   4.3945 degrees of phase from the last. */

#define FLASH_MAIN_PAGE_SIZE                512
#define FLASH_INFO_PAGE_SIZE                128

#define SAMPLE_RATE_IS_4096                 4096
#define ADC_BITS                            16
#define SAMPLES_PER_10_SECONDS              40960
#define LIMP_SAMPLES_PER_10_SECONDS         10240
#define LIMP_SAMPLING_RATIO                 4

#define NUM_PHASES                          1


#define NUM_CURRENT_CHANNELS                (NUM_PHASES + 1)
#define NEUTRAL_CURRENT_CHANNEL             NUM_PHASES
/* This is the accumulated power value that represents one watt-hour
   of energy. It is equal to 100 times the number of ADC samples per
   hour. This results from the energy being accumulated at the same
   rate as the ADC samples, and the power being in 1/100W increments. */
#define ENERGY_WATT_HOUR_THRESHOLD          (SAMPLES_PER_10_SECONDS*36000L)

/* This is for the LED pulse */
#define TOTAL_ENERGY_PULSE_THRESHOLD        (ENERGY_WATT_HOUR_THRESHOLD/TOTAL_ENERGY_PULSES_PER_KW_HOUR)*1000
#define PHASE_ENERGY_PULSE_THRESHOLD        (ENERGY_WATT_HOUR_THRESHOLD/PHASE_ENERGY_PULSES_PER_KW_HOUR)*1000

/*! The following is the number of times an ADC may hit the endstops before
    it is declared to be truly overloaded. A very low value makes the meter
    less tolerant of noise spikes. A large value tends to ignore real overload. */
#define ENDSTOP_HITS_FOR_OVERLOAD           20

#define DEFAULT_V_DC_ESTIMATE               0L
#define LOWER_TAMPER_V_DC_ESTIMATE          -1000L
#define UPPER_TAMPER_V_DC_ESTIMATE          1000L
#define DEFAULT_LIMP_V_DC_ESTIMATE          0L
#define LOWER_LIMP_TAMPER_V_DC_ESTIMATE     -1000L
#define UPPER_LIMP_TAMPER_V_DC_ESTIMATE     1000L
#define DEFAULT_I_DC_ESTIMATE               0L
//--------------------------------------------------------------------------

#if   defined(__AQCOMPILER__)
#define __erasablemem__ _DMEM3
#elif defined(__GNUC__)
#define __erasablemem__ __attribute__ ((section(".erasabletext")))
#elif defined(__IAR_SYSTEMS_ICC__)
#define __infomem__ _Pragma("location=\"INFO\"")
#define __erasablemem__ /**/
#else
#define __infomem__     /**/
#define __erasablemem__ /**/
#endif

typedef struct
{
    int32_t v2;
    int32_t v3;
} goertzel_state_t;

struct phase_correction_s
{
    /*! \brief The step into the history data used for phase correction. */
    int16_t step;
    int16_t fir_beta;
    int16_t fir_gain;
};

struct phase_correction_sd16_s
{
    /*! \brief The step into the history data used for phase correction. */
    int16_t step;
    int16_t sd16_preloaded_offset;
};

/*! This structure contains the working parameters associated with a
    single current sensor (e.g a shunt resistor or a current transformer). */
struct current_sensor_parms_s
{
    /* Channel's DC estimates, accumulated powers, and the logged
       accumulated power from the last measurement cycle. */
    /*! \brief Current channel DC estimates. We need separate estimates for each gain
               stage, as the amps may have significant temperature related DC offsets. */
    int32_t I_dc_estimate[GAIN_STAGES];
    /*! The active power dot product (i.e. V.I) currently in progress. */
    int16_t P_accum[GAIN_STAGES][3];
    /*! The most recently logged active power dot product (i.e. V.I). */
    int16_t /*volatile*/ P_accum_logged[GAIN_STAGES][3];
    /*! The current dot product (i.e. I.I) currently in progress. */
    int16_t I_sq_accum[GAIN_STAGES][3];
    /*! The most recently logged current dot product (i.e. I.I). */
    /*! \brief The latest logged dot product of I.I */
    int16_t /*volatile*/ I_sq_accum_logged[GAIN_STAGES][3];
    /*! The number of ADC samples gathered at any moment for the dot products in
        progress.
        \brief The current number of accumulated samples. */
    int16_t sample_count;
    /*! The number of ADC samples gathered in the most recently logged set of dot
        products. This number varies a little from block to block, as the blocks 
        are synchronised to the cycles of the mains waveform. However, the number
        should be approximately the number of ADC samples in one second.
        \brief The latest logged number of accumulated samples. */
    int16_t /*volatile*/ sample_count_logged;
    struct phase_correction_sd16_s in_phase_correction;
    /*! \brief History of the current samples, for use when making large phase corrections */
    int16_t I_history[GAIN_STAGES][I_HISTORY_STEPS];
    /*! \brief The RMS current, in 1mA steps */
    uint16_t I_rms;
    /*! \brief The active power, in 10mW steps */
    int32_t active_power;
    /*! \brief The number of times the ADC sample has hit one of the end-stops,
               during the current accumulation period. */
    int8_t I_endstops;
    /*! \brief The gain stage currently in use */
    int stage_in_use;
    /*! \brief A countdown used to check when it is safe to switch to the higher gain stage. */
    int stage_hits;
};

/*! This structure contains the non-volatile parameters associated with a
    single current sensor (e.g a shunt resistor or a current transformer). */
struct current_sensor_nv_parms_s
{
    /*! The scaling factor between the raw RMS current value, and milliamps, when in
        normal operation. */
    int16_t I_rms_scale_factor[GAIN_STAGES];
    /*! The scaling factor between the raw RMS current value, and milliamps when in
        limp mode. */
    int16_t I_rms_limp_scale_factor[GAIN_STAGES];
    /*! An offset to remove an approximation of the front end noise from the measured RMS
        current. */
    int32_t ac_offset;
    /*! The scaling factor between the raw active power value, and 0.1W increments, when in
        normal operation. */
    int16_t P_scale_factor[GAIN_STAGES];
    /*! An initial value for the DC estimate, to ensure rapid settling after reset. */
    int32_t initial_dc_estimate;
    int16_t phase_correction[GAIN_STAGES];
};

/*! This structure holds all the working data for one phase. */
struct phase_parms_s
{
    /*! \brief The active power, in steps of 0.01W */
    int32_t active_power;
    /*! \brief The RMS voltage, in 10mV steps */
    uint16_t V_rms;
    /*! \brief The RMS current, in 1mA steps */
    uint16_t I_rms;
    /*! \brief Voltage channel DC estimates. We need separate estimates for normal
               and limp mode, as in limp mode we use a different ADC reference. */
    int32_t V_dc_estimate[2];
    /*! \brief Voltage history, used for phase correction and/or reactive power estimation
               by the quadrature method. */
    int16_t V_history[4];
    /*! \brief Accumulated squares of the phase corrected voltage (V.V). These are used to
               calculate the RMS voltage, and the power factor. We need separate accumulation
               for each current sensor, as we accumulate the phase corrected voltage which matches
               the current sensor's phase shift. The actual phase correction is not important for this.
               However, if we use just one accumulated voltage we would need to use a scaling factor before
               it can be applied. This might reduce precision, so we do things this way. */
    int16_t V_sq_accum[3];
    int16_t /*volatile*/ V_sq_accum_logged[3];

    /*! This is the data for the live lead current sensor. */
    struct current_sensor_parms_s current;
    /*! This is the data for the neutral lead current sensor, for a single phase meter
        with anti-tampering features. */
    struct current_sensor_parms_s neutral;
    /*! \brief This is the number of samples accumulated to date in the block of processed
               signals being gathered. */
    int16_t sample_count;
    /*! \brief This records the number of samples accumulated in a currently saved block of
               processed signals. */
    int16_t /*volatile*/ sample_count_logged;
    int32_t active_power_counter;
    int32_t energy_counter;
    uint32_t consumed_energy;
    /*! \brief The number of samples since the last one which did not appear to be a mains spike */
    int since_last;
    /*! \brief The previous voltage channel sample. */
    int16_t last_V_sample;
    /*! \brief Status bits */
    uint16_t /*volatile*/ status;

    /*! \brief This value counts down each time an ADC sample from the voltage
               channel hits the end of the ADC range. It is used to detect an
               overrange signal in a robust way, ignoring a small number of samples
               hits the ends of the ADC range, which might just be a few noise spikes. */
    int8_t V_endstops;
    /*! \brief This is a pointer to the current latest value in the voltage sample history
               buffer. */
    int8_t V_history_index;
    /*! \brief This times the duration of an energy pulse */
    uint8_t energy_pulse_remaining_time;
};


#define V_HISTORY_MASK      0x3

/*! This structure contains the non-volatile parameters associated with a phase. */
struct phase_nv_parms_s
{
    /*! \brief The non-volatile information (e.g. calibration parameters) for a current channel. */
    struct current_sensor_nv_parms_s current;
    /*! \brief The calibration scaling factor for the measured RMS voltage. */
    int16_t V_rms_scale_factor;
    /*! \brief The calibration scaling factor for the measured RMS voltage in limp mode. */
    int16_t V_rms_limp_scale_factor;
    /*! \brief A value used to prime the voltage signal DC estimate, to ensure quick settling, when the
               meter is powered up. It is set during the calibration process, based on the DC estimate
               measured at that time. */
    int32_t initial_v_dc_estimate;
    /*! \brief A value used to prime the voltage signal DC estimate in limp mode, to ensure quick settling,
               when the meter is powered up. It is set during the calibration process, based on the DC estimate
               measured at that time. */
    int32_t initial_v_limp_dc_estimate;
};

/*! This structure is an image of the non-volatile calibration parameters
    stored in the info memory. */
struct nv_parms_s
{
    /*! \brief 0xFF (i.e. the erased state of the flash memory) for an uncalibrated meter.
               0x00 for a calibrated one. */
    int16_t meter_uncalibrated;
    struct phase_nv_parms_s chan1;
    /*! \brief The non-volatile neutral lead information (e.g. calibration parameters). */
    struct current_sensor_nv_parms_s neutral;
    /*! \brief The room temperature at the time of calibration, in degrees Celcius */
    uint16_t temperature_at_calibration;
    /*! \brief The DC offset of readings from the temperature sensor, relative to zero Celcius. */
    uint16_t temperature_offset;
    /*! \brief The scaling factor between readings from the temperature sensor, and degrees Celcius. */
    uint16_t temperature_scaling;
    /*! \brief The basic parts per million error of the 32kHz crystal. */
    int32_t crystal_base_correction;
    /*! \brief A snapshot of the meter's RTC, taken at calibration time. If the calibration process
               has correctly set the RTC, this should be a record of when the meter was calibrated. */
    rtc_t rtc_at_calibration;
    /*! \brief Pad to word alignment */
    uint8_t xxx;
    /* Keep these administrative strings an even number of bytes each */
    /*! \brief The serial number of the meter */
    uint8_t serial_number[16 + 2];
    /*! \brief The property number of the meter (see IEC1107). */
    uint8_t property_number[16 + 2];
    /*! \brief The factory number of the meter (see IEC1107). */
    uint8_t factory_number[10 + 2];
};

/*! This union overlays a segment of the info, ensuring correct
   alignment. */
struct info_mem_s
{
    union
    {
        struct nv_parms_s s;
        int8_t x[128];
    } seg_a;
#if 0
    union
    {
        int8_t x[128];
    } seg_b;
#endif
};

/* These are the flags defined for the channel's status variable. */
/*! This flag in a channel's status variable indicates the voltage signal is currently in
    the positive half of its cycle. */
#define V_POS                       0x0001

/*! This flag in a channel's status variable indicates the current signal is currently in
    the positive half of its cycle. */
#define I_POS                       0x0002
#define ENERGY_LOGABLE              0x0004
#define NEW_LOG                     0x0008

/*! This flag in a channel's status variable indicates the voltage signal was in overload
    during the last logged interval. Overload is determined by an excessive number of
    samples hitting the end-stops of the ADC's range. */
#define V_OVERRANGE                 0x0010

/*! This flag in a channel's status variable indicates the phase current signal was in overload
    during the last logged interval. Overload is determined by an excessive number of
    samples hitting the end-stops of the ADC's range. */
#define I_OVERRANGE                 0x0020

/*! This flag in a channel's status variable indicates the phae current signal was reversed
    during the last logged interval. */
#define I_REVERSED                  0x0040

/*! This flag in a channel's status variable indicates the phase current signal was in overload
    during the last logged interval. Overload is determined by an excessive number of
    samples hitting the end-stops of the ADC's range. This is only used if the meter supports
    monitoring of both the live and neutral leads for anti-tamper management. */
#define I_NEUTRAL_OVERRANGE         0x0080

/*! This flag in a channel's status variable indicates the neutral current signal was
    reversed during the last logged interval. This is only used if the meter supports
    monitoring of both the live and neutral leads for anti-tamper management. */
#define I_NEUTRAL_REVERSED          0x0100

/*! This flag in a channel's status variable indicates the neutral current is the one
    currently being used. This means it has been judged by the anti-tamper logic to be
    the measurement which can best be trusted. This is only used if the meter supports
    monitoring of both the live and neutral leads for anti-tamper management. */
#define CURRENT_FROM_NEUTRAL        0x0200

/*! This flag in a channel's status variable indicates the neutral current signal is
    currently in the positive half of its cycle. This is only used if the meter supports
    monitoring of both the live and neutral leads for anti-tamper management. */
#define I_NEUTRAL_POS               0x0800

/*! This flag in a channel's status variable indicates the power has been declared to be
    reversed, after the anti-tamper logic has processed the raw indications. */
#define PHASE_REVERSED              0x1000  /* Live, neutral or both leads */

/*! This flag in a channel's status variable indicates the power (current in limp mode)
    has been declared to be unbalanced, after the anti-tamper logic has processed the
    raw indications. */
#define PHASE_UNBALANCED            0x2000

/* TIMER_B sample timing
   Every SAMPLE_PERIOD 32768Hz clocks generate interrupt.
   Start sampling at 8th clock
   This gives it 2 clock cycles for the conversion to be done,
   which is 61us
   8 input conversions needs 8/200K = 40uS
   11 input conversion needs 11/200K = 55uS */

extern uint8_t total_energy_pulse_remaining_time;
extern int32_t total_active_power;
extern int32_t total_active_power_counter;
extern uint32_t total_consumed_energy;

extern uint8_t total_reactive_energy_pulse_remaining_time;
extern int32_t total_reactive_power;
extern int32_t total_reactive_power_counter;
extern uint32_t total_consumed_reactive_energy;

extern struct phase_parms_s chan1;
__infomem__ extern const struct info_mem_s nv_parms;
extern const struct nv_parms_s nv_def;

#define phase                           (&chan1)
#define phase_nv                        (&nv_parms.seg_a.s.chan1)
#define phase_cal                       (&cal_info.chan1)


    #if defined(ADC12SC)
/*! This is the sampling period for normal operation. If limp mode is supported
    the sampling period in limp mode will be LIMP_SAMPLING_RATIO times as long. */
#define SAMPLE_PERIOD                   10

/*! The minimum value which the ADC12 can produce. */
#define ADC_MIN                         (0 + 400)
/*! The maximum value which the ADC12 can produce. */
#define ADC_MAX                         (4095 - 400)

/*! The maximum per sample change in the voltage signal, before we declare a voltage spike. */
#define MAX_PER_SAMPLE_VOLTAGE_SLEW     300

    #else
/*! The minimum value which the SD16 can produce. */
#define ADC_MIN                         -31000
/*! The maximum value which the SD16 can produce. */
#define ADC_MAX                         31000

/*! The maximum per sample change in the voltage signal, before we declare a voltage spike. */
#define MAX_PER_SAMPLE_VOLTAGE_SLEW     4000

/*! Set up meaningful names for the SD16 registers, to match the way we use it. */
    #if defined(CURRENT_CH_2_IS_LIVE)
#define SD16CCTL_LIVE       SD16CCTL1
#define SD16INCTL_LIVE      SD16INCTL1
#define SD16PRE_LIVE        SD16PRE1
#define SD16MEM_LIVE        SD16MEM1
#define SD16CCTL_NEUTRAL    SD16CCTL0
#define SD16INCTL_NEUTRAL   SD16INCTL0
#define SD16PRE_NEUTRAL     SD16PRE0
#define SD16MEM_NEUTRAL     SD16MEM0
    #else
#define SD16CCTL_LIVE       SD16CCTL0
#define SD16INCTL_LIVE      SD16INCTL0
#define SD16PRE_LIVE        SD16PRE0
#define SD16MEM_LIVE        SD16MEM0
#define SD16CCTL_NEUTRAL    SD16CCTL1
#define SD16INCTL_NEUTRAL   SD16INCTL1
#define SD16PRE_NEUTRAL     SD16PRE1
#define SD16MEM_NEUTRAL     SD16MEM1
    #endif
#define SD16CCTL_VOLTAGE    SD16CCTL2
#define SD16INCTL_VOLTAGE   SD16INCTL2
#define SD16PRE_VOLTAGE     SD16PRE2
#define SD16MEM_VOLTAGE     SD16MEM2
    #endif

extern uint16_t temperature;


/*! This variable is used as a series of flag bits for various purposes in the meter. */
extern uint16_t meter_status;
#define TICKER                          0x0001

/*! This bit indicates the meter is in the power down state. */
#define POWER_DOWN                      0x0004
#define SKIP_A_SECOND                   0x0010
#define PWM_RAMPING_DOWN                0x0020

/*! This bit indicates the meter has received a valid access password. */
#define PASSWORD_OK                     0x0040

/*! This bit indicates the current status of the meter is "current flow is reversed", after
    all persistence checking, and other safeguards, have been used to check the validity of the
    reverse indication. */
#define STATUS_REVERSED                 0x0100

/*! This bit indicates the current status of the meter is "current flow is earthed", after
    all persistence checking, and other safeguards, have been used to check the validity of the
    earthed indication. */
#define STATUS_EARTHED                  0x0200

/*! This bit indicates the phase voltage is OK. */
#define STATUS_PHASE_VOLTAGE_OK         0x0400

/*! This bit indicates the battery condition is OK. If battery monitoring is not enabled, this bit
    is not used. */
#define STATUS_BATTERY_OK               0x0800

/*! This bit indicates the current status of the meter is "magnetic field tampering present", after
    all persistence checking, and other safeguards, have been used to check the validity of the
    high magnetic field condition. */
#define STATUS_HIGH_MAGNETIC_FIELD      0x1000

/*! This variable specifies the current operating mode of the meter. */
extern int8_t operating_mode;
#define OPERATING_MODE_NORMAL           0
#define OPERATING_MODE_LIMP             1
#define OPERATING_MODE_LIMP_BURST       2
#define OPERATING_MODE_LCD_ONLY         3
#define OPERATING_MODE_POWERFAIL        4

/*! This defines the number of times the power fail indication is checked before a positive decision on power
    failure is made. */
#define POWER_FAIL_DEBOUNCE             50

/*! This defines the number of times the power fail indication is checked before a positive decision on power
    restoration is made. */
#define POWER_RESTORE_DEBOUNCE          5

/*! This variable tracks the current state of the user interface keys. */
extern uint8_t key_states;

/*! This value accumulates the error in the RTC. If it overflow in the positive direction, this
    represents the RTC being exactly one second too far in advance. If it overflows in the
    negative direction, this represents the RTC being exactly one second behind. */
extern int32_t rtc_correction;


/*! The table of FIR coefficients to produce a range of phase shifts from -1/2 an ADC sample interval
    to +1/2 an ADC sample interval. When the SD16 is used, the hardware phase shifting capability of
    the ADC is used, instead of this table. */
extern const int16_t fir_coeffs[][2];

/*! The array element in fir_coeffs which represents zero phase shift. */
#define FIR_UNITY_LOAD              63

/*! The current sampling rate, in samples per second. If the meter supports a very
    low power limp mode, this value may change with the mode of the meter. */
extern int16_t samples_per_second;
/*! This value is set when a sampling of the battery voltage is initiated. It counts down
    the number of ADC samples which must occur before the battery voltage is actually
    tested. To conserve energy, the battery voltage is usually sampled infrequently (e.g.
    once per minute, or more). */
extern uint16_t battery_countdown;


//--------------------------------------------------------------------------

#if defined(__AQCOMPILER__)
extern void main(void);
#endif


/*! \brief Initialise all the data and peripherals, and prepare the machine to run
    after reset. */
extern void system_setup(void);

/*! This function updates the information displayed on the LCD each time it is called, stepping to the
    next item to be displayed, in a predetermined sequence. It should, therefore, be called at
    regular intervals, at the rate at which such cycling is required.
    \brief Update LCD display. */
extern void update_display(void);

/*! This function increments the RTC by one second.
    \brief Bump RTC by one second. */
extern void rtc_bumper(void);

/*! This function measures the speed of the RTC, by comparing MCLK, generated from the 32kHz RTC clock,
    with an external precision 32kHz signal.
    \brief Assess the speed at which the RTC is currently running, for RTC correction calibration. */
extern int32_t assess_rtc_speed(void);

/*! This function switches the meter to the normal operating state. It is usually used when the meter
    is in the low power state, or limp mode, and the conditions for normal operation have been detected.
    \brief Switch the meter to its normal operating mode. */
extern void switch_to_normal_mode(void);

/*! This function switches the meter to the limp, or live only, mode of operatiion. It is usually used
    the meter is running, but the measured voltage is too low to be valid.
    \brief Switch the meter to limp mode (also known as live only mode). */
extern void switch_to_limp_mode(void);

/*! This function switches the meter to the lowe power state, where only the RTC and power monitoring
    functions continue to operate within a timer interrupt routine. This is usually used when power
    failure has been detected. This function does not return until power has been restored. The
    machine sits in LPM3 within the function, waiting for actions within an interrupt routine to wake
    it up.
    \brief Switch the meter to minimum power operation (usual on supply failure). */
extern void switch_to_powerfail_mode(void);

/*! \brief Display a meter startup message on the LCD. */
extern void display_startup_message(void);

/*! \brief Display a power fail message on the LCD. */
extern void display_power_fail_message(void);

/*! \brief Display a mesage indicating the power supply had reached the low tide mark. */
extern void display_power_4v2_message(void);

/*! \brief Display a mesage indicating the power supply had reached the high tide mark. */
extern void display_power_normal_message(void);

/*! \brief Display a status mesage, showing the current progress of the calibration process. */
extern int display_calibration_status(void);


/*! This functions calculates the frequency of the power, from the information in the phase
    and phase_nv structures.
    \brief Calculate the mains frequency. 
    \return The frequency in 1/100Hz increments. */
extern int16_t frequency(void);

/*! \brief Calculate the RMS voltage of the specified phase, based on the information within that
    phase structure.
    \brief Calculate the phase RMS voltage.
    \return The RMS voltage in 10mV increments. */
extern int32_t voltage(void);

/*! \brief Calculate the RMS current flowing in the specified phase, based on the information within that
    phase structure.
    \brief Calculate the phase RMS current.
    \return The RMS current in 1mA increments. */
extern int32_t current(void);

/*! \brief Calculate the power factor in the specified phase, based on the information within that
    phase structure.
    \brief Calculate the power factor of a phase.
    \return The power factor, as Q1.15 fraction. */
extern int16_t power_factor(void);

/*! \brief Calculate the active power in the specified phase, based on the information within that
    phase structure.
    \brief Calculate the active power.
    \return The active power in 10mW increments. */
extern int32_t active_power(void);

/*! \brief Calculate the reactive power in the specified phase, based on the information within that
    phase structure.
    \brief Calculate the phase reactive power.
    \return The reactive power in 10mW increments. */
extern int32_t reactive_power(void);

/*! This function calculates the apparent power (i.e. the simple product of RMS voltage and RMS current),
    from the information in the phase and phase_nv structures.
    \brief Calculate the apparent power.
    \return The apparent power in 10mW increments. */
extern int32_t apparent_power(void);

#if defined(CUSTOM_MAINLOOP_SUPPORT)
extern void custom_mainloop_handler(void);
#endif
extern void keypad_handler(void);

/* Definitions for custom feature handling, which should only be used if the relevant
   feature is enabled. */

#if !defined(custom_adc_interrupt)
extern custom_adc_interrupt(void);
#endif







#if defined(CUSTOM_SERIAL_MESSAGE_SUPPORT)  &&  !defined(custom_serial_message_handler)
extern void custom_serial_message_handler(void);
#endif





extern uint8_t char_timeout_1107;

#if defined(IO_EXPANDER_SUPPORT)
/* what == -1 to clear bits
   what == 0 to set the whole word
   what == +1 to set bits
   which == the bit pattern to change */
void set_io_expander(int what, int which);
#endif

#define kick_watchdog()             WDTCTL = WDT_ARST_1000;

#define inhibit_rtc_updates()       (IE2 &= ~BTIE)

#define enable_rtc_updates()        (IE2 |= BTIE)

#endif

/* Do some configuration consistency checking */


