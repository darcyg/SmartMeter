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
//  File: emeter-dlt645.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-dlt645.c,v 1.13 2009/04/27 09:46:31 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
#include <stdint.h>
#if !defined(__MSP430__)
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#endif
#if defined(__GNUC__)
#include <signal.h>
#endif
#if defined(__MSP430__)
#include <io.h>
#endif

#include <emeter-toolkit.h>

#include "emeter-structs.h"

#if !defined(NULL)
#define NULL    (void *) 0
#endif

enum
{
    MEASURES_ACTIVE_POWER                       = 0x01,
    MEASURES_TRIGONOMETRIC_REACTIVE_POWER       = 0x02,
    MEASURES_APPARENT_POWER                     = 0x04,
    MEASURES_VRMS                               = 0x08,
    MEASURES_IRMS                               = 0x10,
    MEASURES_POWER_FACTOR                       = 0x20,
    MEASURES_MAINS_FREQUENCY                    = 0x40,
    MEASURES_QUADRATURE_REACTIVE_POWER          = 0x80
};

enum host_commands_e
{
    HOST_CMD_GET_METER_CONFIGURATION            = 0x56,
    HOST_CMD_SET_METER_CONSUMPTION              = 0x57,
    HOST_CMD_SET_RTC                            = 0x58,
    HOST_CMD_GET_RTC                            = 0x59,
    HOST_CMD_ALIGN_WITH_CALIBRATION_FACTORS     = 0x5A,
    HOST_CMD_SET_PASSWORD                       = 0x60,
    HOST_CMD_GET_READINGS_PHASE_1               = 0x61,
    HOST_CMD_GET_READINGS_PHASE_2               = 0x62,
    HOST_CMD_GET_READINGS_PHASE_3               = 0x63,
    HOST_CMD_GET_READINGS_NEUTRAL               = 0x64,
    HOST_CMD_ERASE_FLASH_SEGMENT                = 0x70,
    HOST_CMD_SET_FLASH_POINTER                  = 0x71,
    HOST_CMD_FLASH_DOWNLOAD                     = 0x72,
    HOST_CMD_FLASH_UPLOAD                       = 0x73,
    HOST_CMD_ZAP_MEMORY_AREA                    = 0x74,
    HOST_CMD_SUMCHECK_MEMORY                    = 0x75,
    HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_1       = 0x91,
    HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_2       = 0x92,
    HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_3       = 0x93,
    HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_1     = 0x95,
    HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_2     = 0x96,
    HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_3     = 0x97,
    HOST_CMD_GET_RAW_ACTIVE_POWER_NEUTRAL       = 0x99,
    HOST_CMD_GET_RAW_REACTIVE_POWER_NEUTRAL     = 0x9D,
    HOST_CMD_CHECK_RTC_ERROR                    = 0xA0,
    HOST_CMD_RTC_CORRECTION                     = 0xA1,
    HOST_CMD_MULTIRATE_SET_PARAMETERS           = 0xC0,
    HOST_CMD_MULTIRATE_GET_PARAMETERS           = 0xC1,
    HOST_CMD_MULTIRATE_CLEAR_USAGE              = 0xC2,
    HOST_CMD_MULTIRATE_GET_USAGE                = 0xC3
};

static const uint8_t address[6] =
{
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99
};

#if defined(__MSP430__)  &&  (defined(DLT645_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))

extern serial_msg_buf_t tx_msg[];
extern serial_msg_buf_t rx_msg[];

uint16_t *next_flash_loc;

#endif

#if defined(__MSP430__)  &&  (defined(DLT645_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))
int prepare_tx_message(int port, int len)
{
    int i;
    uint8_t *s;

    s = tx_msg[port].buf.uint8;
    s[0] = 0xFE;
    s[1] = 0xFE;
    s[2] = 0xFE;
    s[3] = 0xFE;
    s[4] = 0x68;
    s[5] = address[0];
    s[6] = address[1];
    s[7] = address[2];
    s[8] = address[3];
    s[9] = address[4];
    s[10] = address[5];
    s[11] = 0x68;
    s[12] = 0x23;
    s[13] = len;
    s[DLT645_MSG_TX_START_BODY + len] = 0;
    s[DLT645_MSG_TX_START_BODY + len + 1] = 0x16;
    for (i = 4;  i < DLT645_MSG_TX_START_BODY + len;  i++)
        s[DLT645_MSG_TX_START_BODY + len] += s[i];
    len = DLT645_MSG_TX_START_BODY + len + 2;
    if (len > 4 + 12 + MAX_DLT645_MSG_BODY)
        return FALSE;
    send_message(port, len);
    return  TRUE;
}

