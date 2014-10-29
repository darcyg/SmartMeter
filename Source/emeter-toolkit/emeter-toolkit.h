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
//  File: emeter-toolkit.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-toolkit.h,v 1.14 2007/08/07 05:53:42 a0754793 Exp $
//
//--------------------------------------------------------------------------

/*! \file */

#if !defined(_EMETER_TOOLKIT_H_)
#define _EMETER_TOOLKIT_H_

//MM24
//typedef unsigned short istate_t;

#if defined(__MSP430__)
#include <msp430.h>
#include <isr_compat.h>
#endif

//#include <iar/extra_peripheral_definitions.h>

//MM added extra definition

#define __inline__ inline

#define DEFL(name, address) __no_init volatile unsigned long int name @ address;
#define DEFLL(name, address) __no_init volatile unsigned long long int name @ address;

#if !defined(DEFXC)
#define DEFXC  volatile unsigned char
#endif
#if !defined(DEFXW)
#define DEFXW  volatile unsigned short
#endif
#if !defined(DEFXA)
#if __REGISTER_MODEL__ == __REGISTER_MODEL_REG20__
#define DEFXA  void __data20 * volatile
#else
#define DEFXA  volatile unsigned short  /* only short access is allowed from C in small memory model */
#endif
#endif

#define DEFXL  volatile unsigned long int
#define DEFXLL volatile unsigned long long int


#if defined(__MSP430_HAS_MPY__)  ||  defined(__MSP430_HAS_MPY32__)
/* Byte, 16 bit word and 32 bit word access to the result register of the 16 bit multiplier */

#if defined(__MSP430_HAS_PMM__)
#define MPY_BASE    0x4C0
#else
#define MPY_BASE    0x130
#endif

#define RES16_32_           (MPY_BASE + 10)   /* 16x16 bit result */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   RES16_32          , RES16_32_)
#else
__no_init union
{
  DEFXC                               RES16_8[4];
  DEFXW                               RES16_16[2];
  DEFXL                               RES16_32;
} @ (MPY_BASE + 10);
#endif

#endif

#if defined(__MSP430_HAS_MPY32__)
/* Byte, 16 bit word, 32 bit word and 64 bit word access to the registers of the 32 bit multiplier */

#if defined(__MSP430_HAS_PMM__)
#define MPY32_BASE_A  0x4D0
#else
#define MPY32_BASE_A  0x140
#endif

#define MPY32_              (MPY32_BASE_A + 0)    /* Multiply Unsigned Operand 1 */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   MPY32             , MPY32_)
#else
__no_init union
{
  DEFXC                               MPY8[4];
  DEFXW                               MPY16[2];
  DEFXL                               MPY32;
} @ (MPY32_BASE_A + 0);
#endif

#define MPYS32_             (MPY32_BASE_A + 4)    /* Multiply Signed Operand 1 */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   MPYS32            , MPYS32_)
#else
__no_init union
{
  DEFXC                               MPYS8[4];
  DEFXW                               MPYS16[2];
  DEFXL                               MPYS32;
} @ (MPY32_BASE_A + 4);
#endif

#define MAC32_              (MPY32_BASE_A + 8)    /* MAC Unsigned Operand 1 */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   MAC32             , MAC32_)
#else
__no_init union
{
  DEFXC                               MAC8[4];
  DEFXW                               MAC16[2];
  DEFXL                               MAC32;
} @ (MPY32_BASE_A + 8);
#endif

#define MACS32_             (MPY32_BASE_A + 12)   /* MAC Signed Operand 1 */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   MACS32            , MACS32_)
#else
__no_init union
{
  DEFXC                               MACS8[4];
  DEFXW                               MACS16[2];
  DEFXL                               MACS32;
} @ (MPY32_BASE_A + 12);
#endif

#define OP2_32_             (MPY32_BASE_A + 16)   /* Operand 2 */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   OP2_32            , OP2_32_)
#else
__no_init union
{
  DEFXC                               OP2_8[4];
  DEFXW                               OP2_16[2];
  DEFXL                               OP2_32X;
} @ (MPY32_BASE_A + 16);
#endif

