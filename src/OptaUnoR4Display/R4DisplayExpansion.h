/* -------------------------------------------------------------------------- */
/* FILE NAME:   R4DisplayExpansion.h
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

#ifndef R4DISPLAYEXPANSION
#define R4DISPLAYEXPANSION

#include "../Controller.h"
#include "../Expansion.h"
#include "CommonOptaUnoR4Display.h"
#include "R4DisplayAddress.h"
#include <stdint.h>

namespace Opta {

class R4DisplayExpansion : public Expansion {

public:
  R4DisplayExpansion();
  ~R4DisplayExpansion();
  R4DisplayExpansion(Expansion &other);
  unsigned int execute(uint32_t what) override;
  BtnEvent_t getButtonsStatus();

protected:
  unsigned int i2c_transaction(uint8_t (R4DisplayExpansion::*prepare)(),
                               bool (R4DisplayExpansion::*parse)(),
                               int rx_bytes);
  uint8_t msg_get_buttons_status();
  bool parse_ans_get_buttons_status();
};

}
#endif