static void dlt645_process_rx_message(int port, serial_msg_t *rx_msg, int rx_len)
{
    int i;
    int32_t z;
    int32_t z1;
    #if !defined(SINGLE_PHASE)
    struct phase_parms_s *phase;
    #endif
    uint16_t *last_flash_loc;
    serial_msg_buf_t *tx;
    uint8_t *tx8;
    uint16_t *tx16;

    tx = &tx_msg[port];
    tx8 = tx->buf.uint8;
    tx16 = tx->buf.uint16;
    uint16_t tx_len = 0;


    /* Messages with type 0x23 are custom messages we
      use for calibration, password protection, etc.
      All other message types go to a custom message
      handler (if available). */
    if (rx_msg->uint8[8] != 0x23)
    {
#if defined(CUSTOM_SERIAL_MESSAGE_SUPPORT)
        custom_serial_message_handler(&rx_msg, rx_msg_len);
#endif
        return;
    }
    if ((rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] & 0x80))
    {
        /* This looks like one of our own messages, which has echoed back
           to us */
        return;
    }

    /* Only process messages if the password has been given correctly
       (except for the password test message, of course). */
    if (!(meter_status & PASSWORD_OK)  &&  rx_msg->uint8[DLT645_MSG_RX_START_BODY] != HOST_CMD_SET_PASSWORD)
        return;

    switch (rx_msg->uint8[DLT645_MSG_RX_START_BODY])
    {
    case HOST_CMD_GET_METER_CONFIGURATION:
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        tx8[DLT645_MSG_TX_START_BODY + 2] = NUM_PHASES;
    #if defined(NEUTRAL_MONITOR_SUPPORT)
        tx8[DLT645_MSG_TX_START_BODY + 3] = 1;
    #else
        tx8[DLT645_MSG_TX_START_BODY + 3] = 0;
    #endif
        tx8[DLT645_MSG_TX_START_BODY + 4] = 0
    #if defined(LIMP_MODE_SUPPORT)
                                                    | 0x01
    #endif
    #if defined(PHASE_CORRECTION_SUPPORT)
                                                    | 0x02
    #endif
    #if defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
                                                    | 0x04
    #endif
    #if defined(RTC_SUPPORT)
                                                    | 0x08
    #endif
    #if defined(CORRECTED_RTC_SUPPORT)
                                                    | 0x10
    #endif
    #if defined(TEMPERATURE_SUPPORT)
                                                    | 0x20
    #endif
    #if defined(SELF_TEST_SUPPORT)
                                                    | 0x40
    #endif
    #if defined(MULTI_RATE_SUPPORT)
                                                    | 0x80
    #endif
                                                    ;

        tx8[DLT645_MSG_TX_START_BODY + 5] = MEASURES_ACTIVE_POWER
    #if defined(REACTIVE_POWER_SUPPORT)  &&  !defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
                                                    | MEASURES_TRIGONOMETRIC_REACTIVE_POWER
    #endif
    #if defined(APPARENT_POWER_SUPPORT)
                                                    | MEASURES_APPARENT_POWER
    #endif
    #if defined(VRMS_SUPPORT)
                                                    | MEASURES_VRMS
    #endif
    #if defined(IRMS_SUPPORT)
                                                    | MEASURES_IRMS
    #endif
    #if defined(POWER_FACTOR_SUPPORT)
                                                    | MEASURES_POWER_FACTOR
    #endif
    #if defined(MAINS_FREQUENCY_SUPPORT)
                                                    | MEASURES_MAINS_FREQUENCY
    #endif
    #if defined(REACTIVE_POWER_SUPPORT)  &&  defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
                                                    | MEASURES_QUADRATURE_REACTIVE_POWER
    #endif
                                                    ;
        tx8[DLT645_MSG_TX_START_BODY + 6] = 0x00
    #if GAIN_STAGES > 1
                                                    | 0x01
    #endif
                                                    ;
        prepare_tx_message(port, 7);
        break;
    case HOST_CMD_SET_METER_CONSUMPTION:
        z = rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
        z |= (int32_t) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 2] << 16;
        z1 = rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 3];
        z1 |= (int32_t) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 4] << 16;
        custom_set_consumption(z, z1);
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        prepare_tx_message(port, 2);
    case HOST_CMD_SET_RTC:
    #if defined(RTC_SUPPORT)
        rtc.year = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 2];
        rtc.month = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 3];
        rtc.day = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 4];
        rtc.hour = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 5];
        rtc.minute = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 6];
        rtc.second = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 7];
        set_rtc_sumcheck();

    #if defined (__MSP430_HAS_RTC_C__)
        RTCCTL0_H = RTCKEY_H;                  // Unlock RTC
        BIN2BCD = rtc.second; RTCSEC = BIN2BCD;
        BIN2BCD = rtc.minute; RTCMIN = BIN2BCD;
        BIN2BCD = rtc.hour;   RTCHOUR = BIN2BCD;
