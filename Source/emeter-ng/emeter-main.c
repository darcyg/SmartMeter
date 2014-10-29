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
//  File: emeter-main.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-main.c,v 1.10 2009/04/27 06:21:22 a0754793 Exp $
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

#if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
typedef union
{
    unsigned char uint8[4];
    unsigned int uint16[2];
    uint32_t uint32;
} power_array;


uint8_t total_active_energy_pulse_remaining_time;
int32_t total_active_power;
power_array    total_active_power_array;
int32_t total_active_power_counter;
    #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
int16_t extra_total_active_power_counter;
    #endif
uint32_t total_consumed_active_energy;
#endif

#if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
uint8_t total_reactive_energy_pulse_remaining_time;
int32_t total_reactive_power;
int32_t total_reactive_power_counter;
    #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
int16_t extra_total_reactive_power_counter;
    #endif
uint32_t total_consumed_reactive_energy;
#endif

#if 0 //CUSTOM_LCD_SUPPORT
/* Keep the toolkit library happy */
const int lcd_cells = LCD_CELLS;
const int lcd_pos_base = LCD_POS_BASE;
const int lcd_pos_step = LCD_POS_STEP;
#endif

#if defined(TEMPERATURE_SUPPORT)
uint16_t temperature;
#endif

#if defined(VCC_MEASURE_SUPPORT)
uint16_t vcc;
#endif

//MM start
long long temp=0, temp2=0;
static int thou,hun,ten,unit,ten_thou, thou_thou, hun_thou;
int lcd_display_mode=-2, change_display=0, LCDM3constant=0;
volatile unsigned int LCD_reg; 
//mm 
enum display_LCD
{
   DISPLAY_VOLTAGE                         = 0,     
   DISPLAY_CURRENT                         = 2,    
   DISPLAY_ACTIVE_POWER                    = 4,
   DISPLAY_REACTIVE_POWER                    = 6,
   DISPLAY_APPARENT_POWER                    = 8,
      DISPLAY_FREQUENCY                       = 10,
   DISPLAY_POWER_FACTOR                    = 12,
      DISPLAY_ACCUMULATED_POWER_TOTAL         = 14

};
 const unsigned char LCD_Char_Map[] =
{
    BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7,                // '0' or 'O'
    BIT5 | BIT6,                                            // '1' or 'I'
    BIT0 | BIT1 | BIT3 | BIT4 | BIT6 | BIT7,                // '2' or 'Z'
    BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7,                // '3'
    BIT0 | BIT1 | BIT2 | BIT5 | BIT6,                       // '4' or 'y'
    BIT0 | BIT1 | BIT2 | BIT4 | BIT5 | BIT7,                // '5' or 'S'
    BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT7,         // '6' or 'b'
    BIT5 | BIT6 | BIT7,                                     // '7'
    BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7,  // '8' or 'B'
    BIT0 | BIT1 | BIT2 | BIT4 | BIT5 | BIT6 | BIT7,         // '9' or 'g'
};
//MM end

/* Meter status flag bits. */
uint16_t meter_status;

/* Current operating mode - normal, limp, power down, etc. */
int8_t operating_mode;

/* Persistence check counters for anti-tamper measures. */
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
int8_t current_reversed;
#endif
#if defined(POWER_BALANCE_DETECTION_SUPPORT)
int8_t current_unbalanced;
#endif
#if defined(MAGNETIC_INTERFERENCE_SUPPORT)
int8_t magnetic_interference_persistence;
#endif

#if defined(IHD430_SUPPORT)
unsigned char RF_Tx[17]={0xFE,0x0C,0x29,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x05,0x01};
#endif 

/* The main per-phase working parameter structure */
#if !defined(SINGLE_PHASE)
struct phase_parms_s chan[NUM_PHASES];
    #if defined(NEUTRAL_MONITOR_SUPPORT)
struct neutral_parms_s neutral;
    #endif
#else
struct phase_parms_s chan1;
#endif