#define RES64_              (MPY32_BASE_A + 20)   /* 32x32 bit result */
#if !defined(__IAR_SYSTEMS_ICC__)
DEFLL(  RES64             , RES64_)
#else
__no_init union
{
  DEFXC                               RES8[8];
  DEFXW                               RES16[4];
  DEFXL                               RES32[2];
  DEFXLL                              RES64;
} @ (MPY32_BASE_A + 20);
#endif

#endif




#if defined(__MSP430_HAS_SD24_B__)
#define SD24BMEM0_32_       (0x0850u)

#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   SD24BMEM0_32      , SD24BMEM0_32_)
#else
__no_init union
{
  DEFXC                               SD24BMEM0_8[4];
  DEFXW                               SD24BMEM0_16[2];
  DEFXL                               SD24BMEM0_32;
} @ (0x0850u);
#endif

#define SD24BMEM1_32_       (0x0854u)
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   SD24BMEM1_32      , SD24BMEM1_32_)
#else
__no_init union
{
  DEFXC                               SD24BMEM1_8[4];
  DEFXW                               SD24BMEM1_16[2];
  DEFXL                               SD24BMEM1_32;
} @ (0x0854u);
#endif

#define SD24BMEM2_32_       (0x0858u)
#if !defined(__IAR_SYSTEMS_ICC__)
DEFL(   SD24BMEM2_32      , SD24BMEM2_32_)
#else
__no_init union
{
  DEFXC                               SD24BMEM2_8[4];
  DEFXW                               SD24BMEM2_16[2];
  DEFXL                               SD24BMEM2_32;
} @ (0x0858u);
#endif

#endif



#if defined(__GNUC__)  &&  !defined(__MSP430__)

#define __infomem__ /**/
#define __uninitialized__ /**/
#elif defined(__GNUC__)  &&  defined(__MSP430__)
#define __infomem__ __attribute__ ((section(".infomem")))
#define __infomem_uninitialized__ __attribute__ ((section(".infomemnobits")))
#define __uninitialized__ __attribute__ ((section(".noinit")))

#elif defined(__AQCOMPILER__)

/* UART0 ME/IE/IFG is different on F12x and F13x/F14x devices.
   With these defines, the right sfrs can be chosen automaticaly.
   These defines should only be used with bit set and clear
   instructions as the real ME/IE/IFG sfrs might be modified
   somewhere else too!
   e.g.:
     ME1 = ME1_INIT;         //enable all other modules first
     ME2 = ME2_INIT;         //enable all other modules first
     U0ME |= UTXE0|URXE0;    //and then the USART0
*/
#if defined(__MSP430_123__)  ||  defined(__MSP430_1232__)
  #define U0ME              ME2         /* RX and TX module enable */
  #define U0IE              IE2         /* RX and TX interrupt of UART0 */
  #define U0IFG             IFG2        /* RX and TX interrupt flags of UART0 */
#else /* not a __msp430x12x or __msp430x12x2 */
  #define U0ME              ME1         /* RX and TX module enable */
  #define U0IE              IE1         /* RX and TX interrupt of UART0 */
  #define U0IFG             IFG1        /* RX and TX interrupt flags of UART0 */
#endif

#define __infomem__ _INFOMEM
#define __uninitialized__ /**/
#define __MSP430__  1

#elif defined(__IAR_SYSTEMS_ICC__)
#define __inline__ inline
#define __uninitialized__ /**/
#endif

#if defined(__MSP430__)
#if defined(__MSP430_HAS_LCD4__)  ||  defined(__MSP430_HAS_LCD_A__)
#define SUPPORT_INTERNAL_LCD    1
#define SUPPORT_LCD_MODULE      0
#else
#define SUPPORT_INTERNAL_LCD    0
#define SUPPORT_LCD_MODULE      1
#endif
#else
#define SUPPORT_INTERNAL_LCD    0
#define SUPPORT_LCD_MODULE      0
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#define BUTTON_PERSISTENCE_CHECK    60

#define DEBOUNCE_JUST_RELEASED      0
#define DEBOUNCE_JUST_HIT           1
#define DEBOUNCE_RELEASED           2
#define DEBOUNCE_HIT                3

typedef struct rtc_s
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t sumcheck;
} rtc_t;

