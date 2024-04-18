/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaUnoR4Display.cpp
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

#include "OptaUnoR4Display/CommonOptaUnoR4Display.h"
#ifndef ARDUINO_OPTA

#include "OptaUnoR4Display.h"
#ifdef OPTA_UNO_R4_DISPLAY

#define EVENT_COUNTER_NUM 50
#define LONG_EVENT_COUNTER_NUM 1000

typedef enum { BTN_IDLE, BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT } BtnStatus_t;

BtnEvent_t fire_event(BtnStatus_t st, int counter, bool &fired,
                      bool &long_fired) {
  BtnEvent_t rv = EVENT_NO_EVENT;
  bool fire = false;
  bool long_fire = false;
  if (counter > EVENT_COUNTER_NUM && !fired) {
    
    fired = true;
    fire = true;
  } else if (counter > LONG_EVENT_COUNTER_NUM && !long_fired) {
    
    long_fired = true;
    long_fire = true;
  }
  if (fire) {
    if (st == BTN_DOWN) {
      rv = EVENT_DOWN;
    } else if (st == BTN_UP) {
      rv = EVENT_UP;

    } else if (st == BTN_LEFT) {
      rv = EVENT_LEFT;

    } else if (st == BTN_RIGHT) {
      rv = EVENT_RIGHT;
    }
  } else if (long_fire) {
    if (st == BTN_DOWN) {
      
      rv = EVENT_DOWN_LONG;

    } else if (st == BTN_UP) {
      rv = EVENT_UP_LONG;

    } else if (st == BTN_LEFT) {
      rv = EVENT_LEFT_LONG;

    } else if (st == BTN_RIGHT) {
      rv = EVENT_RIGHT_LONG;
    }
  } else {
    rv = EVENT_NO_EVENT;
  }
  return rv;
}

BtnEvent_t button_pressed() {
  static bool event_fired = false;
  static bool long_event_fired = false;
  static BtnStatus_t btn_st = BTN_IDLE;
  static int counter = 0;
  BtnStatus_t st = BTN_IDLE;
  static int counter_idle = 0;
  int a = analogRead(A0);
  if (a > 170 && a < 190) {
    st = BTN_DOWN;
    
    counter_idle = 0;
  } else if (a > 311 && a < 331) {
    st = BTN_RIGHT;
    counter_idle = 0;
  } else if (a > 600 && a < 620) {
    st = BTN_UP;
    counter_idle = 0;
  } else if (a > 830 && a < 850) {
    st = BTN_LEFT;
    counter_idle = 0;
  } else {
    st = BTN_IDLE;
    counter_idle++;
    counter = 0;
  }

  if (counter_idle > 50) {
    event_fired = false;
    long_event_fired = false;
  }

  if (btn_st != st) {
    counter = 0;
  } else {
    counter++; 
  }
  btn_st = st;
  return fire_event(btn_st, counter, event_fired, long_event_fired);
}

OptaUnoR4Display::OptaUnoR4Display() : btn_pressed(EVENT_NO_EVENT) {
  //
}

void OptaUnoR4Display::end() {
  //
}
void OptaUnoR4Display::begin() {
  Serial.println("BEGIN OPTA r4 display");
  Module::begin();
  expansion_type = EXPANSION_UNOR4_DISPLAY;
  pinMode(A0, INPUT);
}

void OptaUnoR4Display::update() {
  //
  Module::update();
  BtnEvent_t ev = button_pressed();
  if (btn_pressed == EVENT_NO_EVENT) {
    btn_pressed = ev;
  }

  if (ev != EVENT_NO_EVENT) {
    Serial.print("EVENT ");
    if (ev == EVENT_UP) {
      Serial.println("UP");

    } else if (ev == EVENT_DOWN) {
      Serial.println("DOWN");

    } else if (ev == EVENT_RIGHT) {
      Serial.println("RIGHT");

    } else if (ev == EVENT_LEFT) {
      Serial.println("LEFT");
    } else if (ev == EVENT_UP_LONG) {
      Serial.println("UP LONG");

    } else if (ev == EVENT_DOWN_LONG) {
      Serial.println("DOWN_LONG");

    } else if (ev == EVENT_RIGHT_LONG) {
      Serial.println("RIGHT_LONG");

    } else if (ev == EVENT_LEFT_LONG) {
      Serial.println("LEFT_LONG");
    }
  }
}

int OptaUnoR4Display::msg_ans_get_btn_status() {
  
  tx_buffer[ANS_BUTTON_STATUS_POS] = btn_pressed;
  btn_pressed = EVENT_NO_EVENT;
  return prepareGetAns(tx_buffer, ANS_ARG_R4DISPLAY_GET_BTN,
                       ANS_LEN_R4DISPLAY_GET_BTN, ANS_R4DISPLAY_GET_BTN_LEN);
}

int OptaUnoR4Display::parse_rx() {
  /*
  for(int i = 0; i < 10; i++) {
    Serial.print(rx_buffer[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
  */

  int rv = Module::parse_rx();

  if (rv != -1) {
    /* no need for other msg parsing (message parsed in base version) */
    return rv;
  }

  rv = -1;

  if (checkGetMsgReceived(rx_buffer, ARG_R4DISPLAY_GET_BTN,
                          LEN_R4DISPLAY_GET_BTN, R4DISPLAY_GET_BTN_LEN)) {
    return msg_ans_get_btn_status();
  }

  return rv;
}

#endif
#endif
