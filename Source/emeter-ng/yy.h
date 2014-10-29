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
//    File: emeter-1ph-stefan.h
//  Stefan's FE427 reference board
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-1ph-esp-bare-bones.h,v 1.6 2009/04/27 09:05:25 a0754793 Exp $
//
//--------------------------------------------------------------------------

/* This is a demonstration version of the configuration data file for the
   MSP430 e-meter reference software. For most users, this file will need
   to be edited to match the hardware being used. */

/* N.B.
   This e-meter software uses a sampling rate of 3276.8/s. For a
   mains supply at exactly 50Hz, each sample is 5.49degrees of
   phase from the last. */

#define ESP_SUPPORT                                 1

/*! This switch enables the sending of the current readings, through a UART port,
    for use in cooperative calibration with other meters. */
#define SERIAL_CALIBRATION_REF_SUPPORT              1
#define SERIAL_CALIBRATION_PASSWORD_1               0x1234
#define SERIAL_CALIBRATION_PASSWORD_2               0x5678
#define SERIAL_CALIBRATION_PASSWORD_3               0x9ABC
#define SERIAL_CALIBRATION_PASSWORD_4               0xDEF0


/*! This switch selects single phase mode. If SINGLE_PHASE is not set the
    software will be built for 3-phase operation. */
#define SINGLE_PHASE                                1

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
#define custom_energy_pulse_start()                 (P1OUT |= BIT2)

/*! This is called to end some customer specific energy pulse operation. For example,
    turning off some special indicator device. */
#define custom_energy_pulse_end()                   (P1OUT &= ~BIT2)

/*! The duration of the LED on time for an energy pulse. This is measured in
    ADC samples (i.e. increments 1/3276.8s). The maximum allowed is 255, giving a
    pulse of about 78ms. 163 gives a 50ms pulse. */
#define ENERGY_PULSE_DURATION                       163

/*! Tiny power levels should not record at all, as they may just be rounding errors,
    noise, or the consumption of the meter itself. This value is the cutoff level,
    in 0.01W increments. */
#define RESIDUAL_POWER_CUTOFF                       0 //250

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
#define total_active_energy_pulse_start()           (P1OUT |= BIT3)

/*! This is called to turn off a total energy pulse indicator (e.g. LED or LCD segment) */
#define total_active_energy_pulse_end()             (P1OUT &= ~BIT3)

/*! This is called to turn on a total reactive energy pulse indicator (e.g. LED or LCD segment) */
#define total_reactive_energy_pulse_start()         /**/

/*! This is called to turn off a total reactive energy pulse indicator (e.g. LED or LCD segment) */
#define total_reactive_energy_pulse_end()           /**/
#endif

/*! This is the number of successive measurement blocks which must agree the
    unbalanced status has changed before we accept it. */
#define PHASE_UNBALANCED_PERSISTENCE_CHECK          5

/*! This enables monitoring of the balance between the current in the live and neutral
    leads, for tamper detection. */
#define POWER_BALANCE_DETECTION_SUPPORT             1

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
//#define LIMP_MODE_SUPPORT                         1
#undef LIMP_MODE_SUPPORT

/*! If limp mode is supported, these set the voltage thresholds for switching
    bewteen normal mode and limp mode. */
//#define LIMP_MODE_VOLTAGE_THRESHOLD               50
//#define NORMAL_MODE_VOLTAGE_THRESHOLD             80

/*! If limp mode is supported, this sets the threshold current in mA, below which we
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
#undef POWER_FACTOR_SUPPORT

/*! This switch selects support for measuring the total active energy consumption. */
#define TOTAL_ACTIVE_ENERGY_SUPPORT                 1

/*! This switch selects support for measuring the active energy consumption on a phase
    by phase basis. This is only significant for poly-phase meters. */
#undef PER_PHASE_ACTIVE_ENERGY_SUPPORT

/*! This switch selects support for measuring the total reactive energy. */
#define TOTAL_REACTIVE_ENERGY_SUPPORT               1

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
#define TEMPERATURE_SUPPORT                         1

/*! This switch selects support for the monitoring of a hall-effect magnetic sensor. This can be
    used to detect very high magnetic fields which might be used for tampering (i.e. fields strong
    enough to paralyse CTs, and other magnetic components in the meter's design). */
