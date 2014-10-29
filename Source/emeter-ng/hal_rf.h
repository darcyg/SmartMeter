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
//  $Id: hal_rf.h,v 1.1 2008/10/28 10:33:07 a0754793 Exp $
//
//--------------------------------------------------------------------------

#ifndef HAL_RF_H
#define HAL_RF_H

//----------------------------------------------------------------------------------
// Type declarations
//----------------------------------------------------------------------------------

// The following structure can be used when configuring the RF chip. SmartRF Studio
// can be used to export a predefined instance of this structure.
typedef struct
{
    uint8_t fsctrl1;    // Frequency synthesizer control.
    uint8_t fsctrl0;    // Frequency synthesizer control.
    uint8_t freq2;      // Frequency control word, high byte.
    uint8_t freq1;      // Frequency control word, middle byte.
    uint8_t freq0;      // Frequency control word, low byte.
    uint8_t mdmcfg4;    // Modem configuration.
    uint8_t mdmcfg3;    // Modem configuration.
    uint8_t mdmcfg2;    // Modem configuration.
    uint8_t mdmcfg1;    // Modem configuration.
    uint8_t mdmcfg0;    // Modem configuration.
    uint8_t channr;     // Channel number.
    uint8_t deviatn;    // Modem deviation setting (when FSK modulation is enabled).
    uint8_t frend1;     // Front end RX configuration.
    uint8_t frend0;     // Front end RX configuration.
    uint8_t mcsm0;      // Main Radio Control State Machine configuration.
    uint8_t foccfg;     // Frequency Offset Compensation Configuration.
    uint8_t bscfg;      // Bit synchronization Configuration.
    uint8_t agcctrl2;   // AGC control.
    uint8_t agcctrl1;   // AGC control.
    uint8_t agcctrl0;   // AGC control.
    uint8_t fscal3;     // Frequency synthesizer calibration.
    uint8_t fscal2;     // Frequency synthesizer calibration.
    uint8_t fscal1;     // Frequency synthesizer calibration.
    uint8_t fscal0;     // Frequency synthesizer calibration.
    uint8_t fstest;     // Frequency synthesizer calibration.
    uint8_t test2;      // Various test settings.
    uint8_t test1;      // Various test settings.
    uint8_t test0;      // Various test settings.
    uint8_t iocfg2;     // GDO2 output pin configuration.
    uint8_t iocfg0;     // GDO0 output pin configuration.
    uint8_t pktctrl1;   // Packet automation control.
    uint8_t pktctrl0;   // Packet automation control.
    uint8_t addr;       // Device address.
    uint8_t pktlen;     // Packet length.
} lpw_config_t;

#if 0
// For rapid chip configuration with a minimum of function overhead.
// The array has to be set up with predefined values for all registers.
typedef uint8_t HAL_RF_BURST_CONFIG[47];
#endif

// The chip status byte, returned by chip for all SPI accesses
typedef uint8_t rf_status_t;

//----------------------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------------------

void halRfConfig(const lpw_config_t *rfConfig, const uint8_t *rfPaTable, uint8_t rfPaTableLen);
#if 0
void halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8_t *rfPaTable, uint8_t rfPaTableLen);
#endif
void halRfResetChip(void);
uint8_t halRfGetChipId(void);
uint8_t halRfGetChipVer(void);
uint8_t halRfReadStatusReg(uint8_t addr);
uint8_t halRfReadReg(uint8_t addr);

rf_status_t halRfWriteReg(uint8_t addr, uint8_t data);
rf_status_t halRfWriteFifo(const uint8_t *data, uint8_t length);
rf_status_t halRfReadFifo(uint8_t *data, uint8_t length);
rf_status_t halRfStrobe(uint8_t cmd);
rf_status_t halRfGetTxStatus(void);
rf_status_t halRfGetRxStatus(void);

#endif
