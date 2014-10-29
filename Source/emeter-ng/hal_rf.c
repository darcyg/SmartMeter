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
//  $Id: hal_rf.c,v 1.1 2008/10/28 10:33:07 a0754793 Exp $
//
//--------------------------------------------------------------------------

#include <stdint.h>

#include "spi.h"
#include "hal_rf.h"
#include "cc2500.h"

//----------------------------------------------------------------------------------
//  void halRfResetChip(void)
//
//  DESCRIPTION:
//    Resets the chip using the procedure described in the datasheet.
//----------------------------------------------------------------------------------
void halRfResetChip(void)
{
     spi_reset_device();
}

//----------------------------------------------------------------------------------
//  void halRfConfig(const lpw_config_t *rfConfig, const uint8* rfPaTable, uint8 rfPaTableLen)
//
//  DESCRIPTION:
//    Used to configure the CC1100/CC2500 registers with exported register
//    settings from SmartRF Studio.
//
//  ARGUMENTS:
//    rfConfig     - register settings (as exported from SmartRF Studio)
//    rfPaTable    - array of PA table values (from SmartRF Studio)
//    rfPaTableLen - length of PA table
//
//----------------------------------------------------------------------------------
void halRfConfig(const lpw_config_t *rfConfig, const uint8_t *rfPaTable, uint8_t rfPaTableLen)
{
    halRfWriteReg(CC2500_FSCTRL1,  rfConfig->fsctrl1);    // Frequency synthesizer control.
    halRfWriteReg(CC2500_FSCTRL0,  rfConfig->fsctrl0);    // Frequency synthesizer control.
    halRfWriteReg(CC2500_FREQ2,    rfConfig->freq2);      // Frequency control word, high byte.
    halRfWriteReg(CC2500_FREQ1,    rfConfig->freq1);      // Frequency control word, middle byte.
    halRfWriteReg(CC2500_FREQ0,    rfConfig->freq0);      // Frequency control word, low byte.
    halRfWriteReg(CC2500_MDMCFG4,  rfConfig->mdmcfg4);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG3,  rfConfig->mdmcfg3);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG2,  rfConfig->mdmcfg2);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG1,  rfConfig->mdmcfg1);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG0,  rfConfig->mdmcfg0);    // Modem configuration.
    halRfWriteReg(CC2500_CHANNR,   rfConfig->channr);     // Channel number.
    halRfWriteReg(CC2500_DEVIATN,  rfConfig->deviatn);    // Modem deviation setting (when FSK modulation is enabled).
    halRfWriteReg(CC2500_FREND1,   rfConfig->frend1);     // Front end RX configuration.
    halRfWriteReg(CC2500_FREND0,   rfConfig->frend0);     // Front end RX configuration.
    halRfWriteReg(CC2500_MCSM0,    rfConfig->mcsm0);      // Main Radio Control State Machine configuration.
    halRfWriteReg(CC2500_FOCCFG,   rfConfig->foccfg);     // Frequency Offset Compensation Configuration.
    halRfWriteReg(CC2500_BSCFG,    rfConfig->bscfg);      // Bit synchronization Configuration.
    halRfWriteReg(CC2500_AGCCTRL2, rfConfig->agcctrl2);   // AGC control.
    halRfWriteReg(CC2500_AGCCTRL1, rfConfig->agcctrl1);   // AGC control.
    halRfWriteReg(CC2500_AGCCTRL0, rfConfig->agcctrl0);   // AGC control.
    halRfWriteReg(CC2500_FSCAL3,   rfConfig->fscal3);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSCAL2,   rfConfig->fscal2);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSCAL1,   rfConfig->fscal1);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSCAL0,   rfConfig->fscal0);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSTEST,   rfConfig->fstest);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_TEST2,    rfConfig->test2);      // Various test settings.
    halRfWriteReg(CC2500_TEST1,    rfConfig->test1);      // Various test settings.
    halRfWriteReg(CC2500_TEST0,    rfConfig->test0);      // Various test settings.
    halRfWriteReg(CC2500_IOCFG2,   rfConfig->iocfg2);     // GDO2 output pin configuration.
    halRfWriteReg(CC2500_IOCFG0,   rfConfig->iocfg0);     // GDO0 output pin configuration.
    halRfWriteReg(CC2500_PKTCTRL1, rfConfig->pktctrl1);   // Packet automation control.
    halRfWriteReg(CC2500_PKTCTRL0, rfConfig->pktctrl0);   // Packet automation control.
    halRfWriteReg(CC2500_ADDR,     rfConfig->addr);       // Device address.
    halRfWriteReg(CC2500_PKTLEN,   rfConfig->pktlen);     // Packet length.

    spi_send(CC2500_PATABLE | CC2500_WRITE_BURST, rfPaTable, rfPaTableLen);
}

