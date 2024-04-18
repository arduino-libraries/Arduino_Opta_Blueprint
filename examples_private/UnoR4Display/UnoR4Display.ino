/* -------------------------------------------------------------------------- */
/* FILE NAME:   UnoR4Display.ino
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240418
   DESCRIPTION: 
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla 
                Public License (MPL), v 2.0. You can obtain a copy of the MPL 
                at http://mozilla.org/MPL/2.0/. 
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */


#include "OptaBlue.h"

void printExpansionType(ExpansionType_t t) {
  if(t == EXPANSION_NOT_VALID) {
    Serial.print("Unknown!");
  }
  else if(t == EXPANSION_OPTA_DIGITAL_MEC) {
    Serial.print("Opta --- DIGITAL [Mechanical]  ---");
  }
  else if(t == EXPANSION_OPTA_DIGITAL_STS) {
    Serial.print("Opta --- DIGITAL [Solid State] ---");
  }
  else if(t == EXPANSION_DIGITAL_INVALID) {
    Serial.print("Opta --- DIGITAL [!!Invalid!!] ---");
  }
  else if(t == EXPANSION_OPTA_ANALOG) {
    Serial.print("Opta ~~~ ANALOG ~~~ ");
  }
  else if(t == EXPANSION_UNOR4_DISPLAY) {
	 Serial.print("*** UnoR4 Display ***");
  }
  else {
    Serial.print("Unknown!");
  }
}


void printExpansionInfo() {
  Serial.print("Number of expansions: ");
  Serial.println(OptaController.getExpansionNum());

  for(int i = 0; i < OptaController.getExpansionNum(); i++) {
    Serial.print("Expansion n. ");
    Serial.print(i);
    Serial.print(" type ");
    printExpansionType(OptaController.getExpansionType(i));
    Serial.print(" I2C address ");
    Serial.print(OptaController.getExpansionI2Caddress(i));
    uint8_t M,m,r;
    if(OptaController.getFwVersion(i,M,m,r)) {
      Serial.print(" FW version: ");
      Serial.print(M);
      Serial.print('.');
      Serial.print(m);
      Serial.print('.');
      Serial.print(r);
      Serial.println();
    }
    else {
      Serial.println(" Unable to get FW version");
    }
  }
}

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */    
  Serial.begin(115200);
  delay(2000);

  OptaController.begin();
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  OptaController.update();
  //printExpansionInfo();

  R4DisplayExpansion r4 = OptaController.getExpansion(2);
  if(r4) {
    
	   BtnEvent_t ev = r4.getButtonsStatus(); 
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
}




