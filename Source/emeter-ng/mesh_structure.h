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
//  $Id: mesh_structure.h,v 1.2 2009/01/08 08:30:38 a0754793 Exp $
//
//--------------------------------------------------------------------------
//#define LOCAL_NODE_ID           1

#define PACKET_POLLING          1

#if defined(LOCAL_NODE_ID)  &&  LOCAL_NODE_ID == 1
#define MASTER_NODE             1
#endif

#if defined(PACKET_POLLING)
#define MAX_LINK_PACKET_SIZE    128
#define MAX_MESH_PACKET_SIZE    142
#else
#define MAX_LINK_PACKET_SIZE    64
#define MAX_MESH_PACKET_SIZE    88
#endif

#define SERIAL_MAX_LEN          152

enum
{
    RADIO_MODE_TX = 1,
    RADIO_MODE_RX = 2,
    RADIO_MODE_TIMEOUT = 3
};

enum
{
    RX_OK = 0,
    RX_LENGTH_VIOLATION = 1,
    RX_CRC_MISMATCH = 2,
    RX_FIFO_OVERFLOW = 3,
    RX_IN_TX_MODE = 4,
    RX_TIMED_OUT = 5
};

typedef struct 
{
    /*! \brief The step into the history data used for phase correction. */
    uint8_t  packet_length;
    uint8_t  packet_address;
    uint32_t from_ID;
    uint32_t to_ID;
    uint16_t authentication;
    uint8_t  time_out;
    uint8_t  body_length;
    union
    {
        uint8_t  uint8[MAX_LINK_PACKET_SIZE];
        uint16_t uint16[MAX_LINK_PACKET_SIZE/2];
    } body;  
} packet_fields;

typedef union
{
    packet_fields packet;
    int8_t uint8[MAX_MESH_PACKET_SIZE];
    int16_t uint16[MAX_MESH_PACKET_SIZE/2];
} mesh_packet_t;

typedef struct
{
    uint16_t rxTimeout;          // Variable initialized by the timeout variable in pktStartRx()
                                 // Decremented every 200 us. RX mode is terminated when it reaches 0
    uint8_t rxTimeoutActive;     // Set in the pktStartRx function if timeout != 0. Cleard in
                                 // pktRxHandler() when pktData.rxTimeout reaches 0.
    uint8_t rxInProgress;        // Indicates that a packet is being received
    uint16_t rxBytesRead;        // Variable to keep track of the data in rxBuffer
    uint16_t rxBytesRemaining;   // Variable to keep track of how many bytes are left to be received
    uint8_t *pRxBuffer;          // Pointer to rxBuffer
    uint8_t length;              // Length of packet
    uint8_t lengthByteRead;      // Flag set to 1 when the length byte has been read
    uint8_t status;              // Return value/status from reception
} pkt_data_rx_t;

typedef struct
{
    uint8_t txInProgress;        // Indicates that a packet is being written to the FIFO
    uint8_t txBytesWritten;      // Variable to keep track of the data in txBuffer
    uint8_t txBytesRemaining;    // Variable holding information on number og bytes left in txBuffer
    uint8_t *pTxBuffer;          // Pointer to txBuffer
    uint8_t pktsPending;         // Number of packets pending to be written
} pkt_data_tx_t;

typedef struct
{
    int16_t count;
    char buffer[SERIAL_MAX_LEN];
} serial_buffer_t;

#define ENGAGED                 BIT0
#define FIVE_MINUTE_TIME_OUT    BIT1
#define FIVE_MINUTES            18750
#define __infomem__ _Pragma("location=\"INFO\"")
#define RESPOND_TO_DISCOVERY  BIT0

extern const uint8_t lcd_chars[];

#if defined(MESH_NET_SUPPORT)
extern const uint32_t OWN_ID;
extern int delay_count;
extern uint32_t long_word_table[10];
extern unsigned long int system_status;
extern uint8_t packet_status[];
extern uint16_t rndnum;
extern char message[];
#endif

extern const uint8_t lcd_chars[];

char *string_bin(char *s, unsigned long *l);

uint8_t *str_decu16(uint16_t value, uint8_t *s);
uint8_t *str_decu32(uint32_t value, uint8_t *s);

void decode(mesh_packet_t *comm_buffer_ptr);
uint8_t prepare_receive_packet(int timeout);
int delay(int delay);
void prepare_send_packet(void);
uint8_t *decode_address(uint8_t *s, uint32_t *l);

void meshnet_init(void);

void rf_tick_service(void);

