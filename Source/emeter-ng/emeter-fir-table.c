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
//  File: emeter-fir-table.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-fir-table.c,v 1.11 2009/01/19 10:42:36 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
// This file was generated automatically by generate-fir-table
//
#include <stdint.h>
#include <io.h>
#include <emeter-toolkit.h>

#include "emeter-structs.h"

#if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
/* The SD16 has a hardware method of performing phase shifts between the voltage
   and current channels. */
/* FIR coefficients to correct specific phase shift angles between + and - half
   a sample, at 50Hz */
/* Sampling 3276.8 times per second. */
const int16_t fir_coeffs[][2] =
{
    {  32767,   8231 }, //  2.7466 deg
    {  31762,   8358 }, //  2.7037 deg
    {  30786,   8485 }, //  2.6608 deg
    {  29839,   8612 }, //  2.6178 deg
    {  28921,   8739 }, //  2.5749 deg
    {  28029,   8866 }, //  2.5320 deg
    {  27162,   8993 }, //  2.4891 deg
    {  26319,   9120 }, //  2.4462 deg
    {  25500,   9247 }, //  2.4033 deg
    {  24704,   9374 }, //  2.3603 deg
    {  23928,   9502 }, //  2.3174 deg
    {  23174,   9629 }, //  2.2745 deg
    {  22439,   9756 }, //  2.2316 deg
    {  21723,   9883 }, //  2.1887 deg
    {  21026,  10010 }, //  2.1458 deg
    {  20346,  10137 }, //  2.1029 deg
    {  19683,  10265 }, //  2.0599 deg
    {  19036,  10392 }, //  2.0170 deg
    {  18405,  10519 }, //  1.9741 deg
    {  17790,  10646 }, //  1.9312 deg
    {  17188,  10773 }, //  1.8883 deg
    {  16601,  10901 }, //  1.8454 deg
    {  16028,  11028 }, //  1.8024 deg
    {  15467,  11155 }, //  1.7595 deg
    {  14920,  11282 }, //  1.7166 deg
    {  14384,  11410 }, //  1.6737 deg
    {  13861,  11537 }, //  1.6308 deg
    {  13349,  11664 }, //  1.5879 deg
    {  12848,  11791 }, //  1.5450 deg
    {  12357,  11919 }, //  1.5020 deg
    {  11877,  12046 }, //  1.4591 deg
    {  11407,  12173 }, //  1.4162 deg
    {  10947,  12301 }, //  1.3733 deg
    {  10497,  12428 }, //  1.3304 deg
    {  10055,  12555 }, //  1.2875 deg
    {   9622,  12683 }, //  1.2445 deg
    {   9198,  12810 }, //  1.2016 deg
    {   8783,  12937 }, //  1.1587 deg
    {   8375,  13065 }, //  1.1158 deg
    {   7975,  13192 }, //  1.0729 deg
    {   7583,  13320 }, //  1.0300 deg
    {   7198,  13447 }, //  0.9871 deg
    {   6821,  13575 }, //  0.9441 deg
    {   6450,  13702 }, //  0.9012 deg
    {   6087,  13830 }, //  0.8583 deg
    {   5730,  13957 }, //  0.8154 deg
    {   5379,  14085 }, //  0.7725 deg
    {   5035,  14212 }, //  0.7296 deg
    {   4697,  14340 }, //  0.6866 deg
    {   4365,  14467 }, //  0.6437 deg
    {   4038,  14595 }, //  0.6008 deg
    {   3717,  14723 }, //  0.5579 deg
    {   3402,  14851 }, //  0.5150 deg
    {   3092,  14978 }, //  0.4721 deg
    {   2787,  15106 }, //  0.4292 deg
    {   2488,  15234 }, //  0.3862 deg
    {   2193,  15362 }, //  0.3433 deg
    {   1903,  15489 }, //  0.3004 deg
    {   1618,  15617 }, //  0.2575 deg
    {   1337,  15745 }, //  0.2146 deg
    {   1061,  15873 }, //  0.1717 deg
    {    789,  16001 }, //  0.1287 deg
    {    522,  16129 }, //  0.0858 deg
    {    259,  16257 }, //  0.0429 deg
    {      0,  16384 }, //  0.0000 deg
    {   -255,  16512 }, // -0.0429 deg
    {   -506,  16640 }, // -0.0858 deg
    {   -754,  16769 }, // -0.1287 deg
    {   -997,  16896 }, // -0.1717 deg
    {  -1238,  17025 }, // -0.2146 deg
    {  -1474,  17153 }, // -0.2575 deg
    {  -1707,  17281 }, // -0.3004 deg
    {  -1937,  17409 }, // -0.3433 deg
    {  -2163,  17537 }, // -0.3862 deg
    {  -2387,  17666 }, // -0.4292 deg
    {  -2607,  17794 }, // -0.4721 deg
    {  -2824,  17923 }, // -0.5150 deg
    {  -3037,  18050 }, // -0.5579 deg
    {  -3248,  18179 }, // -0.6008 deg
    {  -3456,  18307 }, // -0.6437 deg
    {  -3661,  18436 }, // -0.6866 deg
    {  -3864,  18565 }, // -0.7296 deg
    {  -4063,  18693 }, // -0.7725 deg
    {  -4260,  18821 }, // -0.8154 deg
    {  -4454,  18950 }, // -0.8583 deg
    {  -4646,  19079 }, // -0.9012 deg
    {  -4835,  19207 }, // -0.9441 deg
    {  -5022,  19336 }, // -0.9871 deg
    {  -5206,  19465 }, // -1.0300 deg
    {  -5388,  19594 }, // -1.0729 deg
    {  -5568,  19723 }, // -1.1158 deg
    {  -5745,  19852 }, // -1.1587 deg
    {  -5920,  19980 }, // -1.2016 deg
    {  -6093,  20110 }, // -1.2445 deg
    {  -6264,  20239 }, // -1.2875 deg
    {  -6432,  20367 }, // -1.3304 deg
    {  -6599,  20497 }, // -1.3733 deg
    {  -6763,  20626 }, // -1.4162 deg
    {  -6926,  20755 }, // -1.4591 deg
    {  -7086,  20884 }, // -1.5020 deg
    {  -7245,  21014 }, // -1.5450 deg
    {  -7401,  21142 }, // -1.5879 deg
    {  -7556,  21272 }, // -1.6308 deg
    {  -7709,  21401 }, // -1.6737 deg
    {  -7860,  21531 }, // -1.7166 deg
    {  -8010,  21661 }, // -1.7595 deg
    {  -8157,  21790 }, // -1.8024 deg
    {  -8303,  21919 }, // -1.8454 deg
    {  -8448,  22049 }, // -1.8883 deg
    {  -8590,  22178 }, // -1.9312 deg
    {  -8731,  22308 }, // -1.9741 deg
    {  -8871,  22438 }, // -2.0170 deg
    {  -9009,  22568 }, // -2.0599 deg
    {  -9145,  22698 }, // -2.1029 deg
    {  -9280,  22828 }, // -2.1458 deg
    {  -9414,  22958 }, // -2.1887 deg
    {  -9546,  23088 }, // -2.2316 deg
    {  -9676,  23218 }, // -2.2745 deg
    {  -9805,  23348 }, // -2.3174 deg
    {  -9933,  23478 }, // -2.3603 deg
    { -10059,  23608 }, // -2.4033 deg
    { -10184,  23738 }, // -2.4462 deg
    { -10308,  23869 }, // -2.4891 deg
    { -10431,  24000 }, // -2.5320 deg
    { -10552,  24130 }, // -2.5749 deg
    { -10672,  24261 }, // -2.6178 deg
    { -10790,  24391 }, // -2.6608 deg
    { -10908,  24522 }  // -2.7037 deg
};
#endif

