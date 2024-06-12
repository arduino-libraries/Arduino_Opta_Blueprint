/* -------------------------------------------------------------------------- */
/* FILE NAME:   testStressAnalog.ino
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240605
   DESCRIPTION: 
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla 
                Public License (MPL), v 2.0. You can obtain a copy of the MPL 
                at http://mozilla.org/MPL/2.0/. 
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "OptaBlue.h"

AnalogExpansion out_expansion;
AnalogExpansion in_expansion;

#define PWM_EXPANSION_INDEX 1

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */    
   Serial.begin(115200);
   delay(5000);

   OptaController.begin();
   

   AnalogExpansion rtd_in = OptaController.getExpansion(PWM_EXPANSION_INDEX);
   if(rtd_in) {
      rtd_in.beginChannelAsRtd(0, true, 1.2);
      rtd_in.beginChannelAsRtd(1, true, 1.2);
      rtd_in.beginChannelAsRtd(2, true, 1.2);
      rtd_in.beginChannelAsRtd(3, true, 1.2);
      /*channels from 0 to 3 are current ADC */
      rtd_in.beginChannelAsRtd(4, true, 1.2);
     
      rtd_in.beginChannelAsVoltageDac(5);
      rtd_in.beginChannelAsVoltageDac(6);
      rtd_in.beginChannelAsVoltageAdc(7);
   }
   else {
      if(Serial) Serial.println("Analog Expansion for RTD test not found... looping forever...");
      while(1) {}
   }
   
   
   

   if(Serial) Serial.println("Exit setup");
     
   
}
   
#define MAX_TEST_VALUES 10

static const float voltages[MAX_TEST_VALUES] = {1.4, 9.7, 3.6, 8.8, 5.2, 2.3, 7.8,
9.2, 4.1, 8.6};
static const float currents[MAX_TEST_VALUES] = {2.3, 12.0, 8.5, 9.2, 7.8, 19.3,
1.9, 14.5, 9.3, 2.2};


int myrand(int _min, int _max) {
  return _min + ( std::rand() % ( _max - _min + 1 ) );
}

#define RANDOMFUNCTION myrand(0,6)

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
   static uint8_t configuration = 0;

   OptaController.update();
   
   /* TEST adc and dac ---- expansion 0 and 1 */
   
   

   AnalogExpansion rtd_in = OptaController.getExpansion(PWM_EXPANSION_INDEX);
   if(rtd_in) {
      Serial.println("Expansion index " + String(rtd_in.getIndex()));

      
      rtd_in.pinVoltage(5,(float)10.0);


      

      Serial.println("----------------- PWM ON ---------------");
      rtd_in.setPwm(OA_PWM_CH_0,10000,5000);
      delay(10000);

      Serial.println("----------------- PWM OFF ---------------");
      rtd_in.setPwm(OA_PWM_CH_0,0,0);
      delay(10000);
      
      
      for(int i = 0; i < 30; i++) {
         float v = rtd_in.pinVoltage(7);
         Serial.println("Valore letto: " + String(v));
         delay(500);
      }

      

      
     

      
	}
}
