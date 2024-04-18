/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaUnoR4Display.h
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

#ifndef OPTAUNOR4DISPLAY
#define OPTAUNOR4DISPLAY
#ifndef ARDUINO_OPTA
#include "../Module.h"
#include "Arduino.h"
#include "CommonOptaUnoR4Display.h"

#define OPTA_UNO_R4_DISPLAY

class OptaUnoR4Display : public Module {
public:
  OptaUnoR4Display();
  virtual void end() override;
  virtual void begin() override;
  virtual void update() override;
  virtual int parse_rx() override;

protected:
  BtnEvent_t btn_pressed;
  int msg_ans_get_btn_status();
};
#endif
#endif
