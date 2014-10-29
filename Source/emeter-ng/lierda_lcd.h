/* LCD definitions for the panel used by Lierda which has
   2 lines of 8 digits
   various indicators for 3-phase e-meters
   160 total segments
   
   These definitions assume a particular mapping of the COM and SEG
   lines of the LCD to pins on the MCU.
 */

/* Each display item is defined by a 16 bit number, where
        Upper 8 bits is the LCD controller memory location
        Lower 8 bits is a mask for the LCD controller location
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

/* bottom row ??? so */
#define LCD_1_1                     icon_loc(1, 0)
/* bottom row ??? wai */
#define LCD_1_2                     icon_loc(1, 1)
/* bottom row bin ching */
#define LCD_1_3                     icon_loc(1, 2)
/* Hz */
#define LCD_LOWER_HERTZ             icon_loc(1, 3)
/* bottom row jim */
#define LCD_1_5                     icon_loc(1, 4)
/* bottom row san */
#define LCD_1_6                     icon_loc(1, 5)
/* bottom row ping */
#define LCD_1_7                     icon_loc(1, 6)
/* bottom row yung */
#define LCD_1_8                     icon_loc(1, 7)

/* bottom row phase A */
#define LCD_BOTTOM_PHASE_A_ICON     icon_loc(2, 0)
/* bottom row phase B */
#define LCD_BOTTOM_PHASE_B_ICON     icon_loc(2, 1)
/* bottom row phase C */
#define LCD_BOTTOM_PHASE_C_ICON     icon_loc(2, 2)
/* bottom row low battery */
#define LCD_BOTTOM_LOW_BATTERY      icon_loc(2, 3)
/* bottom row 4 quadrant indicator, quadrant 1 */
#define LCD_BOTTOM_QUADRANT_1       icon_loc(2, 4)
/* bottom row 4 quadrant indicator, quadrant 2 */
#define LCD_BOTTOM_QUADRANT_2       icon_loc(2, 5)
/* bottom row 4 quadrant indicator, quadrant 3 */
#define LCD_BOTTOM_QUADRANT_3       icon_loc(2, 6)
/* bottom row 4 quadrant indicator, quadrant 4 */
#define LCD_BOTTOM_QUADRANT_4       icon_loc(2, 7)

/* lower row h (add hours to kvar) */
#define LCD_LOWER_HOUR              icon_loc(3, 7)

/* lower row decimal point for digit 2 */
#define LCD_LOWER_DP_2              icon_loc(4, 7)

/* lower row kvar */
#define LCD_LOWER_KVAR              icon_loc(5, 7)

/* lower row decimal point for digit 4 */
#define LCD_LOWER_DP_4              icon_loc(6, 7)

/* lower row decimal point for digit 5 */
#define LCD_LOWER_DP_5              icon_loc(7, 7)

/* lower row decimal point for digit 6 */
#define LCD_LOWER_DP_6              icon_loc(8, 7)

/* lower row decimal point for digit 7 */
#define LCD_LOWER_DP_7              icon_loc(9, 7)

/* lower row A (amps) */
#define LCD_LOWER_AMPS              icon_loc(10, 7)

/* lower row "reactive power" */
#define LCD_LOWER_REACTIVE_POWER    icon_loc(11, 0)
/* top row "active power" */
#define LCD_UPPER_ACTIVE_POWER      icon_loc(11, 1)
/* top row "forwards" */
#define LCD_TOP_FORWARDS            icon_loc(11, 2)
/* top row "reverse" */
#define LCD_TOP_REVERSE             icon_loc(11, 3)
/* top row "voltage" */
#define LCD_UPPER_VOLTAGE           icon_loc(11, 4)
/* top row ???? */
#define LCD_11_6                    icon_loc(11, 5)
/* top row "last month" */
#define LCD_TOP_LAST_MONTH          icon_loc(11, 6)
/* top row "previous" */
#define LCD_TOP_11_8                icon_loc(11, 7)

/* upper row h */
#define LCD_UPPER_HOUR              icon_loc(12, 7)

/* upper row decimal point for digit 2 */
#define LCD_UPPER_DP_2              icon_loc(13, 7)

/* upper row kW */
#define LCD_UPPER_KW                icon_loc(14, 7)

/* upper row decimal point for digit 4 */
#define LCD_UPPER_DP_4              icon_loc(15, 7)

/* upper row decimal point for digit 5 */
#define LCD_UPPER_DP_5              icon_loc(16, 7)

/* upper row decimal point for digit 6 */
#define LCD_UPPER_DP_6              icon_loc(17, 7)

/* upper row decimal point for digit 7 */
#define LCD_UPPER_DP_7              icon_loc(18, 7)

/* upper row V (volts) */
#define LCD_UPPER_VOLTS             icon_loc(19, 7)

/* top row jim */
#define LCD_20_1                    icon_loc(20, 0)
/* top row san */
#define LCD_20_2                    icon_loc(20, 1)
/* top row ping */
#define LCD_20_3                    icon_loc(20, 2)
/* top row yung */
#define LCD_20_4                    icon_loc(20, 3)
/* left side "current" */
#define LCD_LOWER_CURRENT           icon_loc(20, 4)
/* top row "time" */
#define LCD_TOP_TIME                icon_loc(20, 5)
/* top row sui leung */
#define LCD_20_7                    icon_loc(20, 6)
/* top row din leung */
#define LCD_20_8                    icon_loc(20, 7)

#define UPPER_NUMBER_FIRST_DIGIT    12
#define UPPER_NUMBER_DIGITS         8

#define LOWER_NUMBER_FIRST_DIGIT    3
#define LOWER_NUMBER_DIGITS         8