/* Return values for bump_rtc. These indicate the most significant element
   of the rtc which was changed. The rtc is not updated if it's sumcheck is
   incorrect, so nothing is updated for an inconsistent RTC setting. */
#define RTC_INCONSISTENT    0
#define RTC_CHANGED_SECOND  1
#define RTC_CHANGED_MINUTE  2
#define RTC_CHANGED_HOUR    3
#define RTC_CHANGED_DAY     4
#define RTC_CHANGED_MONTH   5
#define RTC_CHANGED_YEAR    6

#if defined(USE_STARBURST)
typedef uint16_t lcd_cell_t;
#else
typedef uint8_t lcd_cell_t;
#endif

/* lcd_cells is the number of character positions in the main body of the LCD display */
extern const int lcd_cells;
/* lcd_pos_base is the position of the first character on the display in the LCD memory */
extern const int lcd_pos_base;
/* lcd_pos_step is 1 if increasing LCD memory locations move to the right on the display.
   It is -1 if they move to the left. */
extern const int lcd_pos_step;

/* Two patterns for LCDs are widely useful, and do not depend on the way segments are
   allocated. Define them here. */
#if !defined(CHAR_SPACE)
#define CHAR_SPACE  0x00
#endif
#if !defined(CHAR_ALL)
#define CHAR_ALL    0xFF
#endif

#endif

static __inline__ int16_t Q1_15_mul(int16_t x, int16_t y)
{
	int32_t res;
    istate_t istate;

    istate = __get_interrupt_state();
    __disable_interrupt();
    MPYS = y;
    OP2 = x;
    res = RES32[0] << 1;
    __set_interrupt_state(istate);
    return res >> 16;
}

static __inline__ int16_t q1_15_mulr(int16_t x, int16_t y)
{
	int32_t res;
    istate_t istate;

    istate = __get_interrupt_state();
    __disable_interrupt();
    MPYS = y;
    OP2 = x;
    res = (RES32[0] + 0x4000L) << 1;
    __set_interrupt_state(istate);
    return res >> 16;
}

static __inline__ int32_t imul16(int16_t x, int16_t y)
{
	int32_t res;
    istate_t istate;

    istate = __get_interrupt_state();
    __disable_interrupt();
    MPYS = y;
    OP2 = x;
    res = RES32[0];
    __set_interrupt_state(istate);
    return res;
}

static __inline__ int32_t mul48_32_16(int32_t x, int16_t y)
{
    int32_t res;
    istate_t istate;

    istate = __get_interrupt_state();
    __disable_interrupt();
    MPYS32 = x;
    OP2 = y;
    _NOP();
    _NOP();
    res = *((int32_t *) &RES16[1]);
    __set_interrupt_state(istate);
    return res;
}

static __inline__ uint32_t mul48u_32_16(uint32_t x, uint16_t y)
{
    uint32_t res;
    istate_t istate;

    istate = __get_interrupt_state();
    __disable_interrupt();
    MPY32 = x;
    OP2 = y;
    _NOP();
    _NOP();
    res = *((uint32_t *) &RES16[1]);
    __set_interrupt_state(istate);
    return res;
}



static __inline__ void int64_to_48(int16_t y[3], int64_t x)
{
    y[2] = x >> 32;
    y[1] = x >> 16;
    y[0] = x;
}

#define transfer64(y,x) (y = x, x = 0)
#define assign64(y,x)   (y = x)


static __inline__ void sqac64_24(int64_t *z, int32_t x)
{
    /* This does not protect against interrupts. It is only for use within an interrupt routine */
    MPYS32 = x;
    OP2_32X = x;
    *z += RES64;
}

static __inline__ void mac64_16_24(int64_t *z, int16_t x, int32_t y)
{
    /* This does not protect against interrupts. It is only for use within an interrupt routine */
    MPYS32 = y;
    OP2 = x;
    *z += RES64;
}



extern int16_t Q1_15_mulq(int16_t operand1, int16_t operand2);
extern void tpd_gen(void);


/*! \brief Find the square root of a 32 bit integer. The result is a 16.16 bit fractional integer.
    \param x The value for which the square root is required.
    \return The square root in the form 16 integer bits : 16 fractional bit.
*/
extern int32_t isqrt32(int32_t x);

#if defined(__GNUC__)
extern int64_t isqrt64(int64_t x);
#endif

