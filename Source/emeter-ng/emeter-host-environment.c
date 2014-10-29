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
//  File: emeter_1ph_foreground.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-host-environment.c,v 1.3 2009/01/21 03:03:13 a0754793 Exp $
//
//--------------------------------------------------------------------------
//
//  MSP430 foreground (non-interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt. 
// 
#include <stdint.h>
#if !defined(__MSP430__)
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>
#endif
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"

#if !defined(__MSP430__)
#include <unistd.h>
#include <audiofile.h>

#include "serial.h"

AFfilehandle inhandle;
AFfilehandle outhandle;

rtc_t rtc;

typedef struct
{
    double rms;
    long int ix1;
    long int ix2;
    long int ix3;
    double r[98];
    double gset;
    int iset;
} awgn_state_t;

#if !defined(__MSP430__)
/* Parameters for development work on a host */
static int input_current;
static int input_phase;
static char *input_sensor;
#endif

/* Gaussian noise generator constants */
#define M1 259200
#define IA1 7141
#define IC1 54773
#define RM1 (1.0/M1)
#define M2 134456
#define IA2 8121
#define IC2 28411
#define RM2 (1.0/M2)
#define M3 243000
#define IA3 4561
#define IC3 51349

static inline int16_t fsaturate(double damp)
{
    if (damp > 32767.0)
        return  INT16_MAX;
    if (damp < -32768.0)
        return  INT16_MIN;
    return rint(damp);
}
/*- End of function --------------------------------------------------------*/

static double ran1(awgn_state_t *s)
{
    double temp;
    int j;

    s->ix1 = (IA1*s->ix1 + IC1)%M1;
    s->ix2 = (IA2*s->ix2 + IC2)%M2;
    s->ix3 = (IA3*s->ix3 + IC3)%M3;
    j = 1 + ((97*s->ix3)/M3);
    if (j > 97  ||  j < 1)
    {
        fprintf(stderr, "Random number generator error - %d\n", j);
        exit(-1);
    }
    temp = s->r[j];
    s->r[j] = (s->ix1 + s->ix2*RM2)*RM1;
    return temp;
}
/*- End of function --------------------------------------------------------*/

void awgn_init(awgn_state_t *s, int idum, int level)
{
    int j;

    if (idum < 0)
        idum = -idum;

    s->rms = pow(10.0, (level - 3.14)/20.0)*(32768.0*0.70711);

    s->ix1 = (IC1 + idum)%M1;
    s->ix1 = (IA1*s->ix1 + IC1)%M1;
    s->ix2 = s->ix1%M2;
    s->ix1 = (IA1*s->ix1 + IC1)%M1;
    s->ix3 = s->ix1%M3;
    s->r[0] = 0.0;
    for (j = 1;  j <= 97;  j++)
    {
        s->ix1 = (IA1*s->ix1 + IC1)%M1;
        s->ix2 = (IA2*s->ix2 + IC2)%M2;
        s->r[j] = (s->ix1 + s->ix2*RM2)*RM1;
    }
    s->gset = 0.0;
    s->iset = 0;
}
/*- End of function --------------------------------------------------------*/

int16_t awgn(awgn_state_t *s)
{
    double fac;
    double r;
    double v1;
    double v2;
    double amp;

    if (s->iset == 0)
    {
        do
        {
            v1 = 2.0*ran1(s) - 1.0;
            v2 = 2.0*ran1(s) - 1.0;
            r = v1*v1 + v2*v2;
        }
        while (r >= 1.0);
        fac = sqrt(-2.0*log(r)/r);
        s->gset = v1*fac;
        s->iset = 1;
        amp = v2*fac*s->rms;
    }
    else
    {
        s->iset = 0;
        amp = s->gset*s->rms;
    }
    return fsaturate(amp);
}
/*- End of function --------------------------------------------------------*/

#define GEN_FREQUENCY   50.0
#define PHASE_SHIFT     (60.0*3.1415926535/180.0)
#define V_PEAK          (220.0*134.068)
#define I_PEAK          (0.2*311.29)
#define BLOCK_PERIOD    4096

#if !defined(__MSP430__)
static int target_serial_port = -1;
static int display_serial_port = -1;
int skip_report = FALSE;
int source = 0;

