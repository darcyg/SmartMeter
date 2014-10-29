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
//	File: 
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: mesh_if_defs.h,v 1.1 2008/10/28 10:33:07 a0754793 Exp $
//
//--------------------------------------------------------------------------

#ifndef MESH_IF_DEFS_H
#define MESH_IF_DEFS_H

typedef void (*ISR_FUNC_PTR)(void);

enum
{
    HAL_DIGIO_INPUT,
    HAL_DIGIO_OUTPUT
};

enum
{
    HAL_DIGIO_INT_FALLING_EDGE,
    HAL_DIGIO_INT_RISING_EDGE
};


typedef struct
{
    uint8_t port;     // Port number
    uint8_t pin;      // Pin number
    uint8_t pin_bm;   // Pin bitmask
    uint8_t dir;      // Direction (input or output)
    uint8_t initval;  // Initial value
} digioConfig;


#define HAL_DIGIO_OK        0
#define HAL_DIGIO_ERROR     (~0)

uint8_t halDigioConfig(const digioConfig *p);

uint8_t halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func);
uint8_t halDigioIntEnable(const digioConfig *p);
uint8_t halDigioIntDisable(const digioConfig *p);
uint8_t halDigioIntClear(const digioConfig *p);
uint8_t halDigioIntSetEdge(const digioConfig *p, uint8_t edge);

#include <io.h>

#define SHORT_FORM           1

//----------------------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------------------

/*
 *  This macro is for use by other macros to form a fully valid C statement.
 *  Without this, the if/else conditionals could show unexpected behavior.
 *
 *  For example, use...
 *    #define SET_REGS()  st( ioreg1 = 0; ioreg2 = 0; )
 *  instead of ...
 *    #define SET_REGS()  { ioreg1 = 0; ioreg2 = 0; }
 *  or
 *    #define  SET_REGS()    ioreg1 = 0; ioreg2 = 0;
 *  The last macro would not behave as expected in the if/else construct.
 *  The second to last macro will cause a compiler error in certain uses
 *  of if/else construct
 *
 *  It is not necessary, or recommended, to use this macro where there is
 *  already a valid C statement.  For example, the following is redundant...
 *    #define CALL_FUNC()   st(  func();  )
 *  This should simply be...
 *    #define CALL_FUNC()   func()
 *
 * (The while condition below evaluates false without generating a
 *  constant-controlling-loop type of warning on most compilers.)
 */
#define st(x)      do { x } while (__LINE__ == -1)

//----------------------------------------------------------------------------------
//  Easy access, zero overhead LED macros
//  Adapted to hardware interface on MSP430 Experimenter Board
//----------------------------------------------------------------------------------
#define HAL_LED_SET_1   (P2OUT |= BIT1)

#define HAL_LED_CLR_1   (P2OUT &= ~BIT1)

#define HAL_LED_TGL_1   (P2OUT ^= BIT1)



//----------------------------------------------------------------------------------
//  Port and pin where GDO0 and GDO2 from CC1100/CC2500 are connected
//----------------------------------------------------------------------------------
#if defined(__MSP430_437__)
#define HAL_IO_GDO0_PORT  2
#define HAL_IO_GDO0_PIN   7
#define HAL_IO_GDO2_PORT  2
#define HAL_IO_GDO2_PIN   6
#else
#define HAL_IO_GDO0_PORT  1
#define HAL_IO_GDO0_PIN   2
#define HAL_IO_GDO2_PORT  1
#define HAL_IO_GDO2_PIN   3
#endif

//----------------------------------------------------------------------------------
//  Define ports and pins used by SPI interface to CC1100/CC2500
//----------------------------------------------------------------------------------
#if defined(__MSP430_437__)
#define HAL_SPI_SOMI_PORT 3
#define HAL_SPI_SOMI_PIN  2
#define HAL_SPI_SIMO_PORT 3
#define HAL_SPI_SIMO_PIN  1
#define HAL_SPI_CLK_PORT  3
#define HAL_SPI_CLK_PIN   3
#define HAL_SPI_CS_PORT   3
#define HAL_SPI_CS_PIN    0
#else
#define HAL_SPI_SOMI_PORT 4
#define HAL_SPI_SOMI_PIN  4
#define HAL_SPI_SIMO_PORT 4
#define HAL_SPI_SIMO_PIN  3
#define HAL_SPI_CLK_PORT  4
#define HAL_SPI_CLK_PIN   5
#define HAL_SPI_CS_PORT   4
#define HAL_SPI_CS_PIN    2
#endif

//----------------------------------------------------------------------------------
// Select interface on MSP430 to use for SPI (define only one!)
//----------------------------------------------------------------------------------

// #define HAL_SPI_INTERFACE_USART0
// #define HAL_SPI_INTERFACE_USART1
// #define HAL_SPI_INTERFACE_USCIA0
// #define HAL_SPI_INTERFACE_USCIA1
// #define HAL_SPI_INTERFACE_USCIB0
// #define HAL_SPI_INTERFACE_USCIB1
// #define HAL_SPI_INTERFACE_USI
// #define HAL_SPI_INTERFACE_BITBANG

#ifdef __cplusplus
extern "C" {
#endif

extern const digioConfig pinLed4;
extern const digioConfig pinGDO0;
extern const digioConfig pinGDO2;

void meshnet_hardware_init(void);

//----------------------------------------------------------------------------------
//  Macros for simple configuration of IO pins on MSP430
//----------------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL(port, pin)   MCU_IO_PERIPHERAL_PREP(port, pin)
#define MCU_IO_INPUT(port, pin)        MCU_IO_INPUT_PREP(port, pin)
#define MCU_IO_OUTPUT(port, pin, val)  MCU_IO_OUTPUT_PREP(port, pin, val)
#define MCU_IO_SET(port, pin)          MCU_IO_SET_PREP(port, pin)
#define MCU_IO_CLR(port, pin)          MCU_IO_CLR_PREP(port, pin)
#define MCU_IO_GET(port, pin)          MCU_IO_GET_PREP(port, pin)

//----------------------------------------------------------------------------------
//  Macros for internal use (the macros above need a new round in the preprocessor)
//----------------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL_PREP(port, pin)  st( P##port##SEL |= BIT##pin##; )
#define MCU_IO_INPUT_PREP(port, pin)       st( P##port##SEL &= ~BIT##pin##; \
                                               P##port##DIR &= ~BIT##pin##; )
#define MCU_IO_OUTPUT_PREP(port, pin, val) st( P##port##SEL &= ~BIT##pin##; \
                                               if (val) \
                                                   { P##port##OUT |= BIT##pin##; } \
                                               else \
                                                   { P##port##OUT &= ~BIT##pin##; } \
                                               P##port##DIR |= BIT##pin##; )
#define MCU_IO_SET_PREP(port, pin)         st( P##port##OUT |= BIT##pin##; )
#define MCU_IO_CLR_PREP(port, pin)         st( P##port##OUT &= ~BIT##pin##; )
#define MCU_IO_GET_PREP(port, pin)         (P##port##IN & BIT##pin##)

#ifdef __cplusplus
}
#endif

#endif