#undef MAGNETIC_INTERFERENCE_SUPPORT

/*! This sets the number of consecutive seconds magnetic interference must be present
    or not present before the magnetic tampering state will switch. */
#define MAGNETIC_INTERFERENCE_PERSISTENCE_CHECK     5

/*! The hall effect sensors used to detect magnetic tampering output pulses, whose on/off ratio
    reflects the scale of the magnetic field. This sets the ratio at which magnetic tampering is
    declared. */
#define MAGNETIC_INTERFERENCE_SAMPLE_RATIO          10

/*! When magnetic tampering is detected, the output of CTs is completely unreliable. The meter can only
    assume there is a very high load at unity power factor, and charge accordingly. This sets the current
    to be used in this calculation. It is specified in milliamps. */
#define MAGNETIC_INTERFERENCE_CURRENT               60000

/* This selects real time clock support. This is implemented in software on
   the MSP430. */
#define RTC_SUPPORT                                 1

/* Related definitions to get special action routines to be called at various
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

/* Corrected RTC support enables temperature and basic error compensation for
   the MSP430's 32kHz crystal oscillator, so it makes for a higher quality RTC
   source, even using low accuracy (eg 20ppm) crystals. */
#define CORRECTED_RTC_SUPPORT                       1

/*! This select support for a custom real time clock. The customer's routine
    will be called every second. */
#undef CUSTOM_RTC_SUPPORT
#if defined(__MSP430__)
#define custom_rtc()                                /**/
#define custom_rtc_set()                            /**/
#define custom_rtc_retrieve(x)                      /**/
#endif

/*! This switch enables power down to battery backup status on loss of line
    power. */
#undef POWER_DOWN_SUPPORT
/*! These switches select a method of detecting when power is restored, if
   power down mode is supported. */
/*! This method assumes the voltage waveform is being turned into simple
    digital pulses into an I/O pin. If this feature is used, POWER_UP_VOLTAGE_PULSE
    must define the way a voltage pulse is sensed. */
#undef POWER_UP_BY_VOLTAGE_PULSES
//#define power_up_voltage_pulse()                  (P1IN & BIT0)

/*! This method assumes the pre-regulator power supply voltage is being
    monitored by Comparator A. This method is suitable for meters which
    support a live/neutral only mode, for tamper resistance. */
#undef POWER_UP_BY_SUPPLY_SENSING

/*! This is called every ADC interrupt, after the main DSP work has finished.
    It can be used for things like custom keypad operations. It is important
    this is a very short routine, as it is called from the main ADC interrupt. */
#define custom_adc_interrupt()                      /**/

/*! This is called at intervals, to allow integration of RF protocols for AMR. */
#define custom_rf_sniffer()                         /**/

/*! This is called at intervals, to allow integration of RF protocols for AMR. */
#define custom_rf_exchange()                        /**/

/*! This switch enables custom routines to be called as the meter enters and
    exits the power fail state. These might be used to save and restore things,
    using non-volatile memory. */
#undef CUSTOM_POWER_DOWN_SUPPORT

/*! Many data logging requirements can be met by using only the MSP430's
    info memory. If an external serial EEPROM is needed for more
    complex requirements, this switch will enable an interface to
    I2C type serial EEPROMs. Basic routines to driver these EEPROMs are
    included in the toolkit. Routines to actually store and retrieve
    information are left to the meter designer. */
#undef EXTERNAL_EEPROM_SUPPORT

#if defined(__MSP430__)
#define enable_eeprom_port()                        /**/
#define disable_eeprom_port()                       /**/
#define set_eeprom_port_idle()                      /**/
#endif

/*! This switch, in combination with the calibrator switch, enables calibration
    with the meter cooperating with an external reference, through a UART port. */
#define SERIAL_CALIBRATION_SUPPORT                  1

/*! This switch enables support of an IR receiver and/or transmitter for
    programming and/or reading the meter. */
#undef IEC1107_SUPPORT
#undef IEC62056_21_SUPPORT
#define DLT645_SUPPORT                              1
/*! This switch selects support for a short range RF link. */
#undef RF_LINK_SUPPORT
#undef MESH_NET_SUPPORT

