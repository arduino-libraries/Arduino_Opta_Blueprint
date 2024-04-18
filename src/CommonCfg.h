/* -------------------------------------------------------------------------- */
/* FILE NAME:   CommonCfg.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20231116
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef COMMONCFG_H
#define COMMONCFG_H

#define OPTA_CONTROLLER_MAX_EXPANSION_NUM 5
/* the first addressed assigned to a MODULE */
#define OPTA_CONTROLLER_FIRST_AVAILABLE_ADDRESS 0x0B
/* first temporary address */
#define OPTA_CONTROLLER_FIRST_TEMPORARY_ADDRESS                                \
  (OPTA_CONTROLLER_FIRST_AVAILABLE_ADDRESS + OPTA_CONTROLLER_MAX_EXPANSION_NUM)
/* this define activate 2 task that make the DIGITAL and the ANALOG FW
 * recognizable by making a LED blinking, this also overwrite the
 * true version of the FW to enable the FW update */
// #define DEBUG_UPDATE_FW // ALWAYS REMOVE FOR PRODUCTION !!!!
//
//
//

#if defined ARDUINO_UNOR4_MINIMA
#define OA_AN_CHANNELS_NUM 4
#define OA_GPO_NUM 4
#define OA_AN_DEVICES_NUM 1
#define OA_PWM_CHANNELS_NUM 4
#else
#define OA_AN_CHANNELS_NUM 8
#define OA_PWM_CHANNELS_NUM 4
#define OA_GPO_NUM 8
#define OA_AN_DEVICES_NUM 2

#endif

#define OA_AN_DEVICE_0 0
#define OA_AN_DEVICE_1 1
#define OA_CH_0 0
#define OA_CH_1 1
#define OA_CH_2 2
#define OA_CH_3 3
#define OA_CH_4 4
#define OA_CH_5 5
#ifdef ARDUINO_UNOR4_MINIMA
#define OA_CH_6 2
#define OA_CH_7 3
#else
#define OA_CH_6 6
#define OA_CH_7 7
#endif
#define OA_CH_8 8
#define OA_CH_9 9
#define OA_CH_10 10
#define OA_CH_11 11
#define OA_CH_NUM 12

#define OA_PWM_CH_FIRST OA_CH_8
#define OA_PWM_CH_0 OA_CH_8
#define OA_PWM_CH_1 OA_CH_9
#define OA_PWM_CH_2 OA_CH_10
#define OA_PWM_CH_3 OA_CH_11
#define OA_PWM_CH_LAST OA_CH_11

#define OA_LED_1 0
#define OA_LED_2 1
#define OA_LED_3 2
#define OA_LED_4 3
#define OA_LED_5 4
#define OA_LED_6 5
#define OA_LED_7 6
#define OA_LED_8 7

#define OA_LED_NUM 8

#define OA_FIRST_PWM_CH 8
#define OA_LAST_PWM_CH 11

#define OA_MAX_DAC_VALUE 8191

#define MAX_FLASH_BUFFER_DIM 32
#define PRODUCTION_DATA_FLASH_ADDRESS 0x1C00
#define EXPANSION_TYPE_ADDITIONA_DATA                                          \
  (PRODUCTION_DATA_FLASH_ADDRESS + MAX_FLASH_BUFFER_DIM)
#define FLASH_OD_TYPE_MECHANICAL 0
#define FLASH_OD_TYPE_STATE_SOLID 1
/* -----------------------------------------------------------------------------
 * TYPES DEFINITION
 * --------------------------------------------------------------------------
 */
typedef union float_union {
  float value;
  unsigned char bytes[4];
} Float_u;

typedef enum {
  EXPANSION_NOT_VALID,
  EXPANSION_DIGITAL_INVALID,
  EXPANSION_OPTA_DIGITAL_MEC,
  EXPANSION_OPTA_DIGITAL_STS,
  EXPANSION_OPTA_ANALOG,
  EXPANSION_UNOR4_DISPLAY
} ExpansionType_t;
typedef enum { OA_VOLTAGE_ADC, OA_CURRENT_ADC, OA_ADC_NOT_USED } OaAdcType_t;
typedef enum { OA_VOLTAGE_DAC, OA_CURRENT_DAC } OaDacType_t;

#define EXPANSION_OPTA_DIGITAL 255

/* -----------------------------------------------------------------------------
 * CONFIGURATION DEFINES
 * -------------------------------------------------------------------------- */
#define OPTA_I2C_BUFFER_DIM 48

#define FLAG_GET_ANALOG_VERSION 0x45
/* number of digital output in opta digital expansion */
#define OPTA_DIGITAL_OUT_NUM 8
#define OPTA_DIGITAL_IN_NUM 16
/* the DEFAULT ADDRESS the SLAVE uses before the address initialization process
 */
#define OPTA_DEFAULT_SLAVE_I2C_ADDRESS 0x0A

#define OPTA_MODULE_INVALID_ADDRESS (OPTA_DEFAULT_SLAVE_I2C_ADDRESS - 1)
/* this define (if defined) allow the reading of OPTA DIGITAL input as ANALOG
   INPUT */
#define OPTA_DIGITAL_ALLOW_ANALOG_USE

/* enable overall serial debug log specific debug function can be enabled in the
 * single configuration file for each device, however without defining this the
 * serial debug is not enabled */
// #define DEBUG_SERIAL
/* when DEBUG_SERIAL is defined print the assigned address of the expansion
 * periodically on serial */
// #define DEBUG_EXPANSION_PRINT_ADDRESS
/* enable the debug messages of the RX content for the module */
// #define DEBUG_RX_MODULE_ENABLE
/* enable the debug messages of the TX content for the module */
// #define DEBUG_TX_MODULE_ENABLE

/* enable the debug messages of the PARSING for the module */
// #define DEBUG_PARSE_MODULE_ENABLE
#endif
