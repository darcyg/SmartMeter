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
//  $Id: emeter-mesh-init.c,v 1.3 2009/01/21 03:03:13 a0754793 Exp $
//
//--------------------------------------------------------------------------

#include <stdlib.h>
#include <stdint.h>

#if defined(__MSP430__)
#include <io.h>
#endif
#include <emeter-toolkit.h>
#include "emeter-structs.h"

#include "mesh_if_defs.h"
#include "spi.h"
#include "hal_rf.h"
#include "cc2500.h"
#include "mesh_structure.h"
#if defined(HAS_HOST_UART)
#include "host.h"
#endif
#if defined(USE_VLO_RANDOM_GEN)
#include "vlo_rand.h"
#endif

#include "my_rf_settings.h"

uint8_t packet_status[2];
uint16_t rndnum;

int delay_count;
unsigned long int long_word_table[10];
unsigned long int system_status;
unsigned long rem_from_ID;
unsigned int long_delay_timer;

char message[] = "MSG REPLY\0";

mesh_packet_t comm_buffer;
volatile uint8_t radioMode;
volatile uint8_t radioModeSet = FALSE;

static volatile uint8_t buttonPushed = FALSE;
static volatile uint8_t packetSent;
static volatile uint8_t packetReceived;
static uint8_t payloadLength;

static uint8_t txSendPacket(uint8_t *data, uint8_t length);
static uint8_t rxRecvPacket(uint8_t *data, uint8_t *length);
static void txISR(void);
static void rxISR(void);

#if defined(PACKET_POLLING)
pkt_data_rx_t rx_structure;
pkt_data_tx_t tx_structure;
#endif

// The constants below define some of the I/O signals used by the board
// Port, pin number, pin bitmask, direction and initial value should be
// set in order to match the target hardware. Once defined, the pins are
// configured in halBoardInit() by calling halDigioConfig()

const digioConfig pinLed1   = {2, 1, BIT1, HAL_DIGIO_OUTPUT, 0};
const digioConfig pinGDO0   = {HAL_IO_GDO0_PORT, HAL_IO_GDO0_PIN, (1 << HAL_IO_GDO0_PIN), HAL_DIGIO_INPUT,  0};
const digioConfig pinGDO2   = {HAL_IO_GDO2_PORT, HAL_IO_GDO2_PIN, (1 << HAL_IO_GDO2_PIN), HAL_DIGIO_INPUT,  0};

//------------------------------------------------------------------------------
//  void halBoardInit(void)
//
//  DESCRIPTION:
//    Set up board. Initialize MCU, configure I/O pins and user interfaces
//------------------------------------------------------------------------------
void meshnet_hardware_init(void)
{
    halDigioConfig(&pinLed1);
    halDigioConfig(&pinGDO0);
    halDigioConfig(&pinGDO2);
    spi_init(0);
}

//----------------------------------------------------------------------------------
//  DESCRIPTION:
//    This function is called (in interrupt context) every time a packet has been
//    transmitted.
//----------------------------------------------------------------------------------
static void txISR(void)
{
    packetSent = TRUE;
}

//----------------------------------------------------------------------------------
//  DESCRIPTION:
//    This function is called (in interrupt context) every time a packet has been
//    revceived.
//----------------------------------------------------------------------------------
static void rxISR(void)
{
    packetReceived = TRUE;
}

uint8_t prepare_receive_packet(int timeout)
{
    halDigioIntSetEdge(&pinGDO0, HAL_DIGIO_INT_FALLING_EDGE);
    halDigioIntConnect(&pinGDO0, &rxISR);
    halDigioIntEnable(&pinGDO0);
    radioMode = RADIO_MODE_RX;

    if (timeout)
        delay_count = timeout;

    return rxRecvPacket((uint8_t *) &comm_buffer.uint8[1], &payloadLength);
}  

void prepare_send_packet(void)
{
    halDigioIntSetEdge(&pinGDO0, HAL_DIGIO_INT_FALLING_EDGE);
    halDigioIntConnect(&pinGDO0, &txISR);
    halDigioIntEnable(&pinGDO0);
        
    txSendPacket((uint8_t *) comm_buffer.uint8, comm_buffer.packet.packet_length + 1);
 
    //display_packet(&comm_buffer);
}

