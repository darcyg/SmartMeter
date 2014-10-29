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
//  File:
//
//  Vincent Chan <vince-chan@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-mesh-decode.c,v 1.4 2009/01/21 03:03:13 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include <stdlib.h>
#include <io.h>
#include <emeter-toolkit.h>

#include "emeter-structs.h"

#include "mesh_if_defs.h"
#include "mesh_structure.h"
#if defined(HAS_HOST_UART)
#include "host.h"
#endif

const char NO_ONE_OUT_THERE[] = "NO ONE OUT THERE\0";

static int16_t rand16(void);

int hex2ascii(int hex)
{
    hex &= 0xF;
    if (hex <= 9)
        return hex + '0';
    return hex + 'A' - 10;
}

uint8_t *str_decu32(uint32_t value, uint8_t *s)
{
    int8_t i;
    int8_t flag;
    uint8_t digit;
    uint8_t bcd[5];
    
    bin2bcd32(bcd, value);
    flag = 0;
    for (i = 0;  i < 10;  i++)
    {
        digit = bcd[i >> 1];
        if ((i & 1) == 0)
            digit >>= 4;
        if ((digit & 0x0F)  ||  i == 9)
            flag = 1;
        if (flag)
            *s++ = hex2ascii(digit);
    }

    return s;    
}

static int delay(int delay)
{
    if (delay < 4)
        delay = 4;
    delay_count = delay;
    _BIS_SR(LPM0_bits + GIE);
    return delay_count;
}

//This generates a 16bit pseudo-random number
static int16_t rand16(void)
{
    rndnum = rndnum*31821U + 13849U;
    return rndnum;
}

//The first char must be a P
//It will decode subsequent char digits 
//until it hit a non digit
//Pointer will stop at this non digit.
uint8_t *decode_address(uint8_t *s, uint32_t *l)
{
    char ch;
    uint32_t lw;

    lw = 0;
    if (*s != 'P')
        return s;
    ++s;
    while (((ch = *s) >= '0')  &&  (ch <= '9'))
    {
        ch -= '0';
        lw *= 10;
        lw += (unsigned long) ch;  
        ++s;
    }
    *l = lw;
    return s; 
}

//Find in queue will try to locate this unit's address in the P queue.
//If found it will return the location of the location after the address
// e.g. P1P2A  (P2 is own address)
// pointer will point to A  
//If not found returns a zero
static char *find_pos_in_queue(char *s, char *s_end, unsigned long id, char *pos)
{
    unsigned long long_word = 0;  

    do
    {
        //pos is an index of position in queue  
        *pos += 1;
        s = (char *) decode_address((uint8_t *) s, &long_word);
        if (long_word == id)
            return s;        //return the char location after the address
    }
    while ((*s == 'P')  &&  (s < s_end));                    //keep looking until Path runs out
    return 0;
}

//Reverse the paths in the queue.
//The paths will get expanded into full ascci form
//So a 32 bit number will may take upto 10 characters.
//returns a pointer to the end of the Paths

static uint8_t *reverse_queue(uint8_t *s, uint8_t *s_end)
{
    uint32_t *lw_ptr;
    uint8_t *s_beg;

    s_beg = s;
    lw_ptr = long_word_table;
    //convert the whole queue address to numbers
    //until hit command or end of queue
    do
    {
        s = decode_address(s, lw_ptr++);
    }
    while ((*s == 'P')  &&  (s < s_end));

    --lw_ptr;                           //point to end of queue address

    while (lw_ptr >= long_word_table)
    {
        *s_beg++ = 'P';             //put separator first
        s_beg = str_decu32(*lw_ptr--, s_beg);  //then the body    
    }

    return s_beg;                     //return the end of the queue as an address
}