/*! This definition specifies the port bit change required to enable the IR
    receiver, when one is used. */
#define enable_ir_receiver()                        /**/

/*! This definition specifies the port bit change required to disable the IR
    receiver, for power saving, when one is used. */
#define disable_ir_receiver()                       /**/

/*! This defines the speed of UART 0 */
#define UART_PORT_0_SUPPORT                             1
#define UART0_BAUD_RATE                                 9600
#undef UART0_IEC62056_21_SUPPORT
#define UART0_DLT645_SUPPORT                            1

/*! This definition specifies that the meter support some form of self-test. The
    level of self-testing may vary with the specific implementation. */
#define SELF_TEST_SUPPORT                               1
#define METER_FAILURE_EEPROM_COMMUNICATION              1
#define METER_FAILURE_EEPROM_CONTENTS                   2

/*! This switch enables a rolling display of all the meter's measured
    parameters, for demonstration purposes. The parameters are displayed
    on a local attached LCD, which may be driven by an MSP430's internal
    LCD controller, or be part of a standard external module.
    Real meters will generally need to replace this with their own custom
    display routines. */
#define BASIC_LCD_SUPPORT   1

/*! This switch enables the calling of a custom LCD display routine. This allows
    easy integration of custom code for specific meter designs. */
#undef CUSTOM_LCD_SUPPORT

/*! This enables use of a startburst LCD display. The display is assumed to be
    a MUX4 type, with each character cell occupying 2 bytes of LCD memory. */
#undef USE_STARBURST

/*! This is called to perform initialisation of a custom display */
#define custom_lcd_init()                           /**/

/*! This is called to perform custom display processing when the meter goes to
    sleep. This is typically turning off the display, to conserve energy. */
#define custom_lcd_sleep_handler()                  (LCDsleep(), display_power_fail_message())

/*! This is called to perform custom display processing when the meter wakes up from
    the sleeping condition. */
#define custom_lcd_wakeup_handler()                 LCDawaken()

/*! This switch enables multi-rate tariff features */
#undef MULTI_RATE_SUPPORT

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
#undef BASIC_KEYPAD_SUPPORT

/*! This switch enables the calling of a custom keypad handler routine. This
    allows easy integration of custom code for specific meter designs. */
#undef CUSTOM_KEYPAD_SUPPORT
#if defined(__MSP430__)
#define custom_keypad_handler()                     /*keypad_handler()*/
#else
#define custom_keypad_handler()                     /**/
#endif

/*! These are used with the above switches to determine the port bits assigned
    to the keys. Only define the buttons which are actually used. */
//#define sense_key_1_up()                            ((P2IN & BIT2))
//#define sense_key_2_up()                            ((P2IN & BIT3))
//#define sense_key_3_up()                            ((P1IN & BIT0))
//#define sense_key_4_up()                            ((P1IN & BIT1))

/*! These are used to select the features required of the keys. They are bits in
    the key_states variable. Only define the features being used. Usually for
    each key the "DOWN" option and either the "LONG_DOWN" or "REPEAT_DOWN" option
    should be specified. Do not select both the "LONG_DOWN" and "REPEAT_DOWN" for the
    same key. */
//#define KEY_1_DOWN                                  0x01
//#define KEY_1_LONG_DOWN                           0x02
//#define KEY_1_REPEAT_DOWN                           0x02
//#define KEY_2_DOWN                                  0x04
//#define KEY_2_LONG_DOWN                             0x08
//#define KEY_2_REPEAT_DOWN                           0x08
//#define KEY_3_DOWN                                0x10
//#define KEY_3_LONG_DOWN                           0x20
//#define KEY_3_REPEAT_DOWN                         0x20
//#define KEY_4_DOWN                                0x40
//#define KEY_4_LONG_DOWN                           0x80
//#define KEY_4_REPEAT_DOWN                         0x80

/*! Normally the meter software only calculates the properly scaled values
    for voltage, current, etc. as these values are needed. This define
    enables additional global parameters, which are regularly updated with
    all the metrics gathered by the meter. This is generally less efficient,
    as it means calculating things more often than necessary. However, some
    may find this easier to use, so it is offered as a choice for the meter
    designer. */
#define PRECALCULATED_PARAMETER_SUPPORT             1