//----------------------------------------------------------------------------------
//  uint8 txSendPacket(uint8_t *data, uint8_t length)
//
//  DESCRIPTION:
//    Send a packet that is smaller than the size of the FIFO, making it possible
//    to write the whole packet at once. Wait for the radio to signal that the packet
//    has been transmitted.
//
//  ARGUMENTS:
//    data   - Data to send. First byte contains length byte
//    length - Total length of packet to send
//
//  RETURNS:
//    This function always returns 0.
//----------------------------------------------------------------------------------
static uint8_t txSendPacket(uint8_t *data, uint8_t length)
{
    uint16_t key;

#if defined(PACKET_POLLING)
    /* Polling mode capable of sending packets up to 255 bytes */
    uint8_t status;
    uint8_t status2;
    uint8_t freeSpaceInFifo;
    uint8_t txInProgress = TRUE;

    packetSent = FALSE;
    while (txInProgress == TRUE)
    {  
        status = halRfGetTxStatus();
        status2 = halRfGetTxStatus();
        while (status != status2)
        {
            status = status2;
            status2 = halRfGetTxStatus();
        }
         
        switch (status & CC2500_STATUS_STATE_BM)
        {
        case CC2500_STATE_IDLE:
            halRfStrobe(CC2500_STX);
        case CC2500_STATE_CALIBRATE:
        case CC2500_STATE_TX:
        case CC2500_STATE_RX:
            // If there's anything to transfer..
            if (freeSpaceInFifo = ((length < (status & CC2500_STATUS_FIFO_BYTES_AVAILABLE_BM))  ?  length  :  (status & CC2500_STATUS_FIFO_BYTES_AVAILABLE_BM)))
            {
                halRfWriteFifo(data, freeSpaceInFifo);
                length -= freeSpaceInFifo;
                data += freeSpaceInFifo;
                if ((status & CC2500_STATUS_FIFO_BYTES_AVAILABLE_BM) < CC2500_STATUS_FIFO_BYTES_AVAILABLE_BM)
                    halRfStrobe(CC2500_STX);                    //Start TX
  
                // Notify the application if all bytes in the packet has been written to the TX FIFO
                if (length == 0)
                    txInProgress = FALSE;
            }
            break;
        case CC2500_STATE_TX_UNDERFLOW:
            // Flush and clean up
            txInProgress = FALSE;
            halRfStrobe(CC2500_SFTX);
            break;
        default:
            break;
        }    
    }
#else
    /* Simple mode for packets up to 61 bytes */
    packetSent = FALSE;
    // Write data to FIFO
    halRfWriteFifo(data, length);
#endif

    // Set radio in transmit mode
    halRfStrobe(CC2500_STX);

    // Wait for packet to be sent
#if 1
    key = __get_interrupt_state();
    __disable_interrupt();
    while (!packetSent)
        __low_power_mode_0();
    __set_interrupt_state(key);
#else
    delay(20);        //wait or time out
#endif    
    return 0;
}

