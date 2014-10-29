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
//  File: emeter-3ph-neutral-47197.h
//  Star's 3-phase meter with custom LCD
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-3ph-neutral-47197.h,v 1.12 2009/04/27 06:21:22 a0754793 Exp $
//
//--------------------------------------------------------------------------

/* This is a demonstration version of the configuration data file for the
   MSP430 e-meter reference software. For most users, this file will need
   to be edited to match the hardware being used. */

/* N.B.
   This e-meter software uses a sampling rate of 3276.8/s. For a
   mains supply at exactly 50Hz, each sample is 5.49degrees of
   phase from the last. */

/*! This switch, in combination with the calibrator switch, enables calibration
    with the meter cooperating with an external reference, through a UART port. */
#define SERIAL_CALIBRATION_SUPPORT
#define SERIAL_CALIBRATION_PASSWORD_1               0x1234
#define SERIAL_CALIBRATION_PASSWORD_2               0x5678
#define SERIAL_CALIBRATION_PASSWORD_3               0x9ABC
#define SERIAL_CALIBRATION_PASSWORD_4               0xDEF0

/*! This switch enables the sending of the current readings, through a UART port,
    for use in cooperative calibration with other meters. */
#define SERIAL_CALIBRATION_REF_SUPPORT

/*! This switch selects single phase mode. If SINGLE_PHASE is not set the
    software will be built for 3-phase operation. */
#define SINGLE_PHASE

/*! This switch enables mains frequency measurement. This may be used as a
    meter feature. It may be a requirement, if non-linear CT sensors are used. */
#define MAINS_FREQUENCY_SUPPORT                     1

/*! The nominal mains frequency is used to prime the mains frequency measurement,
    and make its initial value settle quickly. It is not currently used after
    reset. */
#define MAINS_NOMINAL_FREQUENCY                     50

/*! This sets the number of pulses per kilo-watt hour the meter will produce at
    its total energy pulse LED. It does not affect the energy accumulation process. */
#define TOTAL_ENERGY_PULSES_PER_KW_HOUR             1600

/*! This sets the number of pulses per kilo-watt hour the meter will produce at
    each phase's energy pulse LED. It does not affect the energy accumulation
    process. */
#define PHASE_ENERGY_PULSES_PER_KW_HOUR             1600

/*! This switch inhibits the accumulation of per phase negative power */
#undef INHIBIT_NEGATIVE_PHASE_POWER_ACCUMULATION

/*! This switch inhibits the accumulation of total negative power */
#undef INHIBIT_NEGATIVE_TOTAL_POWER_ACCUMULATION

/*! This is called after the meter has been initialised, and before interrupts are
    enabled for the first time. This is just before the meter enters power
    down mode waiting for the first power good indication. It may be used to implement
    customised initialisation of the meter's special features. */
#define custom_initialisation()                     /**/

/*! This is called just before the meter enters the power fail state. Since the power is
    failing when this is called, the custom routine should not take an extremely long time to
    execute. The maximum safe time, before energy is drawn from the battery, will depend on the
    hardware design, and how quickly the reservoir capacitor may deplete. There should be
    plenty of time to do things like saving essential usage information to EEPROM. */
#define custom_power_fail_handler()                 /**/

/*! This is called when the meter wakes from the power fail state. It is called after the
    main meter functions have all been restarted. */
#define custom_power_restore_handler()              /**/

/*! This is called to start some customer specific energy pulse operation. For example,
    turning on some special indicator device. */
#define custom_energy_pulse_start()                 (P2OUT &= ~BIT2)

/*! This is called to end some customer specific energy pulse operation. For example,
    turning off some special indicator device. */
#define custom_energy_pulse_end()                   (P2OUT |= BIT2)

/*! The duration of the LED on time for an energy pulse. This is measured in
    ADC samples . The maximum allowed is 255, giving a
    pulse of about 78ms. 163 gives a 50ms pulse. */
#define ENERGY_PULSE_DURATION                       163
//#define ENERGY_PULSE_DURATION                       80

/*! Tiny power levels should not record at all, as they may just be rounding errors,
    noise, or the consumption of the meter itself. This value is the cutoff level,
    in 0.01W increments. */
#define RESIDUAL_POWER_CUTOFF                       250

/*! This switch enables monitoring of the neutral lead for anti-fraud purposes. */
#define NEUTRAL_MONITOR_SUPPORT                     1

/*! This is a shift value for comparing currents or powers when looking for
    imbalance between live and neutral. 3 give 12.5%. 4 give 6.25%. These are the
    two commonest values to use. The relaxed version is used at low power levels,
    where the values are less accurate, and a tight imbalance check might give false
    results. */
#define PERMITTED_IMBALANCE_FRACTION                4

/*! This is a relaxed version of the permitted imbalance fraction, for use at low
    powers/currents, where the power (and hence imbalance) measurement may be less
    precise. The tighter imbalance measurement may give false results under high
    EMI conditions. */
#define RELAXED_IMBALANCE_FRACTION                  2

/*! This definition specifies the port bit change required to turn on the
    normal operation indicator LED, when one is used. If this is defined,
    clr_normal_indicator() should also be defined. The normal operation LED will be
    lit when the mains voltage is normal. In limp mode, or power down mode,
    it is switched off. */
#define set_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn off the
    normal operation indicator LED, when one is used.  If this is defined,
    set_normal_indicator() should also be defined. */
#define clr_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn on the
    earthed condition indicator LED, when one is used. If this is defined,
    clr_earthed_indicator() should also be defined. The earthed LED will be lit when
    there is a substantial imbalance in power (normal operating mode) or
    current (limp mode) between the live and neutral leads. If the neutral
    has the higher reading, this will also be used instead of the live reading. */
#define set_earthed_indicator()                     /**/

/*! This definition specifies the port bit change required to turn off the
    earthed condition indicator LED, when one is used. If this is defined,
    set_earthed_indicator() should also be defined. */
#define clr_earthed_indicator()                     /**/

#if defined(__MSP430__)
/*! This is called to turn on a total energy pulse indicator (e.g. LED or LCD segment) */
#define total_active_energy_pulse_start()           (P2OUT &= ~BIT2)