//      BIN2BCD  = rtc.DayOfWeek; RTCDOW = BIN2BCD;
        BIN2BCD = rtc.day;    RTCDAY = BIN2BCD;
        BIN2BCD = rtc.month;  RTCMON = BIN2BCD;
        BIN2BCD = rtc.year;   RTCYEAR = BIN2BCD+0x2000;
        RTCCTL0_H = 0;   // LOCK RTC
    #endif

    #endif
    #if defined(CUSTOM_RTC_SUPPORT)
        custom_rtc_set(rx_msg->uint8);
    #endif
    #if defined(MULTI_RATE_SUPPORT)
        multirate_align_with_rtc();
    #endif
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        prepare_tx_message(port, 2);
        break;
    case HOST_CMD_GET_RTC:
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rx_msg->uint8[DLT645_MSG_RX_START_BODY]; tx_len+=1;
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80; tx_len+=1;
    #if defined(RTC_SUPPORT)
      #if defined (__MSP430_HAS_RTC_C__)
        BCD2BIN = RTCYEAR&0xFF; tx8[DLT645_MSG_TX_START_BODY + tx_len] = BCD2BIN; tx_len+=1;
        BCD2BIN = RTCMON;       tx8[DLT645_MSG_TX_START_BODY + tx_len] = BCD2BIN; tx_len+=1;
        BCD2BIN = RTCDAY;       tx8[DLT645_MSG_TX_START_BODY + tx_len] = BCD2BIN; tx_len+=1;
        BCD2BIN = RTCHOUR;      tx8[DLT645_MSG_TX_START_BODY + tx_len] = BCD2BIN; tx_len+=1;
        BCD2BIN = RTCMIN;       tx8[DLT645_MSG_TX_START_BODY + tx_len] = BCD2BIN; tx_len+=1;
        BCD2BIN = RTCSEC;       tx8[DLT645_MSG_TX_START_BODY + tx_len] = BCD2BIN; tx_len+=1;
      #else
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rtc.year;   tx_len+=1;
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rtc.month;  tx_len+=1;
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rtc.day;    tx_len+=1;
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rtc.hour;   tx_len+=1;
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rtc.minute; tx_len+=1;
        tx8[DLT645_MSG_TX_START_BODY + tx_len] = rtc.second; tx_len+=1;
      #endif
    #endif
    #if defined(CUSTOM_RTC_SUPPORT)
        custom_rtc_retrieve(tx8);
    #endif
    #if defined(TEMPERATURE_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = temperature; tx_len+=2;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = 0; tx_len+=2;
    #endif
    #if defined(VCC_MEASURE_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = vcc; tx_len+=2;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = 0; tx_len+=2;
    #endif
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = meter_status; tx_len+=2;
#if defined(SINGLE_PHASE)
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = phase->status; tx_len+=2;
#else
        for (i=0; i < NUM_PHASES; i++)
          tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = phase[i].status; tx_len+=2;
    #if defined(NEUTRAL_MONITOR_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + tx_len/2] = neutral.status; tx_len+=2;
    #endif
