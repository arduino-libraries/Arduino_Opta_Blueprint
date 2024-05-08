/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaAnalogProtocol.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240508
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTAANALOGPROTOCOL
#define OPTAANALOGPROTOCOL

#include "OptaBlueProtocol.h"
/* ################### OPTA ANALOG MESSAGES ############################### */

#define OA_ENABLE 0x01
#define OA_DISABLE 0x02

#define OA_CHANNEL_POS 0x03

/* #################### */
/* ADC related messages */
/* #################### */

/* REQUEST from controller:  setup channel ADC - argument 0x09 */
#define ARG_OA_CH_ADC 0x09
#define LEN_OA_CH_ADC 0x07
#define OA_CH_ADC_CHANNEL_POS OA_CHANNEL_POS // used also in GET ADC VALUE
#define OA_CH_ADC_TYPE_POS 0x04
#define OA_CH_ADC_PULL_DOWN_POS 0x05
#define OA_CH_ADC_REJECTION_POS 0x06
#define OA_CH_ADC_DIAGNOSTIC_POS 0x07
#define OA_CH_ADC_MOVING_AVE_POS 0x08
#define OA_CH_ADC_ADDING_ADC_POS 0x09
#define OA_CH_ADC_LEN (BP_HEADER_DIM + LEN_OA_CH_ADC)
#define OA_CH_ADC_LEN_CRC (OA_CH_ADC_LEN + 1)
#ifdef BP_USE_CRC
#define CFG_OA_CH_ADC_LEN OA_CH_ADC_LEN_CRC
#else
#define CFG_OA_CH_ADC_LEN OA_CH_ADC_LEN
#endif

/* NO answer expected */

/* REQUEST from controller: get ADC value for channel - argument 0x0A*/
#define ARG_OA_GET_ADC 0x0A
#define LEN_OA_GET_ADC 0x01
#define OA_GET_ADC_LEN (BP_HEADER_DIM + LEN_OA_GET_ADC)
#define OA_GET_ADC_LEN_CRC (OA_GET_ADC_LEN + 1)
/* ANSWER FROM expansion: get ADC value for channel */
#define ANS_ARG_OA_GET_ADC ARG_OA_GET_ADC
#define ANS_LEN_OA_GET_ADC 0x03
#define ANS_OA_ADC_VALUE_POS 0x04
#define ANS_OA_ADC_CHANNEL_POS OA_CHANNEL_POS
#define ANS_OA_GET_ADC_LEN (BP_HEADER_DIM + ANS_LEN_OA_GET_ADC)
#define ANS_OA_GET_ADC_LEN_CRC (ANS_OA_GET_ADC_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_OA_GET_ADC_LEN ANS_OA_GET_ADC_LEN_CRC
#else
#define CTRL_ANS_OA_GET_ADC_LEN ANS_OA_GET_ADC_LEN
#endif

/* REQUEST from controller: get ALL ADC value - argument 0x0B */
#define ARG_OA_GET_ALL_ADC 0x0B
#define LEN_OA_GET_ALL_ADC 0x00
#define OA_GET_ADC_ALL_LEN (BP_HEADER_DIM + LEN_OA_GET_ALL_ADC)
#define OA_GET_ADC_ALL_LEN_CRC (OA_GET_ADC_ALL_LEN + 1)
/* ANSWER from expansion: get ALL ADC value */
#define ANS_ARG_OA_GET_ALL_ADC ARG_OA_GET_ALL_ADC
#define ANS_LEN_OA_GET_ALL_ADC 0x10
#define ANS_OA_ADC_GET_ALL_VALUE_POS 0x03
#define ANS_OA_GET_ADC_ALL_LEN (BP_HEADER_DIM + ANS_LEN_OA_GET_ALL_ADC)
#define ANS_OA_GET_ADC_ALL_LEN_CRC (ANS_OA_GET_ADC_ALL_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_OA_GET_ALL_ADC_LEN ANS_OA_GET_ADC_ALL_LEN_CRC
#else
#define CTRL_ANS_OA_GET_ALL_ADC_LEN ANS_OA_GET_ADC_LEN
#endif

/* #################### */
/* DAC related messages */
/* #################### */

/* REQUEST from controller: setup channel DAC - argument 0x0C */
#define ARG_OA_CH_DAC 0x0C
#define LEN_OA_CH_DAC 0x05
#define OA_CH_DAC_CHANNEL_POS OA_CHANNEL_POS // used also in GET ADC VALUE
#define OA_CH_DAC_TYPE_POS 0x04
#define OA_CH_DAC_LIMIT_CURRENT_POS 0x05
#define OA_CH_DAC_ENABLE_SLEW_POS 0x06
#define OA_CH_DAC_SLEW_RATE_POS 0x07
#define OA_CH_DAC_LEN (BP_HEADER_DIM + LEN_OA_CH_DAC)
#define OA_CH_DAC_LEN_CRC (OA_CH_DAC_LEN + 1)
#ifdef BP_USE_CRC
#define CFG_OA_CH_DAC_LEN OA_CH_DAC_LEN_CRC
#else
#define CFG_OA_CH_DAC_LEN OA_CH_DAC_LEN
#endif
/* No answer expected */