/*! This is called to turn off a total energy pulse indicator (e.g. LED or LCD segment) */
#define total_active_energy_pulse_end()             (P2OUT |= BIT2)

/*! This is called to turn on a total reactive energy pulse indicator (e.g. LED or LCD segment) */
#define total_reactive_energy_pulse_start()         (P5OUT &= ~BIT0)

/*! This is called to turn off a total reactive energy pulse indicator (e.g. LED or LCD segment) */
#define total_reactive_energy_pulse_end()           (P5OUT |= BIT0)
#endif

/*! This is the number of successive measurement blocks which must agree the
    unbalanced status has changed before we accept it. */
#define PHASE_UNBALANCED_PERSISTENCE_CHECK          5

/*! This enables monitoring of the balance between the current in the live and neutral
    leads, for tamper detection. */
#define POWER_BALANCE_DETECTION_SUPPORT

/*! This is the minimum current level (limp mode) and minimum power level (normal
    mode) at which we will make checks for the earthed condition. Very small
    readings lack the accuracy and resolution needed to make meaningfulF comparisons
    between live and neutral. */
#define PHASE_UNBALANCED_THRESHOLD_CURRENT          500
#define PHASE_UNBALANCED_THRESHOLD_POWER            2000

/*! This selects the operation from current only, when only one lead is
    functioning, and the meter is powered by a parasitic CT supply attached to
    the leads. This is for anti-fraud purposes. Energy is accumulated at the
    highest possible rate, assuming unity power factor, and the nominal voltage */
#undef LIMP_MODE_SUPPORT
//#define LIMP_MODE_SUPPORT   1
/*! If limp mode is supported, these set the voltage thresholds for switching
    bewteen normal mode and limp mode. */
#define LIMP_MODE_VOLTAGE_THRESHOLD                 50
#define NORMAL_MODE_VOLTAGE_THRESHOLD               80

/*! If limp mode is supported, this sets the threshold current, in mA, below which we
    no not operate. */
#define LIMP_MODE_MINIMUM_CURRENT                   800

/*! This selects the nominal voltage used for power calculations in limp mode */
#define MAINS_NOMINAL_VOLTAGE                       230

/*! This selects support for measuring the RMS voltage. */
#define VRMS_SUPPORT                                1

/*! This selects support for measuring the RMS current. */
#define IRMS_SUPPORT                                1

/*! This selects support for reactive power measurement. */
#define REACTIVE_POWER_SUPPORT                      1

/*! This selects support for reactive power measurement through quadrature processing.
    This is only effective when REACTIVE_POWER_SUPPORT is enabled. */
#define REACTIVE_POWER_BY_QUADRATURE_SUPPORT        1

/*! The selects support for apparent or VA power measurement. */
#define APPARENT_POWER_SUPPORT                      1

/*! This switch enables support for power factor measurement. This feature
    includes a lead/lag assessment. A frequency independant method, based
    on the ratio of scalar dot products, is used. */
//#undef POWER_FACTOR_SUPPORT

#define POWER_FACTOR_SUPPORT

/*! This switch selects support for measuring the total active energy consumption. */
#define TOTAL_ACTIVE_ENERGY_SUPPORT                 1

/*! This switch selects support for measuring the active energy consumption on a phase
    by phase basis. This is only significant for poly-phase meters. */
#define PER_PHASE_ACTIVE_ENERGY_SUPPORT             1

#define phase_active_energy_pulse_start()          phase_1_active_energy_pulse_start()
#define phase_active_energy_pulse_end()            phase_1_active_energy_pulse_end()


#define phase_1_active_energy_pulse_start()         (P5OUT &= ~BIT7)
#define phase_1_active_energy_pulse_end()           (P5OUT |= BIT7)
#define phase_2_active_energy_pulse_start()         (P5OUT &= ~BIT5)
#define phase_2_active_energy_pulse_end()           (P5OUT |= BIT5)
#define phase_3_active_energy_pulse_start()         (P5OUT &= ~BIT6)
#define phase_3_active_energy_pulse_end()           (P5OUT |= BIT6)

/*! This switch selects support for measuring the total reactive energy. */
#define TOTAL_REACTIVE_ENERGY_SUPPORT

/*! This switch selects support for measuring the reactive energy on a phase by phase
    basis. This is only significant for poly-phase meters. */
#undef PER_PHASE_REACTIVE_ENERGY_SUPPORT

/*! This enables the generation of total energy pulses to an accuracy of 1/32768s, instead
    of 10/32768s, when building meters using the ADC12 ADC converter. This features uses channel
    2 of timer A, and the pulse LED/opto-coupler/etc. must be attached to the TA2 pin of the MCU.
    Because extra hardware resources are used, and most meters do not require this level of pulse
    position accuracy, this feature is made an option. */
#undef FINE_ENERGY_PULSE_TIMING_SUPPORT

/*! This switch enables use of the MSP430's internal temperature diode to
    measure the meter's temperature. */
#define TEMPERATURE_SUPPORT

/*! This switch selects support for the monitoring of a hall-effect magnetic sensor. This can be
    used to detect very high magnetic fields which might be used for tampering (i.e. fields strong
    enough to paralyse CTs, and other magnetic components in the meter's design). */
#undef MAGNETIC_INTERFERENCE_SUPPORT

/*! This sets the number of consecutive seconds magnetic interference must be present
    or not present before the magnetic tampering state will switch. */
//#define MAGNETIC_INTERFERENCE_PERSISTENCE_CHECK     5

/*! The hall effect sensors used to detect magnetic tampering output pulses, whose on/off ratio
    reflects the scale of the magnetic field. This sets the ratio at which magnetic tampering is
    declared. */
//#define MAGNETIC_INTERFERENCE_SAMPLE_RATIO          10

/*! When magnetic tampering is detected, the output of CTs is completely unreliable. The meter can only
    assume there is a very high load at unity power factor, and charge accordingly. This sets the current
    to be used in this calculation. It is specified in milliamps. */
#define MAGNETIC_INTERFERENCE_CURRENT               60000

/*! This selects real time clock support. This is implemented in software on
    the MSP430. */
#define RTC_SUPPORT

