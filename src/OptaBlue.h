/* -------------------------------------------------------------------------- */
/* FILENAME:    OptaBlue.h
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   REVISION:    0.0.1
   DESCRIPTION: Include file for OptaBluprint library
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTA_BLUEPRINT_H
#define OPTA_BLUEPRINT_H

#include "AnalogExpansion.h"
#include "Controller.h"
#include "DigitalExpansion.h"
#include "DigitalMechExpansion.h"
#include "DigitalStSolidExpansion.h"
#include "Expansion.h"
#include "Module.h"
#include "OptaAnalog.h"
#include "OptaAnalogCfg.h"
#include "OptaAnalogTypes.h"
#include "OptaDigital.h"
#include "Protocol.h"
typedef enum {
  OA_HIGH_IMPEDENCE,
  OA_DIGITAL_INPUT,
  OA_DAC_CURRENT,
  OA_DAC_VOLTAGE,
  OA_ADC_CURRENT,
  OA_ADC_VOLTAGE,
  OA_RTD_3_WIRES,
  OA_RTD_2_WIRES,
  OA_PWM_FUNC
} OaChFunction_t;
#endif
