/* -------------------------------------------------------------------------- */
/* FILE NAME:   DigitalStSolidExpansion.cpp
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240209
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifdef ARDUINO_OPTA
#include "DigitalStSolidExpansion.h"
namespace Opta {
DigitalStSolidExpansion::DigitalStSolidExpansion() {}

DigitalStSolidExpansion::~DigitalStSolidExpansion() {}

DigitalStSolidExpansion::DigitalStSolidExpansion(Expansion &other) {
  DigitalStSolidExpansion &dse = (DigitalStSolidExpansion &)other;
  if (other.getType() == EXPANSION_OPTA_DIGITAL_STS) {
    iregs = dse.iregs;
    fregs = dse.fregs;
    type = other.getType();
    i2c_address = other.getI2CAddress();
    ctrl = other.getCtrl();
    index = other.getIndex();
    if (ctrl != nullptr) {
      ctrl->setExpStartUpCb(DigitalExpansion::startUp);
    }
  } else {
    type = EXPANSION_NOT_VALID;
    i2c_address = 0;
    ctrl = other.getCtrl();
    if (ctrl != nullptr) {
      ctrl->setExpStartUpCb(DigitalExpansion::startUp);
    }
    index = 255;
  }
}
} // namespace Opta
#endif
