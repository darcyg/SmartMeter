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
//  File: emeter-communications.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-communication.c,v 1.18 2009/04/27 09:05:25 a0754793 Exp $
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

#if defined(IEC62056_21_SUPPORT)
#include "iec62056-21.h"
#endif

#if !defined(NULL)
#define NULL    (void *) 0
#endif

#if defined(__MSP430__)  &&  defined(IEC62056_21_SUPPORT)
uint8_t iec62056_21_address[] = "Node 1";
uint8_t iec62056_21_address_len = 6;
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

#if defined(__MSP430__)  &&  (defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))

#if defined(UART_PORT_3_SUPPORT)
serial_msg_buf_t tx_msg[4];
serial_msg_buf_t rx_msg[4];
#elif defined(UART_PORT_2_SUPPORT)
serial_msg_buf_t tx_msg[3];
serial_msg_buf_t rx_msg[3];
#elif defined(UART_PORT_1_SUPPORT)
serial_msg_buf_t tx_msg[2];
serial_msg_buf_t rx_msg[2];
#elif defined(UART_PORT_0_SUPPORT)
serial_msg_buf_t tx_msg[1];
serial_msg_buf_t rx_msg[1];
#endif

#endif

int is_calibration_enabled(void)
{
    return TRUE;
}

#if defined(IEC62056_21_SUPPORT)  ||  defined(IEC1107_D_SUPPORT)

int iec1107d_status;