/*! Related definitions to get special action routines to be called at various
    intervals. User supplied callback functions must be provided. Note these
    callback routines are called from within the per second timer interrupt
    service routine. Don't do anything too complex within them. If a long
    activity is required, set a flag within a simple routine, and do the main
    work in the main non-interrupt loop. */
#undef PER_SECOND_ACTIVITY_SUPPORT
#undef PER_MINUTE_ACTIVITY_SUPPORT
#undef PER_HOUR_ACTIVITY_SUPPORT
#undef PER_DAY_ACTIVITY_SUPPORT
#undef PER_MONTH_ACTIVITY_SUPPORT
#undef PER_YEAR_ACTIVITY_SUPPORT

/*! Corrected RTC support enables temperature and basic error compensation for
    the MSP430's 32kHz crystal oscillator, so it makes for a higher quality RTC
    source, even using low accuracy (eg 20ppm) crystals. */
#define CORRECTED_RTC_SUPPORT

/*! This select support for a custom real time clock. The customer's routine
    will be called every second. */
#undef CUSTOM_RTC_SUPPORT
#if defined(__MSP430__)
#define custom_rtc()                                /**/
#define custom_rtc_set()                            /**/
#define custom_rtc_retrieve(x)                      /**/
#endif

/*! The gain setting for the first current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define CURRENT_PHASE_GAIN                          SD24GAIN_2
#define CURRENT_LIVE_GAIN                           CURRENT_PHASE_GAIN

/*! The gain setting for the second current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define CURRENT_NEUTRAL_GAIN                        SD24GAIN_16

/*! The gain setting for the voltage channel of the SD16, for devices using the
    SD16 sigma-delta ADC. This is usually GAIN_1 (i.e. x1 gain). */
#define VOLTAGE_GAIN                                SD24GAIN_2

/*! This switch enables measurement of the third and fifth harmonic contents
    of the current waveform. */
#undef HARMONICS_SUPPORT

/*! This switch select the use of current dependant phase correction, to allow
    the use of non-linear CTs. */
#define PHASE_CORRECTION_SUPPORT

/*! This switch enables dynmaic phase correction for non-linear CTs, or other sensors
    for which the phase changes with the current. */
#undef DYNAMIC_PHASE_CORRECTION_SUPPORT

/* This select support for a custom real time clock. The customer's routine
   will be called every second. */
#undef CUSTOM_RTC_SUPPORT

/* This switch enables power down to battery backup status on loss of line
   power. */
#undef POWER_DOWN_SUPPORT

/* These switches select a method of detecting when power is restored, if
   power down mode is supported. */
/* This method assumes the voltage waveform is being turned into simple
   digital pulses into an I/O pin. If this feature is used, POWER_UP_VOLTAGE_PULSE
   must define the way a voltage pulse is sensed. */
#undef POWER_UP_BY_VOLTAGE_PULSES
//#define power_up_voltage_pulse()                  (P1IN & BIT0)

/* This method assumes the pre-regulator power supply voltage is being
   monitored by Comparator A. This method is suitable for meters which
   support a live/neutral only mode, for tamper resistance. */
#undef POWER_UP_BY_SUPPLY_SENSING

/* This switch enables custom routines to be called as the meter enters and
   exits the power fail state. These might be used to save and restore things,
   using non-volatile memory. */
#undef CUSTOM_POWER_DOWN_SUPPORT

/* Many data logging requirements can be met by using only the MSP430's
   info memory. If an external serial EEPROM is needed for more
   complex requirements, this switch will enable an interface to
   I2C type serial EEPROMs. Basic routines to driver these EEPROMs are
   included in the toolkit. Routines to actually store and retrieve
   information are left to the meter designer. */
#define EXTERNAL_EEPROM_SUPPORT
#if defined(__MSP430__)
#define enable_eeprom_port()                P2OUT &= ~0x03;P2DIR &= ~0x03        /**/
#define disable_eeprom_port()               P2DIR &= ~0x03        /**/
#define set_eeprom_port_idle()              P2OUT &= ~0x03        /**/
#endif

/* This switch enables support of an IR receiver and/or transmitter for
   programming and/or reading the meter. */
//#define IEC1107_SUPPORT
//#define IEC62056_21_SUPPORT                                 1
#undef IEC62056_21_SUPPORT
#define DLT645_SUPPORT
/*! This switch selects support for a short range RF link. */
#undef RF_LINK_SUPPORT
#undef MESH_NET_SUPPORT

/*! This defines the speed of USART 0 or USCI 0 */
#define UART_PORT_0_SUPPORT                                 1
#define UART0_BAUD_RATE                                     9600
#undef UART0_IEC62056_21_SUPPORT                            //1
#define UART0_DLT645_SUPPORT

/*! This defines the speed of USART 1 or USCI 0 */
#undef UART_PORT_1_SUPPORT                                 //1
#undef UART1_BAUD_RATE                                     //9600
#undef UART1_IEC62056_21_SUPPORT
#undef UART1_DLT645_SUPPORT                                //1

#undef UART_PORT_2_SUPPORT
#undef UART2_BAUD_RATE
#undef UART2_IEC62056_21_SUPPORT
#undef UART2_SUPPORT

#undef UART_PORT_3_SUPPORT
#undef UART3_BAUD_RATE
#undef UART3_IEC62056_21_SUPPORT
#undef UART3_SUPPORT

#define INFRA_RED_38K_SUPPORT                               1
#define INFRA_RED_38K_FROM_TIMER_A                          1
//#define INFRA_RED_38K_FROM_TIMER_B                          1

/*! This definition specifies the port bit change required to enable the IR
    receiver, when one is used. */
#define enable_ir_receiver()                                /**/

/*! This definition specifies the port bit change required to disable the IR
    receiver, for power saving, when one is used. */
#define disable_ir_receiver()                               /**/

/*! This switch enables a rolling display of all the meter's measured
    parameters, for demonstration purposes. The parameters are displayed
    on a local attached LCD, which may be driven by an MSP430's internal
    LCD controller, or be part of a standard external module.
    Real meters will generally need to replace this with their own custom
    display routines. */
#define BASIC_LCD_SUPPORT

/*! This switch enables the calling of a custom LCD display routine. This allows
    easy integration of custom code for specific meter designs. */
