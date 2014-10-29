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
//  File: emeter-esp.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-esp.c,v 1.3 2009/04/23 07:32:07 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------

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

union long_word_u
{
    uint32_t u;
    int32_t s;
    uint16_t w[2];
};

enum
{
    ESP_IDLE,
    ESP_MEASURE,
    ESP_CALIBRATION,
    ESP_DONE,
    ESP_REQUEST_CAL
};

/* Set event message request flags: */
#if 0
#define defSET_EVENT \
                0 * WFSRDYME +    /* Interrupt on "New WFS"*/\
                0 * I2GTI1ME +    /* Interrupt on "I2GTI1"*/\
                1 * ILREACHEDME + /* Interrupt on "Interrupt level reached"*/\
                1 * ENRDYME +     /* Interrupt on "Energy samples ready (1sec)"*/\
                1 * ZXLDME +      /* Interrupt on "leading zero cross"*/\
                1 * ZXTRME +      /* Interrupt on "trailing zero cross"*/\
                0 * CALRDYME +    /* Interrupt on "Calibration Data ready"*/\
                0 * TAMPME +      /* Interrupt on "Tampering detected" */\
                1 * NEGENME +     /* Interrupt on "Neg. energy measuered" */\
                0 * VDROPME +     /* Interrupt on "V drop event" */\
                0 * VPEAKME +     /* Interrupt on "V Peak Level reached" */\
                0 * I1PEAKME +    /* Interrupt on "I1 Peak Level reached" */\
                0 * I2PEAKME +    /* Interrupt on "I2 Peak Level reached" */\
                0
#else
#define defSET_EVENT \
                1 * WFSRDYME +    /* Interrupt on "New WFS"*/\
                0 * ILREACHEDME + /* Interrupt on "Interrupt level reached"*/\
                1 * ENRDYME +     /* Interrupt on "Energy samples ready (1sec)"*/\
                0 * ZXLDME +      /* Interrupt on "leading zero cross"*/\
                0 * ZXTRME +      /* Interrupt on "trailing zero cross"*/\
                0 * CALRDYME +    /* Interrupt on "Calibration Data ready"*/\
                0 * TAMPME +      /* Interrupt on "Tampering detected" */\
                1 * NEGENME +     /* Interrupt on "Neg. energy measuered" */\
                0 * VDROPME +     /* Interrupt on "V drop event" */\
                0 * VPEAKME +     /* Interrupt on "V Peak Level reached" */\
                0 * I1PEAKME +    /* Interrupt on "I1 Peak Level reached" */\
                0 * ACTIVEME +    /* Interrupt on "Measurement of Calibration running" */\
                0
#endif

#define defTempGain       ((int16_t) (1168.0 / (1.257 * 0xFFFF) * 10000.0))
#define defTempOffset     (273*100)
#define defTemp_repeat    10    /*repeat temp measurement every 10 sec */
#define defCalTemp        2400  /* Room Temp at Calibraton in x100 Degree C */

uint16_t sys_status;   // System status bits

enum
{
    NEW_VALUES_AVAILABLE = 0x01,
    DO_CALC = 0x02,
    REINITIALISE = 0x04,
    REQUEST_TEMPERATURE_READING = 0x08,
    NEW_TEMPERATURE_READING = 0x10
};

uint16_t OP_Mode;  // operation mode

int16_t samples_per_second;

/// global flag for Edge detection
unsigned char ZXLDFGedge = 0;
/// global flag for Negative Energy
unsigned char negenfg = 0;
/// Saves firmware version during initialization.
unsigned int firmware_version;
/// Cumulated active energy.
float total_energy;

int16_t wfs1;
int16_t wfs2;
int16_t wfs3;
static union long_word_u active_energy;
static union long_word_u reactive_energy;
static union long_word_u apparent_energy;
static int16_t v_rms;
static union long_word_u i_rms;
static int16_t freq;

uint16_t CalCyclCnt = 0;     // Register for Cycle calculation of Calibration