/*! A PWM generator can be used to add dithering to the signals, to expand the
    effective resolution of the ADC12. */
#undef PWM_DITHERING_SUPPORT

/*! The gain setting for the first current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define CURRENT_LIVE_GAIN                           SD16GAIN_16

/*! The gain setting for the second current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define CURRENT_NEUTRAL_GAIN                        SD16GAIN_1

/*! The gain setting for the voltage channel of the SD16, for devices using the
    SD16 sigma-delta ADC. This is usually GAIN_1 (i.e. x1 gain). */
#define VOLTAGE_GAIN                                SD16GAIN_1

/*! The following allows the live and neutral inputs to be swapped. The usual
    arrangement if that current channel 1 (input 0) is live and current channel
    2 (input 1) is neutral. If CURRENT_CH_2_IS_LIVE is defined, these inputs are
    reversed. */
#define CURRENT_CH_2_IS_LIVE

/*! This switch select the use of current dependant phase correction, to allow
    the use of non-linear CTs. */
#define PHASE_CORRECTION_SUPPORT                    1

/*! This switch enables dynmaic phase correction for non-linear CTs, or other sensors
    for which the phase changes with the current. */
#undef DYNAMIC_PHASE_CORRECTION_SUPPORT

#define DEFAULT_BASE_PHASE_CORRECTION               73
#define DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION       73

#define DEFAULT_PHASE_CORRECTION1                   0
#define DEFAULT_GAIN_CORRECTION1                    0
#define DEFAULT_PHASE_CORRECTION2                   0
#define DEFAULT_GAIN_CORRECTION2                    0

/*! This switch selects where a backup battery's condition is monitored. */
#undef BATTERY_MONITOR_SUPPORT

/*! Target specific definitions for controlling the indicator LEDs */
#if defined(__MSP430__)

/*! This selects the detection of the reversed power condition. */
#define PHASE_REVERSED_DETECTION_SUPPORT            1

/*! This selects that the reversed power condition is to be treated as tampering. */
#define PHASE_REVERSED_IS_TAMPERING                 1

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

/*! This alters the behaviour of the meter as it changes from the power down state
    to normal operation. The meter will typically have accumulated some fraction of
    a pulse of energy before the power was lost. When power is restored this may result
    in a pulse being generated almost immediately. 
    
    Some customers expect a meter to count pulses from the time the mains power
    is restored, looosing any fractional count of an energy pulse. This looses the
    power utility a fraction of a pulse of revenue, and means the meter is not
    doing the best it can. It can, however, greatly improve subscriber relations,
    and avoid trivial customer complaints. */
#define LOOSE_FRACTIONAL_PULSE_AT_POWER_ON          1

/*! This defines the default offset for the temperature diode, in ADC units */
#define DEFAULT_TEMPERATURE_OFFSET                  (1615*8)

/*! This defines the default scaling factor for the temperature diode, in ADC units */
#define DEFAULT_TEMPERATURE_SCALING                 (704*2)

/*! This defines the default room temperature, in 1/10 degree Celcius increments. */
#define DEFAULT_ROOM_TEMPERATURE                    250

#define DEFAULT_V_RMS_SCALE_FACTOR_A                20147
#define DEFAULT_V_RMS_LIMP_SCALE_FACTOR_A           21280
#define DEFAULT_I_RMS_SCALE_FACTOR_A                8816
#define DEFAULT_I_RMS_LIMP_SCALE_FACTOR_A           4360
#define DEFAULT_P_SCALE_FACTOR_A_LOW                2989

#define DEFAULT_FREQUENCY_PHASE_FACTOR              500
#define DEFAULT_FREQUENCY_GAIN_FACTOR               0

#define DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL          4905
#define DEFAULT_P_SCALE_FACTOR_NEUTRAL              4900

#define DEFAULT_BASE_PHASE_A_CORRECTION_LOW         0

#define custom_set_consumption(x,y)                 /**/
#define custom_magnetic_sensor_test()               /**/

#include "lierda_1line_lcd.h"

/* LCD display parameters */

#define LCD_CELLS                9
#define LCD_POS_BASE             0
#define LCD_POS_STEP             1
//typedef uint16_t lcd_cell_t;

