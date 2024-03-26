/* -------------------------------------------------------------------------- */
/* FILENAME:    Protocol.h
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   REVISION:    0.0.1
   DESCRIPTION: This file contains configuration defines and the definition of
                some types
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTA_PROTOCOL_COMMON
#define OPTA_PROTOCOL_COMMON
#include "CommonCfg.h"
/* This file contains a few common type and macro plus all the define used for
 * the I2C protocol exchange */

/* -----------------------------------------------------------------------------
 * PROTOCOL DEFINES
 * -------------------------------------------------------------------------- */
#define BP_USE_CRC

#define BP_HEADER_DIM 0x03

#define BP_CMD_POS 0x00
#define BP_ARG_POS 0x01
#define BP_LEN_POS 0x02
#define BP_PAYLOAD_START_POS 0x03

#define BP_CMD_SET 0x01
#define BP_CMD_GET 0x02
#define BP_ANS_GET 0x03
#define BP_ANS_SET 0x04

/* msg reset controller (SET) */
#define ARG_CONTROLLER_RESET 0x01
#define LEN_CONTROLLER_RESET 0x01
#define CONTROLLER_RESET_CODE 0x56
#define CONTROLLER_RESET_LEN (BP_HEADER_DIM + LEN_CONTROLLER_RESET)
#define CONTROLLER_RESET_LEN_CRC (CONTROLLER_RESET_LEN + 1)
/* msg set address (SET) */
#define ARG_ADDRESS 0x02
#define LEN_ADDRESS 0x01
#define MSG_SET_ADDRESS_LEN (BP_HEADER_DIM + LEN_ADDRESS)
#define MSG_SET_ADDRESS_LEN_CRC (MSG_SET_ADDRESS_LEN + 1)
/* msg get address and type (GET) */
#define ARG_ADDRESS_AND_TYPE 0x03
#define LEN_ADDRESS_AND_TYPE 0x00
#define MSG_GET_ADDRESS_AND_TYPE_LEN (BP_HEADER_DIM + LEN_ADDRESS_AND_TYPE)
#define MSG_GET_ADDRESS_AND_TYPE_LEN_CRC (MSG_GET_ADDRESS_AND_TYPE_LEN + 1)
/* msg get opta-digital digital input */
#define ARG_OPDIG_DIGITAL 0x04
#define LEN_OPDIG_DIGITAL 0x00
#define MSG_GET_OPTDIG_LEN (BP_HEADER_DIM + LEN_OPDIG_DIGITAL)
#define MSG_GET_OPTDIG_LEN_CRC (MSG_GET_OPTDIG_LEN + 1)
/* msg get opta-digital analog input */
#define ARG_OPTDIG_ANALOG 0x05
#define LEN_OPTDIG_ANALOG 0x01
#define MSG_GET_OPTDIG_ANALOG_LEN (BP_HEADER_DIM + LEN_OPTDIG_ANALOG)
#define MSG_GET_OPTDIG_ANALOG_LEN_CRC (MSG_GET_OPTDIG_ANALOG_LEN + 1)
/* define set opta-digital digital output*/
#define ARG_OPTDIG_DIGITAL_OUT 0x06
#define LEN_OPTDIG_DIGITAL_OUT 0x01
#define MSG_SET_OPTDIG_DIGITAL_OUT_LEN (BP_HEADER_DIM + LEN_OPTDIG_DIGITAL_OUT)
#define MSG_SET_OPTDIG_DIGITAL_OUT_LEN_CRC (MSG_SET_OPTDIG_DIGITAL_OUT_LEN + 1)
/* define get opta-digital ALL analog input */
#define ARG_OPTDIG_ALL_ANALOG_IN 0x07
#define LEN_OPTDIG_ALL_ANALOG_IN 0x00
#define MSG_GET_OPTDIG_ALL_ANALOG_IN_LEN                                       \
  (BP_HEADER_DIM + LEN_OPTDIG_ALL_ANALOG_IN)
#define MSG_GET_OPTDIG_ALL_ANALOG_IN_LEN_CRC                                   \
  (MSG_GET_OPTDIG_ALL_ANALOG_IN_LEN + 1)

/* define set opta-digital default output and timeout */
#define ARG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT 0x08
#define LEN_OPTDIG_DEFAULT_OUT_AND_TIMEOUT 0x03
#define MSG_SET_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN                             \
  (BP_HEADER_DIM + LEN_OPTDIG_DEFAULT_OUT_AND_TIMEOUT)
#define MSG_SET_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN_CRC                         \
  (MSG_SET_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN + 1)