/// 2 raised to the power of 14 - Required for parameter calculations
#define POW_2_14 0x004000
/// 2 raised to the power of 16 - Required for parameter calculations
#define POW_2_16 0x010000
/// 2 raised to the power of 20 - Required for parameter calculations
#define POW_2_20 0x100000ul
/// 2 raised to the power of 20 - Required for parameter calculations
//#define POW_2_32 0x100000000ul

// Created with Excel Table
// Date:  04/22/2005        Time:  08:19:45

#define V_Gain                        SD16GAIN_1
#define I1_Gain                       SD16GAIN_16
#define I2_Gain                       SD16GAIN_16

#define SHUNT                         0
#define CT                            1
#define ROG_COIL                      2
#define NONE                          3

#define I1SENSOR                      SHUNT
#define I2SENSOR                      SHUNT

#define defVRatio                     12.2133211261158
#define defIRatio                     3.66216525520714
#define defEnergyRatio                44.7272002787487

#define defSET_CALVOLTAGE             240
#define defSET_CALCURRENT             10
#define defCalPhi                     -60
#define defCalCosPhi                  0.5
#define defCalFreq                    MAINS_NOMINAL_FREQUENCY

// Phase Error Correction:
#define defSET_PHASECORR1             5
#define defSET_PHASECORR2             0

#define defSET_INTRPTLEVL_REPEAT      200
#define _defSET_INTRPTLEVL            ((int32_t) 428154) //((int32_t) 4281540036)

#define defSET_INTRPTLEVL_LO          (_defSET_INTRPTLEVL & 0xFFFF)
#define defSET_INTRPTLEVL_HI          ((_defSET_INTRPTLEVL >> 16) & 0xFFFF)

/** Adjustment parameters for the two currents:  */
#define defSET_ADAPTI1                16384
#define defSET_ADAPTI2                16384

//   Nominal Mains Frequency:
#define defSET_NOMFREQ                MAINS_NOMINAL_FREQUENCY


/** Adjustment parameters start up currents: */
/* as Factor of calCurrent */
#ifdef WITH_START_CURRENT
#define defSET_STARTCURR              (0.004 * defSET_CALCURRENT)
#define defSET_STARTCURR_INT          (unsigned int)((defSET_STARTCURR * 1000 / defIRatio))
#define defSET_STARTCURR_FRAC         (unsigned int)((defSET_STARTCURR * POW_2_16 * 1000 / defIRatio) - (POW_2_16 * defSET_STARTCURR_INT))
#else
#define defSET_STARTCURR              0
#define defSET_STARTCURR_INT          0
#define defSET_STARTCURR_FRAC         0
#endif

// Default Gain Settings:
// 1.0  =  #10*4000h/10    Format (+1.14)
#define defSET_GAINCORR1 (1 * POW_2_14)
#define defSET_GAINCORR2 (1 * POW_2_14)

// OFFSET for V, I1, I2
#define defSET_V1OFFSET (0)
#define defSET_I1OFFSET (0)
#define defSET_I2OFFSET (0)

#define  defSET_CTRL0 \
                0 * BIT0 +      /* - 0: disable 2. Current path */\
                0 * BIT1 +      /* - 0: I1 = CT or Shunt  /  1: I1 = RogCoil  */\
                1 * BIT2 +      /* - 1 : Generate Interrupt after spec. number of measurements (-> 50*4096 = 1 sec) */\
                                /* - 0 : Generate Interrupt after interrupt level has been reached */\
                0 * BIT3 +      /* - NE0: Count absolute active energy p1 */\
                1 * BIT4 +      /* - NE1 Count absolute active energy p2 */\
                1 * BIT5 +      /* - Dis/Enable DC removal alorithm on for V1 */\
                0 * BIT6 +      /* - Dis/Enable DC removal alorithm on for I1 */\
                0 * BIT7        /* - Dis/Enable DC removal alorithm on for I2 */