/* The main per-phase non-volatile parameter structure */
__infomem__ const struct info_mem_s nv_parms =
{
    {
    {
        0xFFFF,
#if defined(SELF_TEST_SUPPORT)
        0xFFFF,
        0xFFFF,
#endif
#if !defined(SINGLE_PHASE)
        {
#endif
            {
                {
#if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_A},
    #if defined(LIMP_MODE_SUPPORT)
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR_A},
    #endif
                    0,
#endif
#if GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_A_LOW,
#else
                    {DEFAULT_P_SCALE_FACTOR_A_LOW, DEFAULT_P_SCALE_FACTOR_A_HIGH},
#endif
                    DEFAULT_I_DC_ESTIMATE << 16,
#if defined(PHASE_CORRECTION_SUPPORT)
    #if GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_A_CORRECTION_LOW,
    #else
                    {DEFAULT_BASE_PHASE_A_CORRECTION_LOW, DEFAULT_BASE_PHASE_A_CORRECTION_HIGH},
    #endif
#endif
                },
#if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_A,
    #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR_A,
    #endif
#endif
                DEFAULT_V_DC_ESTIMATE << 16,
#if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
#endif
            },
#if !defined(SINGLE_PHASE)
            {
                {
    #if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_B},
        #if defined(LIMP_MODE_SUPPORT)
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR},
        #endif
                    0,
    #endif
    #if GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_B_LOW,
    #else
                    {DEFAULT_P_SCALE_FACTOR_B_LOW, DEFAULT_P_SCALE_FACTOR_B_HIGH},
    #endif
                    DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
        #if GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_B_CORRECTION_LOW,
        #else
                    {DEFAULT_BASE_PHASE_B_CORRECTION_LOW, DEFAULT_BASE_PHASE_B_CORRECTION_HIGH},
        #endif
    #endif
                },
    #if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_B,
        #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR,
        #endif
    #endif
                DEFAULT_V_DC_ESTIMATE << 16,
    #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
    #endif
            },
            {
                {
    #if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_C},
        #if defined(LIMP_MODE_SUPPORT)
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR},
        #endif
                    0,
    #endif
    #if GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_C_LOW,
    #else
                    {DEFAULT_P_SCALE_FACTOR_C_LOW, DEFAULT_P_SCALE_FACTOR_C_HIGH},
    #endif
                    DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
        #if GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_C_CORRECTION_LOW,
        #else
                    {DEFAULT_BASE_PHASE_C_CORRECTION_LOW, DEFAULT_BASE_PHASE_C_CORRECTION_HIGH},
        #endif
    #endif
                },
    #if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_C,
        #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR,
        #endif
    #endif
                DEFAULT_V_DC_ESTIMATE << 16,
    #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
    #endif
            }
        },
#endif
#if defined(NEUTRAL_MONITOR_SUPPORT)
        {
    #if defined(IRMS_SUPPORT)
            DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL,
        #if defined(LIMP_MODE_SUPPORT)
            DEFAULT_I_RMS_LIMP_SCALE_FACTOR_NEUTRAL,
        #endif
            0,
    #endif
            DEFAULT_P_SCALE_FACTOR_NEUTRAL,
            DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
            DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION,
    #endif
        },
#endif
#if defined(TEMPERATURE_SUPPORT)
        25,
        DEFAULT_TEMPERATURE_OFFSET,
        DEFAULT_TEMPERATURE_SCALING,
#endif
#if defined(CORRECTED_RTC_SUPPORT)
        0,
#endif
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

#if defined(SELF_TEST_SUPPORT)
int record_meter_failure(int type)
{
    /* The error type should be a value between 0 and 15, specifying the unrecoverable error
       type to be recorded in the failures word in flash. */
    /* Don't worry about the time taken to write to flash - we are recording a serious
       error condition! */
    flash_write_int16((int *) &(nv_parms.seg_a.s.meter_failures), nv_parms.seg_a.s.meter_failures & ~(1 << type));
    flash_secure();
    return TRUE;
}

int record_meter_warning(int type)
{
    /* The warning type should be a value between 0 and 15, specifying the warning type to be
       recorded in the recoverable failures word in flash. */
    /* Don't worry about the time taken to write to flash - we are recording a serious
       problem! */
    flash_write_int16((int *) &(nv_parms.seg_a.s.meter_warnings), nv_parms.seg_a.s.meter_warnings & ~(1 << type));
    flash_secure();
    return TRUE;
}
#endif

