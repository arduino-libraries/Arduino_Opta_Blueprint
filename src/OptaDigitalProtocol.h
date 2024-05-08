/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaDigitalProtocol.h
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

#ifndef OPTADIGITALPROTOCOL
#define OPTADIGITALPROTOCOL

#include "OptaBlueProtocol.h"
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

#endif