struct ts_parameters
{
    float VRatio;
    float IRatio;
    float EnergyRatio;
    int pSET_PHASECORR1;
    int pSET_PHASECORR2;
    unsigned int pSET_GAINCORR1;
    unsigned int pSET_GAINCORR2;
    int pSET_V1OFFSET;
    int pSET_I1OFFSET;
    int pSET_I2OFFSET;
    unsigned int pSET_ADAPTI1;
    unsigned int pSET_ADAPTI2;
    unsigned int pSET_STARTCURR_INT;
    unsigned int pSET_STARTCURR_FRAC;
    union long_word_u pSET_INTRPTLEVL;
    unsigned long ulTempGain;
    unsigned long ulTempOffset;
    union long_word_u pSET_POFFSET1;
    union long_word_u pSET_POFFSET2;
};

const struct ts_parameters s_parameters =
{
    defVRatio,
    defIRatio,
    defEnergyRatio,
    defSET_PHASECORR1,
    defSET_PHASECORR2,
    defSET_GAINCORR1,
    defSET_GAINCORR2,
    defSET_V1OFFSET,
    defSET_I1OFFSET,
    defSET_I2OFFSET,
    defSET_ADAPTI1,
    defSET_ADAPTI2,
    defSET_STARTCURR_INT,
    defSET_STARTCURR_FRAC,
    {
        .w[0] = defSET_INTRPTLEVL_LO,
        .w[1] = defSET_INTRPTLEVL_HI
    },
    defTempGain,
    defTempOffset,
    0,  // P1Offset
    0   // P2Offset
};

#ifdef withTempCorrection
#define PHI_Corr(x) (int)(x/(360ul*defSET_NOMFREQ)*POW_2_20 + 0.5)
const int phasecorr_table[] =
{
    PHI_Corr(-60*PHI_Temp_Ratio),  // -40 <-> -30
    PHI_Corr(-50*PHI_Temp_Ratio),  // -30 <-> -20
    PHI_Corr(-40*PHI_Temp_Ratio),  // -40 <-> -10
    PHI_Corr(-30*PHI_Temp_Ratio),  // -10 <->   0
    PHI_Corr(-20*PHI_Temp_Ratio),  //   0 <->  10
    PHI_Corr(-10*PHI_Temp_Ratio),  //  10 <->  20
    PHI_Corr(  0*PHI_Temp_Ratio),  //  20 <->  30
    PHI_Corr( 10*PHI_Temp_Ratio),  //  30 <->  40
    PHI_Corr( 20*PHI_Temp_Ratio),  //  40 <->  50
    PHI_Corr( 30*PHI_Temp_Ratio),  //  50 <->  60
    PHI_Corr( 40*PHI_Temp_Ratio),  //  60 <->  70
    PHI_Corr( 50*PHI_Temp_Ratio),  //  70 <->  80
    PHI_Corr( 60*PHI_Temp_Ratio),  //  80 <->  85
};
#endif

union long_word_u tempenergy;

unsigned long P_reading = 0;

uint16_t uiIntLevelRepeatCount = 0;

void display_esp_comms_error(void)
{
}

/* Set one parameter of the ESP430CE1 module. */
static void set_parameter(uint16_t param, uint16_t data)
{
    volatile uint16_t timeout = 0xFFFF;

    _DINT();
    MBOUT1 = data;
    MBOUT0 = param;
    do
    {
        while (((MBCTL & IN0IFG) == 0)  &&  ((--timeout) > 0))
            ;
        if (timeout == 0)
        {
            display_esp_comms_error();
            _EINT();
            return;
        }
    }
    while ((MBIN0 != mPARAMSET)  ||  (MBIN1 != param));
    _EINT();
}

/* Read one parameter of the ESP430CE1 module. */
static uint16_t read_parameter(uint16_t param)
{
    volatile uint16_t timeout = 0xFFFF;
    uint16_t temp;

    _DINT();
    MBOUT1 = param;
    MBOUT0 = mREAD_PARAM;

    do
    {
        while (((MBCTL & IN0IFG) == 0)  &&  ((--timeout) > 0))
            ;
        if (timeout == 0)
        {
            display_esp_comms_error();
            _EINT();
            return(0);
        }
        /* Save data in temp variable to avoid overwrite */
        temp = MBIN1;
    }
    while (MBIN0 != mPARAMRDY);
    _EINT();
    return temp;
}