#if defined(BATTERY_MONITOR_SUPPORT)
void test_battery(void)
{
    P3DIR |= (BIT1);
    P3OUT &= ~(BIT1);
    battery_countdown = 1000;
}
#endif

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

#if defined(__AQCOMPILER__)  ||  defined(__IAR_SYSTEMS_ICC__)
void main(void)
#else
int main(int argc, char *argv[])
#endif
{
#if !defined(SINGLE_PHASE)
    int ch;
    static struct phase_parms_s *phase;
    static struct phase_nv_parms_s const *phase_nv;
#endif
    static int32_t x;
    int i;

#if !defined(__MSP430__)
    if (start_host_environment(argc, argv) < 0)
        exit(2);
#endif
    system_setup();

#if !defined(ESP_SUPPORT)  &&  defined(PHASE_CORRECTION_SUPPORT)  &&  !defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
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
    #else
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
#endif

#if defined(ESP_SUPPORT)
    esp_init();
    esp_start_measurement();
#endif

#if defined(MULTI_RATE_SUPPORT)
    tariff_initialise();
#endif
    send_message(0, 1);

    for (;;)
    {
        kick_watchdog();
#if !defined(__MSP430__)
        /* In the host environment we need to simulate interrupts here */
        adc_interrupt();
#endif
#if !defined(SINGLE_PHASE)
        phase = chan;
        phase_nv = nv_parms.seg_a.s.chan;

        for (ch = 0;  ch < NUM_PHASES;  ch++)
        {
#endif
            /* Unless we are in normal operating mode, we should wait to be
               woken by a significant event from the interrupt routines. */
////#if  1 //// 0 //defined(__MSP430__)
////            if (operating_mode != OPERATING_MODE_NORMAL)
#ifdef USE_LPM
                _BIS_SR(LPM0_bits);
#endif
#if defined(POWER_DOWN_SUPPORT)
            if (operating_mode == OPERATING_MODE_POWERFAIL)
                switch_to_powerfail_mode();
#endif
#if defined(LIMP_MODE_SUPPORT)  &&  defined(IEC1107_SUPPORT)
            if (nv_parms.seg_a.s.meter_uncalibrated)
                enable_ir_receiver();
#endif
            if ((phase->status & NEW_LOG))
            {
                /* The background activity has informed us that it is time to
                   perform a block processing operation. */
                phase->status &= ~NEW_LOG;
#if defined(MAGNETIC_INTERFERENCE_SUPPORT)
                if ((meter_status & STATUS_HIGH_MAGNETIC_FIELD))
                {
                    /* The meter is suffering magnetic tampering, so continuously
                       charge for a great deal of electricity. */
                    x = phase->readings.V_rms*MAGNETIC_INTERFERENCE_CURRENT/(10*100);
                }
                else
#endif
             
          
          
  //MM Added for LCD Fix; Start     
         
    
    
     #if defined(IHD430_SUPPORT)
         
               RF_Tx[12]=total_active_power_array.uint8[0];
               RF_Tx[13]=total_active_power_array.uint8[1];
               RF_Tx[14]=total_active_power_array.uint8[2];
               RF_Tx[15]=total_active_power_array.uint8[3]; 
               RF_Tx[16] =0x28 ^ RF_Tx[12] ^ RF_Tx[13] ^ RF_Tx[14] ^ RF_Tx[15];
               for (i=0; i<17; i++)
               {
                  UCA2TXBUF=RF_Tx[i];
                  while(!(UCA2IFG&UCTXIFG));
               }               
         #endif
    lcd_display_mode++;
     switch( lcd_display_mode ) 
              {
                  case DISPLAY_VOLTAGE:
                      change_display=1;
                      temp= phase->readings.V_rms;
                       if (temp <0) temp=0;
                       LCDM1=0xC;
                       LCDM2=0x29;
                      break;
                  case DISPLAY_CURRENT:
                       change_display=1;
                       temp= phase->readings.I_rms;
                       if (temp <0) temp=0;
                       LCDM1=0x9C;
                       LCDM2=0x1;
                       //MM Take care of different decimal.
                      break;
                  case DISPLAY_ACTIVE_POWER:
                     change_display=1;
                      temp= phase->readings.active_power;
                      if (temp <0) temp=0;
                      LCDM1=0xCF;
                       LCDM2=0x1;
                      break;
                  case DISPLAY_REACTIVE_POWER:
                      change_display=1;
                       temp= phase->readings.reactive_power;
                       if (temp <0) temp=0;
                       LCDM1=0xC7;
                       LCDM2=0x3;
                      break;
                  case DISPLAY_APPARENT_POWER:
                      change_display=1;
                       temp= phase->readings.apparent_power;
                       if (temp <0) temp=0;
                       LCDM1=0xB7;
                       LCDM2=0x1;
                      break;
                  case DISPLAY_FREQUENCY:
                      change_display=1;
                       temp= phase->readings.frequency;
                       if (temp <0) temp=0;
                       LCDM1=0x8F;
                       LCDM2=0x1;
                      break;  
                  case DISPLAY_POWER_FACTOR:
                      change_display=1;
                       temp= phase->readings.power_factor/10;
                       LCDM1=0x8F;
                       LCDM2=0x50;
                       if (temp < 0)
                       {
                          temp*= -1;
                         LCDM3=0x1C;
                       }
                       else
                       {
                          LCDM3= 0x9C; 
                       }
                       LCDM3constant=1;
                       //Take negative and positive case for inductive and capacitive
                      break;                  
                 case    DISPLAY_ACCUMULATED_POWER_TOTAL: 
                      change_display=1;
                      temp= phase->consumed_active_energy/10;
                      LCDM1=0x9F;
                       LCDM2=0x1;
                      break; 
              default:
                change_display=0;
                break;
                
                                     
            }
     //temp=12345;
         //total_active_power=0x12345;
// MM    Comment back in to display active power
//            if (total_active_power <0)
//            temp=-(total_active_power);
//          else
//            temp=total_active_power;
    
//MM Comment in two lines below  for counter test         //temp=12345;
//          temp2++;
//          temp=temp2;
         
            if(change_display)
            {
//               LCDM1 = 0;
//          LCDM2 = 0;          
          
             if(!LCDM3constant)
             {
               LCDM3 = 0;             
             }
             else
             {
               LCDM3constant=0;
             }
               
          LCDM4 = 0;
          LCDM5 = 0;
          LCDM6 = 0;
          LCDM7 = 0;
          LCDM8 = 0;
          LCDM9 = 0;
          LCDM10 = 0;
          LCDM11 = 0;
          LCDM12 = 0;          
          LCDM13 = 0;
          LCDM14 = 0;
          LCDM15 = 0;
          LCDM16 = 0;
          LCDM17 = 0;
          LCDM18 = 0;
          LCDM19 = 0;
          LCDM20 = 0;
          
         
       thou_thou=0;
         hun_thou=0;       
        ten_thou=0;
    thou=0;
    hun=0;
    ten=0;
    unit=0; 
                while (temp >=1000000)
                {
                    thou_thou++;
                    temp-=1000000;
                }
                 while (temp >=100000)
                {
                    hun_thou++;
                    temp-=100000;
                }
                while (temp >=10000)
                {
                    ten_thou++;
                    temp-=10000;
                }
                while (temp >=1000)
                {
                    thou++;
                    temp-=1000;
                }
                while (temp >=100)
                {
                    hun++;
                    temp-=100;
                }
                while (temp >=10)
                {
                    ten++;
                    temp-=10;
                }
                while (temp >=1)
                {
                    unit++;
                    temp--;
                }
               
                //MM: LCD fix to display higher active power readings
                if(thou_thou)
                {
                  LCDM11 = LCD_Char_Map[hun];
                  LCDM9 = LCD_Char_Map[thou];
                  //LCDM8 = 0x1;
                  LCDM7 = LCD_Char_Map[ten_thou];
                  LCDM5 = LCD_Char_Map[hun_thou];
                  LCDM3 = LCD_Char_Map[thou_thou];          
                }
                else if(hun_thou)
                {        
                  
                  LCDM11 = LCD_Char_Map[ten];
                  if (lcd_display_mode==DISPLAY_CURRENT) LCDM8 = 0x1;      
                  else if(lcd_display_mode!=DISPLAY_ACCUMULATED_POWER_TOTAL) LCDM10 = 0x1; //MM do nothing    
                  LCDM9 = LCD_Char_Map[hun];
                  LCDM7 = LCD_Char_Map[thou];
                  LCDM5 = LCD_Char_Map[ten_thou];
                  LCDM3 = LCD_Char_Map[hun_thou]; 
                }
                else
                {
                  LCDM11 = LCD_Char_Map[unit];
                  LCDM9 = LCD_Char_Map[ten];
                  if (lcd_display_mode==DISPLAY_CURRENT || lcd_display_mode==DISPLAY_POWER_FACTOR) 
                  {
                    LCDM6 = 0x1;
                  }
                  else if(lcd_display_mode==DISPLAY_ACCUMULATED_POWER_TOTAL) 
                  {
                    
                    LCDM10 = 0x1;
                      
                  }
                  else
                  {
                                       LCDM8 = 0x1; 
                  }
                     
                  if (lcd_display_mode!=DISPLAY_POWER_FACTOR) 
                     {
                       
                       LCDM3 = LCD_Char_Map[ten_thou];
 
                     }

                  LCDM7 = LCD_Char_Map[hun];
                  LCDM5 = LCD_Char_Map[thou];
                 
                }
                if (lcd_display_mode==DISPLAY_ACCUMULATED_POWER_TOTAL)
                {
                    lcd_display_mode=-2; 
                }
               
            }  
            
          //MM end        
                  
                if (operating_mode == OPERATING_MODE_NORMAL)
                {
                    /* We can only do real power assessment in full operating mode */
#if !defined(SINGLE_PHASE)
                    x = active_power(phase, phase_nv);
    #if defined(PRECALCULATED_PARAMETER_SUPPORT)
        #if defined(IRMS_SUPPORT)
                    phase->readings.I_rms = current(phase, phase_nv, ch);
        #endif
        #if defined(VRMS_SUPPORT)
                    phase->readings.V_rms = voltage(phase, phase_nv);
        #endif
    #endif
#else
                    x = active_power();
                    
    #if defined(PRECALCULATED_PARAMETER_SUPPORT)
        #if defined(IRMS_SUPPORT)
                    phase->readings.I_rms = current();
        #endif
        #if defined(VRMS_SUPPORT)
                    phase->readings.V_rms = voltage();
        #endif
    #endif
#endif
                }
#if defined(LIMP_MODE_SUPPORT)
                else if (operating_mode == OPERATING_MODE_LIMP)
                {
                    /* In limp mode we must assess estimated power from only the measured current. */
                    /* We cannot properly determine current reversal in this mode. Also, current
                       imbalance is really just a measure of which lead is still connected.
                       Just treat both the imbalance and reversal conditions as OK */
    #if !defined(SINGLE_PHASE)
                    x = current(phase, phase_nv, ch);
        #if defined(PRECALCULATED_PARAMETER_SUPPORT)  &&  defined(VRMS_SUPPORT)
                    phase->readings.V_rms = voltage(phase, phase_nv);
        #endif
    #else
                    x = current();
        #if defined(PRECALCULATED_PARAMETER_SUPPORT)  &&  defined(VRMS_SUPPORT)
                    phase->readings.V_rms = voltage();
        #endif
    #endif
    #if defined(PRECALCULATED_PARAMETER_SUPPORT)  &&  defined(IRMS_SUPPORT)
                    phase->readings.I_rms = x;
    #endif
                    x = x*MAINS_NOMINAL_VOLTAGE/10;
                }
#endif
                if (labs(x) < RESIDUAL_POWER_CUTOFF  ||  (phase->status & V_OVERRANGE))
                {
                    x = 0;
#if defined(PRECALCULATED_PARAMETER_SUPPORT)  &&  defined(IRMS_SUPPORT)
                    /* Avoid displaying a residual current, which is nothing more
                       than integrated noise. */
                    //phase->I_rms = 0;
#endif
                    /* Turn off the LEDs, regardless of the internal state of the
                       reverse and imbalance assessments. */
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
                    meter_status &= ~STATUS_REVERSED;
                    clr_reverse_current_indicator();
#endif
#if defined(POWER_BALANCE_DETECTION_SUPPORT)
                    meter_status &= ~STATUS_EARTHED;
                    clr_earthed_indicator();
#endif
                }
                else
                {
                    if (operating_mode == OPERATING_MODE_NORMAL)
                    {
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)  &&  defined(PHASE_REVERSED_IS_TAMPERING)
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
#endif
#if defined(POWER_BALANCE_DETECTION_SUPPORT)
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
#endif
                    }
#if defined(LIMP_MODE_SUPPORT)
                    else
                    {
    #if defined(PHASE_REVERSED_DETECTION_SUPPORT)
                        /* We cannot tell forward from reverse current in limp mode,
                           so just say it is not reversed. */
                        meter_status &= ~STATUS_REVERSED;
                        clr_reverse_current_indicator();
    #endif
    #if defined(POWER_BALANCE_DETECTION_SUPPORT)
                        /* We are definitely in the unbalanced state, but only set
                           the indicator if we have persistence checked, and the current
                           is sufficient to sustain operation. */
                        if ((phase->status & PHASE_UNBALANCED)  &&  phase->readings.I_rms >= LIMP_MODE_MINIMUM_CURRENT)
                        {
                            meter_status |= STATUS_EARTHED;
                            set_earthed_indicator();
                        }
                        else
                        {
                            meter_status &= ~STATUS_EARTHED;
                            clr_earthed_indicator();
                        }
    #endif
                        /* Only run the IR interface if we are sure there is enough power from the
                           supply to support the additional current drain. If we have not yet been
                           calibrated we had better keep the IR port running so we can complete the
                           calibration. */
#if defined(LIMP_MODE_SUPPORT)  &&  defined(IEC1107_SUPPORT)
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
#endif
                    }
#endif
                }
                //x /= 10;
#if defined(SINGLE_PHASE)  &&  defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
                total_active_power = x;
#else
                total_active_power += (x - phase->readings.active_power);
#endif
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)  &&  defined(PHASE_REVERSED_IS_GENERATION)
#endif
               #if defined(IHD430_SUPPORT)
                total_active_power_array.uint32=total_active_power;

                #endif
                phase->readings.active_power = x;
#if defined(PRECALCULATED_PARAMETER_SUPPORT)
    #if defined(REACTIVE_POWER_SUPPORT)
        #if defined(SINGLE_PHASE)
                x = reactive_power();
        #else
                x = reactive_power(phase, phase_nv);
        #endif
        #if defined(SINGLE_PHASE)  &&  defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
                total_reactive_power = x;
        #else
                total_reactive_power += (x - phase->readings.reactive_power);
        #endif
                phase->readings.reactive_power = x;
    #endif
    #if defined(APPARENT_POWER_SUPPORT)
        #if defined(SINGLE_PHASE)
                phase->readings.apparent_power = apparent_power();
        #else
                phase->readings.apparent_power = apparent_power(phase, phase_nv);
        #endif
    #endif
    #if defined(POWER_FACTOR_SUPPORT)
                /* The power factor should be calculated last */
        #if defined(SINGLE_PHASE)
                phase->readings.power_factor = power_factor();
        #else
                phase->readings.power_factor = power_factor(phase, phase_nv);
        #endif
    #endif
#endif
#if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
//                phase->active_energy_counter += x*phase->metrology.dot_prod_logged.sample_count;
//                while (phase->active_energy_counter > ENERGY_WATT_HOUR_THRESHOLD)
//                {
//                    phase->active_energy_counter -= ENERGY_WATT_HOUR_THRESHOLD;
//                    phase->consumed_active_energy++;
//                }
#endif
#if defined(PRECALCULATED_PARAMETER_SUPPORT)  && defined(MAINS_FREQUENCY_SUPPORT)
    #if defined(SINGLE_PHASE)
                phase->readings.frequency = frequency();
    #else
                phase->readings.frequency = frequency(phase, phase_nv);
    #endif
#endif
#if defined(MAGNETIC_INTERFERENCE_SUPPORT)
    #if !defined(SINGLE_PHASE)
                if (ch == 0)
    #endif
                {
                    if ((meter_status & STATUS_HIGH_MAGNETIC_FIELD))
                    {
                        if (phase->sample_count_logged/magnetic_sensor_count_logged < MAGNETIC_INTERFERENCE_SAMPLE_RATIO)
                        {
                            if (--magnetic_interference_persistence <= -MAGNETIC_INTERFERENCE_PERSISTENCE_CHECK)
                            {
                                meter_status &= ~STATUS_HIGH_MAGNETIC_FIELD;
                                magnetic_interference_persistence = 0;
                            }
                        }
                        else
                        {
                            magnetic_interference_persistence = 0;
                        }
                    }
                    else
                    {
                        if (phase->sample_count_logged/magnetic_sensor_count_logged >= MAGNETIC_INTERFERENCE_SAMPLE_RATIO)
                        {
                            if (++magnetic_interference_persistence >= MAGNETIC_INTERFERENCE_PERSISTENCE_CHECK)
                            {
                                meter_status |= STATUS_HIGH_MAGNETIC_FIELD;
                                magnetic_interference_persistence = 0;
                            }
                        }
                        else
                        {
                            magnetic_interference_persistence = 0;
                        }
                    }
                }
#endif
            }
#if defined(LIMP_MODE_SUPPORT)
            /* The voltage channel DC estimate will never move very much when the
               meter is operating normally. If it does move, there must be some
               tampering, such as a diode between the grid and the meter. */
            if (operating_mode == OPERATING_MODE_NORMAL)
            {
                if (phase->readings.V_rms < LIMP_MODE_VOLTAGE_THRESHOLD*100
                    ||
                    phase->metrology.V_dc_estimate[0] > UPPER_TAMPER_V_DC_ESTIMATE
                    ||
                    phase->metrology.V_dc_estimate[0] < LOWER_TAMPER_V_DC_ESTIMATE)
                {
                    switch_to_limp_mode();
                }
            }
            else if (operating_mode == OPERATING_MODE_LIMP)
            {
                if (phase->readings.V_rms >= NORMAL_MODE_VOLTAGE_THRESHOLD*100
                    &&
                    phase->metrology.V_dc_estimate[1] <= UPPER_LIMP_TAMPER_V_DC_ESTIMATE
                    &&
                    phase->metrology.V_dc_estimate[1] >= LOWER_LIMP_TAMPER_V_DC_ESTIMATE)
                {
                    /* The LCD might need to be revived */
    #if defined(__MSP430__)
                    LCDawaken();
    #else
                    /* Tell the world we are ready to start */
    #endif
                    switch_to_normal_mode();
                }
            }
#endif

#if !defined(SINGLE_PHASE)
            phase++;
            phase_nv++;
        }
