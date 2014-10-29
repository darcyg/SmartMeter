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
//  $Id: spi.c,v 1.2 2008/10/28 10:33:08 a0754793 Exp $
//
//--------------------------------------------------------------------------

#include <stdint.h>

#include "mesh_if_defs.h"
#include "spi.h"
#include "spi_private.h"
#include "cc2500.h"

//----------------------------------------------------------------------------------
//   Generic SPI plugin module. Supports all known MSP430 SPI interfaces.
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//  USART0
//----------------------------------------------------------------------------------
#if defined HAL_SPI_INTERFACE_USART0

void spi_init(uint8_t clockrate)
{
    // Keep peripheral in reset state
    U0CTL = SWRST;

    // 8-bit SPI Master 3-pin mode, with SMCLK as clock source
    U0CTL |= (CHAR | SYNC | MM);
    U0TCTL |= (CKPH | SSEL1 | SSEL0 | STC);

    // Ignore clockrate argument for now, just use clock source/2
    U0BR0 = 0x02;
    U0BR1 = 0x00;
    U0MCTL = 0x00;

    // Enable SPI mode
    ME1 |= USPIE0;

    // Set up pins used by peripheral unit
    MCU_IO_PERIPHERAL(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_CLK_PORT,  HAL_SPI_CLK_PIN);
    MCU_IO_OUTPUT(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN, 1);

    // Release for operation
    U0CTL &= ~SWRST;
}

//----------------------------------------------------------------------------------
//  USART1
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USART1

void spi_init(uint8_t clockrate)
{
    // Keep peripheral in reset state
    U1CTL = SWRST;

    // 8-bit SPI Master 3-pin mode, with SMCLK as clock source
    U1CTL |= (CHAR | SYNC | MM);
    U1TCTL |= (CKPH | SSEL1 | SSEL0 | STC);

    // Ignore clockrate argument for now, just use clock source/2
    U1BR0 = 0x02;
    U1BR1 = 0x00;
    U1MCTL = 0x00;

    // Enable SPI mode
    ME2 |= USPIE1;

    // Set up pins used by peripheral unit
    MCU_IO_PERIPHERAL(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_CLK_PORT,  HAL_SPI_CLK_PIN);
    MCU_IO_OUTPUT(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN, 1);

    // Release for operation
    U1CTL &= ~SWRST;
}

//----------------------------------------------------------------------------------
//  USCIA0
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USCIA0

void spi_init(uint8_t clockrate)
{
    // Keep peripheral in reset state
    UCA0CTL1 = UCSWRST;

    // 8-bit SPI Master 3-pin mode, with SMCLK as clock source
    UCA0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;
    UCA0CTL1 |= (UCSSEL1 | UCSSEL0);

    // Ignore clockrate argument for now, just use clock source/2
    UCA0BR0 = 0x02;
    UCA0BR1 = 0x00;

    // Set up pins used by peripheral unit
    MCU_IO_PERIPHERAL(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_CLK_PORT,  HAL_SPI_CLK_PIN);
    MCU_IO_OUTPUT(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN, 1);

    // Release for operation
    UCA0CTL1 &= ~UCSWRST;
}

//----------------------------------------------------------------------------------
//  USCIB0
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USCIB0

void spi_init(uint8_t clockrate)
{
    // Keep peripheral in reset state
    UCB0CTL1 = UCSWRST;

    // 8-bit SPI Master 3-pin mode, with SMCLK as clock source
    UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;
    UCB0CTL1 |= (UCSSEL1 | UCSSEL0);

    // Ignore clockrate argument for now, just use clock source/2
    UCB0BR0 = 0x02;
    UCB0BR1 = 0x00;

    // Set up pins used by peripheral unit
    MCU_IO_PERIPHERAL(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_CLK_PORT,  HAL_SPI_CLK_PIN);
    MCU_IO_OUTPUT(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN, 1);

    // Release for operation
    UCB0CTL1 &= ~UCSWRST;
}

//----------------------------------------------------------------------------------
//  USI
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_USI