/*! \brief Find the square root of a 32 bit integer. The result is a 16 bit integer.
    \param x The value for which the square root is required.
    \return The square root, as a 16 bit integer.
*/
extern int16_t isqrt32i(int32_t x);

/*! \brief Remove the DC content from a signal.
    \param p A pointer to a running DC estimation. This should be zeroed before the first call to dc_filter.
    \param x A signal sample to be filtered.
*/

extern int16_t dc_filter(int32_t *p, int16_t x);

extern int16_t dc_filter16(int32_t *p, int16_t x);

extern void dc_filter16_init(int32_t *p, int16_t x);

extern int32_t dc_filter24(int16_t p[3], int32_t x);

extern void dc_filter24_init(int16_t p[3], int16_t x);

//extern void sqac64_24(int64_t *z, int32_t x);

void sqac48_16(int16_t z[3], int16_t x);

//extern void mac64_16_24(int64_t *z, int16_t x, int32_t y);

//extern int32_t mul48_32_16(int32_t x, int16_t y);

//extern uint32_t mul48u_32_16(uint32_t x, uint16_t y);

extern int64_t isqrt64(int64_t x);



/*! \brief Convert a 16 bit unsigned integer to nibble packed BCD.
    \param bcd The buffer which will contain the BCD result.
    \param bin The integer value to be converted.
*/

extern void bin2bcd16(uint8_t bcd[3], uint16_t bin);
/*! \brief Convert a 32 bit unsigned integer to nibble packed BCD.
    \param bcd The buffer which will contain the BCD result.
    \param bin The integer value to be converted.
*/
extern void bin2bcd32(uint8_t bcd[5], uint32_t bin);

extern const lcd_cell_t lcd_digit_table[16];
extern const lcd_cell_t char_minus;

extern void LCDinit(void);
extern void LCDsleep(void);
extern void LCDawaken(void);
extern void LCDchars(const uint8_t *s, int pos, int len);
extern void LCDchar(uint16_t ch, int pos);
extern void LCDmodify_char(uint16_t ch, int pos, int on);

extern void LCDdec16(int16_t value, int pos, int digits, int after);
extern void LCDdec32(int32_t value, int pos, int digits, int after);
extern void LCDdecu16(uint16_t value, int pos, int digits, int after);
extern void LCDdecu32(uint32_t value, int pos, int digits, int after);

extern int debounce(uint8_t *deb, uint8_t state);

extern int16_t rand16(void);

extern int16_t dds(int *phase_acc, int phase_rate);
extern int16_t dds_offset(int phase_acc, int phase_offset);

extern const int cos_table[];

extern int bump_rtc(void);
extern void set_rtc_sumcheck(void);
extern int check_rtc_sumcheck(void);
extern int weekday(void);

extern rtc_t rtc;
extern int32_t time_to_next_hop;

extern void flash_write_int8(int8_t *ptr, int8_t value);
extern void flash_write_int16( int16_t *ptr, int16_t value);
extern void flash_write_int32( int32_t *ptr, int32_t value);
extern void flash_memcpy( char *ptr, char *from, int len);
extern void flash_clr( int *ptr);
extern void flash_secure(void);
extern void flash_replace16(int16_t *ptr, int16_t word);
extern void flash_replace32(int32_t *ptr, int32_t word);

extern void host_hex16(uint16_t value);
extern void host_hex32(uint32_t value);
extern void host_dec16(int16_t value);
extern void host_dec32(int32_t value);
extern void host_decu16(uint16_t value);
extern void host_decu32(uint32_t value);
extern int hex2ASCII(int hex);
extern void host_char(char ch);
extern void host_str(const char *s);

extern int iicEEPROM_read(uint16_t addr, void *dat, int len);
extern int iicEEPROM_write(uint16_t addr, void *dat, int len);
extern int iicEEPROM_init(void);

#if defined(BCSCTL1_)  &&  defined(TACCR0_)
extern void set_dco(int freq);
#endif

/* AQ430 and IAR cannot handle int64_t data type. GNU can, but it
   wastes some memory when we are accumulating things only a little
   longer than 32 bits. Use some simple functions to achieve some
   basic handling of a 48 bit int data type. This is stored as a
   3 element int16_t array. */

