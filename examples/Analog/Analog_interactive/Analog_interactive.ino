/* -------------------------------------------------------------------------- */
/* FILE NAME:   Analog_interactive.ino
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240326
   DESCRIPTION: This example shows how to use the OptaBlue library to control
                the Opta Analog expansion. The example shows how to set the
                set up different channels and values, using an interactive
                serial interface.
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */


#include "OptaBlue.h"
#define PERIODIC_UPDATE_TIME 5000
#define DELAY_AFTER_SETUP 200

/* -------------------------------------------------------------------------- */
void printExpansionType(ExpansionType_t t) {
/* -------------------------------------------------------------------------- */
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
    Serial.print("~~~ Opta  ANALOG ~~~");
  }
  else {
    Serial.print("Unknown!");
  }
}

/* -------------------------------------------------------------------------- */
void printExpansionInfo() {
/* -------------------------------------------------------------------------- */
  static long int start = millis();
  
  if(millis() - start > 5000) {
    start = millis();
    Serial.print("Number of expansions: ");
    Serial.println(OptaController.getExpansionNum());

    for(int i = 0; i < OptaController.getExpansionNum(); i++) {
      Serial.print("Expansion n. ");
      Serial.print(i);
      Serial.print(" type ");
      printExpansionType(OptaController.getExpansionType(i));
      Serial.print(" I2C address ");
      Serial.println(OptaController.getExpansionI2Caddress(i));
    }
  }  
}



int getIntegerFromSerial() {
  /* basic function that get a integer from serial 
   * (can be improved....)*/

  int rv = -1;
  /* wait for user to write something on the serial line */
  while(!Serial.available()) {
  }
  
  /* */
  while(Serial.available()) {
    
    int num = Serial.read();
    
    if( (num >= 0x30 && num <= 0x39) ) {
      if(rv == -1) {
        rv = 0;
      }
      rv *= 10;
      rv += num - 0x30;
    }
  }
  return rv;
}

int getIntegerNonBlocking() {
   /* basic function that get a integer from serial 
   * (can be improved....)*/
  int rv = -1;
  if(Serial.available()) {
    rv = 0;
  }
  
  while(Serial.available()) {
    int num = Serial.read();
    
    if( (num >= 0x30 && num <= 0x39) ) {
      rv *= 10;
      rv += num - 0x30;
    }
  }
  return rv;
}


char getCharFromSerial() {
  char rv = 255;
  while(!Serial.available()) {
  }
  
  bool read = true;

  while(Serial.available()) {
    
    int num = Serial.read();
    
    if(read) {
      read = false;
      rv = (char)num;
    }
  } 
  return rv;
}

float getFloatFromSerial() {
  /* basic function that get a float from serial 
   * (can be improved....)*/

  float rv = 0;
  float decimal = 0;
  float dec_mult = 0.1;
  /* wait for user to write something on the serial line */
  while(!Serial.available()) {
  }
  
  bool integer_part = true;

  /* */
  while(Serial.available()) {
    
    int num = Serial.read();

    if(integer_part) {
      if( (num >= 0x30 && num <= 0x39) ) {
        rv *= 10;
        rv += (float)(num-0x30);
      }
      else if(num == '.') {
        integer_part = false;
      }
    }
    else {
      if( (num >= 0x30 && num <= 0x39) ) {
        float a = dec_mult * (float)(num-0x30);
        decimal += a;
        dec_mult /= 10;
      }
    }
  } 
  rv += decimal;
  return rv;
}


void configureChannels(uint8_t device) {
  
    if(device >= 0 && device < 5) {

      AnalogExpansion e = OptaController.getExpansion(device);

      if(e) {
        Serial.println("ANALOG expansion at index " + String(device) + " found!");

        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {
            Serial.println();
            Serial.println("  1-> DAC VOLTAGE");
            Serial.println("  2-> DAC CURRENT");
            Serial.println("  3-> ADC VOLTAGE");
            Serial.println("  4-> ADC CURRENT");
            Serial.println("  5-> RTD 2 WIRES");
            Serial.println("  6-> RTD 3 WIRES");
            Serial.println("  7-> Digital Input");
            Serial.println("  8-> HIGH IMPEDENCE");
            Serial.println("Select function for CHANNEL  n. " + String(ch) + ":>");

            int sel = getIntegerFromSerial();
            Serial.print(" CHANNEL " + String(ch) + " set as");
            if(sel == 1) {
              Serial.println(" DAC VOLTAGE");
              e.beginChannelAsVoltageDac(ch); // use default parameters
            }
            else if(sel == 2) {
              Serial.println(" DAC CURRENT");
              e.beginChannelAsCurrentDac(ch); // use default parameters
            }
            else if(sel == 3) {
              Serial.println(" ADC VOLTAGE");
              e.beginChannelAsVoltageAdc(ch); // use default parameters
            }
            else if(sel == 4) {
              Serial.println(" ADC CURRENT");
              e.beginChannelAsCurrentAdc(ch); // use default parameters
            }
            else if(sel == 5) {
              Serial.println(" RTD 2 WIRES");
              e.beginChannelAsRtd(ch,false,1.0); // third parameter is unused in
              // this case
            }
            else if(sel == 6) {
              Serial.println(" RTD 3 WIRES");
              e.beginChannelAsRtd(ch,true,1.5); 
            }
            else if(sel == 7) {
              Serial.println(" Digital Input");
              e.beginChannelAsDigitalInput(ch); 
            }
            else if(sel == 8) {
              Serial.println("  High Impedence");
              e.beginChannelAsHighImpedence(ch);
            }
            else {
              Serial.println("ERROR: invalid choice, channel " + String(ch) + " will not be configured");
            }
        }
      }
      else {
       Serial.println("ERROR: could not find an Analog Expansion at index " + String(device));
      }
   }


}