void spi_init(uint8_t clockrate)
{
    // Keep peripheral in reset state
    USICTL0 = USISWRST;

    // 8-bit SPI Master 3-pin mode
    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USIMST | USIOE);
    USICTL1 = USICKPH;

    // Use SMCLK as clock source, bit rate is clock source/2
    USICKCTL = USIDIV0 | USISSEL1;

    // Set up pins used by peripheral unit
    MCU_IO_PERIPHERAL(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
    MCU_IO_PERIPHERAL(HAL_SPI_CLK_PORT,  HAL_SPI_CLK_PIN);
    MCU_IO_OUTPUT(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN, 1);

    // Release for operation
    USICTL0 &= ~USISWRST;

    // Ensure SDO (SIMO) low
    USISRL = 0x00;
    USICNT = 1;
}

//----------------------------------------------------------------------------------
//  BITBANG
//----------------------------------------------------------------------------------
#elif defined HAL_SPI_INTERFACE_BITBANG

void spi_init(uint8_t clockrate)
{
    // Set up pins
    MCU_IO_INPUT(HAL_SPI_SOMI_PORT, HAL_SPI_SOMI_PIN);
    MCU_IO_OUTPUT(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN, 0);
    MCU_IO_OUTPUT(HAL_SPI_CLK_PORT,  HAL_SPI_CLK_PIN, 0);
    MCU_IO_OUTPUT(HAL_SPI_CS_PORT, HAL_SPI_CS_PIN, 1);
}

