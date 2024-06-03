/* -------------------------------------------------------------------------- */
/* FILE NAME:   testDACsetting.ino
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240603
   DESCRIPTION: Test setting of DAC at the very beginning without delay
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla 
                Public License (MPL), v 2.0. You can obtain a copy of the MPL 
                at http://mozilla.org/MPL/2.0/. 
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "OptaBlue.h"



/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */    
   Serial.begin(115200);
   delay(2000);

   OptaController.begin();
   delay(1000);

   /* Test is suppose to  Opta Analog 0-1 */

   AnalogExpansion a1 = OptaController.getExpansion(0);
   AnalogExpansion a2 = OptaController.getExpansion(1);

  

   if(a1 && a2) {
      
      Serial.println("Time needed after setting up channels");


      for(int ch = 0; ch < 8; ch++ ) {
         a1.beginChannelAsDac(ch,
                              OA_VOLTAGE_DAC,
                              true,
                              false,
                              OA_SLEW_RATE_0);

         unsigned int start = millis();
         a1.pinVoltage((uint8_t)ch, (float)3.0);

         a2.beginChannelAsAdc(ch, // the output channel you are using
                              OA_VOLTAGE_ADC, // adc type
                              true, // enable pull down
                              false, // disable rejection
                              false, // disable diagnostic
                              0); // disable averaging

         float value = a2.pinVoltage((uint8_t)ch);

         while(value > 3.1 || value < 2.9) {
            value = a2.pinVoltage((uint8_t)ch);  
         }

         unsigned int stop = millis();

         Serial.println("Channel " + String(ch) + " took " + String(stop -  start));

      }   
   
   }
   else {
      Serial.println("Analog expansion at index 0 and 1 not found");
   }


   Serial.println("Exit setup in 10 second main loop is going to begin");
   delay(5000);

     
   
}   
   





/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
   OptaController.update();
   

   static float set_value = 0.0;
   static bool increment = true;

   AnalogExpansion a1 = OptaController.getExpansion(0);
   AnalogExpansion a2 = OptaController.getExpansion(1);

   if(a1 && a2) {
      
      for(int ch = 0; ch < 8; ch++ ) {

         Serial.print("Set value = " +  String(set_value));

         unsigned int start = millis();
         a1.pinVoltage((uint8_t)ch, (float)set_value);

         float value = a2.pinVoltage((uint8_t)ch);

         while(value > (set_value + 0.1) || value < (set_value - 0.1)) {
            value = a2.pinVoltage((uint8_t)ch);  
         }

         unsigned int stop = millis();

         Serial.println("  Channel " + String(ch) + " took " + String(stop -  start));

      }   
   
   }
   else {
      Serial.println("Analog expansion at index 0 and 1 not found");
   }

   if(increment) {

      set_value += 1.0;
   }
   else {
      set_value -= 1.0;
   }

   
   if(set_value > 10) {
      increment = false;
      set_value = 9.0;
   }

   

   if(set_value < 0.0) {
      increment = true;
      set_value = 0.0;
   }

   

  
   

}