/* If an accumulated value in one of our 48 bit ints is composed of
   many 32 bit values, then dividing by the number of values returns
   us to a 32 bit value. 48/16 => 32 division is, therefore, ideal for
   scalar dot product handling. */
int32_t div48(register int16_t x[3], register int16_t y);

/* A 16 bit into 48 bit divide, with upwards preshifting of the 48 bit
   number. */
int32_t div_sh48(register int16_t x[3], int sh, register int16_t y);

/* Multiply an int32_t by an int16_t, and return the top 32 bits of the
   48 bit result. */
int32_t mul48(int32_t x, int16_t y);

void shift48(register int16_t x[3], register int how_far);

#if !defined(__MSP430__)
/* When int64_t is available, this can be a useful thing to have. */
static __inline__ int64_t int48_to_64(register int16_t x[3])
{
    int64_t y;

    y = x[2];
    y <<= 16;
    y |= (uint16_t) x[1];
    y <<= 16;
    y |= (uint16_t) x[0];
    return  y;
}
#endif

static __inline__ void transfer48(register int16_t y[3], register int16_t x[3])
{
    y[2] = x[2];
    y[1] = x[1];
    y[0] = x[0];
    x[0] =
    x[1] =
    x[2] = 0;
}

static __inline__ void assign48(register int16_t y[3], register int16_t x[3])
{
    y[2] = x[2];
    y[1] = x[1];
    y[0] = x[0];
}

#if defined(__IAR_SYSTEMS_ICC__)
void accum48(register int16_t x[3], register int32_t y);
#else
static __inline__ void accum48(register int16_t x[3], register int32_t y)
{
    /* Accumulate a 32 bit integer value into a 48 bit one represented
       by a 3 element int16_t array */
#if defined(__MSP430__)
#if defined(__GNUC__)
    register int16_t y_ex;

    __asm__ __volatile__ (
        " mov   %B[y],%[y_ex] \n"
        " rla   %[y_ex] \n"
        " subc  %[y_ex],%[y_ex] \n"
        " inv   %[y_ex] \n"
        " add   %A[y],0(%[x]) \n"
        " addc  %B[y],2(%[x]) \n"
        " addc  %[y_ex],4(%[x]) \n"
        : 
        : [x] "r"(x), [y] "r"(y), [y_ex] "r"(y_ex));
#elif defined(__AQCOMPILER__)
    register int16_t y_ex;

    /$
        mov     @y.0,@y_ex
        rla     @y_ex
        subc    @y_ex,@y_ex
        inv     @y_ex
        add     @y.1,0(@x)
        addc    @y.0,2(@x)
        addc    @y_ex,4(@x)
    $/
#elif defined(__IAR_SYSTEMS_ICC__)
    int64_t acc;

    acc = (uint16_t) x[2];
    acc <<= 16;
    acc |= (uint16_t) x[1];
    acc <<= 16;
    acc |= (uint16_t) x[0];
    acc += y;
    x[0] = acc;
    acc >>= 16;
    x[1] = acc;
    acc >>= 16;
    x[2] = acc;
#else
#error "Don't know how to accum48"
#endif
#else
    int64_t acc;

    acc = (uint16_t) x[2];
    acc <<= 16;
    acc |= (uint16_t) x[1];
    acc <<= 16;
    acc |= (uint16_t) x[0];
    acc += y;
    x[0] = acc;
    acc >>= 16;
    x[1] = acc;
    acc >>= 16;
    x[2] = acc;
#endif
}
#endif

static void __inline__ brief_pause(register unsigned int n)
{
#if defined(__MSP430__)
#if defined(__GNUC__)
    __asm__ __volatile__ (
        "1: \n"
        " dec   %[n] \n"
        " jne   1b \n"
        : [n] "+r"(n));
#elif defined(__AQCOMPILER__)
    /$
        dec @n
        jne $-2
    $/
#elif defined(__IAR_SYSTEMS_ICC__)
    while (n > 0)
    {
        n--;
        _NOP();
    }
#else
#error "Don't know how to brief_pause"
#endif
#endif
}

//MM
//static void __inline__ restart_watchdog(void)
//{
//#if defined(__MSP430__)
//    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTCNTCL;
//#endif
//}

//#endif