#endif
        prepare_tx_message(port, /*18*/ tx_len+=2);
        break;
    case HOST_CMD_ALIGN_WITH_CALIBRATION_FACTORS:
        align_hardware_with_calibration_data();
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        prepare_tx_message(port, 2);
        break;
    case HOST_CMD_SET_PASSWORD:
        /* Check the calibration password */
        if (rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1] == SERIAL_CALIBRATION_PASSWORD_1
            &&
            rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 2] == SERIAL_CALIBRATION_PASSWORD_2
            &&
            rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 3] == SERIAL_CALIBRATION_PASSWORD_3
            &&
            rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 4] == SERIAL_CALIBRATION_PASSWORD_4)
        {
            meter_status |= PASSWORD_OK;
            tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
            tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            prepare_tx_message(port, 2);
        }
        else
        {
            /* Only respond to a bad password, if the password was good before. That lets
               us know we have unset the password OK, but doesn't give any information to
               people trying to attack the meter. */
            if ((meter_status & PASSWORD_OK))
            {
                tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
                tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
                prepare_tx_message(port, 2);
            }
            meter_status &= ~PASSWORD_OK;
        }
        break;
    case HOST_CMD_GET_READINGS_PHASE_1:
    #if !defined(SINGLE_PHASE)
    case HOST_CMD_GET_READINGS_PHASE_2:
    case HOST_CMD_GET_READINGS_PHASE_3:
        /* Exchange voltage, current and power readings (neutral).
           frequency, power factor and reactive power readings. */
        phase = &chan[rx_msg->uint8[DLT645_MSG_RX_START_BODY] - HOST_CMD_GET_READINGS_PHASE_1];
    #endif
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
    #if defined(VRMS_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = phase->readings.V_rms;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = 0;
    #endif
    #if defined(IRMS_SUPPORT)
    #if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.current.I_rms;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.I_rms;
    #endif
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = 0;
    #endif
    #if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.current.active_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.current.active_power >> 16;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.active_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.active_power >> 16;
    #endif
    #if defined(REACTIVE_POWER_SUPPORT)
         #if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->readings.reactive_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->readings.reactive_power >> 16;
        #else
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->readings.reactive_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->readings.reactive_power >> 16;
        #endif
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = 0;
    #endif
    #if defined(APPARENT_POWER_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = phase->readings.apparent_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 8] = phase->readings.apparent_power >> 16;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 8] = 0;
    #endif

    #if defined(POWER_FACTOR_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 9] = phase->readings.power_factor;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 9] = 0;
    #endif
    #if defined(MAINS_FREQUENCY_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 10] = phase->readings.frequency;
    #else
        tx16[DLT645_MSG_TX_START_BODY_W + 10] = 0;
    #endif
    #if !defined(ESP_SUPPORT)
        #if defined(LIMP_MODE_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 11] = phase->metrology.V_dc_estimate[0];
        tx16[DLT645_MSG_TX_START_BODY_W + 12] = phase->metrology.V_dc_estimate[0] >> 16;
        #else
        //MM24
        //tx16[DLT645_MSG_TX_START_BODY_W + 11] = phase->metrology.V_dc_estimate;
        //tx16[DLT645_MSG_TX_START_BODY_W + 12] = phase->metrology.V_dc_estimate >> 16;
        
       tx16[DLT645_MSG_TX_START_BODY_W + 11] = phase->metrology.V_dc_estimate[0];
        tx16[DLT645_MSG_TX_START_BODY_W + 12] = phase->metrology.V_dc_estimate[0] >> 16;
        #endif
        //MM24
        //tx16[DLT645_MSG_TX_START_BODY_W + 13] = phase->metrology.current.I_dc_estimate[0];
        //tx16[DLT645_MSG_TX_START_BODY_W + 14] = phase->metrology.current.I_dc_estimate[0] >> 16;
         tx16[DLT645_MSG_TX_START_BODY_W + 13] = phase->metrology.current.I_dc_estimate[0][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 14] = phase->metrology.current.I_dc_estimate[0][1];
        tx8[DLT645_MSG_TX_START_BODY + 30] = 0;
    #endif
        prepare_tx_message(port, 31);
        break;
    #if defined(NEUTRAL_MONITOR_SUPPORT)
    case HOST_CMD_GET_READINGS_NEUTRAL:
        /* Exchange voltage, current and power readings (neutral).
           frequency, power factor and reactive power readings. */
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        #if defined(SINGLE_PHASE)
            #if defined(VRMS_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = phase->readings.V_rms;
            #else
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = 0;
            #endif
            #if defined(ESP_SUPPORT)
                #if defined(IRMS_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.neutral_I_rms;
                #else
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = 0;
                #endif
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.neutral_active_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.neutral_active_power >> 16;
                #if defined(REACTIVE_POWER_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->metrology.neutral_reactive_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->metrology.neutral_reactive_power >> 16;
                #else
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = 0;
                #endif
            #else
                #if defined(IRMS_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.neutral.I_rms;
                #else
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = 0;
                #endif
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.neutral.active_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.neutral.active_power >> 16;
                #if defined(REACTIVE_POWER_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->metrology.neutral.reactive_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->metrology.neutral.reactive_power >> 16;
                #else
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = 0;
                #endif
            #endif
            #if defined(APPARENT_POWER_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = phase->readings.apparent_power;
        tx16[DLT645_MSG_TX_START_BODY_W + 8] = phase->readings.apparent_power >> 16;
            #else
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 8] = 0;
            #endif
            #if defined(POWER_FACTOR_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 9] = phase->readings.power_factor;
            #else
        tx16[DLT645_MSG_TX_START_BODY_W + 9] = 0;
            #endif
        tx16[DLT645_MSG_TX_START_BODY_W + 10] = phase->readings.frequency;
            #if !defined(ESP_SUPPORT)
                #if defined(LIMP_MODE_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 11] = phase->metrology.V_dc_estimate[0];
        tx16[DLT645_MSG_TX_START_BODY_W + 12] = phase->metrology.V_dc_estimate[0] >> 16;
                #else
        tx16[DLT645_MSG_TX_START_BODY_W + 11] = phase->metrology.V_dc_estimate;
        tx16[DLT645_MSG_TX_START_BODY_W + 12] = phase->metrology.V_dc_estimate >> 16;
                #endif
        tx16[DLT645_MSG_TX_START_BODY_W + 13] = phase->metrology.neutral.I_dc_estimate[0];
        tx16[DLT645_MSG_TX_START_BODY_W + 14] = phase->metrology.neutral.I_dc_estimate[0] >> 16;
            #endif
        tx8[DLT645_MSG_TX_START_BODY + 30] = 0;
        #else
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = neutral.readings.I_rms;
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 8] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 9] = 0;
#if defined(MAINS_FREQUENCY_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 10] = neutral.readings.frequency;
#else
        tx16[DLT645_MSG_TX_START_BODY_W + 10] = 0;
