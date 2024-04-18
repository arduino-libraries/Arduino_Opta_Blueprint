/* -------------------------------------------------------------------------- */
/* FILE NAME:   CommonOptaUnoR4Display.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240417
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef COMMONOPTAUNOR4DISPLAY
#define COMMONOPTAUNOR4DISPLAY

#define HEADER_DIMENSION 3

typedef enum {
  EVENT_NO_EVENT,
  EVENT_UP,
  EVENT_DOWN,
  EVENT_LEFT,
  EVENT_RIGHT,
  EVENT_UP_LONG,
  EVENT_DOWN_LONG,
  EVENT_LEFT_LONG,
  EVENT_RIGHT_LONG
} BtnEvent_t;

#define OPERATION_GET_BUTTON_STATUS 255

#define ARG_R4DISPLAY_GET_BTN (uint8_t)0xFA
#define LEN_R4DISPLAY_GET_BTN (uint8_t)0x00
#define R4DISPLAY_GET_BTN_LEN                                                  \
  (uint8_t)(HEADER_DIMENSION + LEN_R4DISPLAY_GET_BTN)

/* ANSWER from expansion: get DI value */
#define ANS_ARG_R4DISPLAY_GET_BTN (uint8_t) ARG_R4DISPLAY_GET_BTN
#define ANS_LEN_R4DISPLAY_GET_BTN (uint8_t)0x01
#define ANS_BUTTON_STATUS_POS (uint8_t)0x03
#define ANS_R4DISPLAY_GET_BTN_LEN                                              \
  (uint8_t)(HEADER_DIMENSION + ANS_LEN_R4DISPLAY_GET_BTN)

#endif