#undef CUSTOM_LCD_SUPPORT

/*! This is called to perform initialisation of a custom display */
#define custom_lcd_init()                                   /**/

/*! This is called to perform custom display processing when the meter goes to
    sleep. This is typically turning off the display, to conserve energy. */
#define custom_lcd_sleep_handler()                          (LCDsleep(), display_power_fail_message())

/*! This is called to perform custom display processing when the meter wakes up from
    the sleeping condition. */
#define custom_lcd_wakeup_handler()                         LCDawaken()

/*! This switch enables multi-rate tariff features */
#undef MULTI_RATE_SUPPORT

/*! This switch enables self-test features */
#undef SELF_TEST_SUPPORT

/*! This is called from within the main processing loop. This allows easy integration
    of custom code for specific meter designs. This routine is called often in normal
    operating mode. In limp mode it is called once each time the background activity
    informs the foreground activity there is a block of data to be processed. It will
    also be called when a key is pressed, or for each repeat if a key is in repeat
    mode. */
#define custom_mainloop_handler()                   /**/

/*! This is called in the main processing once every 2 seconds. */
#if defined(__MSP430__)
#define custom_2second_handler()                    /**/
#endif

/*! This switch enables support for some basic keypad handling, to work
    with the basic demonstration LCD display routines. It demonstrates an
    approach to keypad handling within an e-meter. */
#define BASIC_KEYPAD_SUPPORT

/*! This switch enables the calling of a custom keypad handler routine. This
    allows easy integration of custom code for specific meter designs. */
#undef CUSTOM_KEYPAD_SUPPORT

/*! These are used with the above switches to determine the port bits assigned
    to the keys. Only define the buttons which are actually used. */
#define sense_key_1_up()                            (!(P1IN & BIT6))
#undef sense_key_2_up
#undef sense_key_3_up
#undef sense_key_4_up

/*! These are used to select the features required of the keys. They are bits in
    the key_states variable. Only define the features being used. Usually for
    each key the "DOWN" option and either the "LONG_DOWN" or "REPEAT_DOWN" option
    should be specified. Do not select both the "LONG_DOWN" and "REPEAT_DOWN" for the
    same key. */
#define KEY_1_DOWN                                  0x01
#undef KEY_1_LONG_DOWN                           //0x02
#define KEY_1_REPEAT_DOWN                           0x02
#undef KEY_2_DOWN                                //0x04
#undef KEY_2_LONG_DOWN                           //0x08
#undef KEY_2_REPEAT_DOWN                         //0x08
#undef KEY_3_DOWN                                //0x10
#undef KEY_3_LONG_DOWN                           //0x20
#undef KEY_3_REPEAT_DOWN                         //0x20
#undef KEY_4_DOWN                                //0x40
#undef KEY_4_LONG_DOWN                           //0x80
#undef KEY_4_REPEAT_DOWN                         //0x80

/*! Normally the meter software only calculates the properly scaled values
    for voltage, current, etc. as these values are needed. This define
    enables additional global parameters, which are regularly updated with
    all the metrics gathered by the meter. This is generally less efficient,
    as it means calculating things more often than necessary. However, some
    may find this easier to use, so it is offered as a choice for the meter
    designer. */
#define PRECALCULATED_PARAMETER_SUPPORT

/*! A PWM generator can be used to add dithering to the signals, to expand the
    effective resolution of the ADC. */
#undef PWM_DITHERING_SUPPORT

#undef DAC12_DITHERING_SUPPORT

/*! This switch select the use of current dependant phase correction, to allow
    the use of non-linear CTs. */
#define DYNAMIC_FIR_SUPPORT

/*! This switch selects where a backup battery's condition is monitored. */
#undef BATTERY_MONITOR_SUPPORT

/*! Target specific definitions for controlling the indicator LEDs */
#if defined(__MSP430__)
/*! This definition specifies the port bit change required to turn on the
    normal operation indicator LED, when one is used. If this is defined,
    clr_normal_indicator() should also be defined. The normal operation LED will be
    lit when the mains voltage is normal. In limp mode, or power down mode,
    it is switched off. */
#define set_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn off the
    normal operation indicator LED, when one is used.  If this is defined,
    set_normal_indicator() should also be defined. */
#define clr_normal_indicator()                      /**/

/*! This definition specifies the port bit change required to turn on the
    earthed condition indicator LED, when one is used. If this is defined,
    clr_earthed_indicator() should also be defined. The earthed LED will be lit when
    there is a substantial imbalance in power (normal operating mode) or
    current (limp mode) between the live and neutral leads. If the neutral
    has the higher reading, this will also be used instead of the live reading. */
#define set_earthed_indicator()                     /**/

/*! This definition specifies the port bit change required to turn off the
    earthed condition indicator LED, when one is used. If this is defined,
    set_earthed_indicator() should also be defined. */
#define clr_earthed_indicator()                     /**/

/*! This is the number of successive measurement blocks which must agree the
    unbalanced status has changed before we accept it. */
#define PHASE_UNBALANCED_PERSISTENCE_CHECK          5

/*! These is the minimum current level (limp mode) and minimum power level (normal
    mode) at which we will make checks for the earthed condition. Very small
    readings lack the accuracy and resolution needed to make meaningfulF comparisons
    between live and neutral. */
#define PHASE_UNBALANCED_THRESHOLD_CURRENT          500
#define PHASE_UNBALANCED_THRESHOLD_POWER            2000

/*! This selects the detection of the reversed power condition. */
#define PHASE_REVERSED_DETECTION_SUPPORT

/*! This selects that the reversed power condition is to be treated as tampering. */
#define PHASE_REVERSED_IS_TAMPERING

/*! This definition specifies the port bit change required to turn on the
    reverse condition indicator LED, when one is used. If this is defined,
    clr_reverse_current_indicator() should also be defined. The reversed LED will be lit
    in normal operating mode, if the raw power reading is negative in the live
    lead (without neutral lead monitoring) or in either of the leads (with
    neutral lead monitoring). The power value used is simply minus the actual
    reading. This only functions in the normal operating mode. In limp mode we
    have no simple way to tell forward current flow from reverse. In limp mode
    the LED is switched off. Note that if the meter is used in applications
    means we are actively feeding power to the public grid this functionality
    is inappropriate. In those cases the negative power would generally be used
    to accumulate a separate "generated energy" reading, to complement the usual
    "consumed energy" reading. */
