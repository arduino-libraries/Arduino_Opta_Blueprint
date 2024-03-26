/* -------------------------------------------------------------------------- */
/* FILENAME: OptaAnalog.ino
   AUTHOR: Daniele Aimo
   DATE: 20231012
   REVISION: 0.0.1
   DESCRIPTION: Example and Test Sketch for Opta Analog
   PRODUCT: Opta Analog
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "OptaBlue.h"


OptaAnalog *oa; 

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */
  OptaExpansion = new OptaAnalog();

  if(OptaExpansion != nullptr) {
    OptaExpansion->begin();
  }

  oa = (OptaAnalog *)OptaExpansion;

}
//#define DEBUG_ANALOG_FW
#ifdef DEBUG_ANALOG_FW
void debug_task() {
  static unsigned long int start = millis();
  if(millis() -  start > 5000) {
    start = millis();
    oa->displayOaDebugInformation();
    for(int i = 0; i < 8; i++) {
      Serial.print("   Channel ");
      Serial.print(i);
      Serial.print(" value ");
      Serial.println(oa->getRtdValue(i));
    }
    
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
#ifdef DEBUG_ANALOG_FW
    debug_task();
#endif

  }

}
