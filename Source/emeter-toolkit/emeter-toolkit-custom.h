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
//  File: emeter-toolkit-custom.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-toolkit-custom.h,v 1.7 2007/08/07 05:53:42 a0754793 Exp $
//
//--------------------------------------------------------------------------

//  Declarations to customise the build of this library for a particular
//  MCU and peripheral set.
#if !defined(_EMETER_TOOLKIT_CUSTOM_H_)
#define _EMETER_TOOLKIT_CUSTOM_H_

#include "io.h"

#if defined(__MSP430__)
#define SUPPORT_IICEEPROM       1
#else
#define SUPPORT_IICEEPROM       0
#endif

#if     defined(__MSP430_E423__) || defined(__MSP430_E425__) || defined(__MSP430_E427__) \
    ||  defined(__MSP430_E435__) || defined(__MSP430_E436__) || defined(__MSP430_E437__) \
    ||  defined(__MSP430_E447__) || defined(__MSP430_E448__) || defined(__MSP430_E449__)
#define SUPPORT_IICEEPROM_1     1
#define SUPPORT_IICEEPROM_11    0
#define SUPPORT_IICEEPROM_2     0
#elif defined(__MSP430_1232__)
#define SUPPORT_LCD_MODULE_1    1
#define SUPPORT_LCD_MODULE_2    0
#define SUPPORT_IICEEPROM_1     1
#define SUPPORT_IICEEPROM_11    0
#define SUPPORT_IICEEPROM_2     0
#else
#define SUPPORT_LCD_MODULE_1    0
#define SUPPORT_LCD_MODULE_2    1
#define SUPPORT_IICEEPROM_1     0
#define SUPPORT_IICEEPROM_11    0
#define SUPPORT_IICEEPROM_2     1
#endif

//The following are bit and port definitions for the LCD
//display interface. Assuming all the bits are on the same
//port, then changing these values and recompiling should be
//all that is needed to change the LCD interface to use
//different pins on the MSP430.
#if SUPPORT_LCD_MODULE_1
#define LCD_CS_BIT      BIT3
#define LCD_DATA_BIT    BIT2
#define LCD_WR_BIT      BIT1
#define LCD_RD_BIT      BIT0

#define LCD_PORT_OUT    P3OUT
#define LCD_PORT_IN     P3IN
#define LCD_PORT_DIR    P3DIR
//The number of character cells on the display
#define LCD_CELLS       8
#endif

#if SUPPORT_LCD_MODULE_2
#define LCD_CS_BIT      BIT0
#define LCD_DATA_BIT    BIT1
#define LCD_WR_BIT      BIT2
#define LCD_RD_BIT      BIT3

#define LCD_PORT_OUT    P5OUT
#define LCD_PORT_IN     P5IN
#define LCD_PORT_DIR    P5DIR

//The number of character cells on the display
#define LCD_CELLS       8
#endif

#if SUPPORT_IICEEPROM

#define EEPROM_PAGE_SIZE 32         // Assumed to be an 8kx8 device, or larger
//#define EEPROM_PAGE_SIZE 16       // Assumed to be less than an 8kx8 device
//#define EEPROM_PAGE_SIZE 8        // Assumed to be less than an 8kx8 device
//#define EEPROM_IS_REALLY_FRAM   1 // FRAMs can be read and written much faster. This changes the timing

//The following are bit and port definitions for the I2C
//interface to the EEPROM. Assuming all the bits are on the
//same port, then changing these values and recompiling should
//be all that is needed to change the I2C interface to use
//different pins on the MSP430.
#if SUPPORT_IICEEPROM_1
#define IICEEPROM_SDA_BIT   BIT3
#define IICEEPROM_SCL_BIT   BIT2

#define IICEEPROM_PORT_OUT  P2OUT
#define IICEEPROM_PORT_IN   P2IN
#define IICEEPROM_PORT_DIR  P2DIR
#endif

#if SUPPORT_IICEEPROM_11
#define IICEEPROM_SDA_BIT   BIT2
#define IICEEPROM_SCL_BIT   BIT1

#define IICEEPROM_PORT_OUT  P1OUT
#define IICEEPROM_PORT_IN   P1IN
#define IICEEPROM_PORT_DIR  P1DIR
#endif

#if SUPPORT_IICEEPROM_2
#define IICEEPROM_SDA_BIT   BIT1
#define IICEEPROM_SCL_BIT   BIT0

#define IICEEPROM_PORT_OUT  P2OUT
#define IICEEPROM_PORT_IN   P2IN
#define IICEEPROM_PORT_DIR  P2DIR
#endif
#endif

#endif