/* answer get address and type */
#define ANS_ARG_ADDRESS_AND_TYPE ARG_ADDRESS_AND_TYPE
#define ANS_LEN_ADDRESS_AND_TYPE 0x02
#define ANS_MSG_ADDRESS_AND_TYPE_LEN (BP_HEADER_DIM + ANS_LEN_ADDRESS_AND_TYPE)
#define ANS_MSG_ADDRESS_AND_TYPE_LEN_CRC (ANS_MSG_ADDRESS_AND_TYPE_LEN + 1)
/* answer get opta-digital digital input */
#define ANS_ARG_OPDIG_DIGITAL ARG_OPDIG_DIGITAL
#define ANS_LEN_OPDIG_DIGITAL 0x02
#define ANS_MSG_OPDIG_DIGITAL_LEN (BP_HEADER_DIM + ANS_LEN_OPDIG_DIGITAL)
#define ANS_MSG_OPDIG_DIGITAL_LEN_CRC (ANS_MSG_OPDIG_DIGITAL_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_OD_GET_DIGITAL_LEN ANS_MSG_OPDIG_DIGITAL_LEN_CRC
#else
#define CTRL_ANS_MSG_OD_GET_DIGITAL_LEN ANS_MSG_OPDIG_DIGITAL_LEN
#endif

/* answer get opta-digital analog input */
#define ANS_ARG_OPTDIG_ANALOG ARG_OPTDIG_ANALOG
#define ANS_LEN_OPTDIG_ANALOG 0x02
#define ANS_MSG_OPTDIG_ANALOG_LEN (BP_HEADER_DIM + ANS_LEN_OPTDIG_ANALOG)
#define ANS_MSG_OPTDIG_ANALOG_LEN_CRC (ANS_MSG_OPTDIG_ANALOG_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_OD_GET_ANALOG_LEN ANS_MSG_OPTDIG_ANALOG_LEN_CRC
#else
#define CTRL_ANS_MSG_OD_GET_ANALOG_LEN ANS_MSG_OPTDIG_ANALOG_LEN
#endif

/* answer get opta-digital ALL analog input */
#define ANS_ARG_OPTDIG_ALL_ANALOG_IN ARG_OPTDIG_ALL_ANALOG_IN
#define ANS_LEN_OPTDIG_ALL_ANALOG_IN (16 * 2)
#define ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN                                       \
  (BP_HEADER_DIM + ANS_LEN_OPTDIG_ALL_ANALOG_IN)