#if 0
//----------------------------------------------------------------------------------
//  void  halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8* rfPaTable, uint8 rfPaTableLen)
//
//  DESCRIPTION:
//    Used to configure all of the CC1100/CC2500 registers in one burst write.
//
//  ARGUMENTS:
//    rfConfig     - register settings
//    rfPaTable    - array of PA table values (from SmartRF Studio)
//    rfPaTableLen - length of PA table
//
//----------------------------------------------------------------------------------
void halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8_t *rfPaTable, uint8_t rfPaTableLen)
{
    spi_send(CC2500_IOCFG2  | CC2500_WRITE_BURST, rfConfig, sizeof(rfConfig));
    spi_send(CC2500_PATABLE | CC2500_WRITE_BURST, rfPaTable, rfPaTableLen);
}
#endif

//----------------------------------------------------------------------------------
//  uint8 halRfGetChipId(void)
//----------------------------------------------------------------------------------
uint8_t halRfGetChipId(void)
{
    return halRfReadStatusReg(CC2500_PARTNUM);
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetChipVer(void)
//----------------------------------------------------------------------------------
uint8_t halRfGetChipVer(void)
{
    return halRfReadStatusReg(CC2500_VERSION);
}

//----------------------------------------------------------------------------------
//  rf_status_t halRfStrobe(uint8 cmd)
//----------------------------------------------------------------------------------
rf_status_t halRfStrobe(uint8_t cmd)
{
    return spi_send_byte(cmd);
}

//----------------------------------------------------------------------------------
//  uint8 halRfReadStatusReg(uint8 addr)
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. The CC1100 and CC2500 errata notes explain the
//      problem and propose several workarounds.
//
//----------------------------------------------------------------------------------
uint8_t halRfReadStatusReg(uint8_t addr)
{
    uint8_t reg;

    spi_receive(addr | CC2500_READ_BURST, &reg, 1);
    return reg;
}

//----------------------------------------------------------------------------------
//  uint8 halRfReadReg(uint8 addr)
//----------------------------------------------------------------------------------
uint8_t halRfReadReg(uint8_t addr)
{
    uint8_t reg;

    spi_receive(addr | CC2500_READ_SINGLE, &reg, 1);
    return reg;
}

//----------------------------------------------------------------------------------
//  rf_status_t halRfWriteReg(uint8 addr, uint8 data)
//----------------------------------------------------------------------------------
rf_status_t halRfWriteReg(uint8_t addr, uint8_t data)
{
    uint8_t rc;

    rc = spi_send(addr, &data, 1);
    return rc;
}

//----------------------------------------------------------------------------------
//  rf_status_t halRfWriteFifo(uint8* data, uint8 length)
//----------------------------------------------------------------------------------
rf_status_t halRfWriteFifo(const uint8_t *data, uint8_t length)
{
    return spi_send(CC2500_TXFIFO | CC2500_WRITE_BURST, data, length);
}

//----------------------------------------------------------------------------------
//  rf_status_t halRfReadFifo(uint8* data, uint8 length)
//----------------------------------------------------------------------------------
rf_status_t halRfReadFifo(uint8_t *data, uint8_t length)
{
    return spi_receive(CC2500_RXFIFO | CC2500_READ_BURST, data, length);
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetTxStatus(void)
//
//  DESCRIPTION:
//      This function transmits a No Operation Strobe (SNOP) to get the status of
//      the radio and the number of free bytes in the TX FIFO
//
//      Status byte:
//
//      ---------------------------------------------------------------------------
//      |          |            |                                                 |
//      | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (free bytes in the TX FIFO |
//      |          |            |                                                 |
//      ---------------------------------------------------------------------------
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. This also applies to the chip status byte. The
//      CC1100 and CC2500 errata notes explain the problem and propose several
//      workarounds.
//
//----------------------------------------------------------------------------------
rf_status_t halRfGetTxStatus(void)
{
    return spi_send_byte(CC2500_SNOP);
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetRxStatus(void)
//
//  DESCRIPTION:
//      This function transmits a No Operation Strobe (SNOP) with the read bit set
//      to get the status of the radio and the number of available bytes in the RX
//      FIFO.
//
//      Status byte:
//
//      --------------------------------------------------------------------------------
//      |          |            |                                                      |
//      | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (available bytes in the RX FIFO |
//      |          |            |                                                      |
//      --------------------------------------------------------------------------------
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. This also applies to the chip status byte. The
//      CC1100 and CC2500 errata notes explain the problem and propose several
//      workarounds.
//
//----------------------------------------------------------------------------------
rf_status_t halRfGetRxStatus(void)
{
    return spi_send_byte(CC2500_SNOP | CC2500_READ_SINGLE);
}