//*****************************************************************
// This is the main routine that decode and respond to r-f
// messages
//*****************************************************************
// Message types are:
// 
// Discovery: 
//    If the to_ID is 0, then it is a discovery call.
//    Unit check that it is able to respond and then send an answering
//    package after some random delay.
//    The reply will include the receieved RF strength
// Send Discovery:
//    Asked to send a discovery. A discovery package is sent.
//
// Lock:
//    Stop the unit from responding to any discovery package
// Unlock:
//    Enable the unit to respond to discovery packate
// ACK:
//    When received will light up one LED
//    It will also not response to discovery from the sender for the next 5 minutes
//    This lets the other units respond to discovery requests
// NAK:
//    When receieved will turn off one LED
//    This is use as a test function
// Pass:
//    The unit is part of a multi unit paths, it will pass the message along
//    to the next one on the queue
//    If it is the second in the queue, it will send back the received strength
//    from the first one in the queue. So the end node will always report with
//    the path's strength included.
//
// Queue structure is:
//From_ID,To_ID,Authentication,Timeout,body length,body (in chars)
//Body:
//PxPyPzCOMMAND
//eg  send acknowledge from P1 through P3 to P2
//P1P3P2A
//Discovered is triggered when To_ID is 0

void decode(mesh_packet_t *comm_buffer_ptr)
{
    uint8_t *s;
    uint8_t *ch;
    uint8_t *s_end;
    char pos;
    int8_t u;
    int i;
    unsigned long long_word;
    unsigned int length;
    extern unsigned long rem_from_ID;
    extern unsigned int long_delay_timer;

    s = comm_buffer_ptr->packet.body.uint8;          //ptr to beg of body
    s_end = s + comm_buffer_ptr->packet.body_length;   //ptr to end of body

    // Discovery would have been treated as special case, since it does not have recipient address.
    // Function will return 0 if not the final node
    if (comm_buffer_ptr->packet.to_ID == 0)
    {
        /* Promiscuous access */
        if (system_status & RESPOND_TO_DISCOVERY)
        {
            //I am now in discovery mode
            //If I am part of the QUEUE ignore
            if (ch = (uint8_t *) find_pos_in_queue((char *) s, (char *) s_end, OWN_ID, &pos))
                return;                                     //if already registered
            if (comm_buffer_ptr->packet.from_ID == rem_from_ID)
                return;

            //else wait a random time, in one of 32 x 128ms time slots 
            //then send acknowledge back. Because I am end point reverse the queue
            // Ack sent all the way back to the concentrator
            delay((rand16() >> 4) & 0x1F);
            comm_buffer_ptr->packet.to_ID = comm_buffer_ptr->packet.from_ID;
            comm_buffer_ptr->packet.from_ID = OWN_ID;
            s = comm_buffer_ptr->packet.body.uint8;  //ptr to beg of body
            s_end = s + comm_buffer_ptr->packet.body_length;   //ptr to end of body
            // Important!! Discovery must only have a single D at the end!!
            --s_end;                                            //point to the command D itself
            if (*s_end == 'd')
            {
                /* Force response to discovery, when the discovery command is 'd' instead of 'D' */
                rem_from_ID = 0;
                long_delay_timer = 0;
            }
            *s_end++ = 'P';                                     //replace with P+OWNID
            s_end = str_decu32(OWN_ID, s_end);
            // Put a separator here in case the queue has other previous rubbish
            *s_end = 'A';
            s_end = reverse_queue(s, s_end);
            // Replace 'A' with 'x'
            *s_end++ = 'x';
            u = packet_status[0];
            if (u < 0)
            {
                *s_end++ = '-';
                u = -u;
            } 
            s_end = str_decu32((unsigned long) u, s_end);
            // Terminate with a zero
            *s_end = 0;
            length = s_end - comm_buffer_ptr->packet.body.uint8;
            comm_buffer_ptr->packet.body_length = length;
            comm_buffer_ptr->packet.packet_length = length + 14;
            prepare_send_packet();
        }
        return;
    }
    // Not a discovery
    /* If its not promiscuous, it will be ignored if it is not explicitly for this node */
    if (comm_buffer_ptr->packet.to_ID != OWN_ID)
        return;

    pos = 0;
    if (ch = (uint8_t *) find_pos_in_queue((char *) s, (char *) s_end, OWN_ID, &pos))      //see if we are the final node
    {
        switch (*ch)
        {
        case 'd':
        case 'D':
            //need to send out a discovery request
            //The "to"_ID is 0, which indicates discovery
            comm_buffer_ptr->packet.to_ID = 0;
            comm_buffer_ptr->packet.from_ID = OWN_ID;
            prepare_send_packet();
#if 1
            prepare_receive_packet(0x40);       //give it a long delay for now
            length = delay_count;
            s = comm_buffer_ptr->packet.body.uint8;  //ptr to beg of body
            s_end = s + comm_buffer_ptr->packet.body_length;   //ptr to end of body  
            //If delay is interrupted by a received packet, then to_ID will be OWN_ID
            //else to_ID will remain 0
            if (comm_buffer_ptr->packet.to_ID == OWN_ID)
            {
                // There is still some time left for replys. Sit it out, then reply to the sender
                delay(length);
                ch = (uint8_t *) find_pos_in_queue((char *) s, (char *) s_end, OWN_ID, &pos);
                if (*ch == 'P')                    
                {
                    // First send back to the concentrator that we have found one
                    // the new guy would have arranged the queue in the right order
                    // extract send to address expect the master to confirm
                    // registration by sending ACK to new guy
                    ch = decode_address(ch, &long_word);
                    comm_buffer_ptr->packet.to_ID = long_word;
                    long_word = comm_buffer_ptr->packet.from_ID;
                    comm_buffer_ptr->packet.from_ID = OWN_ID;
                    /* Append the signal strength to the end of the message */
                    *s_end++ = 'r';
                    u = packet_status[0];
                    if (u < 0)
                    {
                        *s_end++ = '-';
                        u = -u;
                    } 
                    s_end = str_decu32((unsigned long) u, s_end);
                    length = s_end - comm_buffer_ptr->packet.body.uint8;
                    *s_end = '\0';
                    comm_buffer_ptr->packet.body_length = length;
                    comm_buffer_ptr->packet.packet_length = length + 14;
                    comm_buffer_ptr->packet.packet_length = comm_buffer_ptr->packet.body_length + 14;
                    prepare_send_packet();
                }
                else
                {
                    // I am the last one in the queue, so I am the master. Display the packet
                }
            }
            else
            {
                // Time out with no reply. Send NACK back. Reverse the path
                s_end = reverse_queue(s, s_end);
                s_end = (uint8_t *) find_pos_in_queue((char *) s, (char *) s_end, OWN_ID, &pos);  //find my own position
                if (*s_end == 'P')
                {
                    //next one should be the to address
                    s_end = decode_address(s_end, &long_word);
                    comm_buffer_ptr->packet.to_ID = long_word;
                    comm_buffer_ptr->packet.from_ID = OWN_ID;
                    while (*s_end == 'P')                    //find end of queue
                        s_end = decode_address(s_end, &long_word);
                    *s_end++ = 'N';                              //put NACK there
                    length = s_end-comm_buffer_ptr->packet.body.uint8;
                    *s_end = 0;                                  //safty stop
                    comm_buffer_ptr->packet.body_length = length;   
                    comm_buffer_ptr->packet.packet_length = length + 14;   
                    delay(0x20);
                    prepare_send_packet();                  //send NACK back 
                }
                else
                {
                    //no one to report the NACK to. I am the only guy
                }
            }
#endif          
            break;
        case 'L':
            system_status &= ~RESPOND_TO_DISCOVERY;
            P2OUT |= BIT1;
            goto SEND_NAK;
        case 'U':
            system_status |= RESPOND_TO_DISCOVERY;
            P2OUT &= ~BIT1;
            goto SEND_NAK;
        case 'A':
            // Turn on LED 2 to indicate we are not in the discovery locked state
            P2OUT |= BIT2;
            // remember from_ID for 5 minutes
            rem_from_ID = comm_buffer_ptr->packet.from_ID;
            long_delay_timer = FIVE_MINUTES;
            goto SEND_NAK;
        case 'a':
            // Turn off LED 2 to indicate we are not in the discovery locked state
            P2OUT &= ~BIT2;
            // Forget the from_ID
            rem_from_ID = 0;
            long_delay_timer = 0;
            goto SEND_NAK;
        case 'N':
            // turn on LED 2 to indicate 
            P2OUT &= ~BIT2;
            // get end address
            // find first in path
            // get to address from first Pxx
            // skip P to point to the number
SEND_NAK:
            comm_buffer_ptr->packet.to_ID=comm_buffer_ptr->packet.from_ID;
            comm_buffer_ptr->packet.from_ID=OWN_ID;
            s = comm_buffer_ptr->packet.body.uint8;          //ptr to beg of body
            s_end = s + comm_buffer_ptr->packet.body_length;   //ptr to end of body
            s_end = reverse_queue(s, s_end);
            *s_end++ = 'x';
            u = packet_status[0];
            if (u < 0)
            {
                *s_end++ = '-';
                u = -u;
            }
            s_end = str_decu32((unsigned long) u, s_end);
            *s_end = 0;
            length = s_end - comm_buffer_ptr->packet.body.uint8; 
            comm_buffer_ptr->packet.body_length = length;
            comm_buffer_ptr->packet.packet_length = length + 14;
            prepare_send_packet();
            break;
        case 'P':
            //I am in the middle of a path, send out the packet as is
            //everything as is, just the from and to ID change    
            decode_address(ch, &long_word);
            comm_buffer_ptr->packet.to_ID = long_word;
            comm_buffer_ptr->packet.from_ID = OWN_ID;
            if (pos == 2)
            {
                decode_address(s, &long_word);
                //find out if it is outgoing or incoming. only report strength when incoming
                if (long_word != 1)
                {
                    /* Append the signal strength to the end of the message */
                    *s_end++ = 'r';
                    u = packet_status[0];
                    if (u < 0)
                    {
                        *s_end++ = '-';
                        u = -u;
                    } 
                    s_end = str_decu32((unsigned long) u, s_end);
                    length = s_end-comm_buffer_ptr->packet.body.uint8;
                    *s_end++ = 0;
                }
            }
            length = s_end-comm_buffer_ptr->packet.body.uint8; 
            comm_buffer_ptr->packet.body_length = length;
            comm_buffer_ptr->packet.packet_length = length + 14;
            prepare_send_packet();  
            break;
        case 'R':
            comm_buffer_ptr->packet.to_ID = comm_buffer_ptr->packet.from_ID;
            comm_buffer_ptr->packet.from_ID = OWN_ID;
            s = comm_buffer_ptr->packet.body.uint8;             //ptr to beg of body
            s_end = s + comm_buffer_ptr->packet.body_length;    //ptr to end of body
            s_end = reverse_queue(s, s_end);
            ch = (uint8_t *) &message[0];
            *s_end++ = 'm';
            *s_end++ = ' ';                                     //message
            while (*ch)
                *s_end++ = *ch++;
          
            *s_end++ = ' ';
            *s_end++ = 'x';
            u = packet_status[0];
            if (u < 0)
            {
                *s_end++ = '-';
                u = -u;
            } 
            s_end = str_decu32((unsigned long) u, s_end);
            length = s_end-comm_buffer_ptr->packet.body.uint8; 
            comm_buffer_ptr->packet.body_length = length;
            comm_buffer_ptr->packet.packet_length = length + 14;
            prepare_send_packet();
            break;
        case 'Q':
            comm_buffer_ptr->packet.to_ID = comm_buffer_ptr->packet.from_ID;
            comm_buffer_ptr->packet.from_ID = OWN_ID;
            s = comm_buffer_ptr->packet.body.uint8;             //ptr to beg of body
            s_end = s + comm_buffer_ptr->packet.body_length;    //ptr to end of body
            s_end = reverse_queue(s, s_end);
            *s_end++ = 'q';
            *s_end++ = ' ';
            /* Send the active power reading for each phase */
            for (i = 0;  i < NUM_PHASES;  i++)
            {
                s_end = str_decu32(chan[i].readings.active_power, s_end);
                *s_end++ = ' ';
            }
            /* Add an 'x' as a delimiter */
            *s_end++ = 'x';
            u = packet_status[0];
            if (u < 0)
            {
                *s_end++ = '-';
                u = -u;
            } 
            s_end = str_decu32((unsigned long) u, s_end);
            length = s_end-comm_buffer_ptr->packet.body.uint8; 
            comm_buffer_ptr->packet.body_length = length;
            comm_buffer_ptr->packet.packet_length = length + 14;
            prepare_send_packet();
            break;
        }
    }
}