#endif
#if !defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)  &&  defined(IRMS_SUPPORT)
        if ((neutral.status & NEW_LOG))
        {
            /* The background activity has informed us that it is time to
               perform a block processing operation. */
            neutral.status &= ~NEW_LOG;
            neutral.readings.I_rms = neutral_current();
        }
#endif

#if defined(MULTI_RATE_SUPPORT)
        tariff_management();
#endif

        /* Do display and other housekeeping here */
        if ((meter_status & TICKER))
        {
            /* Two seconds have passed */
            /* We have a 2 second tick */
            meter_status &= ~TICKER;
#if defined(__MSP430__)  &&  defined(BASIC_LCD_SUPPORT)
            /* Update the display, cycling through the phases */
            update_display();
#endif
            custom_2second_handler();
#if (defined(RTC_SUPPORT)  ||  defined(CUSTOM_RTC_SUPPORT))  &&  defined(CORRECTED_RTC_SUPPORT)
            correct_rtc();
#endif
        }
        custom_keypad_handler();
        custom_mainloop_handler();
#if defined(MESH_NET_SUPPORT)
        if (rf_service)
        {
            rf_service = 0;
            rf_tick_service();
        }
#endif
    }
#if !defined(__AQCOMPILER__)  &&  !defined(__IAR_SYSTEMS_ICC__)
    return  0;
