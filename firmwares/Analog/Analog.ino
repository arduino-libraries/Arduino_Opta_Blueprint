/* -------------------------------------------------------------------------- */
/* FILENAME: OptaAnalog.ino
   AUTHOR: Daniele Aimo
   DATE: 20231012
   REVISION: 0.0.1
   DESCRIPTION: Example and Test Sketch for Opta Analog
   PRODUCT: Opta Analog
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "OptaAnalog.h"



#ifdef DEBUG_SERIAL
OptaAnalog *oa; 
#endif

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */
  #ifdef DEBUG_SERIAL
  Serial.begin(115200);
  delay(3000);
  #endif

  OptaExpansion = new OptaAnalog();

  if(OptaExpansion != nullptr) {
    OptaExpansion->begin();
  }


  #ifdef DEBUG_SERIAL
  oa = (OptaAnalog *)OptaExpansion;
  #endif
}

#ifdef DEBUG_SERIAL
void dbg() {
  static unsigned long s = millis() + 11000;
  if(millis()-s > 5000) {
    s = millis();
    oa->displayOaDebugInformation();
  }
}
#endif



/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */
  if(OptaExpansion != nullptr) {
    OptaExpansion->update();
    #ifdef DEBUG_SERIAL
    dbg();
    #endif
  }
}
