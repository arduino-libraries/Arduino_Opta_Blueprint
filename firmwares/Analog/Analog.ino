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


/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */
  if(OptaExpansion != nullptr) {
    OptaExpansion->update();
  }
}