static uint8_t spi_bitbang_out(uint8_t data)
{
    uint8_t i;
    uint8_t rc = 0;

    for (i = 8;  i > 0;  i--)
    {
        // clock low
        MCU_IO_CLR(HAL_SPI_CLK_PORT, HAL_SPI_CLK_PIN);

        if (data & 0x80)
            MCU_IO_SET(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
        else
            MCU_IO_CLR(HAL_SPI_SIMO_PORT, HAL_SPI_SIMO_PIN);
        data <<= 1;

        // clock high
        MCU_IO_SET(HAL_SPI_CLK_PORT, HAL_SPI_CLK_PIN);

        rc <<= 1;
        if (HAL_SPI_SOMI_VAL)
            rc |= 0x01;
    }

    // clock low
    MCU_IO_CLR(HAL_SPI_CLK_PORT, HAL_SPI_CLK_PIN);

    return(rc);
}

static uint8_t spi_bitbang_in(void)
{
    uint8_t i;
    uint8_t data = 0;

    for (i = 8;  i > 0;  i--)
    {
        // clock high
        MCU_IO_SET(HAL_SPI_CLK_PORT, HAL_SPI_CLK_PIN);

        data <<= 1;
        if (HAL_SPI_SOMI_VAL)
            data |= 0x01;

        // clock low
        MCU_IO_CLR(HAL_SPI_CLK_PORT, HAL_SPI_CLK_PIN);
    }

    return(data);
}

//----------------------------------------------------------------------------------
//  Unknown interface
//----------------------------------------------------------------------------------
#else
#error "You must select an SPI interface (USART/USCI/USI/BITBANG)"
#endif

//-----------------------------------------------------------------------------
//  DESCRIPTION:
//    Busy wait function. Waits the specified number of microseconds. Use
//    assumptions about number of clock cycles needed for the various instructions.
//    The duration of one cycle depends on MCLK. In this HAL, it is set
//    to 4 MHz, thus 4 cycles per usec.
//
//    NB! This function is highly dependent on architecture and compiler!
//-----------------------------------------------------------------------------
static void wait_us(uint16_t usec) // 5 cycles for calling
{
    // The least we can wait is 3 usec:
    // ~1 usec for call, 1 for first compare and 1 for return

    while(usec > 3)       // 2 cycles for compare
    {                     // 2 cycles for jump
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        usec -= 2;        // 1 cycles for optimized decrement
    }
}                         // 4 cycles for returning

void spi_reset_device(void)
{
    // Toggle chip select signal
    HAL_SPI_CS_DEASSERT();
    wait_us(30);
    HAL_SPI_CS_ASSERT();
    wait_us(30);
    HAL_SPI_CS_DEASSERT();
    wait_us(45);

    // Send SRES command
    HAL_SPI_CS_ASSERT();
    while(HAL_SPI_SOMI_VAL())
        ;
    HAL_SPI_TXBUF_SET(CC2500_SRES);
    HAL_SPI_WAIT_TXFIN();

    // Wait for chip to finish internal reset
    while (HAL_SPI_SOMI_VAL())
        ;
    HAL_SPI_CS_DEASSERT();
}

//----------------------------------------------------------------------------------
//  void spi_send(uint8_t addr, const uint8_t *buffer, uint16 length)
//
//  DESCRIPTION:
//    Write data to device, starting at internal device address "addr".
//    The device will increment the address internally for every new byte
//    that is written. For single byte write, set length to 1.
//----------------------------------------------------------------------------------
uint8_t spi_send(uint8_t addr, const uint8_t *data, uint16_t length)
{
    uint16_t i;
    uint8_t rc;

    HAL_SPI_BEGIN();
#if defined(HAL_SPI_INTERFACE_BITBANG)
    rc = spi_bitbang_out(addr);
    for (i = 0;  i < length;  i++)
        spi_bitbang_out(data[i]);
#else
    HAL_SPI_TXBUF_SET(addr);
    HAL_SPI_WAIT_TXFIN();
    rc = HAL_SPI_RXBUF();
    for (i = 0;  i < length;  i++)
    {
        HAL_SPI_TXBUF_SET(data[i]);
        HAL_SPI_WAIT_TXBUF();
    }
#if !defined(HAL_SPI_INTERFACE_USI)
    HAL_SPI_WAIT_TXFIN();
#endif
#endif
    HAL_SPI_END();
    return rc;
}

//----------------------------------------------------------------------------------
//  uint8_t spi_send_byte(uint8_t cmd)
//
//  DESCRIPTION:
//    Special write function, writing only one byte (cmd) to the device.
//----------------------------------------------------------------------------------
uint8_t spi_send_byte(uint8_t cmd)
{
    uint8_t rc;

    HAL_SPI_BEGIN();
#if defined(HAL_SPI_INTERFACE_BITBANG)
    rc = spi_bitbang_out(cmd);
#else
    HAL_SPI_TXBUF_SET(cmd);
    HAL_SPI_WAIT_TXFIN();
    rc = HAL_SPI_RXBUF();
#endif
    HAL_SPI_END();
    return rc;
}

//----------------------------------------------------------------------------------
//  uint8_t spi_receive(uint8_t addr, uint8_t *data, uint16_t length)
//
//  DESCRIPTION:
//    Read data from device, starting at internal device address "addr".
//    The device will increment the address internally for every new byte
//    that is read. Note that the master device needs to write a dummy byte
//    (in this case 0) for every new byte in order to generate the clock to
//    clock out the data. For single byte read, set length to 1.
//----------------------------------------------------------------------------------
uint8_t spi_receive(uint8_t addr, uint8_t *data, uint16_t length)
{
    uint16_t i;
    uint8_t rc;

    HAL_SPI_BEGIN();
#if defined(HAL_SPI_INTERFACE_BITBANG)
    rc = spi_bitbang_out(addr);
    for (i = 0;  i < length;  i++)
        data[i] = spi_bitbang_in();
#else
    HAL_SPI_TXBUF_SET(addr);
    HAL_SPI_WAIT_TXFIN();
    rc = HAL_SPI_RXBUF();
    for (i = 0;  i < length;  i++)
    {
        HAL_SPI_TXBUF_SET(0);        // Dummy write to read data byte
        HAL_SPI_WAIT_TXFIN();
        data[i] = HAL_SPI_RXBUF();     // Store data from last data RX
    }
#endif
    HAL_SPI_END();
    return rc;
}