/* Initializes ESP430CE1 Paramters */
static void esp_init_parameters(void)
{
    volatile unsigned int timeout;

    /** Then the parameters are initialized.
      *
      *  Control 0: make settings for :
      * - Use current channel I2 - tamper-detection
      * - Count absolute active energy
      *   (negative energy is considered as tampering)
      * - Switch DC removal alorithm on for I1
      * - Switch DC removal alorithm on for I2
      */
    set_parameter(mSET_CTRL0, defSET_CTRL0);

    /** \Set Number of Measurement:
      *    e.g.  4096 * 50Hz. => int after 1sec
      */
    set_parameter(mSET_INTRPTLEVL_LO, s_parameters.pSET_INTRPTLEVL.w[0]);
    set_parameter(mSET_INTRPTLEVL_HI, s_parameters.pSET_INTRPTLEVL.w[1]);

    /**  Nominal mains frequency:
      *    e.g.  50Hz.
      */
    set_parameter(mSET_NOMFREQ, defSET_NOMFREQ);

    /** Phase error correction:
      * Sets phase error for current 1/2 at nominal mains frequency for
      * current transformer according to its specification
      * The phase error of the shunt is zero.
      */
    set_parameter(mSET_PHASECORR1, (int) s_parameters.pSET_PHASECORR1);
#if defined(NEUTRAL_MONITOR_SUPPORT)
    set_parameter(mSET_PHASECORR2, (int) s_parameters.pSET_PHASECORR2);
#endif

    /** Adjustment parameters for the two currents:
      * Current Transformer:
      *
      * There are two possibilties to adjust the two current
      * values:
      * -# Only one current value is scaled with a factor > 1
      *    to fit the others current's value. The factor
      *    for the later is set to 1.\n
      *    In this example current I1 would be scaled by 0.32/0.30 = 1.06667.
      *    (The RMS values at the maximum current are used to calculate the
      *     factor.)
      *    The current transformer's value is scaled to fit with
      *    shunt's value, because the shunt's values are bigger,
      *    so the current transformer's values are multiplied with
      *    a factor > 1 - otherwise the shunt's values would be
      *    multiplied with a factor < 1 and this would result
      *    in degraded accuracy.
      * -# Both currents are scaled with a factor > 1 to use the maximum FSR
      *    (Full Scale Range) at the maximum current Imax.\n
      *    In this example current I1 would be scaled by 0.44/0.30 = 1.46667
      *    and current I2 would be scaled by 0.44/0.32 = 1.25.
      *    (The RMS values at the maximum current are used to calculate the
      *     factor. VFSR(Peak) = 0.625 (see datasheet) => VFSR(RMS) = 0.44)
      */
    set_parameter(mSET_ADAPTI1, s_parameters.pSET_ADAPTI1); // = 1 * POW_2_14 = 16384
#if defined(NEUTRAL_MONITOR_SUPPORT)
    set_parameter(mSET_ADAPTI2, s_parameters.pSET_ADAPTI2); // = 1 * POW_2_14 = 16384
#endif

    /* Adjustment parameters gain settings: */
    set_parameter(mSET_GAINCORR1, s_parameters.pSET_GAINCORR1);
#if defined(NEUTRAL_MONITOR_SUPPORT)
    set_parameter(mSET_GAINCORR2, s_parameters.pSET_GAINCORR2);
#endif

    /* Adjustment parameters offset settings: */
    set_parameter(mSET_V1OFFSET, s_parameters.pSET_V1OFFSET);
    set_parameter(mSET_I1OFFSET, s_parameters.pSET_I1OFFSET);
#if defined(NEUTRAL_MONITOR_SUPPORT)
    set_parameter(mSET_I2OFFSET, s_parameters.pSET_I2OFFSET);
#endif
    set_parameter(mSET_POFFSET1_LO, s_parameters.pSET_POFFSET1.w[0]);
    set_parameter(mSET_POFFSET1_HI, s_parameters.pSET_POFFSET1.w[1]);
#if defined(NEUTRAL_MONITOR_SUPPORT)
    set_parameter(mSET_POFFSET2_LO, s_parameters.pSET_POFFSET2.w[0]);
    set_parameter(mSET_POFFSET2_HI, s_parameters.pSET_POFFSET2.w[1]);
#endif

    /* Adjustment parameters start up currents: */
    set_parameter(mSET_STARTCURR_INT, s_parameters.pSET_STARTCURR_INT);
    set_parameter(mSET_STARTCURR_FRAC, s_parameters.pSET_STARTCURR_FRAC);

    /* Adjustment parameters for DC removal periods: */
#if defined(NEUTRAL_MONITOR_SUPPORT)
    set_parameter(mSET_DCREMPER, defSET_DCREMPER);
#endif
}

