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

#include "pinCfg.h"

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
#define OPTA_AN_ALERT_VI_ERR_MASK_A (1 << 0)
#define OPTA_AN_ALERT_VI_ERR_MASK_B (1 << 1)
#define OPTA_AN_ALERT_VI_ERR_MASK_C (1 << 2)
#define OPTA_AN_ALERT_VI_ERR_MASK_D (1 << 3)
#define OPTA_AN_HI_TEMP_ERR_MASK (1 << 4)
#define OPTA_AN_CHARGE_PUMP_ERR_MASK (1 << 5)
#define OPTA_AN_ALDO5V_ERR_MASK (1 << 6)
#define OPTA_AN_AVDD_ERR_MASK (1 << 7)
#define OPTA_AN_DVCC_ERR_MASK (1 << 8)
#define OPTA_AN_ALDO1V8_ERR_MASK (1 << 9)
#define OPTA_AN_ADC_CONV_ERR_MASK (1 << 10)
#define OPTA_AN_ADC_SAT_ERR_MAK (1 << 11)
#define OPTA_AN_SPI_SCLK_CNT_ERR_MASK (1 << 12)
#define OPTA_AN_SPI_CRC_ERR_MASK (1 << 13)
#define OPTA_AN_CAL_MEM_ERR_MASK (1 << 14)

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

#define OA_PWM_CH_0 OA_CH_8
#define OA_PWM_CH_1 OA_CH_9
#define OA_PWM_CH_2 OA_CH_10
#define OA_PWM_CH_3 OA_CH_11

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
} ExpansionType_t;

typedef enum { OA_VOLTAGE_ADC, OA_CURRENT_ADC, OA_ADC_NOT_USED } OaAdcType_t;
typedef enum { OA_VOLTAGE_DAC, OA_CURRENT_DAC } OaDacType_t;

typedef enum {
  OA_ADC_RANGE_10V,
  OA_ADC_RANGE_2_5V,
  OA_ADC_RANGE_2_5V_SELF_POWERED,
  OA_ADC_RANGE_2_5_BIPOLAR
} OaAdcRange_t;

typedef enum {
  OA_SLEW_RATE_0,  /* max vertical step 64 bit, update rate 4 kHz */
  OA_SLEW_RATE_1,  /* max vertical step 120 bit, update rate 4 kHz */
  OA_SLEW_RATE_2,  /* max vertical step 500 bit, update rate 4 kHz */
  OA_SLEW_RATE_3,  /* max vertical step 1820 bit, update rate 4 kHz */
  OA_SLEW_RATE_4,  /* max vertical step 64 bit, update rate 64 kHz */
  OA_SLEW_RATE_5,  /* max vertical step 120 bit, update rate 64 kHz */
  OA_SLEW_RATE_6,  /* max vertical step 500 bit, update rate 64 kHz */
  OA_SLEW_RATE_7,  /* max vertical step 1280 bit, update rate 64 kHz */
  OA_SLEW_RATE_8,  /* max vertical step 64 bit, update rate 150 kHz */
  OA_SLEW_RATE_9,  /* max vertical step 120 bit, update rate 150 kHz */
  OA_SLEW_RATE_10, /* max vertical step 500 bit, update rate 150 kHz */
  OA_SLEW_RATE_11, /* max vertical step 1280 bit, update rate 150 kHz */
  OA_SLEW_RATE_12, /* max vertical step 64 bit, update rate 240 kHz */
  OA_SLEW_RATE_13, /* max vertical step 120 bit, update rate 240 kHz */
  OA_SLEW_RATE_14, /* max vertical step 500 bit, update rate 240 kHz */
  OA_SLEW_RATE_15  /* max vertical step 1280 bit, update rate 240 kHz */
} OaDacSlewRate_t;

typedef enum {
  OA_DI_DEB_TIME_0,  /* No debounce time */
  OA_DI_DEB_TIME_1,  /* 0.0130 ms */
  OA_DI_DEB_TIME_2,  /* 0.0187 ms */
  OA_DI_DEB_TIME_3,  /* 0.0244 ms */
  OA_DI_DEB_TIME_4,  /* 0.0325 ms */
  OA_DI_DEB_TIME_5,  /* 0.0423 ms */
  OA_DI_DEB_TIME_6,  /* 0.0561 ms */
  OA_DI_DEB_TIME_7,  /* 0.0756 ms */
  OA_DI_DEB_TIME_8,  /* 0.1008 ms */
  OA_DI_DEB_TIME_9,  /* 0.1301 ms */
  OA_DI_DEB_TIME_10, /* 0.1805 ms */
  OA_DI_DEB_TIME_11, /* 0.2406 ms */
  OA_DI_DEB_TIME_12, /* 0.3203 ms */
  OA_DI_DEB_TIME_13, /* 0.4203 ms */
  OA_DI_DEB_TIME_14, /* 0.5602 ms */
  OA_DI_DEB_TIME_15, /* 0.7504 ms */
  OA_DI_DEB_TIME_16, /* 1.0008 ms */
  OA_DI_DEB_TIME_17, /* 1.3008 ms */
  OA_DI_DEB_TIME_18, /* 1.8008 ms */
  OA_DI_DEB_TIME_19, /* 2.4008 ms */
  OA_DI_DEB_TIME_20, /* 3.2008 ms */
  OA_DI_DEB_TIME_21, /* 4.2008 ms */
  OA_DI_DEB_TIME_22, /* 5.6008 ms */
  OA_DI_DEB_TIME_23, /* 7.5007 ms */
  OA_DI_DEB_TIME_24, /* 10.0007 ms */
  OA_DI_DEB_TIME_25, /* 13.0007 ms */
  OA_DI_DEB_TIME_26, /* 18.0007 ms */
  OA_DI_DEB_TIME_27, /* 24.0006 ms */
  OA_DI_DEB_TIME_28, /* 32.0005 ms */
  OA_DI_DEB_TIME_29, /* 42.0004 ms */
  OA_DI_DEB_TIME_30, /* 56.0003 ms */
  OA_DI_DEB_TIME_31, /* 75.0000 ms */
} OaDebounceTime_t;

/* step of 120 uA (although the manual says from 0 to 1.8mA in 0.120 uA steps
 * but with 4 bytes...)*/
typedef enum {
  OA_DI_SINK_0, /* No sink current */
  OA_DI_SINK_1, /* 120 uA */
  OA_DI_SINK_2,
  OA_DI_SINK_3,
  OA_DI_SINK_4,
  OA_DI_SINK_5,
  OA_DI_SINK_6,
  OA_DI_SINK_7,
  OA_DI_SINK_8,
  OA_DI_SINK_9,
  OA_DI_SINK_10,
  OA_DI_SINK_11,
  OA_DI_SINK_12,
  OA_DI_SINK_13,
  OA_DI_SINK_14,
  OA_DI_SINK_15,
  OA_DI_SINK_16,
  OA_DI_SINK_17,
  OA_DI_SINK_18,
  OA_DI_SINK_19,
  OA_DI_SINK_20,
  OA_DI_SINK_21,
  OA_DI_SINK_22,
  OA_DI_SINK_23,
  OA_DI_SINK_24,
  OA_DI_SINK_25,
  OA_DI_SINK_26,
  OA_DI_SINK_27,
  OA_DI_SINK_28,
  OA_DI_SINK_29,
  OA_DI_SINK_30,
  OA_DI_SINK_31, /* 1.8 mA */
} OaDiSinkCurrent_t;

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