#define set_reverse_current_indicator()             /**/

/*! This definition specifies the port bit change required to turn on the
    reverse condition indicator LED, when one is used. If this is defined,
    set_reverse_current_indicator() should also be defined. */
#define clr_reverse_current_indicator()             /**/

/*! This is the number of successive measurement blocks which must agree the
    reversed current status has changed before we accept it. */
#define PHASE_REVERSED_PERSISTENCE_CHECK            5

/*! This is the minimum power level at which we will make checks for the reverse
    condition. Very small readings are not a reliable indicator, due to noise. */
#define PHASE_REVERSED_THRESHOLD_POWER              2000

#endif

/*! Some customers expect a meter to count pulses from the time the mains power
    is restored, looosing any fractional count of an LED pulse. This looses the
    power company a fraction of a pulse of revenue, and means the meter is not
    doing the best it can. In the end, we just do what the customer wants. */
#define LOSE_FRACTIONAL_PULSE_AT_POWER_ON

/*! Target specific definitions for the analogue I/O pins */
#define VOLTAGE_INPUT_1                             INCH_2
#define VOLTAGE_INPUT_2                             INCH_5
#define VOLTAGE_INPUT_3                             INCH_8
#define LIVE_LOW_CURRENT_INPUT_1                    INCH_1
#define LIVE_LOW_CURRENT_INPUT_2                    INCH_4
#define LIVE_LOW_CURRENT_INPUT_3                    INCH_7
#define LIVE_HIGH_CURRENT_INPUT_1                   INCH_0
#define LIVE_HIGH_CURRENT_INPUT_2                   INCH_3
#define LIVE_HIGH_CURRENT_INPUT_3                   INCH_6
#define NEUTRAL_CURRENT_INPUT                       INCH_9
#define AGND_INPUT                                  INCH_9


#define DEFAULT_TEMPERATURE_OFFSET                  (1615*8)
#define DEFAULT_TEMPERATURE_SCALING                 (704*2)

#define DEFAULT_ROOM_TEMPERATURE                    250

#define DEFAULT_V_RMS_SCALE_FACTOR_A                9884
//#define DEFAULT_V_RMS_SCALE_FACTOR_B                14597
#define DEFAULT_V_RMS_SCALE_FACTOR_C                14503
//#define DEFAULT_V_RMS_LIMP_SCALE_FACTOR_A           21280
#define DEFAULT_V_RMS_LIMP_SCALE_FACTOR             21280
#define DEFAULT_I_RMS_SCALE_FACTOR_A                11163
//#define DEFAULT_I_RMS_SCALE_FACTOR_B                5581
#define DEFAULT_I_RMS_SCALE_FACTOR_C                5564
#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR             4360
//#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR_A           4360

#if 1
#define DEFAULT_P_SCALE_FACTOR_A_LOW                13871        /* Low current, high gain, range. Test at 2A on 30A max meter */
//#define DEFAULT_P_SCALE_FACTOR_A_HIGH               14087       /* High current, low gain, range. Test at 7A on 30A max meter */
#define DEFAULT_P_SCALE_FACTOR_B_LOW                10180
//#define DEFAULT_P_SCALE_FACTOR_B_HIGH               14217
#define DEFAULT_P_SCALE_FACTOR_C_LOW                10089
//#define DEFAULT_P_SCALE_FACTOR_C_HIGH               14121
#else
#define DEFAULT_P_SCALE_FACTOR_A_LOW                1039        /* Low current, high gain, range. Test at 2A on 30A max meter */
#define DEFAULT_P_SCALE_FACTOR_A_HIGH               15663       /* High current, low gain, range. Test at 7A on 30A max meter */
#define DEFAULT_P_SCALE_FACTOR_B_LOW                1039
#define DEFAULT_P_SCALE_FACTOR_B_HIGH               15663
#define DEFAULT_P_SCALE_FACTOR_C_LOW                1039
#define DEFAULT_P_SCALE_FACTOR_C_HIGH               15663
#endif



#define DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL          4905
#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR_NEUTRAL     4360
#define DEFAULT_P_SCALE_FACTOR_NEUTRAL              6210

#define DEFAULT_FREQUENCY_PHASE_FACTOR              500
#define DEFAULT_FREQUENCY_GAIN_FACTOR               0

#define DEFAULT_BASE_PHASE_A_CORRECTION_LOW         57   /* Low current, high gain, range. Test at 2A on 30A max meter. */
//#define DEFAULT_BASE_PHASE_A_CORRECTION_HIGH        0           /* High current, low gain, range. Test at 7A on 30A max meter. */
#define DEFAULT_BASE_PHASE_B_CORRECTION_LOW         8           /* Value is phase angle in 1/256th of a sample increments. */
//#define DEFAULT_BASE_PHASE_B_CORRECTION_HIGH        -3
#define DEFAULT_BASE_PHASE_C_CORRECTION_LOW         9
//#define DEFAULT_BASE_PHASE_C_CORRECTION_HIGH        2

#define DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION       73

#define DEFAULT_PHASE_CORRECTION1                   57
#define DEFAULT_GAIN_CORRECTION1                    0
#define DEFAULT_PHASE_CORRECTION2                   0
#define DEFAULT_GAIN_CORRECTION2                    0



#define custom_set_consumption(x,y)                 /**/
#define custom_magnetic_sensor_test()               /**/

/*! This is called every ADC interrupt, after the main DSP work has finished.
    It can be used for things like custom keypad operations. It is important
    this is a very short routine, as it is called from the main ADC interrupt. */
#define custom_adc_interrupt()                      /**/

/*! This is called at intervals, to allow integration of RF protocols for AMR. */
#define custom_rf_sniffer()                         /**/

/*! This is called at intervals, to allow integration of RF protocols for AMR. */
#define custom_rf_exchange()                        /**/