/* Initializes ESP430CE1. */
void esp_init(void)
{
    volatile unsigned int timeout;

    /* Make sure the ESP is enabled. */
    ESPCTL |= ESPEN;
    MBCTL = 0;

    /* ... and that it is not in measurement or calibration mode. */
    if ((RET0 & 0x8000) != 0)
    {
        /* Set ESP to "Idle" mode */
        MBOUT1 = modeIDLE;
        MBOUT0 = mSET_MODE;
        timeout = 0xFFFF;
        while (((RET0 & 0x8000) != 0)  &&  (timeout-- > 0))
            ;
    }

    /* ...and that it is ready to receive messages by requesting the firmware version. */
    MBOUT0 = mSWVERSION;
    timeout = 0xFFFF;
    do
    {
        while (((MBCTL & IN0IFG) == 0)  &&  (timeout-- > 0))
            ;
        if (timeout == 0)
        {
            display_esp_comms_error();
            return;
        }
    }
    while (MBIN0 != mSWRDY);
    firmware_version = MBIN1;

    esp_init_parameters();
}

/* Starts energy measurement. */
void esp_start_measurement(void)
{
    /* Set event message request flags: */
    set_parameter(mSET_EVENT, defSET_EVENT);

    total_energy = 0;

    MBCTL = IN0IE;

    /* Start measurement (set Embedded Signal Processing into "Measurement" mode) */
    MBOUT1 = modeMEASURE; //ESP_MEASURE;
    MBOUT0 = mSET_MODE;
    OP_Mode = ESP_MEASURE;
}

void esp_start_calibration(void)
{
    volatile unsigned int timeout;

    /* ensure that it is not in measurement or calibration mode, */
    if ((RET0 & 0x8000))
    {
        // Set Embedded Signal Processing into "Idle" mode
        MBOUT1 = modeIDLE; // ESP_IDLE;
        MBOUT0 = mSET_MODE;
        timeout = 0xFFFF;
        while (((RET0 & 0x8000) != 0)  &&  (timeout-- > 0))
            ;
    }
    MBCTL = 0;
    total_energy = 0;

    // Set event message request flags:
    set_parameter(mSET_EVENT, CALRDYME);  // Interrupt on "Calibration values ready"

    set_parameter(mSET_CALCYCLCNT, CalCyclCnt); // = set requested cycles for measurement

    // Start measurement (set Embedded Signal Processing into "Calibration" mode)
    set_parameter(mSET_MODE, modeCALIBRATION);
    OP_Mode = ESP_CALIBRATION;
    MBCTL = IN0IE;
}

void esp_set_active(void)
{
    // Restart measurement (set Embedded Signal Processing into "Measurement" mode)
    SD16CTL |= SD16REFON;         // Switch Reference on
    ESPCTL &= ~(0x08 + ESPSUSP);  // Set ESP into Active Mode
    MBCTL= IN0IE;                 // Enable Mailbox Interrupts

    // Startmeasurement (set Embedded Signal Processing into "Measurement" mode)
    //  ensure that it is not in measurement or calibration mode,
    if ((RET0 & 0x8000) == 0)
    {
        // Set Embedded Signal Processing into "Idle" mode
        MBOUT1= modeMEASURE; // modeMEASURE;
        MBOUT0= mSET_MODE;
    }
    OP_Mode = ESP_MEASURE;
#ifdef TIMERA_PULSE_OUTPUT
    Init_TimerA();        // Start TimerA
#endif
    SVSCTL = 0x70;  // set SVS to 2.65 V to detect voltage drops below safe operating area
}

