/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaDevicesCfg.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20231117
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTA_DEVICES_CONFIGURATION_H
#define OPTA_DEVICES_CONFIGURATION_H

#ifdef ARDUINO_OPTA_DIGITAL

#include "OptaDigitalCfg.h"
#elif defined ARDUINO_OPTA_ANALOG
#include "OptaAnalogCfg.h"

#elif defined ARDUINO_UNOR4_MINIMA
#include "./OptaUnoR4Display/OptaUnoR4DisplayCfg.h"

#endif

#if defined ARDUINO_UNO_TESTALOG_SHIELD

#define DIO_RESET A0

#define OPTA_LED_RED (18u)
#define OPTA_LED_BLUE (19u)
#define OPTA_LED_GREEN (20u)

#define LED_RGB_ON LOW
#define LED_RGB_OFF HIGH

#define DETECT_IN (25u)
#define DETECT_OUT (26u)

#define PWM_0 (4u)
#define PWM_1 (7u)
#define PWM_2 (6u)
#define PWM_3 (5u)
#endif

#endif
