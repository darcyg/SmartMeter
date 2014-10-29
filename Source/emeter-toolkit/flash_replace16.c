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
//  File: flash_replace16.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: flash_replace16.c,v 1.3 2007/08/07 05:53:43 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <signal.h>
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

#if defined(__MSP430__)
#define FSEG_A 0x01080      // Flash Segment A start address
#define FSEG_B 0x01000      // Flash Segment B start address

// ***********************************************************************
//  This routine makes the flash looks like EEPROM. It will erase and
//  replace just one word
//  This routine copies will erase SEGA and then image SEGB to SEGA
//  It will then erase SEGB and copy from SEGA back to SEGB all 128 bytes
//  except the one to be replaced.
// ***********************************************************************
void flash_replace16(int16_t *ptr, int16_t word)
{
    int *read_ptr;
    int *write_ptr;
    int w;
  
    //Optimise the case where the new and old values are the same
    if (*ptr == word)
        return;
    flash_clr((int *) FSEG_A);

    _DINT();
    //Set to write mode to prepare for copy
    FCTL3 = FWKEY;          /* Lock = 0 */
    FCTL1 = FWKEY | WRT;

    //Copy block B to A
    read_ptr = (int *) FSEG_B;
    write_ptr = (int *) FSEG_A;
    for (w = 0;  w < 64;  w++)
        *write_ptr++ = *read_ptr++;
    flash_clr((int *) FSEG_B);

    //Set to write mode to prepare for copy
    FCTL3 = FWKEY;          /* Lock = 0 */
    FCTL1 = FWKEY | WRT;

    //Copy block A to B, slipping in the new value at the right location
    read_ptr = (int *) FSEG_A;
    write_ptr = (int *) FSEG_B;
    for (w = 0;  w < 64;  w++, read_ptr++, write_ptr++)
    {
        if (write_ptr == ptr)
            *write_ptr = word;
        else
            *write_ptr = *read_ptr;
    }
    flash_secure();
    _EINT();
}
#endif