void esp_set_idle(void)
{
    // Stop measurement (set Embedded Signal Processing into "Idle" mode)
    // Set Embedded Signal Processing into "Idle" mode
    MBOUT1 = modeIDLE; // ESP_IDLE;
    MBOUT0 = mSET_MODE;
    while ((RET0 & 0x8000) != 0)
        ; // Wait for Idle mode

    // Shut donw ESP (set Embedded Signal Processing into "Suspend" mode)
    // ensure that it is not in measurement or calibration mode,
    if ((RET0 & 0x8000) == 0)
    {
        ESPCTL |= (0x08 | ESPSUSP); // Set ESP into Suspend Mode
                                    // incl.  Bug Fix for Suspend Mode
    }
    // wait 10 clock cycles, until proper access to the SD16 is possilbe (9 clock cycles are required)
    _NOP();_NOP();_NOP();_NOP();_NOP();
    _NOP();_NOP();_NOP();_NOP();_NOP();

    MBCTL &= ~(IN0IFG + IN0IE);    // Clear any Pending MB interrupt and disable
                                   // ESP interrupt
    SD16CTL &= ~SD16REFON; // Switch Reference off
    OP_Mode = ESP_IDLE;
#ifdef withDisplay
    DisplayIDLE();
    //LCD_OFF;        // Switch LCD off
#endif
    P1OUT |= 0x03;   // set P1.0 + P1.1 -> LED off
    custom_energy_pulse_end();

#ifdef TIMERA_PULSE_OUTPUT
    TACTL = TACLR | MC_0 | TASSEL_1; // Stop TimerA
    TACCTL0 &= ~CCIFG;               // Clear Pending ISR for CCR0
#endif
    SVSCTL = 0x00;  // switch SVS off
}

