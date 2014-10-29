/* LCD definitions for the panel used by Lierda which has
   1 line of 2 digits and 1 line of 6 digits (basically a 1 line display with a two
   digit label field.
   various indicators for 1-phase e-meters
   ??? total segments
   
   These definitions assume a particular mapping of the COM and SEG
   lines of the LCD to pins on the MCU.
 */

/* Segment mapping for the digits. All digits are consistent on this display */
#define SEG_a_BIT                   4
#define SEG_b_BIT                   5
#define SEG_c_BIT                   6
#define SEG_d_BIT                   3
#define SEG_e_BIT                   2
#define SEG_f_BIT                   0
#define SEG_g_BIT                   1
#define SEG_h_BIT                   7

#define SEG_a                       (1 << SEG_a_BIT)
#define SEG_b                       (1 << SEG_b_BIT)
#define SEG_c                       (1 << SEG_c_BIT)
#define SEG_d                       (1 << SEG_d_BIT)
#define SEG_e                       (1 << SEG_e_BIT)
#define SEG_f                       (1 << SEG_f_BIT)
#define SEG_g                       (1 << SEG_g_BIT)
#define SEG_h                       (1 << SEG_h_BIT)

//#define icon_loc(cell,bit)          ((cell << 8) | (1 << bit))
#define icon_loc(cell,bit)          ((bit << 5) | cell)

/* h (add hours to kW) */
#define LCD_H_FOR_KW                icon_loc(0, 7)

/* kW */
#define LCD_KW                      icon_loc(1, 7)

/* colon for digit 2 */
#define LCD_COLON_2                 icon_loc(2, 7)

/* decimal point for digit 2 */
#define LCD_DP_2                    icon_loc(3, 7)

/* colon for digit 4 */
#define LCD_COLON_4                 icon_loc(4, 7)

/* decimal point for digit 4 */
#define LCD_DP_4                    icon_loc(5, 7)

/* decimal point for digit 5 */
#define LCD_DP_5                    icon_loc(6, 7)

/* low battery */
#define LCD_LOW_BATTERY             icon_loc(7, 7)

/* si duen */
#define LCD_8_0                     icon_loc(8, 0)
/* "previous" */
#define LCD_8_1                     icon_loc(8, 1)
/* "last month" */
#define LCD_LAST_MONTH              icon_loc(8, 2)
/* ??? */
#define LCD_8_3                     icon_loc(8, 3)
/* jim */
#define LCD_8_4                     icon_loc(8, 4)
/* san */
#define LCD_8_5                     icon_loc(8, 5)
/* ping */
#define LCD_8_6                     icon_loc(8, 6)
/* yung */
#define LCD_8_7                     icon_loc(8, 7)

/* din leung */
#define LCD_9_0                     icon_loc(9, 0)
/* sui leung */
#define LCD_9_1                     icon_loc(9, 1)
/* kok ho */
#define LCD_9_2                     icon_loc(9, 2)
/* ??? */
#define LCD_9_3                     icon_loc(9, 3)
/* date */
#define LCD_DATE                    icon_loc(9, 4)
/* time */
#define LCD_TIME                    icon_loc(9, 5)
/* ??? */
#define LCD_9_6                     icon_loc(9, 6)

/* Digit 9 bit 7 does not seem to do anything */

#define UPPER_NUMBER_FIRST_DIGIT    0
#define UPPER_NUMBER_DIGITS         2

#define LOWER_NUMBER_FIRST_DIGIT    2
#define LOWER_NUMBER_DIGITS         6
