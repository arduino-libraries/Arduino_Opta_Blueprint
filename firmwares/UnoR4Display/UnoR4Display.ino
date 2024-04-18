/* -------------------------------------------------------------------------- */
/* FILE NAME:   UnoR4Display.ino
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


#define OPTA_UNO_R4_DISPLAY

#include "OptaDevicesInclude.h"

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */    
  Serial.begin(115200);
  delay(5000);
  Serial.println("UNO R4 Display");

  OptaExpansion = new OptaUnoR4Display();

   if(OptaExpansion != nullptr) {
      OptaExpansion->begin();
   }
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  if(OptaExpansion != nullptr) {
      OptaExpansion->update();
   }
}