#endif
}

#if defined(PRECALCULATED_PARAMETER_SUPPORT)
int32_t current_consumed_active_energy(int ph)
{
#if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
    if (ph == FAKE_PHASE_TOTAL)
        return total_consumed_active_energy;
#endif
#if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
#if defined(SINGLE_PHASE)
    return chan1.consumed_active_energy;
#else
    return chan[ph].consumed_active_energy;
#endif
#else
    return 0;
#endif
}

int32_t current_active_power(int ph)
{
    if (ph == FAKE_PHASE_TOTAL)
        return total_active_power;
#if defined(SINGLE_PHASE)
    return chan1.readings.active_power;
#else
    return chan[ph].readings.active_power;
#endif
}

#if defined(REACTIVE_POWER_SUPPORT)
int32_t current_consumed_reactive_energy(int ph)
{
#if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
    if (ph == FAKE_PHASE_TOTAL)
        return total_consumed_reactive_energy;
#endif
#if defined(PER_PHASE_REACTIVE_ENERGY_SUPPORT)
#if defined(SINGLE_PHASE)
    return chan1.consumed_reactive_energy;
#else
    return chan[ph].consumed_reactive_energy;
#endif
#else
    return 0;
#endif
}

int32_t current_reactive_power(int ph)
{
    if (ph == FAKE_PHASE_TOTAL)
        return total_reactive_power;
#if defined(SINGLE_PHASE)
    return chan1.readings.reactive_power;
#else
    return chan[ph].readings.reactive_power;
#endif
}
#endif