/* REQUEST from controller: set DAC value for channel - argument 0x0D */
#define ARG_OA_SET_DAC 0x0D
#define LEN_OA_SET_DAC 0x04
#define OA_SET_DAC_CHANNEL_POS OA_CHANNEL_POS
#define OA_SET_DAC_VALUE_POS 0x04
#define OA_SET_DAC_UPDATE_VALUE 0x06
#define OA_SET_DAC_LEN (BP_HEADER_DIM + LEN_OA_SET_DAC)
#define OA_SET_DAC_LEN_CRC (OA_SET_DAC_LEN + 1)

#define ARG_OA_SET_ALL_DAC 0x22
#define LEN_OA_SET_ALL_DAC 0
#define OA_SET_ALL_DAC_LEN (BP_HEADER_DIM + LEN_OA_SET_ALL_DAC)
#define OA_SET_ALL_DAC_LEN_CRC (OA_SET_ALL_DAC_LEN + 1)
#ifdef BP_USE_CRC
#define CFG_OA_SET_ALL_DAC_LEN OA_SET_ALL_DAC_LEN
#else
#define CFG_OA_SET_ALL_DAC_LEN OA_SET_ALL_DAC_LEN_CRC
#endif

/* #################### */
/* RTD related messages */
/* #################### */

/* REQUEST from controller: setup channel RTD - argument 0x0E */
#define ARG_OA_CH_RTD 0x0E
// payload is 6 bytes:
// - channel (1 byte)
// - 3-wire (1 byte)
// - current (4 byte)
#define LEN_OA_CH_RTD 0x06
#define OA_CH_RTD_CHANNEL_POS OA_CHANNEL_POS
#define OA_CH_RTD_3WIRE_POS 0x04
#define OA_CH_RTD_CURRENT_POS 0x05
#define OA_CH_RTD_LEN (BP_HEADER_DIM + LEN_OA_CH_RTD)
#define OA_CH_RTD_LEN_CRC (OA_CH_RTD_LEN + 1)
#ifdef BP_USE_CRC
#define CFG_OA_CH_RTD_LEN OA_CH_RTD_LEN_CRC
#else
#define CFG_OA_CH_RTD_LEN OA_CH_RTD_LEN
#endif

/* REQUEST from controller: get RTD value - argument 0x0F */
#define ARG_OA_GET_RTD 0x0F
#define LEN_OA_GET_RTD 0x01
#define OA_GET_RTD_CHANNEL_POS OA_CHANNEL_POS
#define OA_GET_RTD_LEN (BP_HEADER_DIM + LEN_OA_GET_RTD)
#define OA_GET_RTD_LEN_CRC (OA_GET_RTD_LEN + 1)

/* ANSWER from expansion: get RTD value */
#define ANS_ARG_OA_GET_RTD ARG_OA_GET_RTD
// payload is 5 bytes:
// - channel (1 byte)
// - value (4 bytes) (the data is transferred as a float)
#define ANS_LEN_OA_GET_RTD 0x05
#define ANS_OA_GET_RTD_CHANNEL_POS 0x3
#define ANS_OA_GET_RTD_VALUE_POS 0x04
#define ANS_OA_GET_RTD_LEN (BP_HEADER_DIM + ANS_LEN_OA_GET_RTD)
#define ANS_OA_GET_RTD_LEN_CRC (ANS_OA_GET_RTD_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_OA_GET_RTD_LEN ANS_OA_GET_RTD_LEN_CRC
#else
#define CTRL_ANS_OA_GET_RTD_LEN ANS_OA_GET_RTD_LEN
#endif

/* REQUEST from controller: message set update time for all rtd */
#define ARG_OA_SET_RTD_UPDATE_TIME 0x10
#define LEN_OA_SET_RTD_UPDATE_TIME 0x02
#define OA_SET_RTD_UPDATE_TIME_POS 0x03
#define OA_SET_RTD_UPDATE_TIME_LEN (BP_HEADER_DIM + LEN_OA_SET_RTD_UPDATE_TIME)
#define OA_SET_RTD_UPDATE_TIME_LEN_CRC (OA_SET_RTD_UPDATE_TIME_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_SET_RTD_TIME_LEN OA_SET_RTD_UPDATE_TIME_LEN_CRC
#else
#define CTRL_SET_RTD_TIME_LEN OA_SET_RTD_UPDATE_TIME_LEN
#endif

/* #################### */
/* DI related messages  */
/* #################### */

