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
//  File: iiceeprom.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: iiceeprom.c,v 1.5 2008/05/21 08:04:33 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

// These routines support a single I2C EEPROM or FRAM attached to
// two pins of a port on an MSP430.
//
//Please note:
//
// These routines have been tested with an Atmel 2401. They
// are intended to work with a range of I2C interface serial
// EEPROMs, but should be thoroughly tested against any
// other EEPROM before serious use.
//
// Since the EEPROM will be operating at a low voltage, when
// connected to the MSP430, this code is designed to operate
// at the worst case timing for the Atmel chips, at their
// lowest recommended supply voltage, and with the MSP430
// running at 8MHz. They could be tuned for faster operation
// in other environments.

#if defined(SUPPORT_IICEEPROM)

#define MAX_IIC_TRIES 200

#if defined(EEPROM_IS_REALLY_FRAM)
/* No delay is needed for FRAMs */
#define delay(x) /**/
#else
/* This delay is needed for EEPROMs */
static void delay(int i)
{
    for (  ;  i;  i--)
    {
        _NOP();
        _NOP();
        _NOP();
        _NOP();
    }
}
#endif

static void iic_start(void)
{
    /* Send a start to the EEPROM */
    /* A start is the data bit falling while the clock is high */
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
    delay(2);   //At least ???
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
    delay(5);   //At least 4.7us
    IICEEPROM_PORT_DIR |= IICEEPROM_SDA_BIT;
    delay(5);   //At least 4.7us
    IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
}

static void iic_stop(void)
{
    /* Send a stop to the EEPROM */
    /* A stop is the data bit rising while the clock is high */
    IICEEPROM_PORT_DIR |= IICEEPROM_SDA_BIT;
    delay(2);   //At least ???
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
    delay(5);   //At least 4.7us
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
    delay(5);   //At least 4.7us
    IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
}

static int iic_send(uint8_t data)
{
    int i;
    
    /* Send 8 bits of data */
    IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
    for (i = 8;  i > 0;  i--)
    {
        /* The data can now change without delay */
        if (data & 0x80)
            IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
        else
            IICEEPROM_PORT_DIR |= IICEEPROM_SDA_BIT;
        data <<= 1;
        /* Pulse the clock high while the data bit is steady */
        delay(5);   //At least 4.7us
        IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
        delay(5);   //At least 4.7us
        IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
    }
    /* Check the acknowledgement from the EEPROM */
    /* Pulse the clock high, and see what the device says while the clock is high */
    delay(5);
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
    delay(5);
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
    delay(5);
    i = IICEEPROM_PORT_IN & IICEEPROM_SDA_BIT;
    IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
    delay(4);
    return i;
}

static uint8_t iic_receive(int ok)
{
    uint8_t data;
    int i;

    /* Get 8 bits of data */
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;     //Input
    data = 0;
    for (i = 8;  i > 0;  i--)
    {
        IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
        delay(5);   //At least 4.7us
        data <<= 1;
        if (IICEEPROM_PORT_IN & IICEEPROM_SDA_BIT)
            data |= 0x01;
        IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
        delay(5);   //At least 4.7us
    }
    /* Send the acknowledgement to the EEPROM */
    if (ok)
        IICEEPROM_PORT_DIR |= IICEEPROM_SDA_BIT;
    else
        IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
    delay(4);
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
    delay(4);
    IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
    return data;
}

static int test_SDA(void)
{
    int i;
  
    iic_stop();
    IICEEPROM_PORT_DIR &= ~IICEEPROM_SDA_BIT;
    delay(4);
    for (i = 16;  i > 0;  i--)
    {
        delay(5);
        if (!(IICEEPROM_PORT_IN & IICEEPROM_SDA_BIT))
            break;
    }
    return i;
}

int iicEEPROM_read(uint16_t addr, void *dat, int len)
{
    int i;
    int j;
    uint8_t *p;

    for (i = 0;  i < MAX_IIC_TRIES;  ++i)
    {
        if (i)
        {
            /* Read FALSE, retry */
            if (test_SDA())
                continue;
        }
        iic_start();
#if EEPROM_PAGE_SIZE == 32
        if (iic_send(0xA0)  ||  iic_send(addr/0x100)  ||  iic_send(addr))
            continue;
#else
        if (iic_send(0xA0 | ((uint8_t)(addr/0x100)*2))  ||  iic_send(addr))
            continue;
#endif
        p = (uint8_t *) dat;

        iic_start();
#if EEPROM_PAGE_SIZE == 32
        if (iic_send(0xA1))
            continue;
#else
        if (iic_send(0xA1 | ((uint8_t)(addr/0x100)*2)))
            continue;
#endif
        for (j = len;  j > 0;  j--)
            *p++ = iic_receive(TRUE);
        *p = iic_receive(FALSE);
        iic_stop();
        return TRUE;
    }
    iic_stop();
    return FALSE;
}

int iicEEPROM_write(uint16_t addr, void *dat, int len)
{
    int i;
    int j;
    int section_len;
    uint8_t *p;
    uint8_t *q;

    /* If the write spreads across pages in the EEPROM, we need to split the write
       into sections. */
    q = (uint8_t *) dat;
    while (len > 0)
    {
        if (addr + len > ((addr + EEPROM_PAGE_SIZE) & ~(EEPROM_PAGE_SIZE - 1)))
            section_len = ((addr + EEPROM_PAGE_SIZE) & ~(EEPROM_PAGE_SIZE - 1)) - addr;
        else
            section_len = len;
        for (i = 0;  i < MAX_IIC_TRIES;  ++i)
        {
            if (i)
            {
                /* Write FALSE, retry */
                if (test_SDA())
                    continue;
            }

            iic_start();
#if EEPROM_PAGE_SIZE == 32
            if (iic_send(0xA0)  ||  iic_send(addr/0x100)  ||  iic_send(addr))
                continue;
#else
            if (iic_send(0xA0 | ((uint8_t)(addr/0x100)*2))  ||  iic_send(addr))
                continue;
#endif
            p = q;
            for (j = section_len;  j > 0;  j--)
            {
                if (iic_send(*p++))
                    break;
            }
            if (j == 0)
                break;
            iic_stop();
        }
        iic_stop();
        if (i >= MAX_IIC_TRIES)
            return FALSE;
        len -= section_len;
        addr += section_len;
        q += section_len;
    }
    return TRUE;
}

int iicEEPROM_init(void)
{
    int i;

    /* While idling, the EEPROM clock should be low */
    IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
    /* If we happen to have restarted in the middle of a read from
       the EEPROM/FRAM, we need to regain control of the device. If we
       give it enough clocks, and do no acknowledge it we should get out
       of any odd conditions. Then we do a stop, and we should be OK. If
       the device was mid write when the restart occurred we cannot really
       act in a clean way. */
    delay(5);   //At least 4.7us
    for (i = 0;  i < 16;  i++)
    {
        IICEEPROM_PORT_DIR &= ~IICEEPROM_SCL_BIT;
        delay(5);   //At least 4.7us
        IICEEPROM_PORT_DIR |= IICEEPROM_SCL_BIT;
        delay(5);   //At least 4.7us
    }
    iic_stop();
    return 0;
}
#endif