/* Interrupt service routine for messages sent by ESP430CE1. */
ISR(ESP430, esp430_interrupt)
{
    unsigned int msg_data = MBIN1;
    unsigned int msg = MBIN0;

    if (msg == mEVENT)
    {
        /* Get the Calibration data */
        if ((msg_data & CALRDYFG) == CALRDYFG) // Calibration data available
        {
            if (OP_Mode == ESP_CALIBRATION)
            {
                active_energy.w[0] = ACTENERGY1_LO;
                active_energy.w[1] = ACTENERGY1_HI;
                OP_Mode = ESP_DONE;
                // Set Embedded Signal Processing into "Measure" mode
                msg_data = 0; // prevent that the other functions in the ISR are executed
            }
        }

        msg_data = msg_data & defSET_EVENT; //mask with event request mask

        /* Get the WFS */
        if ((msg_data & WFSRDYFG)) // New Waveform Samples available
        {
            wfs1 = WAVEFSV1;
            wfs2 = WAVEFSI1;
#if defined(NEUTRAL_MONITOR_SUPPORT)
            wfs3 = WAVEFSI2;
#endif


        /* Get energy and create pulses */
          if ((total_active_power_counter += labs(phase->readings.active_power)) >= TOTAL_ENERGY_PULSE_THRESHOLD)
          {
              total_active_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
              custom_energy_pulse_start();
              uiIntLevelRepeatCount = 164;
          }
  
          if (uiIntLevelRepeatCount  &&  --uiIntLevelRepeatCount == 0)
              custom_energy_pulse_end();

        }
        
        
        if (msg_data & ZXTRFG)
            ZXLDFGedge = 1;
        if ((msg_data & ZXLDFG)  &&  ZXLDFGedge)
            ZXLDFGedge = 0;
/*
        //  DO NOT USE THIS FLAG AS IT IS NOT WORKING OK IN THIS VERSION !!!
        if (msg_data & NEGENFG) // Negative Energy measured ?
            //P1OUT |= BIT0;
        else
            //P1OUT &= ~BIT0;
*/
        /**
          * Accumulates the active energy depending on the tampering flag so
          * that always the "worst case" energy is calculated.
          */
        if ((msg_data & ENRDYFG))
        {
            static unsigned char temp_count = defTemp_repeat;

#if defined(NEUTRAL_MONITOR_SUPPORT)
            if (!(msg_data & I2GTI1FG))
            {
                /* I1 > I2 */
                active_energy.w[0] = ACTENERGY1_LO;
                active_energy.w[1] = ACTENERGY1_HI;
            }
            else
            {
                /* I1 < I2 */
                active_energy.w[0] = ACTENERGY2_LO;
                active_energy.w[1] = ACTENERGY2_HI;
            }
#else
            active_energy.w[0] = ACTENERGY1_LO;
            active_energy.w[1] = ACTENERGY1_HI;
#endif
            reactive_energy.w[0] = REACTENERGY_LO;
            reactive_energy.w[1] = REACTENERGY_HI;
            apparent_energy.w[0] = APPENERGY_LO;
            apparent_energy.w[1] = APPENERGY_HI;
            v_rms = V1RMS;
            i_rms.w[0] = IRMS_LO;
            i_rms.w[1] = IRMS_HI;
            freq = MAINSPERIOD;

            if ((active_energy.w[1] & 0x8000))
            {
                /* Negative energy measured */
                negenfg = TRUE;
	    }
            else
            {
                negenfg = FALSE;
            }
	
            if ((msg_data & NEGENFG))
                total_energy -= (float) active_energy.s;
            else
                total_energy += (float) active_energy.s;

            /* The seconds are counted based on the ESP430CE1 energy ready interrupts. */
            temp_count--;
            if ((temp_count == 0)  &&  (defTemp_repeat != 0))
            {
                /* Every minute a temperature measurement is requested. */
                temp_count = defTemp_repeat;
                sys_status |= REQUEST_TEMPERATURE_READING;
            }
            phase->status |= NEW_LOG;

            sys_status |= NEW_VALUES_AVAILABLE;
            if (OP_Mode != ESP_IDLE)
                LPM3_EXIT;    // back to active mode after ISR
        }
    }

    if (msg == mTEMPRDY)
    {
        /* The temperature is saved as soon as it is delivered by the ESP430. */
        temperature = msg_data;
        sys_status |= NEW_TEMPERATURE_READING;
    }
}

/* Interrupt service routine for SD16 (Dummy). */
ISR(SD16, adc_interrupt)
{
}

#if defined(SINGLE_PHASE)
int32_t active_power(void)
#else
int32_t active_power(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
#endif
{
    return active_energy.s/85;
}

#if defined(REACTIVE_POWER_SUPPORT)
    #if defined(SINGLE_PHASE)
int32_t reactive_power(void)
    #else
int32_t reactive_power(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
    #endif
{
    return reactive_energy.s/85;
}
#endif

#if defined(APPARENT_POWER_SUPPORT)
    #if defined(SINGLE_PHASE)
int32_t apparent_power(void)
    #else
int32_t apparent_power(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
    #endif
{
    return apparent_energy.s/85;
}
#endif

#if defined(VRMS_SUPPORT)
    #if defined(SINGLE_PHASE)
int32_t voltage(void)
    #else
int32_t voltage(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv)
    #endif
{
    int32_t x;

    x = v_rms;
    x *= 7496;
    x >>= 12;
    return x;
}
#endif

#if defined(IRMS_SUPPORT)
    #if defined(SINGLE_PHASE)
int32_t current(void)
    #else
int32_t current(struct phase_parms_s *phase, struct phase_nv_parms_s const *phase_nv, int ph)
    #endif
{
    int32_t x;

    x = i_rms.s;
    x /= 10350;
    return x;
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

    x = (int32_t) SAMPLES_PER_10_SECONDS*256L*10L/freq;
    return x;
}
#endif