/* REQUEST from controller: setup channel DI - argument 0x11 */
#define ARG_OA_CH_DI 0x11
#define LEN_OA_CH_DI 0x09
#define OA_CH_DI_CHANNEL_POS 0x03
#define OA_CH_DI_FILTER_COMP_POS 0x04
#define OA_CH_DI_INVERT_COMP_POS 0x05
#define OA_CH_DI_ENABLE_COMP_POS 0x06
#define OA_CH_DI_DEBOUNCE_SIMPLE_POS 0x07
#define OA_CH_DI_SCALE_COMP_POS 0x08
#define OA_CH_DI_COMP_TH_POS 0x09
#define OA_CH_DI_CURR_SINK_POS 0x0A
#define OA_CH_DI_DEBOUNCE_TIME_POS 0x0B
#define OA_CH_DI_LEN (BP_HEADER_DIM + LEN_OA_CH_DI)
#define OA_CH_DI_LEN_CRC (OA_CH_DI_LEN + 1)
#ifdef BP_USE_CRC
#define CFG_OA_CH_DI_LEN OA_CH_DI_LEN_CRC
#else
#define CFG_OA_CH_DI_LEN OA_CH_DI_LEN
#endif

/* REQUEST from controller: get DI value - argument 0x12 */
#define ARG_OA_GET_DI 0x12
#define LEN_OA_GET_DI 0x00
#define OA_GET_DI_LEN (BP_HEADER_DIM + LEN_OA_GET_DI)
#define OA_GET_DI_LEN_CRC (OA_GET_DI_LEN + 1)

/* ANSWER from expansion: get DI value */
#define ANS_ARG_OA_GET_DI ARG_OA_GET_DI
#define ANS_LEN_OA_GET_DI 0x01
#define ANS_OA_GET_DI_VALUE_POS 0x03
#define ANS_OA_GET_DI_LEN (BP_HEADER_DIM + ANS_LEN_OA_GET_DI)
#define ANS_OA_GET_DI_LEN_CRC (ANS_OA_GET_DI_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_OA_GET_DI_LEN ANS_OA_GET_DI_LEN_CRC
#else
#define CTRL_ANS_OA_GET_DI_LEN ANS_OA_GET_DI_LEN
#endif

/* #################### */
/* PWM related messages */
/* #################### */

/* REQUEST from controller: setup channel PWM - argument 0x13 */
#define ARG_OA_SET_PWM 0x13
#define LEN_OA_SET_PWM 0x09
#define OA_SET_PWM_CHANNEL_POS 0x03
#define OA_SET_PWM_PERIOD_POS 0x04
#define OA_SET_PWM_PULSE_POS 0x08
#define OA_SET_PWM_LEN (BP_HEADER_DIM + LEN_OA_SET_PWM)
#define OA_SET_PWM_LEN_CRC (OA_SET_PWM_LEN + 1)
#ifdef BP_USE_CRC
#define CFG_OA_CH_PWM_LEN OA_SET_PWM_LEN_CRC
#else
#define CFG_OA_CH_PWM_LEN OA_SET_PWM_LEN
#endif

/* #################### */
/* GPO related messages */
/* #################### */

/* NOTE: Please note that this message is defined for completeness reasons but
 * the message is not available since GPO are not available on Opta Analog */

/* REQUEST from controller: setup channel GPO - argument 0x14 */
#define ARG_OA_SET_GPO 0x14
#define LEN_OA_SET_GPO 0x01
#define OA_SET_GPO_VALUE_POS 0x03
#define OA_SET_GPO_LEN (BP_HEADER_DIM + LEN_OA_SET_GPO)
#define OA_SET_GPO_LEN_CRC (OA_SET_GPO_LEN + 1)

/* #################### */
/* LED related messages */
/* #################### */
#define ARG_OA_SET_LED 0x15
#define LEN_OA_SET_LED 0x01
#define OA_SET_LED_VALUE_POS 0x03
#define OA_SET_LED_LEN (BP_HEADER_DIM + LEN_OA_SET_LED)
#define OA_SET_LED_LEN_CRC (OA_SET_LED_LEN + 1)

#define ARG_OA_CH_HIGH_IMPEDENCE 0x24
#define LEN_OA_CH_HIGH_IMPEDENCE 0x01
#define OA_HIGH_IMPEDENCE_CH_POS OA_CHANNEL_POS
#define OA_CH_HIGH_IMPEDENCE_LEN (BP_HEADER_DIM + LEN_OA_CH_HIGH_IMPEDENCE)
#define OA_CH_HIGH_IMPEDENCE_LEN_CRC (OA_CH_HIGH_IMPEDENCE + 1)

/* Acknowledge message for all those messages that to have to answer something
 */
#define ANS_ARG_OA_ACK 0x20
#define ANS_LEN_OA_ACK 0
#define ANS_ACK_OA_LEN (BP_HEADER_DIM + ANS_LEN_OA_ACK)
#define ANS_ACK_OA_LEN_CRC (ANS_ACK_OA_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_OA_LEN ANS_ACK_OA_LEN_CRC
#else
#define CTRL_ANS_OA_LEN ANS_ACK_OA_LEN
#endif

#endif