#define custom_rtc()                                /* not used */
#define custom_rtc_set()                            /* not used */
#define custom_rtc_retrieve(x)                      /* not used */
#define custom_energy_pulse()                       /* not used */
#define custom_keypad_handler()                     /* not used */
#define custom_mainloop_handler()                   /* not used */
#define custom_power_fail_handler()                 /* not used */
#define custom_power_restore_handler()              /* not used */

#include "lierda_lcd.h"

/* LCD display parameters */
#define icon_desciptor(cell,bit)                    ((bit << 5) | cell)

#define LCD_CELLS                                   20
#define LCD_POS_BASE                                -1
#define LCD_POS_STEP                                1
//typedef uint8_t lcd_cell_t;

/* The LCD has 2 rows of 8 digits, plus various symbols. */
#define TWO_LINE_LCD
#define FIRST_ROW_START                             UPPER_NUMBER_FIRST_DIGIT
#define FIRST_ROW_CHARS                             UPPER_NUMBER_DIGITS
#define SECOND_ROW_START                            LOWER_NUMBER_FIRST_DIGIT
#define SECOND_ROW_CHARS                            LOWER_NUMBER_DIGITS

#define ICON_PHASE_A                                LCD_BOTTOM_PHASE_A_ICON
#define ICON_PHASE_B                                LCD_BOTTOM_PHASE_B_ICON
#define ICON_PHASE_C                                LCD_BOTTOM_PHASE_C_ICON

#define ICON_H                                      LCD_UPPER_HOUR

#define ICON_BATTERY                                LCD_BOTTOM_LOW_BATTERY

#define FIRST_POSITION                              UPPER_NUMBER_FIRST_DIGIT

#define INFO_POSITION                               SECOND_ROW_START
#define INFO_CHARS                                  SECOND_ROW_CHARS

//#define ICON_TIME                                   LCD_TOP_TIME
#define HOUR_POSITION                               (UPPER_NUMBER_FIRST_DIGIT + 0)
#define ICON_TIME_COLON_1                           icon_loc(UPPER_NUMBER_FIRST_DIGIT + 2, 1)   /* Seg G of intervening digit */
#define MINUTE_POSITION                             (UPPER_NUMBER_FIRST_DIGIT + 3)
#define ICON_TIME_COLON_2                           icon_loc(UPPER_NUMBER_FIRST_DIGIT + 5, 1)   /* Seg G of intervening digit */
#define SECONDS_POSITION                            (UPPER_NUMBER_FIRST_DIGIT + 6)

#define YEAR_POSITION                               (LOWER_NUMBER_FIRST_DIGIT + 0)
#define ICON_DATE_COLON_1                           icon_loc(LOWER_NUMBER_FIRST_DIGIT + 2, 1)   /* Seg G of intervening digit */
#define MONTH_POSITION                              (LOWER_NUMBER_FIRST_DIGIT + 3)
#define ICON_DATE_COLON_2                           icon_loc(LOWER_NUMBER_FIRST_DIGIT + 5, 1)   /* Seg G of intervening digit */
#define DAY_POSITION                                (LOWER_NUMBER_FIRST_DIGIT + 6)

#define ICON_kW                                     LCD_UPPER_KW
//#define ICON_ACTIVE_POWER                           LCD_UPPER_ACTIVE_POWER
#define FIRST_ACTIVE_POWER_POSITION                 UPPER_NUMBER_FIRST_DIGIT
#define ACTIVE_POWER_DIGITS                         UPPER_NUMBER_DIGITS
#define ICON_ACTIVE_POWER_DECIMAL_POINT             LCD_UPPER_DP_4
#define ACTIVE_POWER_RESOLUTION                     4

#define ICON_kvar                                   LCD_LOWER_KVAR
//#define ICON_REACTIVE_POWER                         LCD_LOWER_REACTIVE_POWER
#define FIRST_REACTIVE_POWER_POSITION               LOWER_NUMBER_FIRST_DIGIT
#define REACTIVE_POWER_DIGITS                       LOWER_NUMBER_DIGITS
#define ICON_REACTIVE_POWER_DECIMAL_POINT           LCD_LOWER_DP_4
#define REACTIVE_POWER_RESOLUTION                   4

#define ICON_kVA                                    LCD_UPPER_KW
//#define ICON_APPARENT_POWER
#define FIRST_APPARENT_POWER_POSITION               UPPER_NUMBER_FIRST_DIGIT
#define APPARENT_POWER_DIGITS                       UPPER_NUMBER_DIGITS
#define ICON_APPARENT_POWER_DECIMAL_POINT           LCD_UPPER_DP_4
#define APPARENT_POWER_RESOLUTION                   4

//#define ICON_ACTIVE_ENERGY                          LCD_UPPER_ACTIVE_POWER
#define ICON_H_FOR_kW                               LCD_UPPER_HOUR
#define FIRST_ACTIVE_ENERGY_POSITION                UPPER_NUMBER_FIRST_DIGIT
#define ACTIVE_ENERGY_DIGITS                        UPPER_NUMBER_DIGITS
#define ICON_ACTIVE_ENERGY_DECIMAL_POINT            LCD_UPPER_DP_6
#define ACTIVE_ENERGY_RESOLUTION                    2

//#define ICON_REACTIVE_ENERGY                        LCD_LOWER_REACTIVE_POWER
#define ICON_H_FOR_kvar                             LCD_LOWER_HOUR
#define FIRST_REACTIVE_ENERGY_POSITION              LOWER_NUMBER_FIRST_DIGIT
#define REACTIVE_ENERGY_DIGITS                      LOWER_NUMBER_DIGITS
#define ICON_REACTIVE_ENERGY_DECIMAL_POINT          LCD_LOWER_DP_6
#define REACTIVE_ENERGY_RESOLUTION                  2

//#define ICON_POWER_FACTOR
//#define ICON_COS_PHI
#define FIRST_POWER_FACTOR_POSITION                 LOWER_NUMBER_FIRST_DIGIT
#define POWER_FACTOR_DIGITS                         LOWER_NUMBER_DIGITS
#define ICON_POWER_FACTOR_DECIMAL_POINT             LCD_LOWER_DP_4
#define POWER_FACTOR_RESOLUTION                     4