#endif
        tx16[DLT645_MSG_TX_START_BODY_W + 11] = 0;
        tx16[DLT645_MSG_TX_START_BODY_W + 12] = 0;
        //MM24
//        tx16[DLT645_MSG_TX_START_BODY_W + 13] = neutral.metrology.I_dc_estimate[0];
//        tx16[DLT645_MSG_TX_START_BODY_W + 14] = neutral.metrology.I_dc_estimate[0] >> 16;
        tx16[DLT645_MSG_TX_START_BODY_W + 13] = neutral.metrology.I_dc_estimate[0][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 14] = neutral.metrology.I_dc_estimate[0][1];
        tx8[DLT645_MSG_TX_START_BODY + 30] = 0;
        #endif
        prepare_tx_message(port, 31);
        break;
    #endif
    case HOST_CMD_ERASE_FLASH_SEGMENT:
        if (is_calibration_enabled())
        {
            /* Initialise flash data download, by erasing the area to be used, and setting the
               write pointer. */
            /* There is no checking here to ensure we do not erase inappropriate places. */
            next_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
            flash_clr((int16_t *) next_flash_loc);
            tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
            tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            prepare_tx_message(port, 2);
        }
        break;
    case HOST_CMD_SET_FLASH_POINTER:
        next_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        prepare_tx_message(port, 2);
        break;
    case HOST_CMD_FLASH_DOWNLOAD:
        if (is_calibration_enabled())
        {
            next_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
            for (i = 2;  i < (rx_len - 12) >> 1;  i++)
                flash_write_int16((int16_t *) next_flash_loc++, rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + i]);
            flash_secure();
            /* Return the next address to the host, so it can check for missed messages */
            tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
            tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            tx16[DLT645_MSG_TX_START_BODY_W + 1] = (uint16_t) next_flash_loc;
            prepare_tx_message(port, 4);
        }
        break;
    case HOST_CMD_FLASH_UPLOAD:
        if (is_calibration_enabled())
        {
            next_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
            tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
            tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            for (i = 1;  i < MAX_DLT645_MSG_BODY/2;  i++)
                tx16[DLT645_MSG_TX_START_BODY_W + i] = *next_flash_loc++;
            prepare_tx_message(port, MAX_DLT645_MSG_BODY);
        }
        break;
    case HOST_CMD_ZAP_MEMORY_AREA:
        /* Zap memory area (usually this will be flash) */
        if (is_calibration_enabled())
        {
            next_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
            last_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 2];
            while (next_flash_loc < last_flash_loc)
                flash_write_int16((int16_t *) next_flash_loc++, rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 3]);
            flash_secure();
            tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
            tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            tx16[DLT645_MSG_TX_START_BODY_W + 1] = (uint16_t) next_flash_loc;
            prepare_tx_message(port, 4);
        }
        break;
    case HOST_CMD_SUMCHECK_MEMORY:
        /* Sumcheck a specified memory area, and return the sumcheck */
        if (is_calibration_enabled())
        {
            next_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 1];
            last_flash_loc = (uint16_t *) rx_msg->uint16[DLT645_MSG_RX_START_BODY_W + 2];
            tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
            tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            tx16[DLT645_MSG_TX_START_BODY_W + 1] = 0;
            while (next_flash_loc < last_flash_loc)
                tx16[DLT645_MSG_TX_START_BODY_W + 1] += *next_flash_loc++;
            flash_secure();
            prepare_tx_message(port, 4);
        }
        break;
    case HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_1:
    #if !defined(SINGLE_PHASE)
    case HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_2:
    case HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_3:
        phase = &chan[rx_msg->uint8[DLT645_MSG_RX_START_BODY] - HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_1];
    #endif
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
#if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = phase->metrology.current.dot_prod_logged.sample_count;
        //MM24
        //tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.current.dot_prod_logged.P_active[0][0];
        //tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.current.dot_prod_logged.P_active[0][1];
        //tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.current.dot_prod_logged.P_active[0][2];
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.current.dot_prod_logged.P_active[0];
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.current.dot_prod_logged.P_active[1];
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.current.dot_prod_logged.P_active[2];
#if GAIN_STAGES > 1
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->metrology.current.dot_prod_logged.P_active[1][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->metrology.current.dot_prod_logged.P_active[1][1];
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = phase->metrology.current.dot_prod_logged.P_active[1][2];
        prepare_tx_message(port, 16);
#else
        prepare_tx_message(port, 10);
#endif
#endif
        break;
    #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
    case HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_1:
        #if !defined(SINGLE_PHASE)
    case HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_2:
    case HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_3:
        phase = &chan[rx_msg->uint8[DLT645_MSG_RX_START_BODY] - HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_1];
        #endif
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
#if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = phase->metrology.current.dot_prod_logged.sample_count;
        //MM24
//        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.current.dot_prod_logged.P_reactive[0][0];
//        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.current.dot_prod_logged.P_reactive[0][1];
//        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.current.dot_prod_logged.P_reactive[0][2];
         tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.current.dot_prod_logged.P_reactive[0];
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.current.dot_prod_logged.P_reactive[1];
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.current.dot_prod_logged.P_reactive[2];
#if GAIN_STAGES > 1
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->metrology.current.dot_prod_logged.P_reactive[1][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->metrology.current.dot_prod_logged.P_reactive[1][1];
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = phase->metrology.current.dot_prod_logged.P_reactive[1][2];
        prepare_tx_message(port, 16);
#else
        prepare_tx_message(port, 10);
#endif
#endif
        break;
    #endif
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    case HOST_CMD_GET_RAW_ACTIVE_POWER_NEUTRAL:
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        #if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = phase->metrology.neutral.dot_prod_logged.sample_count;
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.neutral.dot_prod_logged.P_active[0][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.neutral.dot_prod_logged.P_active[0][1];
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.neutral.dot_prod_logged.P_active[0][2];
            #if GAIN_STAGES > 1
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->metrology.neutral.dot_prod_logged.P_active[1][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->metrology.neutral.dot_prod_logged.P_active[1][1];
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = phase->metrology.neutral.dot_prod_logged.P_active[1][2];
        prepare_tx_message(port, 16);
            #else
        prepare_tx_message(port, 10);
            #endif
        #endif
        break;
        #if defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
    case HOST_CMD_GET_RAW_REACTIVE_POWER_NEUTRAL:
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
            #if !defined(ESP_SUPPORT)
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = phase->metrology.neutral.dot_prod_logged.sample_count;
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = phase->metrology.neutral.dot_prod_logged.P_reactive[0][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 3] = phase->metrology.neutral.dot_prod_logged.P_reactive[0][1];
        tx16[DLT645_MSG_TX_START_BODY_W + 4] = phase->metrology.neutral.dot_prod_logged.P_reactive[0][2];
                #if GAIN_STAGES > 1
        tx16[DLT645_MSG_TX_START_BODY_W + 5] = phase->metrology.neutral.dot_prod_logged.P_reactive[1][0];
        tx16[DLT645_MSG_TX_START_BODY_W + 6] = phase->metrology.neutral.dot_prod_logged.P_reactive[1][1];
        tx16[DLT645_MSG_TX_START_BODY_W + 7] = phase->metrology.neutral.dot_prod_logged.P_reactive[1][2];
        prepare_tx_message(port, 16);
                #else
        prepare_tx_message(port, 10);
                #endif
        break;
            #endif
        #endif
    #endif
    #if defined(CORRECTED_RTC_SUPPORT)
    case HOST_CMD_CHECK_RTC_ERROR:
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
    #if defined(__MSP430_HAS_TA3__)
        z = assess_rtc_speed();
    #endif
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = z;
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = z >> 16;
        prepare_tx_message(port, 6);
        break;
    case HOST_CMD_RTC_CORRECTION:
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = rtc_correction;
        tx16[DLT645_MSG_TX_START_BODY_W + 2] = rtc_correction >> 16;
        prepare_tx_message(port, 6);
        break;
    #endif
    #if defined(MULTI_RATE_SUPPORT)
    case HOST_CMD_MULTIRATE_SET_PARAMETERS:
        i = multirate_put(&rx_msg->uint8[DLT645_MSG_RX_START_BODY]);
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = i;
        prepare_tx_message(port, 4);
        break;
    case HOST_CMD_MULTIRATE_GET_PARAMETERS:
        i = multirate_get(&rx_msg->uint8[DLT645_MSG_RX_START_BODY], &tx8[DLT645_MSG_TX_START_BODY]);
        prepare_tx_message(port, i);
        break;
    case HOST_CMD_MULTIRATE_CLEAR_USAGE:
        i = multirate_clear_usage(&rx_msg->uint8[DLT645_MSG_RX_START_BODY]);
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = rx_msg->uint8[DLT645_MSG_RX_START_BODY + 1] | 0x80;
        tx16[DLT645_MSG_TX_START_BODY_W + 1] = i;
        prepare_tx_message(port, 4);
        break;
    case HOST_CMD_MULTIRATE_GET_USAGE:
        i = multirate_get_usage(&rx_msg->uint8[DLT645_MSG_RX_START_BODY], &tx8[DLT645_MSG_TX_START_BODY]);
        prepare_tx_message(port, i);
        break;
    #endif
    default:
        /* For all other message types reply with type 0xFF - bad message type */
        tx8[DLT645_MSG_TX_START_BODY] = rx_msg->uint8[DLT645_MSG_RX_START_BODY];
        tx8[DLT645_MSG_TX_START_BODY + 1] = 0xFF;
        prepare_tx_message(port, 2);
        break;
    }
}
#endif

#if defined(__MSP430__)  &&  (defined(DLT645_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))
void dlt645_rx_byte(int port, uint8_t ch)
{
    int i;
    int sum;

    //if (rx_msg[port].inter_char_timeout == 0)
    //    rx_msg[port].ptr = 0;
    rx_msg[port].inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (rx_msg[port].ptr == 0)
    {
        if (ch == 0x68)
        {
            rx_msg[port].buf.uint8[rx_msg[port].ptr++] = ch;
            rx_msg[port].len = 12 + MAX_DLT645_MSG_BODY;
        }
    }
    else
    {
        if (rx_msg[port].ptr == 9)
        {
            if (ch <= MAX_DLT645_MSG_BODY)
                rx_msg[port].len = 12 + ch;
            else
                rx_msg[port].ptr = 0;
        }
        rx_msg[port].buf.uint8[rx_msg[port].ptr++] = ch;
        if (rx_msg[port].ptr == rx_msg[port].len)
        {
            /* End of message */
            sum = rx_msg[port].buf.uint8[0];
            for (i = 1;  i < rx_msg[port].len - 2;  i++)
                sum += rx_msg[port].buf.uint8[i];
            if (rx_msg[port].buf.uint8[rx_msg[port].len - 2] == (sum & 0xFF)
                &&
                rx_msg[port].buf.uint8[rx_msg[port].len - 1] == 0x16)
            {
                /* Good message received */
                dlt645_process_rx_message(port, &rx_msg[port].buf, rx_msg[port].len);
            }
            rx_msg[port].ptr = 0;
        }
    }
}
#endif
