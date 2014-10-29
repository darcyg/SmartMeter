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
//  $Id: spi_private.h,v 1.1 2008/10/28 10:15:12 a0754793 Exp $
//
//--------------------------------------------------------------------------

#ifndef SPI_PRIVATE_H
#define SPI_PRIVATE_H

//----------------------------------------------------------------------------------
//   Generic SPI plugin module. Supports all known MSP430 SPI interfaces.
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//   Common Macros
//----------------------------------------------------------------------------------
#define HAL_SPI_CS_DEASSERT()   MCU_IO_SET(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN)
#define HAL_SPI_CS_ASSERT()     MCU_IO_CLR(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN)
#define HAL_SPI_SOMI_VAL()      MCU_IO_GET(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN)
#define HAL_SPI_BEGIN()         st(HAL_SPI_CS_ASSERT(); while(HAL_SPI_SOMI_VAL());)
#define HAL_SPI_END()           st(HAL_SPI_CS_DEASSERT();)


//----------------------------------------------------------------------------------
//  USART0
//----------------------------------------------------------------------------------
#if defined HAL_SPI_INTERFACE_USART0

#define HAL_SPI_WAIT_RXFIN()    st(while(!(IFG1 & URXIFG0));)
#define HAL_SPI_WAIT_TXFIN()    st(while(!(U0TCTL & TXEPT));)
#define HAL_SPI_WAIT_TXBUF()    st(while(!(IFG1 & UTXIFG0));)
#define HAL_SPI_RXBUF()         U0RXBUF
#define HAL_SPI_TXBUF_SET(x)    st(U0TXBUF = (x);)

//----------------------------------------------------------------------------------
//  USART1
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USART1

#define HAL_SPI_WAIT_RXFIN()    st(while(!(IFG2 & URXIFG1));)
#define HAL_SPI_WAIT_TXFIN()    st(while(!(U1TCTL & TXEPT));)
#define HAL_SPI_WAIT_TXBUF()    st(while(!(IFG2 & UTXIFG1));)
#define HAL_SPI_RXBUF()         U1RXBUF
#define HAL_SPI_TXBUF_SET(x)    st(U1TXBUF = (x);)

//----------------------------------------------------------------------------------
//  USCIA0
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USCIA0

#define HAL_SPI_WAIT_RXFIN()    st(while(!(IFG2 & UCA0RXIFG));)
#define HAL_SPI_WAIT_TXFIN()    st(while(UCA0STAT & UCBUSY);)
#define HAL_SPI_WAIT_TXBUF()    st(while(!(IFG2 & UCA0TXIFG));)
#define HAL_SPI_RXBUF()         UCA0RXBUF
#define HAL_SPI_TXBUF_SET(x)    st(UCA0TXBUF = (x);)

//----------------------------------------------------------------------------------
//  USCIB0
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USCIB0

#define HAL_SPI_WAIT_RXFIN()    st(while(!(IFG2 & UCB0RXIFG));)
#define HAL_SPI_WAIT_TXFIN()    st(while(UCB0STAT & UCBUSY);)
#define HAL_SPI_WAIT_TXBUF()    st(while(!(IFG2 & UCB0TXIFG));)
#define HAL_SPI_RXBUF()         UCB0RXBUF
#define HAL_SPI_TXBUF_SET(x)    st(UCB0TXBUF = (x);)

//----------------------------------------------------------------------------------
//  USI
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USI

#define HAL_SPI_WAIT_RXFIN()    st(USICNT = 8; while(!(USICTL1&USIIFG));)
#define HAL_SPI_WAIT_TXFIN()    st(USICNT = 8; while(!(USICTL1&USIIFG));)
#define HAL_SPI_WAIT_TXBUF()    st(USICNT = 8; while(!(USICTL1&USIIFG));)
#define HAL_SPI_RXBUF()         USISRL
#define HAL_SPI_TXBUF_SET(x)    st(USISRL = (x);)

//----------------------------------------------------------------------------------
//  BITBANG
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_BITBANG

uint8 spi_bitbang_out(uint8 data);
uint8 spi_bitbang_in(void);

#define HAL_SPI_WAIT_RXFIN()   asm(" nop")
#define HAL_SPI_WAIT_TXFIN()   asm(" nop")
#define HAL_SPI_WAIT_TXBUF()   asm(" nop")
#define HAL_SPI_RXBUF()
#define HAL_SPI_TXBUF_SET(x) spi_bitbang_out((x))

//----------------------------------------------------------------------------------
//  Unknown interface
//----------------------------------------------------------------------------------
#else
#error "You must select an SPI interface (USART/USCI/USI/BITBANG)"
#endif

#endif