//#define ICON_VOLTAGE                                LCD_UPPER_VOLTAGE
#define ICON_V                                      LCD_UPPER_VOLTS
#define FIRST_VOLTAGE_POSITION                      UPPER_NUMBER_FIRST_DIGIT
#define VOLTAGE_DIGITS                              UPPER_NUMBER_DIGITS
#define ICON_VOLTAGE_DECIMAL_POINT                  LCD_UPPER_DP_6
#define VOLTAGE_RESOLUTION                          2

//#define ICON_CURRENT                                LCD_LOWER_CURRENT
#define ICON_A                                      LCD_LOWER_AMPS
#define FIRST_CURRENT_POSITION                      LOWER_NUMBER_FIRST_DIGIT
#define CURRENT_DIGITS                              LOWER_NUMBER_DIGITS
#define ICON_CURRENT_DECIMAL_POINT                  LCD_LOWER_DP_5
#define CURRENT_RESOLUTION                          3

//#define ICON_TEMPERATURE
//#define ICON_CELCIUS
#define FIRST_TEMPERATURE_POSITION                  UPPER_NUMBER_FIRST_DIGIT
#define TEMPERATURE_DIGITS                          UPPER_NUMBER_DIGITS
#define TEMPERATURE_RESOLUTION                      1
#define ICON_TEMPERATURE_DECIMAL_POINT              LCD_UPPER_DP_7

//#define ICON_FREQUENCY
#define ICON_HERTZ                                  LCD_LOWER_HERTZ
#define FIRST_FREQUENCY_POSITION                    LOWER_NUMBER_FIRST_DIGIT
#define FREQUENCY_DIGITS                            LOWER_NUMBER_DIGITS
#define FREQUENCY_RESOLUTION                        2
#define ICON_FREQUENCY_DECIMAL_POINT                LCD_LOWER_DP_6

#define DISPLAY_TYPE_POSITION                       12

#define TEXT_MESSAGE_LENGTH                         6

/* LCD display sequence table */
#define DISPLAY_STEP_SEQUENCE \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_ACTIVE_POWER, \
    DISPLAY_ITEM_REACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_VOLTAGE, \
    DISPLAY_ITEM_CURRENT, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_MAINS_FREQUENCY, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_ACTIVE_ENERGY, \
    DISPLAY_ITEM_REACTIVE_ENERGY, \
    DISPLAY_ITEM_SELECT_PHASE_2, \
    DISPLAY_ITEM_ACTIVE_POWER, \
    DISPLAY_ITEM_REACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_PHASE_2, \
    DISPLAY_ITEM_VOLTAGE, \
    DISPLAY_ITEM_CURRENT, \
    DISPLAY_ITEM_SELECT_PHASE_2, \
    DISPLAY_ITEM_MAINS_FREQUENCY, \
    DISPLAY_ITEM_SELECT_PHASE_2, \
    DISPLAY_ITEM_ACTIVE_ENERGY, \
    DISPLAY_ITEM_REACTIVE_ENERGY, \
    DISPLAY_ITEM_SELECT_PHASE_3, \
    DISPLAY_ITEM_ACTIVE_POWER, \
    DISPLAY_ITEM_REACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_PHASE_3, \
    DISPLAY_ITEM_VOLTAGE, \
    DISPLAY_ITEM_CURRENT, \
    DISPLAY_ITEM_SELECT_PHASE_3, \
    DISPLAY_ITEM_MAINS_FREQUENCY, \
    DISPLAY_ITEM_SELECT_PHASE_3, \
    DISPLAY_ITEM_ACTIVE_ENERGY, \
    DISPLAY_ITEM_REACTIVE_ENERGY, \
    DISPLAY_ITEM_SELECT_TOTAL, \
    DISPLAY_ITEM_ACTIVE_POWER, \
    DISPLAY_ITEM_REACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_TOTAL, \
    DISPLAY_ITEM_TIME, \
    DISPLAY_ITEM_DATE, \
    DISPLAY_ITEM_SELECT_RESTART

#define custom_lcd_clear_periphery() \
    LCDicon(LCD_1_1, FALSE); \
    LCDicon(LCD_1_2, FALSE); \
    LCDicon(LCD_1_3, FALSE); \
    LCDicon(LCD_1_5, FALSE); \
    LCDicon(LCD_1_6, FALSE); \
    LCDicon(LCD_1_7, FALSE); \
    LCDicon(LCD_1_8, FALSE); \
    LCDicon(LCD_TOP_FORWARDS, FALSE); \
    LCDicon(LCD_TOP_REVERSE, FALSE); \
    LCDicon(LCD_UPPER_VOLTAGE, FALSE); \
    LCDicon(LCD_11_6, FALSE); \
    LCDicon(LCD_TOP_LAST_MONTH, FALSE); \
    LCDicon(LCD_TOP_11_8, FALSE); \
    LCDicon(LCD_20_1, FALSE); \
    LCDicon(LCD_20_2, FALSE); \
    LCDicon(LCD_20_3, FALSE); \
    LCDicon(LCD_20_4, FALSE); \
    LCDicon(LCD_TOP_TIME, FALSE); \
    LCDicon(LCD_20_7, FALSE); \
    LCDicon(LCD_20_8, FALSE); \
    LCDicon(LCD_BOTTOM_QUADRANT_1, FALSE); \
    LCDicon(LCD_BOTTOM_QUADRANT_2, FALSE); \
    LCDicon(LCD_BOTTOM_QUADRANT_3, FALSE); \
    LCDicon(LCD_BOTTOM_QUADRANT_4, FALSE); \
    LCDicon(ICON_BATTERY, FALSE);

#define custom_lcd_clear_line_1_tags() \
    LCDicon(LCD_UPPER_HOUR, FALSE); \
    LCDicon(LCD_UPPER_DP_2, FALSE); \
    LCDicon(LCD_UPPER_KW, FALSE); \
    LCDicon(LCD_UPPER_DP_4, FALSE); \
    LCDicon(LCD_UPPER_DP_5, FALSE); \
    LCDicon(LCD_UPPER_DP_6, FALSE); \
    LCDicon(LCD_UPPER_DP_7, FALSE); \
    LCDicon(LCD_UPPER_VOLTS, FALSE); \
    LCDicon(LCD_UPPER_ACTIVE_POWER, FALSE);

