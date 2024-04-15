/* -------------------------------------------------------------------------- */
/* FILE NAME:   CommonDigital.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240415
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef COMMONDIGITAL
#define COMMONDIGITAL

/* number of digital output in opta digital expansion */
#define OPTA_DIGITAL_OUT_NUM 8
#define OPTA_DIGITAL_IN_NUM 16
/* the DEFAULT ADDRESS the SLAVE uses before the address initialization process
 */

/* this define (if defined) allow the reading of OPTA DIGITAL input as ANALOG
   INPUT */
#define OPTA_DIGITAL_ALLOW_ANALOG_USE

#endif