/* The LCD has 2 rows of 8 digits, plus various symbols. */
#define TWO_LINE_LCD
#define FIRST_ROW_START                             0
#define FIRST_ROW_CHARS                             2
#define SECOND_ROW_START                            2
#define SECOND_ROW_CHARS                            6

#define ICON_BATTERY                                LCD_LOW_BATTERY

#define DISPLAY_TYPE_POSITION                       FIRST_ROW_START

#define INFO_POSITION                               SECOND_ROW_START
#define INFO_CHARS                                  SECOND_ROW_CHARS

//#define ICON_TIME                                   LCD_TOP_TIME
#define HOUR_POSITION                               (SECOND_ROW_START + 0)
#define ICON_TIME_COLON_1                           LCD_COLON_2
#define MINUTE_POSITION                             (SECOND_ROW_START + 2)
#define ICON_TIME_COLON_2                           LCD_COLON_4
#define SECONDS_POSITION                            (SECOND_ROW_START + 4)

#define YEAR_POSITION                               (SECOND_ROW_START + 0)
#define ICON_DATE_COLON_1                           LCD_COLON_2
#define MONTH_POSITION                              (SECOND_ROW_START + 2)
#define ICON_DATE_COLON_2                           LCD_COLON_4
#define DAY_POSITION                                (SECOND_ROW_START + 4)

//#define ICON_ACTIVE_POWER
#define ICON_kW                                     LCD_KW
#define FIRST_ACTIVE_POWER_POSITION                 SECOND_ROW_START
#define ACTIVE_POWER_DIGITS                         6
#define ICON_ACTIVE_POWER_DECIMAL_POINT             LCD_DP_2
#define ACTIVE_POWER_RESOLUTION                     4

//#define ICON_REACTIVE_POWER
//#define ICON_kvar
#define FIRST_REACTIVE_POWER_POSITION               SECOND_ROW_START
#define REACTIVE_POWER_DIGITS                       6
#define ICON_REACTIVE_POWER_DECIMAL_POINT           LCD_DP_2
#define REACTIVE_POWER_RESOLUTION                   4

#define FIRST_APPARENT_POWER_POSITION               SECOND_ROW_START
#define APPARENT_POWER_DIGITS                       6
#define ICON_APPARENT_POWER_DECIMAL_POINT           LCD_DP_2
#define APPARENT_POWER_RESOLUTION                   4

//#define ICON_ACTIVE_ENERGY
#define ICON_ICON_H_FOR_kW                          LCD_H_FOR_KW
#define FIRST_ACTIVE_ENERGY_POSITION                SECOND_ROW_START
#define ACTIVE_ENERGY_DIGITS                        6
#define ICON_ACTIVE_ENERGY_DECIMAL_POINT            LCD_DP_4
#define ACTIVE_ENERGY_RESOLUTION                    2

//#define ICON_REACTIVE_ENERGY
//#define ICON_kvarH
#define FIRST_REACTIVE_ENERGY_POSITION              SECOND_ROW_START
#define REACTIVE_ENERGY_DIGITS                      6
#define ICON_REACTIVE_ENERGY_DECIMAL_POINT          LCD_DP_4
#define REACTIVE_ENERGY_RESOLUTION                  2

//#define ICON_POWER_FACTOR
//#define ICON_COS_PHI
#define FIRST_POWER_FACTOR_POSITION                 SECOND_ROW_START
#define POWER_FACTOR_DIGITS                         6
#define ICON_POWER_FACTOR_DECIMAL_POINT             LCD_DP_2
#define POWER_FACTOR_RESOLUTION                     4

//#define ICON_VOLTAGE
//#define ICON_V
#define FIRST_VOLTAGE_POSITION                      SECOND_ROW_START
#define VOLTAGE_DIGITS                              6
#define ICON_VOLTAGE_DECIMAL_POINT                  LCD_DP_4
#define VOLTAGE_RESOLUTION                          2

//#define ICON_CURRENT
//#define ICON_A
#define FIRST_CURRENT_POSITION                      SECOND_ROW_START
#define CURRENT_DIGITS                              6
#define ICON_CURRENT_DECIMAL_POINT                  LCD_DP_4
#define CURRENT_RESOLUTION                          2