#define custom_lcd_clear_line_2_tags() \
    LCDicon(LCD_LOWER_HERTZ, FALSE); \
    LCDicon(LCD_LOWER_HOUR, FALSE); \
    LCDicon(LCD_LOWER_DP_2, FALSE); \
    LCDicon(LCD_LOWER_KVAR, FALSE); \
    LCDicon(LCD_LOWER_DP_4, FALSE); \
    LCDicon(LCD_LOWER_DP_5, FALSE); \
    LCDicon(LCD_LOWER_DP_6, FALSE); \
    LCDicon(LCD_LOWER_DP_7, FALSE); \
    LCDicon(LCD_LOWER_AMPS, FALSE); \
    LCDicon(LCD_LOWER_REACTIVE_POWER, FALSE); \
    LCDicon(LCD_LOWER_CURRENT, FALSE);

#define USE_WATCHDOG

/*
    P1.0 =                        - 38kHz modulation signal
    P1.1 =                        - Button 1
    P1.2 = RX                     - Button 2
    P1.3 = TX                     - GDO0 (wireless)
    P1.4 =                        - GDO1 (wireless)
    P1.5 =                        - GDO2 (wireless)
    P1.6 =                        - RS232C Tx
    P1.7 =                        - RS232C Rx
 */
#define P1DIR_INIT                                  (BIT3)
#define P1SEL_INIT                                  (BIT3 | BIT2)
#define P1OUT_INIT                                  (0)

/*
    P2.0 =                        - SCL for EEPROM
    P2.1 =                        - SDA for EEPROM
    P2.2 =                        - active pulse
    P2.3 =                        - TXE (wireless)
    P2.4 =                        - IR port Tx
    P2.5 =                        - IR port Rx
    P2.6 =                        - Voltage sense 0
    P2.7 =                        - Voltage sense 1
 */
#define P2DIR_INIT                                  (BIT7 | BIT6 | BIT4 | BIT3 | BIT2)
#define P2SEL_INIT                                  (BIT5 | BIT4)
#define P2OUT_INIT                                  (0)

/*
    P3.0 = CS (wireless)
    P3.1 = SI (wireless)
    P3.2 = SO (wireless)
    P3.3 = SCLK (wireless)
    P3.4 = LCD segment line
    P3.5 = LCD segment line
    P3.6 = LCD segment line
    P3.7 = LCD segment line
 */
#define P3DIR_INIT                                  (BIT7 | BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define P3SEL_INIT                                  0
#define P3OUT_INIT                                  (BIT7 | BIT6 | BIT5 | BIT2 | BIT1 | BIT0)

/*
    P4.0 = LCD segment line
    P4.1 = LCD segment line
    P4.2 = LCD segment line
    P4.3 = LCD segment line
    P4.4 = LCD segment line
    P4.5 = LCD segment line
    P4.6 = LCD segment line
    P4.7 = LCD segment line
 */
#define P4DIR_INIT                                  0
#define P4SEL_INIT                                  0
#define P4OUT_INIT                                  0

/*
    P5.0 = reactive pulse
    P5.1 = COM0
    P5.2 = COM1
    P5.3 = COM2
    P5.4 = COM3
    P5.5 = right LED
    P5.6 = middle LED
    P5.7 = left LED
*/
#define P5DIR_INIT                                  (BIT7 | BIT6 | BIT5 | BIT0)
#define P5SEL_INIT                                  (BIT4 | BIT3 | BIT2 | BIT1)
#define P5OUT_INIT                                  0

/*
    P7.0 = LCD segment line
    P7.1 = LCD segment line
    P7.2 = LCD segment line
    P7.3 = LCD segment line
    P7.4 = LCD segment line
    P7.5 = LCD segment line
    P7.6 = LCD segment line
    P7.7 = LCD segment line
 */
#define P7DIR_INIT                                  0
#define P7SEL_INIT                                  0
#define P7OUT_INIT                                  0

/*
    P8.0 = LCD segment line
    P8.1 = LCD segment line
    P8.2 = LCD segment line
    P8.3 = LCD segment line
    P8.4 = LCD segment line
    P8.5 = LCD segment line
    P8.6 = LCD segment line
    P8.7 = LCD segment line
 */
#define P8DIR_INIT                                  0
#define P8SEL_INIT                                  0
#define P8OUT_INIT                                  0

/*
    P9.0 = LCD segment line
    P9.1 = LCD segment line
    P9.2 = LCD segment line
    P9.3 = LCD segment line
    P9.4 = LCD segment line
    P9.5 = LCD segment line
    P9.6 = LCD segment line
    P9.7 = LCD segment line
 */
#define P9DIR_INIT                                  0
#define P9SEL_INIT                                  0
#define P9OUT_INIT                                  0

/*
    P10.0 = LCD segment line
    P10.1 = LCD segment line
    P10.2 = LCD segment line
    P10.3 = LCD segment line
    P10.4 =
    P10.5 =
    P10.6 =
    P10.7 =
 */
//#define P10DIR_INIT                                 0
//#define P10SEL_INIT                                 0
//#define P10OUT_INIT                                 0

/*
    The LCD is a full 160 segment display, with 2 lines of 8 digits, plus various symbols.
 */
#define LCDACTL_INIT                                (LCDFREQ_128 | LCD4MUX | LCDSON | LCDON)
#define LCDAPCTL0_INIT                              (LCDS0 | LCDS4 | LCDS8 | LCDS12 | LCDS16 | LCDS20 | LCDS24 | LCDS28)
#define LCDAPCTL1_INIT                              (LCDS32 | LCDS36)
#define LCDAVCTL0_INIT                              (LCDCPEN)
#define LCDAVCTL1_INIT                              (0)

#define GAIN_STAGES                                 1
#define NEUTRAL_GAIN_STAGES                         1
#define I_HISTORY_STEPS                             1

/* Some things to improve stripping of the source code */
#if !defined(__MSP430__)
    #define __MSP430__
    #undef __MSP430_HAS_SD16_2__
    #undef __MSP430_HAS_SD16_3__
    #define __MSP430_HAS_ADC12__
#endif

#define with_DMA