void send_1107d_report(void)
{
    int i;
    uint8_t x;
    uint8_t bcd[5];
    const uint8_t *s;
    static const uint8_t id[] = "/XXXXXXXXXX V1.00\r\n";
    static const uint8_t serial_number_tag[] = "1-0:0.0.9*255(";
    static const uint8_t property_number_tag[] = "1-0:0.0.0*255(";
    static const uint8_t energy_tag[] = "1-0:1.8.0*255(";
    static const uint8_t status_tag[] = "1-0:96.5.5*255(";
    static const uint8_t factory_number_tag[] = "0-0:96.1.255*255(";
    static const uint8_t even_parity[128] =
    {
        0x00, 0x81, 0x82, 0x03, 0x84, 0x05, 0x06, 0x87,
        0x88, 0x09, 0x0A, 0x8B, 0x0C, 0x8D, 0x8E, 0x0F,
        0x90, 0x11, 0x12, 0x93, 0x14, 0x95, 0x96, 0x17,
        0x18, 0x99, 0x9A, 0x1B, 0x9C, 0x1D, 0x1E, 0x9F,
        0xA0, 0x21, 0x22, 0xA3, 0x24, 0xA5, 0xA6, 0x27,
        0x28, 0xA9, 0xAA, 0x2B, 0xAC, 0x2D, 0x2E, 0xAF,
        0x30, 0xB1, 0xB2, 0x33, 0xB4, 0x35, 0x36, 0xB7,
        0xB8, 0x39, 0x3A, 0xBB, 0x3C, 0xBD, 0xBE, 0x3F,
        0xC0, 0x41, 0x42, 0xC3, 0x44, 0xC5, 0xC6, 0x47,
        0x48, 0xC9, 0xCA, 0x4B, 0xCC, 0x4D, 0x4E, 0xCF,
        0x50, 0xD1, 0xD2, 0x53, 0xD4, 0x55, 0x56, 0xD7,
        0xD8, 0x59, 0x5A, 0xDB, 0x5C, 0xDD, 0xDE, 0x5F,
        0x60, 0xE1, 0xE2, 0x63, 0xE4, 0x65, 0x66, 0xE7,
        0xE8, 0x69, 0x6A, 0xEB, 0x6C, 0xED, 0xEE, 0x6F,
        0xF0, 0x71, 0x72, 0xF3, 0x74, 0xF5, 0xF6, 0x77,
        0x78, 0xF9, 0xFA, 0x7B, 0xFC, 0x7D, 0x7E, 0xFF,
    };

    /* IEC1107 specifies 7 bit + even parity data. We want to use 8 bit data for calibration, so we
       use software generation for the parity bit. */
    /* If the password is set we are in calibration mode, and the IEC1107D messages should not be sent.
       After calibration is complete, the password may or may not have been cleared. If it is still set
       a reset of the MCU will restore normal operation. */
    if ((meter_status & PASSWORD_OK))
        return;
    iec1107d_status = 0;
#if 0
    if (total_power > 0)
        iec1107d_status |= 0x80;
#endif
#if defined(SELF_TEST_SUPPORT)
    if (nv_parms.s.meter_failures != 0xFFFF)
        iec1107d_status |= 0x01;
#endif

    i = 0;
    s = id;
    while (*s)
        tx_msg[0].buf.uint8[i++] = even_parity[*s++];

    tx_msg[0].buf.uint8[i++] = even_parity['\r'];
    tx_msg[0].buf.uint8[i++] = even_parity['\n'];

#if 0
#if 0
    if (nv_parms.s.property_number[0])
    {
        s = property_number_tag;
        while (*s)
            tx_msg[0].buf.uint8[i++] = even_parity[*s++];
        s = nv_parms.s.property_number;
        while (*s)
            tx_msg[0].buf.uint8[i++] = even_parity[*s++];
        tx_msg[0].buf.uint8[i++] = even_parity[')'];
        tx_msg[0].buf.uint8[i++] = even_parity['\r'];
        tx_msg[0].buf.uint8[i++] = even_parity['\n'];
    }

    if (nv_parms.s.serial_number[0])
    {
        s = serial_number_tag;
        while (*s)
            tx_msg[0].buf.uint8[i++] = even_parity[*s++];
        s = nv_parms.s.serial_number;
        while (*s)
            tx_msg[0].buf.uint8[i++] = even_parity[*s++];
        tx_msg[0].buf.uint8[i++] = even_parity[')'];
        tx_msg[0].buf.uint8[i++] = even_parity['\r'];
        tx_msg[0].buf.uint8[i++] = even_parity['\n'];
    }
#endif

    s = energy_tag;
    while (*s)
        tx_msg[0].buf.uint8[i++] = even_parity[*s++];

    bin2bcd32(bcd, (long int) &chan[0].current.active_power);
    tx_msg[0].buf.uint8[i++] = even_parity['0' + ((bcd[2] >> 4) & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + (bcd[2] & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + ((bcd[3] >> 4) & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + (bcd[3] & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + ((bcd[4] >> 4) & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + (bcd[4] & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['.'];
    bin2bcd16(bcd, 0);
    tx_msg[0].buf.uint8[i++] = even_parity['0' + ((bcd[1] >> 4) & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + (bcd[1] & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + ((bcd[2] >> 4) & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity['0' + (bcd[2] & 0xF)];
    tx_msg[0].buf.uint8[i++] = even_parity[')'];
    tx_msg[0].buf.uint8[i++] = even_parity['\r'];
    tx_msg[0].buf.uint8[i++] = even_parity['\n'];

    s = status_tag;
    while (*s)
        tx_msg[0].buf.uint8[i++] = even_parity[*s++];
    x = '0' + ((iec1107d_status >> 4) & 0xF);
    if (x > '9')
        x += 7;
    tx_msg[0].buf.uint8[i++] = even_parity[x];
    x = '0' + (iec1107d_status & 0xF);
    if (x > '9')
        x += 7;
    tx_msg[0].buf.uint8[i++] = even_parity[x];
    tx_msg[0].buf.uint8[i++] = even_parity[')'];
    tx_msg[0].buf.uint8[i++] = even_parity['\r'];
    tx_msg[0].buf.uint8[i++] = even_parity['\n'];

#if 0
    if (nv_parms.s.factory_number[0])
    {
        s = factory_number_tag;
        while (*s)
            tx_msg[0].buf.uint8[i++] = even_parity[*s++];
        s = nv_parms.s.factory_number;
        while (*s)
            tx_msg[0].buf.uint8[i++] = even_parity[*s++];
        tx_msg[0].buf.uint8[i++] = even_parity[')'];
        tx_msg[0].buf.uint8[i++] = even_parity['\r'];
        tx_msg[0].buf.uint8[i++] = even_parity['\n'];
    }
#endif
#endif
    tx_msg[0].buf.uint8[i++] = even_parity['!'];
    tx_msg[0].buf.uint8[i++] = even_parity['\r'];
    tx_msg[0].buf.uint8[i++] = even_parity['\n'];
    send_message(0, i);

#if 0
    #if defined(USART1TX_VECTOR)
    U1IE |= UTXIE1;
    #else
    U0IE |= UTXIE0;
    #endif
#endif
}
#endif

#if defined(__MSP430__)  &&  defined(IEC62056_21_SUPPORT)
int iec62056_21_process_ack(const uint8_t *msg, int len)
{
    return len;
}

int iec62056_21_process_nak(const uint8_t *msg, int len)
{
    return len;
}

int iec62056_21_process_ident(const uint8_t *msg, int len)
{
    return len;
}

void iec62056_21_process_request(void)
{
    if (tx_msg[0].len == 0)
        send_1107d_report();
}

int iec62056_21_process_header(const uint8_t *msg, int len)
{
    int i;

    for (i = 0;  i < len  &&  msg[i] != 2;  i++)
        ;
    return i;
}

int iec62056_21_process_field(const uint8_t instance_id[6], const uint8_t *val, int len)
{
    return len;
}
#endif

#if defined(__MSP430__)  &&  (defined(IEC62056_21_SUPPORT)  ||  defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT))
/* Interrupt routines to send serial messages. */

    #if defined(UART_PORT_0_SUPPORT)
        #if defined(__MSP430_HAS_UART0__)
ISR(USART0RX, serial_rx_interrupt0)
{
#if defined(IEC62056_21_SUPPORT)  ||  defined(DLT645_SUPPORT)
    uint8_t ch;

    ch = RXBUF0;
#endif
#if defined(UART0_IEC62056_21_SUPPORT)
    if (inter_char_timeout == 0)
        iec62056_21_rx_restart();
    inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[0].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART0_DLT645_SUPPORT)
    dlt645_rx_byte(0, ch);
#endif
}

ISR(USART0TX, serial_tx_interrupt0)
{
    TXBUF0 = tx_msg[0].buf.uint8[tx_msg[0].ptr++];
    if (tx_msg[0].ptr >= tx_msg[0].len)
    {
        /* Stop transmission */
        U0IE &= ~UTXIE0;
        tx_msg[0].ptr = 0;
        tx_msg[0].len = 0;
    }
}
        #elif defined(__MSP430_HAS_USCI_AB0__)
ISR(USCIAB0RX, serial_rx_interrupt0)
{
    uint8_t ch;

    ch = UCA0RXBUF;
#if defined(UART0_IEC62056_21_SUPPORT)
    //if (inter_char_timeout == 0)
    //    iec62056_21_rx_restart();
    //inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[0].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART0_DLT645_SUPPORT)
    dlt645_rx_byte(0, ch);
#endif
}

ISR(USCIAB0TX, serial_tx_interrupt0)
{
    UCA0TXBUF = tx_msg[0].buf.uint8[tx_msg[0].ptr++];
    if (tx_msg[0].ptr >= tx_msg[0].len)
    {
        /* Stop transmission */
        UC0IE &= ~UCA0TXIE;
        tx_msg[0].ptr = 0;
        tx_msg[0].len = 0;
    }
}
        #elif defined(__MSP430_HAS_USCI_A0__)
ISR(USCI_A0, serial_interrupt0)
{
    uint8_t ch;
    switch(__even_in_range(UCA0IV,USCI_UART_UCRXIFG))
    {
    case USCI_UART_UCRXIFG:
    ch = UCA0RXBUF;
#if defined(UART0_IEC62056_21_SUPPORT)
    //if (inter_char_timeout == 0)
    //    iec62056_21_rx_restart();
    //inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[0].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART0_DLT645_SUPPORT)
    dlt645_rx_byte(0, ch);
#endif
    break;
    case USCI_UART_UCTXIFG:
    UCA0TXBUF = tx_msg[0].buf.uint8[tx_msg[0].ptr++];
    if (tx_msg[0].ptr >= tx_msg[0].len)
    {
        /* Stop transmission */
        UCA0IE &= ~UCTXIE;
        tx_msg[0].ptr = 0;
        tx_msg[0].len = 0;
    }
    break;
    }

}
        #elif defined(__MSP430_HAS_EUSCI_A0__)
ISR(USCI_A0, serial_interrupt0)
{
    uint8_t ch;
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:
    break;
    case USCI_UART_UCRXIFG:
    ch = UCA0RXBUF;
#if defined(UART0_IEC62056_21_SUPPORT)
    //if (inter_char_timeout == 0)
    //    iec62056_21_rx_restart();
    //inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[0].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART0_DLT645_SUPPORT)
    dlt645_rx_byte(0, ch);
#endif
    break;
    case USCI_UART_UCTXIFG:
    UCA0TXBUF = tx_msg[0].buf.uint8[tx_msg[0].ptr++];
    if (tx_msg[0].ptr >= tx_msg[0].len)
    {
        /* Stop transmission */
        UCA0IE &= ~UCTXIE;
        tx_msg[0].ptr = 0;
        tx_msg[0].len = 0;
    }
    break;
    case USCI_UART_UCSTTIFG:
    break;
    case USCI_UART_UCTXCPTIFG:
    break;
    }

}
        #else
            #error Device does not have a UART port 0
        #endif
    #endif

    #if defined(UART_PORT_1_SUPPORT)
        #if defined(__MSP430_HAS_UART1__)
ISR(USART1RX, serial_rx_interrupt1)
{
    uint8_t ch;

    ch = RXBUF1;
#if defined(UART1_IEC62056_21_SUPPORT)
    if (inter_char_timeout == 0)
        iec62056_21_rx_restart();
    inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART1_DLT645_SUPPORT)
    dlt645_rx_byte(1, ch);
#endif
}

ISR(USART1TX, serial_tx_interrupt1)
{
    TXBUF1 = tx_msg[1].uint8[tx_msg[1].ptr++];
    if (tx_msg[1].ptr >= tx_msg[1].len)
    {
        /* Stop transmission */
        U1IE &= ~UTXIE1;
        tx_msg[1].ptr = 0;
        tx_msg[1].len = 0;
    }
}
        #elif defined(__MSP430_HAS_USCI_AB1__)
ISR(USCIAB1RX, serial_rx_interrupt1)
{
    uint8_t ch;

    ch = UCA1RXBUF;
#if defined(UART1_IEC62056_21_SUPPORT)
    //if (inter_char_timeout == 0)
    //    iec62056_21_rx_restart();
    //inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[0].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART1_DLT645_SUPPORT)
    dlt645_rx_byte(1, ch);
#endif
}

ISR(USCIAB1TX, serial_tx_interrupt1)
{
    UCA1TXBUF = tx_msg[1].buf.uint8[tx_msg[1].ptr++];
    if (tx_msg[1].ptr >= tx_msg[1].len)
    {
        /* Stop transmission */
        UC1IE &= ~UCA1TXIE;
        tx_msg[1].ptr = 0;
        tx_msg[1].len = 0;
    }
}
        #elif defined(__MSP430_HAS_USCI_A1__)
ISR(USCI_A1, serial_interrupt1)
{
    uint8_t ch;
    switch(__even_in_range(UCA1IV,USCI_UART_UCRXIFG))
    {
    case USCI_UART_UCRXIFG:
    ch = UCA1RXBUF;
#if defined(UART1_IEC62056_21_SUPPORT)
    //if (inter_char_timeout == 0)
    //    iec62056_21_rx_restart();
    //inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[1].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART0_DLT645_SUPPORT)
    dlt645_rx_byte(1, ch);
#endif
    break;
    case USCI_UART_UCTXIFG:
    UCA0TXBUF = tx_msg[1].buf.uint8[tx_msg[1].ptr++];
    if (tx_msg[1].ptr >= tx_msg[1].len)
    {
        /* Stop transmission */
        UCA1IE &= ~UCTXIE;
        tx_msg[1].ptr = 0;
        tx_msg[1].len = 0;
    }
    break;
    }

}
        #elif defined(__MSP430_HAS_EUSCI_A1__)
ISR(USCI_A1, serial_interrupt1)
{
    uint8_t ch;
    switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
    {
    case USCI_UART_UCRXIFG:
    ch = UCA1RXBUF;
#if defined(UART1_IEC62056_21_SUPPORT)
    //if (inter_char_timeout == 0)
    //    iec62056_21_rx_restart();
    //inter_char_timeout = SAMPLES_PER_10_SECONDS/200;
    if (tx_msg[1].len == 0)
        iec62056_21_rx_byte(ch & 0x7F);
#endif
#if defined(UART1_DLT645_SUPPORT)
    dlt645_rx_byte(1, ch);
#endif
    break;
    case USCI_UART_UCTXIFG:
    UCA1TXBUF = tx_msg[1].buf.uint8[tx_msg[1].ptr++];
    if (tx_msg[1].ptr >= tx_msg[1].len)
    {
        /* Stop transmission */
        UCA1IE &= ~UCTXIE;
        tx_msg[1].ptr = 0;
        tx_msg[1].len = 0;
    }
    break;
    case USCI_UART_UCSTTIFG:
    break;
    case USCI_UART_UCTXCPTIFG:
    break;
    }

}
        #else
            #error Device does not have a UART port 1
        #endif
    #endif

    #if defined(UART_PORT_2_SUPPORT)
        #if defined(__MSP430_HAS_USCI_AB2__)
ISR(USCIAB2TX, serial_tx_interrupt2)
{
    UCA2TXBUF = tx_msg[2].uint8[tx_msg[2].ptr++];
    if (tx_msg[2].ptr >= tx_msg[2].len)
    {
        /* Stop transmission */
        UC2IE &= ~UCA2TXIE;
        tx_msg[2].ptr = 0;
        tx_msg[2].len = 0;
    }
}
        #else
            #error Device does not have a UART port 2
        #endif
    #endif

    #if defined(UART_PORT_3_SUPPORT)
        #if defined(__MSP430_HAS_USCI_AB3__)
ISR(USCIAB3TX, serial_tx_interrupt3)
{
    UCA3TXBUF = tx_msg[3].uint8[tx_msg[3].ptr++];
    if (tx_msg[3].ptr >= tx_msg[3].len)
    {
        /* Stop transmission */
        UC3IE &= ~UCA3TXIE;
        tx_msg[3].ptr = 0;
        tx_msg[3].len = 0;
    }
}
        #else
            #error Device does not have a UART port 3
        #endif
    #endif

void send_message(int port, int len)
{
    tx_msg[port].ptr = 0;
    tx_msg[port].len = len;
    switch (port)
    {
    #if defined(UART_PORT_0_SUPPORT)
    case 0:
        #if defined(__MSP430_HAS_UART0__)
        U0IE |= UTXIE0;
        #elif defined(__MSP430_HAS_USCI_A0__) || defined(__MSP430_HAS_EUSCI_A0__)
        UCA0IE |= UCTXIE;
        #else
        UC0IE |= UCA0TXIE;
        #endif
        break;
    #endif
    #if defined(UART_PORT_1_SUPPORT)
    case 1:
        #if defined(__MSP430_HAS_UART1__)
        U1IE |= UTXIE1;
        #elif defined(__MSP430_HAS_USCI_A1__) || defined(__MSP430_HAS_EUSCI_A1__)
        UCA1IE |= UCTXIE;
        #else
        UC1IE |= UCA1TXIE;
        #endif
        break;
    #endif
    #if defined(UART_PORT_2_SUPPORT)
    case 2:
        UC2IE |= UCA2TXIE;
        break;
    #endif
    #if defined(UART_PORT_3_SUPPORT)
    case 3:
        UC3IE |= UCA3TXIE;
        break;
    #endif
    }
}

#endif