#if defined(APPARENT_POWER_SUPPORT)
int32_t current_apparent_power(int ph)
{
#if defined(SINGLE_PHASE)
    return chan1.readings.apparent_power;
#else
    return chan[ph].readings.apparent_power;
#endif
}
#endif

#if defined(POWER_FACTOR_SUPPORT)
int32_t current_power_factor(int ph)
{
#if defined(SINGLE_PHASE)
    return chan1.readings.power_factor;
#else
    return chan[ph].readings.power_factor;
#endif
}
#endif

#if defined(VRMS_SUPPORT)
int32_t current_rms_voltage(int ph)
{
    int32_t x;

#if defined(SINGLE_PHASE)
    if (chan1.readings.V_rms == 0xFFFF)
        x = -1;
    else
        x = chan1.readings.V_rms;
#else
    if (chan[ph].readings.V_rms == 0xFFFF)
        x = -1;
    else
        x = chan[ph].readings.V_rms;
#endif
    return x;
}
#endif

#if defined(IRMS_SUPPORT)
int32_t current_rms_current(int ph)
{
    int32_t x;

#if !defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    if (ph == 3)
        x = neutral.readings.I_rms;
    else
#endif
#if defined(SINGLE_PHASE)
        x = chan1.readings.I_rms;
#else
        x = chan[ph].readings.I_rms;
#endif
    if (x == 0xFFFF)
        x = -1;
    return x;
}
#endif