/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */
  Serial.begin(115200);
  delay(3000);

  OptaController.begin();

  Serial.println("*** Generic Analog interactive example ***");
  Serial.println();
  Serial.println("Make sure that your Opta Analog are connected and powered up!");

  int go = 1000;

  do {
    configureChannels(go);
    Serial.println("Select the Analog expansion to be configured [0-4] or just hit return to finish: ");
    go = getIntegerFromSerial();
    Serial.println("Selected expansion at index " + String(go));

  } while(go != -1);
}

void watchExpansion(uint8_t device, bool change) {
  if(device >= 0 && device < 5) {
     Serial.println(" Selected expansion at index " + String(device));
     AnalogExpansion e = OptaController.getExpansion(device);
     if(e) {
        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {

            if(e.isChVoltageDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC VOLTAGE");
              if(change) {
                Serial.println("  Enter the new value in Volts [0-11]: ");
                float v = getFloatFromSerial();
                Serial.println("  -> Entered voltage: " + String(v) + " V");

                if(v > 0 && v < 11.0) {
                  Serial.println("     setting new value...");
                  e.pinVoltage(ch,v);
                }
                else {
                  Serial.println("ERROR: wrong value");
                }
              }
            }
            else if(e.isChCurrentDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC CURRENT");
              if(change) {
                Serial.println("  Enter the new value in milli-Ampere [0-25]: ");
                float v = getFloatFromSerial();
                Serial.println("  -> Entered current: " + String(v) + " mA");
                if(v > 0 && v < 25.0) {
                  Serial.println("      setting new value...");
                  e.pinCurrent(ch,v);
                }
                else {
                  Serial.println("ERROR: wrong value");
                }
              }
            }
            else if(e.isChVoltageAdc(ch) && !change) {
              Serial.print("Channel " + String(ch) + " is ADC VOLTAGE");
              Serial.println("  Measured Voltage: " + String(e.pinVoltage(ch)) +
              " Volts");
            }
            else if(e.isChCurrentAdc(ch)&& !change) {
              Serial.print("Channel " + String(ch) + " is ADC CURRENT");
              Serial.println("  Measured Current: " + String(e.pinCurrent(ch)) +
              " milliAmpere");
            }
            else if(e.isChRtd(ch) && !change) {
              Serial.print("Channel " + String(ch) + " is RTD");
              Serial.println("  Measured Resistance: " + String(e.getRtd(ch)));
            }
            else if(e.isChDigitalInput(ch) && !change) {
              Serial.print("Channel " + String(ch) + " is DIGITAL INPUT");
              if(e.digitalRead(ch,true) == HIGH) {
                Serial.println("  Status is HIGH");
              }
              else {
                Serial.println("  Status is LOW");
              }
            }
            else if(e.isChHighImpedence(ch) && !change) {
              Serial.println("Channel " + String(ch) + " is HIGH IMPEDENCE");
            }
            else {
              Serial.println("ERROR: Unknown channel function " + String(ch));
            }
        } 
     }
     else {
      Serial.println("ERROR: could not find an Analog Expansion at index " + String(device));
     }
  }
}


/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  OptaController.update();
  
  static unsigned long start = millis();
  if(millis() - start > 10000) {
    start = millis();
    Serial.println("\n* Main menu: ");
    Serial.println("  1.  WATCH an Analog Expansion (see input values)");
    Serial.println("  2.  CHANGE an Analog Expansion (set output values)");
    Serial.println("  3   UPDATE an Analog Expansion Configuration (change what channels do)");
  }

  int go = getIntegerNonBlocking();
  static int device = -1;

  if(go != -1) {
    if(go >= 1 && go <= 3) {
      Serial.println("    -> Select the expansion: > ");
      int dev = getIntegerFromSerial();
      device = dev;
      if(dev >= 0 && dev < OptaController.getExpansionNum()) {
         if(go == 1)
            watchExpansion(dev, false);
         else if(go == 2)
            watchExpansion(dev, true);
         else
            configureChannels(dev);
            
        
      }
    }
  }
  else {
    if(device != -1) {
        watchExpansion(device, false);
        delay(1000);
    }
  }
}