int start_host_environment(int argc, char *argv[])
{
    AFfilesetup filesetup;
    char *sample_source_name;
    int source_type;
    int i;

    printf("Start\n");
    i = 1;
    source_type = 0;
    if (i < argc)
    {
        if (strcmp(argv[i], "-s") == 0)
        {
            source_type = 1;
            i++;
        }
    }
    if (i < argc)
        sample_source_name = argv[i++];
    else
        sample_source_name = "captured.wav";
    if (i < argc)
        input_current = atof(argv[i++])*100.0;
    if (i < argc)
        input_phase = atoi(argv[i++]);
    if (i < argc)
        input_sensor = argv[i++];
    else
        input_sensor = "X";

    filesetup = afNewFileSetup ();
    if (filesetup == AF_NULL_FILESETUP)
    {
        fprintf(stderr, "Failed to create file setup\n");
        exit (2);
    }
    afInitSampleFormat(filesetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
    afInitRate(filesetup, AF_DEFAULT_TRACK, (float) SAMPLES_PER_10_SECONDS/10);
    afInitFileFormat(filesetup, AF_FILE_WAVE);
    afInitChannels(filesetup, AF_DEFAULT_TRACK, 3);

    outhandle = afOpenFile("samples.wav", "wb", filesetup);
    if (outhandle == AF_NULL_FILEHANDLE)
    {
        fprintf(stderr, "Cannot open captured data file '%s'\n", "samples.wav");
        exit(2);
    }

    source = source_type;
    if (source == 0)
    {
        inhandle = afOpenFile(sample_source_name, "rb", NULL);
        if (inhandle == AF_NULL_FILEHANDLE)
        {
            fprintf(stderr, "Cannot open captured data file '%s'\n", sample_source_name);
            exit(2);
        }
        return  0;
    }
    if (source == 1)
    {
        /* Open the port to the ADC gathering unit */
        if ((target_serial_port = serial_open(sample_source_name, 460800, 0, 8)) < 0)
        {
            fprintf(stderr, "Cannot open serial port '%s'\n", sample_source_name);
            exit(2);
        }
        /* Open the port to the LED flashing unit */
        if ((display_serial_port = serial_open("com1", 9600, 0, 8)) < 0)
        {
            fprintf(stderr, "Cannot open serial port '%s'\n", sample_source_name);
            exit(2);
        }
        serial_flushinput(target_serial_port);
        serial_flushinput(display_serial_port);
    
        printf("Serial port open\n");
        return  0;
    }
    return  0;
}

void get_sample(int16_t adc_buffer[])
{
    static uint16_t sample_buf[1024*3];
    static int buf_ptr = 0;
    int outframes;
    int len;
    int bit;
    int cyclic;
    static int last_cyclic = 0;
    uint8_t buf[100];
    int16_t V_sample;
    int16_t I_live_sample;
    int16_t I_neutral_sample;
    static int i = 0;
    static awgn_state_t voltage_noise;
    static awgn_state_t live_current_noise;
    static awgn_state_t neutral_current_noise;
    int inframes;

    if (source == 0)
    {
        inframes = afReadFrames(inhandle,
                                AF_DEFAULT_TRACK,
                                adc_buffer,
                                1);
        adc_buffer[2] = adc_buffer[1];
        if (inframes != 1)
        {
            //report_results();
            exit(0);
        }
#if 1
        sample_buf[buf_ptr++] = 0;//adc_buffer[0];
        sample_buf[buf_ptr++] = adc_buffer[1];
        sample_buf[buf_ptr++] = phase->active_power;
        if (buf_ptr >= 1024*3)
        {
            outframes = afWriteFrames(outhandle,
                                      AF_DEFAULT_TRACK,
                                      sample_buf,
                                      1024);
            if (outframes != 1024)
            {
                fprintf(stderr, "Error writing sample.save file\n");
                exit(2);
            }            
            buf_ptr = 0;
        }
#endif
        return;
    }

    if (source == 1)
    {
        for (;;)
        {
            if ((bit = serial_read(target_serial_port, buf, 1, -1)) < 0)
                exit(2);
            if (bit < 1  ||  ((buf[0] & 0x80) == 0))
                continue;
            break;
        }
        len = 8;
        do
        {
            if ((bit = serial_read(target_serial_port, buf + 9 - len, len, -1)) < 0)
                exit(2);
            len -= bit;
        }
        while (len > 0);
        i = 6;
        adc_buffer[0] = (uint16_t) ((buf[i] & 0xF) << 12) | ((buf[i + 1] & 0x3F) << 6) | (buf[i + 2] & 0x3F);
        i = 3;
        adc_buffer[1] = (uint16_t) ((buf[i] & 0xF) << 12) | ((buf[i + 1] & 0x3F) << 6) | (buf[i + 2] & 0x3F);
        i = 0;
        adc_buffer[2] = (uint16_t) ((buf[i] & 0xF) << 12) | ((buf[i + 1] & 0x3F) << 6) | (buf[i + 2] & 0x3F);
#if 1
        cyclic = adc_buffer[2] & 0xFFFF;
        if (cyclic != ((last_cyclic + 1) & 0xFFFF))
        {
            printf("Hiccup %d %d, diff %d\n", last_cyclic, cyclic, cyclic - last_cyclic);
            fprintf(stderr, "Hiccup %d %d, diff %d\n", last_cyclic, cyclic, cyclic - last_cyclic);
            skip_report = TRUE;
        }
        last_cyclic = cyclic;
#endif
#if 1
        sample_buf[buf_ptr++] = adc_buffer[0];
        sample_buf[buf_ptr++] = adc_buffer[1];
        sample_buf[buf_ptr++] = adc_buffer[2];
        if (buf_ptr >= 1024*3)
        {
            outframes = afWriteFrames(outhandle,
                                      AF_DEFAULT_TRACK,
                                      sample_buf,
                                      1024);
            if (outframes != 1024)
            {
                fprintf(stderr, "Error writing sample.save file\n");
                exit(2);
            }            
            buf_ptr = 0;
        }
#endif
        adc_buffer[2] = 0;
        return;
    }
    /* Synthesize signals */
    if (i == 0)
    {
        awgn_init(&voltage_noise, 123456, -60);
        awgn_init(&live_current_noise, 123456, -70);
        awgn_init(&neutral_current_noise, 123456, -70);
    }
    V_sample = V_PEAK*sin(i*2.0*3.14159265*GEN_FREQUENCY/4096.0) + awgn(&voltage_noise);
    I_live_sample = I_PEAK*sin(PHASE_SHIFT + i*2.0*3.14159265*GEN_FREQUENCY/4096.0) + awgn(&live_current_noise);
    I_neutral_sample = I_PEAK*sin(PHASE_SHIFT + i*2.0*3.14159265*GEN_FREQUENCY/4096.0) + awgn(&neutral_current_noise);
    adc_buffer[0] = V_sample;
    adc_buffer[1] = I_live_sample;
    adc_buffer[2] = I_neutral_sample;
    adc_buffer[3] = 0;
    i++;
}

#if 0 //!defined(__MSP430__)

extern FILE *csv_log;

int host_load_parms(struct nv_parms_s *nv)
{
#if !defined(SINGLE_PHASE)
    struct phase_parms_s *phase;
    struct phase_nv_parms_s *phase_nv;
    struct phase_nv_parms_s *phase_cal;
    int j;
#endif
    int i;
    int val;
    char buf[80 + 1];
    FILE *parms;

    printf("Loading calibration settings\n");
    parms = fopen("emeter.prm", "r");
    if (parms == NULL)
    {
        fprintf(stderr, "Cannot open the parameter file\n");
        return  FALSE;
    }
    if (fgets(buf, 80, parms) == NULL)
        return  FALSE;
    if (sscanf(buf, "%d", &val) != 1)
        return  FALSE;
    nv->meter_uncalibrated = val;

#if !defined(SINGLE_PHASE)
    phase = chan;
    phase_nv = nv_parms.s.chan;
    phase_cal = cal_info.chan;
    for (j = 0;  j < NUM_PHASES;  j++)
    {
#endif
#if defined(VRMS_SUPPORT)
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->V_rms_scale_factor = val;
#endif
#if defined(IRMS_SUPPORT)
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->current.I_rms_scale_factor = val;
#endif
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->current.V_fir_coeff = val;
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->current.phase_correction = val;
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->current.P_scale_factor = val;
#if defined(NEUTRAL_MONITOR_SUPPORT)
#if defined(IRMS_SUPPORT)
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->neutral.I_rms_scale_factor = val;
#endif
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->neutral.V_fir_coeff = val;
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->neutral.phase_correction = val;
        if (fgets(buf, 80, parms) == NULL)
            return  FALSE;
        if (sscanf(buf, "%d", &val) != 1)
            return  FALSE;
        phase_nv->neutral.P_scale_factor = val;
#endif
#if !defined(SINGLE_PHASE)
        phase++;
        phase_nv++;
        phase_cal++;
    }
#endif
#if defined(TEMPERATURE_SUPPORT)
    if (fgets(buf, 80, parms) == NULL)
        return  FALSE;
    if (sscanf(buf, "%d", &val) != 1)
        return  FALSE;
    nv->temperature_at_calibration = val;
#endif
    printf("Calibration settings loaded\n");

    return  TRUE;
}
#endif

int host_store_parms(struct nv_parms_s *nv)
{
    return FALSE;
}

void custom_2second_handler(void)
{
    uint8_t buf[2];
    int32_t next_total_power;

    printf("%s: V.V %04x%04x%04x  I.I %04x%04x%04x  I.I %04x%04x%04x  V.I %04x%04x%04x  V.I %04x%04x%04x %4d\n",
           (skip_report)  ?  "Skip-dots"  :  "Dots",
           phase->V_sq_accum_logged[2] & 0xFFFF, phase->V_sq_accum_logged[1] & 0xFFFF, phase->V_sq_accum_logged[0] & 0xFFFF,
           phase->current.I_sq_accum_logged[2] & 0xFFFF, phase->current.I_sq_accum_logged[1] & 0xFFFF, phase->current.I_sq_accum_logged[0] & 0xFFFF,
           phase->neutral.I_sq_accum_logged[2] & 0xFFFF, phase->neutral.I_sq_accum_logged[1] & 0xFFFF, phase->neutral.I_sq_accum_logged[0] & 0xFFFF,
           phase->current.P_accum_logged[2] & 0xFFFF, phase->current.P_accum_logged[1] & 0xFFFF, phase->current.P_accum_logged[0] & 0xFFFF,
           phase->neutral.P_accum_logged[2] & 0xFFFF, phase->neutral.P_accum_logged[1] & 0xFFFF, phase->neutral.P_accum_logged[0] & 0xFFFF,
           phase->sample_count_logged);
    printf("%s: %10ld %10ld %10ld\n",
           (skip_report)  ?  "Skip-dcs"  :  "DCs",
           phase->V_dc_estimate[0],
           phase->current.I_dc_estimate,
           phase->neutral.I_dc_estimate);
    printf("%s: %7.2fV %8.3fA %8.2fW %8.2fW %8.2fW %7.4fPF %6.2fHz\n",
           (skip_report)  ?  "Skip-readings"  :  "Readings",
           phase->V_rms/100.0,
           phase->I_rms/1000.0,
           phase->active_power/100.0,
           phase->reactive_power/100.0,
           phase->apparent_power/100.0,
           phase->power_factor/10000.0,
           phase->frequency/100.0);
    if (skip_report)
    {
        printf("Skipped report\n");
        fprintf(stderr, "Skipped report\n");
        skip_report = FALSE;
    }
    else
    {
        next_total_power = total_power;
        buf[0] = ((next_total_power >> 30) & 0x03) | 0x80;
        buf[1] = (next_total_power >> 24) & 0x3F;
        buf[2] = (next_total_power >> 18) & 0x3F;
        buf[3] = (next_total_power >> 12) & 0x3F;
        buf[4] = (next_total_power >> 6) & 0x3F;
        buf[5] = next_total_power & 0x3F;
        serial_write(display_serial_port, buf, 6);
    }
}

static uint8_t EEPROM_sim[8192];

int iicEEPROM_read(uint16_t addr, void *dat, int len)
{
    memcpy(dat, &EEPROM_sim[addr], len);
    return FALSE;
}

int iicEEPROM_write(uint16_t addr, void *dat, int len)
{
    memcpy(&EEPROM_sim[addr], dat, len);
    return TRUE;
}

int iicEEPROM_init(void)
{
    return 0;
}

void flash_write_int16(int16_t *ptr, int16_t value)
{
}

void flash_secure(void)
{
}
#endif

#endif
