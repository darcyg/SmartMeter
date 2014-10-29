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
//  File: rtc.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: rtc.c,v 1.5 2007/08/07 05:53:48 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

const int8_t month_lengths[13] =
{
    00,
    31, 28, 31,
    30, 31, 30,
    31, 31, 30,
    31, 30, 31
};

void set_rtc_sumcheck(void)
{
    rtc.sumcheck = ~(rtc.second + rtc.minute + rtc.hour + rtc.day + rtc.month + rtc.year);
}

/* Bump the RTC by a second, and return an indication of the most significant
   element of the date to have changed. Maintain a sumcheck byte to help check
   for corrupted RTC settings after a reset. */
int bump_rtc(void)
{
    /* First check the consistency of the current rtc setting. If it is inconsistent,
       (i.e. has a bad sumcheck) do not bump it. */
    if (!check_rtc_sumcheck())
        return RTC_INCONSISTENT;
    if (++rtc.second < 60)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_SECOND;
    }
    rtc.second = 0;
    if (++rtc.minute < 60)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_MINUTE;
    }
    rtc.minute = 0;
    if (++rtc.hour < 24)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_HOUR;
    }
    rtc.hour = 0;
    if ((rtc.month == 2  &&  (rtc.year & 3) == 0  &&  rtc.day < 29)
        ||
        rtc.day < month_lengths[rtc.month])
    {
        ++rtc.day;
        set_rtc_sumcheck();
        return RTC_CHANGED_DAY;
    }
    rtc.day = 1;
    if (++rtc.month <= 12)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_MONTH;
    }
    rtc.month = 1;
    ++rtc.year;
    set_rtc_sumcheck();
    return RTC_CHANGED_YEAR;
}

/* This can be used to check the integrity of the RTC after a reset */
int check_rtc_sumcheck(void)
{
    return rtc.sumcheck == ((~(rtc.second + rtc.minute + rtc.hour + rtc.day + rtc.month + rtc.year)) & 0xFF);
}

/* Return the current day of the week, based on the RTC. */
int weekday(void)
{
    int i;
    int days;

    /* This works for years 2000-2099 */
    /* Result is 0=Sunday, 1=Monday, etc. */
    /* Allow for the day of the month */
    days = rtc.day + 6;
    /* Allow for the months to date this year... */
    for (i = 1;  i < rtc.month;  i++)
        days += month_lengths[i];
    /* ...with a little offset if we are early in a leap year */
    if ((rtc.year & 0x03) == 0  &&  rtc.month <= 2)
        days--;
    /* Allow for the years... */
    days += rtc.year;
    /* ...and a little extra for the leap years */
    days += (rtc.year >> 2);
    days %= 7;
    return days;
}