#define ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN_CRC                                   \
  (ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_OD_ALL_ANALOG_IN_LEN ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN_CRC
#else
#define CTRL_ANS_MSG_OD_ALL_ANALOG_IN_LEN ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN
#endif

#define ANS_ARG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT                                 \
  ARG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT
#define ANS_LEN_OPTDIG_DEFAULT_OUT_AND_TIMEOUT 0
#define ANS_MSG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN                             \
  (BP_HEADER_DIM + ANS_LEN_OPTDIG_DEFAULT_OUT_AND_TIMEOUT)
#define ANS_MSG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN_CRC                         \
  (ANS_MSG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN + 1)

#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_OD_SET_DEFAULT_LEN                                        \
  ANS_MSG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN_CRC
#else
#define CTRL_ANS_MSG_OD_SET_DEFAULT_LEN                                        \
  ANS_MSG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN
#endif
/* answer get opta-digital set default and timeout */
#define ANS_ARG_OPTDIG_DIGITAL_OUT ARG_OPTDIG_DIGITAL_OUT
#define ANS_LEN_OPTDIG_DIGITAL_OUT 0
#define ANS_MSG_OPTDIG_DIGITAL_OUT_LEN                                         \
  (BP_HEADER_DIM + ANS_LEN_OPTDIG_DIGITAL_OUT)
#define ANS_MSG_OPTDIG_DIGITAL_OUT_LEN_CRC (ANS_MSG_OPTDIG_DIGITAL_OUT_LEN + 1)

#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_OD_DIGITAL_OUT ANS_MSG_OPTDIG_DIGITAL_OUT_LEN_CRC
#else
#define CTRL_ANS_MSG_OD_DIGITAL_OUT ANS_MSG_OPTDIG_DIGITAL_OUT_LEN
#endif

#define OPTA_DIGITAL_GET_DIN_BUFFER_DIM ANS_MSG_OPDIG_DIGITAL_LEN_CRC
#define OPTA_DIGITAL_GET_ALL_AIN_BUFFER_DIM ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN_CRC
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

/* ######################## */
/* VERSION related messages */
/* ######################## */

/* This message is common to all expansions */
#define ARG_GET_VERSION 0x16
#define LEN_GET_VERSION 0x00
#define GET_VERSION_LEN (BP_HEADER_DIM + LEN_GET_VERSION)
#define GET_VERSION_LEN_CRC (GET_VERSION_LEN + 1)

#define ANS_ARG_GET_VERSION ARG_GET_VERSION
#define ANS_LEN_GET_VERSION 3
#define GET_VERSION_MAJOR_POS (BP_HEADER_DIM)
#define GET_VERSION_MINOR_POS (BP_HEADER_DIM + 1)
#define GET_VERSION_RELEASE_POS (BP_HEADER_DIM + 2)
#define ANS_GET_VERSION_LEN (BP_HEADER_DIM + ANS_LEN_GET_VERSION)
#define ANS_GET_VERSION_LEN_CRC (ANS_GET_VERSION_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_GET_VERSION_LEN ANS_GET_VERSION_LEN_CRC
#else
#define CTRL_ANS_GET_VERSION_LEN ANS_GET_VERSION_LEN
#endif

/* ######################## */
/* REBOOT related messages  */
/* ######################## */

#define ARG_REBOOT 0xF3
#define LEN_REBOOT 0x02
#define REBOOT_1_POS (BP_HEADER_DIM)
#define REBOOT_2_POS (BP_HEADER_DIM + 1)
#define REBOOT_1_VALUE 0x58
#define REBOOT_2_VALUE 0x32
#define REBOOT_LEN (BP_HEADER_DIM + LEN_REBOOT)
#define REBOOT_LEN_CRC (REBOOT_LEN + 1)

#define ANS_ARG_REBOOT ARG_REBOOT
#define ANS_LEN_REBOOT 0x01
#define ANS_REBOOT_CODE_POS (BP_HEADER_DIM)
#define ANS_REBOOT_CODE 0x74
#define ANS_REBOOT_LEN (BP_HEADER_DIM + ANS_LEN_REBOOT)
#define ANS_REBOOT_LEN_CRC (ANS_REBOOT_LEN + 1)

/* ######################## */
/* FLASH related messages  */
/* ######################## */

#define ARG_SAVE_IN_DATA_FLASH 0x17
#define LEN_SAVE_IN_DATA_FLASH 35
#define SAVE_ADDRESS_1_POS (BP_HEADER_DIM)
#define SAVE_ADDRESS_2_POS (BP_HEADER_DIM + 1)
#define SAVE_DIMENSION_POS (BP_HEADER_DIM + 2)
#define SAVE_DATA_INIT_POS (BP_HEADER_DIM + 3)
#define SAVE_DATA_LEN (BP_HEADER_DIM + LEN_SAVE_IN_DATA_FLASH)
#define SAVE_DATA_LEN_CRC (SAVE_DATA_LEN + CRC)

#define ARG_GET_DATA_FROM_FLASH 0x18
#define LEN_GET_DATA_FROM_FLASH 3
#define READ_ADDRESS_1_POS (BP_HEADER_DIM)
#define READ_ADDRESS_2_POS (BP_HEADER_DIM + 1)
#define READ_DATA_DIM_POS (BP_HEADER_DIM + 2)
#define READ_DATA_LEN (BP_HEADER_DIM + LEN_GET_DATA_FROM_FLASH)
#define READ_DATA_LEN_CRC (READ_DATA_LEN + 1)

#define ANS_ARG_GET_DATA_FROM_FLASH 0x19
#define ANS_LEN_GET_DATA_FROM_FLASH 35
#define ANS_GET_DATA_ADDRESS_1_POS (BP_HEADER_DIM)
#define ANS_GET_DATA_ADDRESS_2_POS (BP_HEADER_DIM + 1)
#define ANS_GET_DATA_DIMENSION_POS (BP_HEADER_DIM + 2)
#define ANS_GET_DATA_DATA_INIT_POS (BP_HEADER_DIM + 3)
#define ANS_GET_DATA_LEN (BP_HEADER_DIM + ANS_LEN_GET_DATA_FROM_FLASH)
#define ANS_GET_DATA_LEN_CRC (ANS_GET_DATA_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_GET_FLASH_LEN ANS_GET_DATA_LEN_CRC
#else
#define CTRL_ANS_MSG_GET_FLASH_LEN ANS_GET_DATA_LEN
#endif

#define ANS_ARG_OA_ACK 0x20
#define ANS_LEN_OA_ACK 0
#define ANS_ACK_OA_LEN (BP_HEADER_DIM + ANS_LEN_OA_ACK)
#define ANS_ACK_OA_LEN_CRC (ANS_ACK_OA_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_OA_LEN ANS_ACK_OA_LEN_CRC
#else
#define CTRL_ANS_OA_LEN ANS_ACK_OA_LEN
#endif

//
//
//
//
//
#endif