#if (defined(__HAS_SD_ADC__)  ||  !defined(__MSP430__))  &&  defined(REACTIVE_POWER_BY_QUADRATURE_SUPPORT)
/* The FIR table is needed to create accurate 90 degree phase shifts */
// FIR coefficients to correct specific phase shift angles at 50Hz
// and sampling 4096.000000 times per second.
const int16_t fir_coeffs[][2] =
{
    {  32767,   8198 }, // 2.1973 deg
    {  31761,   8326 }, // 2.1629 deg
    {  30785,   8454 }, // 2.1286 deg
    {  29838,   8582 }, // 2.0943 deg
    {  28918,   8710 }, // 2.0599 deg
    {  28025,   8838 }, // 2.0256 deg
    {  27158,   8965 }, // 1.9913 deg
    {  26315,   9093 }, // 1.9569 deg
    {  25496,   9221 }, // 1.9226 deg
    {  24698,   9349 }, // 1.8883 deg
    {  23923,   9477 }, // 1.8539 deg
    {  23168,   9605 }, // 1.8196 deg
    {  22433,   9733 }, // 1.7853 deg
    {  21716,   9861 }, // 1.7509 deg
    {  21019,   9988 }, // 1.7166 deg
    {  20338,  10116 }, // 1.6823 deg
    {  19675,  10244 }, // 1.6479 deg
    {  19028,  10372 }, // 1.6136 deg
    {  18397,  10500 }, // 1.5793 deg
    {  17781,  10628 }, // 1.5450 deg
    {  17180,  10756 }, // 1.5106 deg
    {  16593,  10884 }, // 1.4763 deg
    {  16019,  11011 }, // 1.4420 deg
    {  15459,  11139 }, // 1.4076 deg
    {  14911,  11267 }, // 1.3733 deg
    {  14376,  11395 }, // 1.3390 deg
    {  13852,  11523 }, // 1.3046 deg
    {  13340,  11651 }, // 1.2703 deg
    {  12839,  11779 }, // 1.2360 deg
    {  12348,  11907 }, // 1.2016 deg
    {  11869,  12035 }, // 1.1673 deg
    {  11399,  12162 }, // 1.1330 deg
    {  10939,  12290 }, // 1.0986 deg
    {  10488,  12418 }, // 1.0643 deg
    {  10047,  12546 }, // 1.0300 deg
    {   9614,  12674 }, // 0.9956 deg
    {   9190,  12802 }, // 0.9613 deg
    {   8775,  12930 }, // 0.9270 deg
    {   8367,  13058 }, // 0.8926 deg
    {   7968,  13185 }, // 0.8583 deg
    {   7576,  13313 }, // 0.8240 deg
    {   7191,  13441 }, // 0.7896 deg
    {   6814,  13569 }, // 0.7553 deg
    {   6444,  13697 }, // 0.7210 deg
    {   6080,  13825 }, // 0.6866 deg
    {   5724,  13953 }, // 0.6523 deg
    {   5373,  14081 }, // 0.6180 deg
    {   5029,  14209 }, // 0.5836 deg
    {   4691,  14337 }, // 0.5493 deg
    {   4360,  14465 }, // 0.5150 deg
    {   4033,  14593 }, // 0.4807 deg
    {   3713,  14720 }, // 0.4463 deg
    {   3398,  14848 }, // 0.4120 deg
    {   3088,  14976 }, // 0.3777 deg
    {   2784,  15104 }, // 0.3433 deg
    {   2485,  15232 }, // 0.3090 deg
    {   2190,  15360 }, // 0.2747 deg
    {   1901,  15488 }, // 0.2403 deg
    {   1616,  15616 }, // 0.2060 deg
    {   1336,  15744 }, // 0.1717 deg
    {   1060,  15872 }, // 0.1373 deg
    {    789,  16000 }, // 0.1030 deg
    {    522,  16128 }, // 0.0687 deg
    {    259,  16256 }, // 0.0343 deg
    {     -0,  16384 }, // -0.0000 deg
    {   -255,  16512 }, // -0.0343 deg
    {   -506,  16640 }, // -0.0687 deg
    {   -753,  16768 }, // -0.1030 deg
    {   -996,  16896 }, // -0.1373 deg
    {  -1236,  17024 }, // -0.1717 deg
    {  -1472,  17152 }, // -0.2060 deg
    {  -1705,  17280 }, // -0.2403 deg
    {  -1934,  17408 }, // -0.2747 deg
    {  -2160,  17536 }, // -0.3090 deg
    {  -2383,  17664 }, // -0.3433 deg
    {  -2602,  17792 }, // -0.3777 deg
    {  -2819,  17920 }, // -0.4120 deg
    {  -3032,  18049 }, // -0.4463 deg
    {  -3242,  18177 }, // -0.4807 deg
    {  -3450,  18305 }, // -0.5150 deg
    {  -3654,  18433 }, // -0.5493 deg
    {  -3856,  18561 }, // -0.5836 deg
    {  -4055,  18689 }, // -0.6180 deg
    {  -4252,  18817 }, // -0.6523 deg
    {  -4445,  18945 }, // -0.6866 deg
    {  -4636,  19074 }, // -0.7210 deg
    {  -4825,  19202 }, // -0.7553 deg
    {  -5011,  19330 }, // -0.7896 deg
    {  -5195,  19458 }, // -0.8240 deg
    {  -5376,  19586 }, // -0.8583 deg
    {  -5555,  19714 }, // -0.8926 deg
    {  -5732,  19843 }, // -0.9270 deg
    {  -5906,  19971 }, // -0.9613 deg
    {  -6079,  20099 }, // -0.9956 deg
    {  -6249,  20227 }, // -1.0300 deg
    {  -6417,  20356 }, // -1.0643 deg
    {  -6583,  20484 }, // -1.0986 deg
    {  -6747,  20612 }, // -1.1330 deg
    {  -6908,  20740 }, // -1.1673 deg
    {  -7068,  20869 }, // -1.2016 deg
    {  -7226,  20997 }, // -1.2360 deg
    {  -7382,  21125 }, // -1.2703 deg
    {  -7536,  21254 }, // -1.3046 deg
    {  -7689,  21382 }, // -1.3390 deg
    {  -7839,  21510 }, // -1.3733 deg
    {  -7988,  21639 }, // -1.4076 deg
    {  -8135,  21767 }, // -1.4420 deg
    {  -8281,  21895 }, // -1.4763 deg
    {  -8424,  22024 }, // -1.5106 deg
    {  -8566,  22152 }, // -1.5450 deg
    {  -8707,  22281 }, // -1.5793 deg
    {  -8846,  22409 }, // -1.6136 deg
    {  -8983,  22537 }, // -1.6479 deg
    {  -9119,  22666 }, // -1.6823 deg
    {  -9253,  22794 }, // -1.7166 deg
    {  -9386,  22923 }, // -1.7509 deg
    {  -9517,  23051 }, // -1.7853 deg
    {  -9647,  23180 }, // -1.8196 deg
    {  -9775,  23308 }, // -1.8539 deg
    {  -9903,  23437 }, // -1.8883 deg
    { -10028,  23565 }, // -1.9226 deg
    { -10153,  23694 }, // -1.9569 deg
    { -10276,  23823 }, // -1.9913 deg
    { -10397,  23951 }, // -2.0256 deg
    { -10518,  24080 }, // -2.0599 deg
    { -10637,  24208 }, // -2.0943 deg
    { -10755,  24337 }, // -2.1286 deg
    { -10872,  24466 }, // -2.1629 deg
};
#endif