//----------------------------------------------------------------------------------
//  uint8 rxRecvPacket(uint8_t *data, uint8_t *length)
//
//  DESCRIPTION:
//    Receive a packet that is smaller than the size of the FIFO, i.e. wait for the
//    complete packet to be received before reading from the FIFO. This function sets
//    the CC1100/CC2500 in RX and waits for the chip to signal that a packet is received.
//
//  ARGUMENTS:
//    data   - Where to write incoming data.
//    length - Length of payload.
//
//  RETURNS:
//    0 if a packet was received successfully.
//    1 if chip is in overflow state (packet longer than FIFO).
//    2 if the length of the packet is illegal (0 or > 61).
//    3 if the CRC of the packet is not OK.
//    4 UART RX interrupt, a packet is requested to be sent
//    5 Timeout waiting for a packet
//----------------------------------------------------------------------------------
static uint8_t rxRecvPacket(uint8_t *data, uint8_t *length)
{
#if defined(PACKET_POLLING)
    /* Polling mode capable of sending packets up to 255 bytes */
    uint8_t r_status;
    uint8_t status;
    uint8_t status2;
    uint8_t bytesInFifo;
    uint8_t rxBytesRemaining;
    uint8_t lengthByteRead = FALSE;
    uint8_t rxInProgress = TRUE;

    packetReceived = FALSE;
    r_status = RX_OK;

    // Set radio in RX mode
    halRfStrobe(CC2500_SRX);

    while (rxInProgress == TRUE)
    {  
        if (radioMode == RADIO_MODE_TX)    
            return RX_IN_TX_MODE;
        if (radioMode == RADIO_MODE_TIMEOUT)
            return RX_TIMED_OUT;
        // Which state?
        status = halRfGetRxStatus();
        status2 = halRfGetRxStatus();
        while (status != status2)
        {
            status = status2;
            status2 = halRfGetRxStatus();
        }
  
        switch (status & CC2500_STATUS_STATE_BM)
        {
        case CC2500_STATE_IDLE:
        case CC2500_STATE_RX:
            // If there's anything in the RX FIFO....
            if (bytesInFifo = (status & CC2500_STATUS_FIFO_BYTES_AVAILABLE_BM))
            {
                // Start by getting the packet length
                if (lengthByteRead == FALSE)
                {
                    // Make sure that the RX FIFO is not emptied
                    // (see the CC1100 or 2500 Errata Note)
                    if (bytesInFifo > 1)
                    {
                        halRfReadFifo(length, 1);
                        lengthByteRead = TRUE;
                        rxBytesRemaining = *length + 2; // Packet Length + 2 appended bytes
                        bytesInFifo--;
                    }
                    else
                    {
                        // Need more data in FIFO before reading the length byte
                        // (the first byte of the packet)
                        break;
                    }
                }

                // Make sure that the RX FIFO is not emptied
                // (see the CC1100 or 2500 Errata Note)
                if ((bytesInFifo > 1) && (bytesInFifo != rxBytesRemaining))
                {
                    // Leave one byte in FIFO
                    bytesInFifo--;
                }
                else if ((bytesInFifo <= 1) && (bytesInFifo != rx_structure.rxBytesRemaining))
                {
                    // Need more data in FIFO before reading additional bytes
                    break;
                }

                // Read from RX FIFO and store the data in rxBuffer
                halRfReadFifo(data, bytesInFifo);
                data += bytesInFifo;
                rxBytesRemaining -= bytesInFifo;
                // Done?
                if ((rxBytesRemaining == 0) && (lengthByteRead))
                {                     
                    // Check CRC
                    packet_status[1] = *(--data);   //point to packet_status[1]
                    *data = 0;
                    packet_status[0] = *(--data);   //point to packet_status[0]
                    *data = 0;                     
                    if ((packet_status[1] & CC2500_LQI_CRC_OK_BM) != CC2500_LQI_CRC_OK_BM)
                    {
                        r_status = RX_CRC_MISMATCH;
                    }
                    else
                    {
                        r_status = RX_OK;
                    }
                    rxInProgress = FALSE;
                }
            }
            break;
        case CC2500_STATE_RX_OVERFLOW:
            rxInProgress = FALSE;
            halRfStrobe(CC2500_SIDLE);
            halRfStrobe(CC2500_SFRX);
            r_status = RX_FIFO_OVERFLOW;
            break;
        default:
            break;
        }
    }
    return r_status;
#else
    /* Simple mode for packets up to 61 bytes */
    uint8_t status;

    packetReceived = FALSE;
    status = RX_OK;

    // Put radio in RX mode
    halRfStrobe(CC2500_SRX);
    _BIS_SR(LPM0_bits + GIE);
    
    if (radioMode == RADIO_MODE_TX)    
        return RX_IN_TX_MODE;
    if (radioMode == RADIO_MODE_TIMEOUT)
        return RX_TIMED_OUT;

    // Read first element of packet from the RX FIFO
    status = halRfReadFifo(length, 1);
    if ((status & CC2500_STATUS_STATE_BM) == CC2500_STATE_RX_OVERFLOW)
    {
        halRfStrobe(CC2500_SIDLE);
        halRfStrobe(CC2500_SFRX);
        status = RX_FIFO_OVERFLOW;
    }
    else if (*length == 0  ||  *length > (MAX_LINK_PACKET_SIZE - 3))
    {
        halRfStrobe(CC2500_SIDLE);
        halRfStrobe(CC2500_SFRX);
        status = RX_LENGTH_VIOLATION;
    }
    else
    {
        // Get the packet payload
        halRfReadFifo(data, *length);
        // Get the packet status bytes [RSSI, LQI]
        halRfReadFifo(packet_status, 2);
        // Check the CRC
        if ((packet_status[1] & CC2500_LQI_CRC_OK_BM) != CC2500_LQI_CRC_OK_BM)
            status = RX_CRC_MISMATCH;
        else
            status = RX_OK;
    }
    return status;
#endif
}

//----------------------------------------------------------------------------------
//  void main(void)
//
//  DESCRIPTION:
//    This is the main entry of the "link" application. It sets up the board and
//    lets the user select operating mode (RX or TX) by pressing either button
//    S1 or S2. In TX mode, one packet (with arbitrary length) is sent over the air
//    every time the button is pressed. In RX mode, the LCD will be updated every time
//    a new packet is successfully received. If an error occurred during reception,
//    one of the LEDs will flash.
//----------------------------------------------------------------------------------
__infomem__ const uint32_t OWN_ID = LOCAL_NODE_ID;