//#define ICON_TEMPERATURE
//#define ICON_CELCIUS
#define FIRST_TEMPERATURE_POSITION                  SECOND_ROW_START
#define TEMPERATURE_DIGITS                          6
#define TEMPERATURE_RESOLUTION                      1
#define ICON_TEMPERATURE_DECIMAL_POINT              LCD_DP_5

//#define ICON_FREQUENCY
//#define ICON_HERTZ
#define FIRST_FREQUENCY_POSITION                    SECOND_ROW_START
#define FREQUENCY_DIGITS                            6
#define FREQUENCY_RESOLUTION                        2
#define ICON_FREQUENCY_DECIMAL_POINT                LCD_DP_4

#define TEXT_MESSAGE_LENGTH     6

/* LCD display sequence table */
#define DISPLAY_STEP_SEQUENCE \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_ACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_REACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_VOLTAGE, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_CURRENT, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_MAINS_FREQUENCY, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_ACTIVE_ENERGY, \
    DISPLAY_ITEM_SELECT_PHASE_1, \
    DISPLAY_ITEM_REACTIVE_ENERGY, \
    DISPLAY_ITEM_SELECT_TOTAL, \
    DISPLAY_ITEM_ACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_TOTAL, \
    DISPLAY_ITEM_REACTIVE_POWER, \
    DISPLAY_ITEM_SELECT_TOTAL, \
    DISPLAY_ITEM_TIME, \
    DISPLAY_ITEM_SELECT_TOTAL, \
    DISPLAY_ITEM_DATE, \
    DISPLAY_ITEM_SELECT_RESTART

#define custom_lcd_clear_periphery() \
    LCDicon(LCD_LOW_BATTERY, FALSE); \
    LCDicon(LCD_8_0, FALSE); \
    LCDicon(LCD_8_1, FALSE); \
    LCDicon(LCD_LAST_MONTH, FALSE); \
    LCDicon(LCD_8_3, FALSE); \
    LCDicon(LCD_8_4, FALSE); \
    LCDicon(LCD_8_5, FALSE); \
    LCDicon(LCD_8_6, FALSE); \
    LCDicon(LCD_8_7, FALSE); \
    LCDicon(LCD_9_0, FALSE); \
    LCDicon(LCD_9_1, FALSE); \
    LCDicon(LCD_9_2, FALSE); \
    LCDicon(LCD_9_3, FALSE); \
    LCDicon(LCD_DATE, FALSE); \
    LCDicon(LCD_TIME, FALSE); \
    LCDicon(LCD_9_6, FALSE);

#define custom_lcd_clear_line_1_tags(); /**/

#define custom_lcd_clear_line_2_tags(); \
    LCDicon(LCD_H_FOR_KW, FALSE); \
    LCDicon(LCD_KW, FALSE); \
    LCDicon(LCD_COLON_2, FALSE); \
    LCDicon(LCD_DP_2, FALSE); \
    LCDicon(LCD_COLON_4, FALSE); \
    LCDicon(LCD_DP_4, FALSE); \
    LCDicon(LCD_DP_5, FALSE);

#define USE_WATCHDOG            1

// P1.0 == LED
// P1.1 == LED
// P1.2 == LCD + EEPROM SDA
// P1.3 == LCD + EEPROM SCL
// P1.4 == LCD
// P1.5 == LCD
// P1.6 == LCD
// P1.7 == LCD
// P2.0 == LCD
// P2.1 == LCD
// P2.2 == push button
// P2.3 == push button
// P2.4 == TxD
// P2.5 == RxD
#define P1DIR_INIT      (BIT3 | BIT2 | BIT1)
#define P1SEL_INIT      (0)
#define P1OUT_INIT      (0)

#define P2DIR_INIT      (BIT4)
#define P2SEL_INIT      (BIT5 | BIT4)
#define P2OUT_INIT      0

#define LCD_INIT        (LCDSG0_2 | LCD4MUX | LCDON)


#define GAIN_STAGES                                 1
#define NEUTRAL_GAIN_STAGES                         1
#define I_HISTORY_STEPS                             2

/* Some things to improve stripping of the source code */
#if !defined(__MSP430__)
    #define __MSP430__
    #define __MSP430_HAS_SD16_3__
    #undef __MSP430_HAS_ADC12__
#endif
