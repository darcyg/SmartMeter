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
//  $Id: emeter-iiceeprom.c,v 1.4 2009/01/08 08:30:37 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if !defined(__MSP430__)
#include <stdio.h>
#endif
#if defined(__GNUC__)
#include <signal.h>
#endif
#include <io.h>
#include <emeter-toolkit.h>

#include "emeter-structs.h"

#if defined(EXTERNAL_EEPROM_SUPPORT)

static int external_eeprom_startup(void)
{
#if defined(SELF_TEST_SUPPORT)
    int error;
    int i;
#endif
#if 0
    int j;
    int k;
    uint16_t buf[64];
#endif

#if 0
    while ((CACTL2 & CAOUT))
        kick_watchdog();
#endif

#if 0
    /* EEPROM interface test */
    /* Go to 8MHz */
    SCFI0 = FN_3 | FLLD_4;
    SCFQCTL = 64 - 1;

    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
    for (k = 0;  k < 32767;  k++)
    {
        LCDdec16(k, FIRST_POSITION + 1, NUMBER_WIDTH - 2, 0);
        for (j = 0;  j < 8192;  j += 128)
        {
            for (i = 0;  i < 64;  i++)
                buf[i] = j + (i << 1);
            if (!iicEEPROM_write(j, buf, 128))
            {
                LCDdec16(9991, FIRST_POSITION + 1, NUMBER_WIDTH - 2, 0);
                for (;;);
            }
        }
        for (j = 0;  j < 8192;  j += 128)
        {
            if (!iicEEPROM_read(j, buf, 128))
            {
                LCDdec16(9992, FIRST_POSITION + 1, NUMBER_WIDTH - 2, 0);
                for (;;);
            }
            for (i = 0;  i < 64;  i++)
            {
                if (buf[i] != j + (i << 1))
                {
                    LCDdec16(9993, FIRST_POSITION + 1, NUMBER_WIDTH - 2, 0);
                    for (;;);
                }
            }
        }
    }
#endif

#if defined(SELF_TEST_SUPPORT)
    error = 0;
    /* Allow several retries of reading the EEPROM */
    for (i = 0;  i < 5;  i++)
    {
    }
    if (error)
    {
        record_meter_failure(METER_FAILURE_EEPROM_COMMUNICATION);
        return TRUE;
    }
    if (error)
        record_meter_failure(METER_FAILURE_EEPROM_CONTENTS);
#endif
    return TRUE;
}

void external_eeprom_init(void)
{
#if 0
    CACTL1 = CAREF_2 | CAEX | CAON;
    CACTL2 = P2CA1 | CAF;
    P1SEL |= BIT7;
    CAPD |= BIT7;

    while ((CACTL2 & CAOUT))
        kick_watchdog();
#endif

    iicEEPROM_init();
    external_eeprom_startup();
}
#endif