void meshnet_init(void)
{
    //uint8_t id;
    //uint8_t ver;

    meshnet_hardware_init();

#if defined(HAS_HOST_UART)
    host_interface_init();
    host_announce();
#endif

    halRfResetChip();

    //id = halRfGetChipId();
    //ver = halRfGetChipVer();

    //lcd_display_value((id << 8) | ver, HAL_LCD_RADIX_HEX);

    /* Put radio to sleep */
    halRfStrobe(CC2500_SPWD);

    /* Setup chip with register settings from SmartRF Studio */
    halRfConfig(&myRfConfig, myPaTable, myPaTableLen);

    /* Additional chip configuration for this example */
    halRfWriteReg(CC2500_MCSM0,    0x18);   // Calibration from IDLE to TX/RX
    halRfWriteReg(CC2500_MCSM1,    0x00);   // No CCA, IDLE after TX and RX
    halRfWriteReg(CC2500_PKTCTRL0, 0x45);   // Enable data whitening and CRC
    halRfWriteReg(CC2500_PKTCTRL1, 0x04);   // Enable append mode
    halRfWriteReg(CC2500_IOCFG0,   0x06);   // Set GDO0 to be packet received signal


    // In this example, the packets being sent are smaller than the size of the
    // FIFO, thus all larger packets should be discarded. The packet length
    // filtering on the receiver side is necessary in order to handle the
    // CC2500 RX FIFO overflow errata, described in the CC2500 Errata Note.
    // Given a FIFO size of 64, the maximum packet is set such that the FIFO
    // has room for the length byte + payload + 2 appended status bytes (giving
    // a maximum payload size of 64 - 1 - 2 = 61.
    halRfWriteReg(CC2500_PKTLEN, MAX_LINK_PACKET_SIZE - 3);

    system_status |= RESPOND_TO_DISCOVERY;
    rem_from_ID = 0xFFFF;
}

void rf_service_service(void)
{
    uint8_t status;

    //stay in receive mode until either a packet has been recevied 
    //or a frame of data has been receive on UART which needs to be sent out
    if ((status = prepare_receive_packet(0)) == RX_OK)
    {
        comm_buffer.packet.packet_length = payloadLength;
        decode(&comm_buffer);
    }
    else if (status == RX_IN_TX_MODE)
    {
        // Command line interrupt for transmission. Send something
        prepare_send_packet();         
    }
}

#if 0
void main(void)
{
    uint8_t status;
    uint16_t counter;
    uint8_t *s_end;
    int8_t u;

    meshnet_init();

    //Waiting for input here
    counter = 0;
    system_status |= RESPOND_TO_DISCOVERY;
    rem_from_ID = 0xFFFF;
    for (;;)
    {
        //stay in receive mode until either a packet has been recevied 
        //or a frame of data has been receive on UART which needs to be sent out
        lcd_display_value(lcd_chars[1], HAL_LCD_STATUS);
        if ((status = prepare_receive_packet(0)) == RX_OK)
        {
            comm_buffer.packet.packet_length = payloadLength;
            lcd_display_value(++counter, HAL_LCD_RADIX_HEX);
            //lcd_display_value(comm_buffer.packet.body_length, HAL_LCD_RADIX_HEX);
            //If master, just display, don't decode
#if defined(MASTER_NODE)
            if (comm_buffer.packet.to_ID == OWN_ID)
            {
                // Force a /0 at the end of the string
                comm_buffer.packet.body.uint8[comm_buffer.packet.body_length++] = '!';
                u = packet_status[0];
                if (u < 0)
                {
                    comm_buffer.packet.body.uint8[comm_buffer.packet.body_length++] = '-';
                    u = -u;
                } 
                s_end = &comm_buffer.packet.body.uint8[comm_buffer.packet.body_length];
                s_end = (uint8_t *) str_decu32((unsigned long) u, (char *) s_end);
                comm_buffer.packet.body_length = s_end - comm_buffer.packet.body.uint8;
                comm_buffer.packet.body.uint8[comm_buffer.packet.body_length] = 0;
                display_packet(&comm_buffer);
            }
#else
#if defined(HAS_HOST_UART)
            display_packet(&comm_buffer);
#endif
            decode(&comm_buffer);
#endif
        }
        else if (status == RX_IN_TX_MODE)
        {
            // Command line interrupt for transmission. Send something
            lcd_display_value(lcd_chars[2], HAL_LCD_STATUS);   
            prepare_send_packet();         
        }
        else                        //else it is error
        {
            HAL_LED_TGL_1;
            //halMcuWaitUs(20000);
            HAL_LED_TGL_1;
        }
    }
}
#endif

void rf_tick_service(void)
{
    if (delay_count)
    {
        if (--delay_count <= 0)
        {
            radioMode = RADIO_MODE_TIMEOUT;
            //LPM3_EXIT;
        }
    }   
    if (long_delay_timer)
    {
        //used in discovery mode to stop responding once explored
        if (--long_delay_timer <= 0)
            rem_from_ID = 0;
    }
    rf_service_service();
}

#if 0
// Basic Timer Interrupt Service Routine
// 16ms interrupt cylce time
ISR(BASICTIMER, basic_timer_isr)
{
    //P5DIR |= BIT1;          //To test its timing only
    //P5OUT ^= BIT1;
    rf_tick_service();
}
#endif