#if defined(MAINS_FREQUENCY_SUPPORT)
int32_t current_mains_frequency(int ph)
{
#if defined(SINGLE_PHASE)
     return chan1.readings.frequency;
#else
     return chan[ph].readings.frequency;
#endif
}
#endif
#else
int32_t current_consumed_active_energy(int ph)
{
    retirm chan[ph].consumed_active_energy;
}

int32_t current_active_power(int ph)
{
    if (ph == FAKE_PHASE_TOTAL)
        return total_active_power;
    return active_power(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}

#if defined(REACTIVE_POWER_SUPPORT)
int32_t current_consumed_reactive_energy(int ph)
{
    retirm chan[ph].consumed_active_energy;
}

int32_t current_reactive_power(int ph)
{
    if (ph == FAKE_PHASE_TOTAL)
        return total_reactive_power;
    return reactive_power(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}
#endif

#if defined(APPARENT_POWER_SUPPORT)
int32_t current_apparent_power(int ph)
{
    return apparent_power(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}
#endif

#if defined(POWER_FACTOR_SUPPORT)
int32_t current_power_factor(int ph)
{
    return power_factor(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}
#endif

#if defined(VRMS_SUPPORT)
int32_t current_rms_voltage(int ph)
{
    return voltage(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}
#endif

#if defined(IRMS_SUPPORT)
int32_t current_rms_current(int ph)
{
    return current(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}
#endif

#if defined(MAINS_FREQUENCY_SUPPORT)
int32_t current_mains_frequency(int ph)
{
    return frequency(&chan[ph], &nv_parms.seg_a.s.chan[ch]);
}
#endif
#endif
