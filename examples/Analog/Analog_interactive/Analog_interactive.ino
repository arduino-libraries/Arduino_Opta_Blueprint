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


/* given an expansion type it prints the type of expansion*/
void printExpansionType(ExpansionType_t t);

/* print a little summary for each expansions found 
 * - index
 * - type
 * - I2C address */
void printExpansionInfo();

/* get an integer value from serial in blocking way (it waits for 
 * the user to enter something)*/
int getIntegerFromSerial();

/* get the first char entered in the serial line */
char getCharFromSerial();

/* get a real number from serial line (use . as decimal separator) 
 * in a blocking way */
float getFloatFromSerial();

/* given the index of a certain device it asks the user to select
 * the configuration for each channel (if the device is an ANALOG expansion)*/
void configureChannels(uint8_t device);

/* given the index of an Analog expansion it shows the status 
 * of each channel of that device */
void watchExpansion(uint8_t device);

/* given the index of an Analog expansion it allows the user to
 * change the values of its outputs (DAC channels) */
void changeExpansion(uint8_t device);


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

/* -------------------------------------------------------------------------- */
int getIntegerFromSerial() {
/* -------------------------------------------------------------------------- */
  /* basic function that get a integer from serial 
   * (can be improved....)*/
  
  /* this function returns -1 if the user just hit return without entering 
   * any number */
  int rv = -1;
  /* wait for user to write something on the serial line */
  while(!Serial.available()) {
  }
  
  /* get the number (everything is not a number is ignored) */
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

/* -------------------------------------------------------------------------- */
int getIntegerNonBlocking() {
/* -------------------------------------------------------------------------- */
   /* basic function that get a integer from serial 
   * (can be improved....)
   * it does not wait for input from serial and return -1 if*/
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


/* -------------------------------------------------------------------------- */
char getCharFromSerial() {
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
float getFloatFromSerial() {
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* given a certain Analog expansion 'device' index this function ask 
 * the user to configure the 8 channels of the analog expansion by
 * choosing from a menu */
/* -------------------------------------------------------------------------- */
void configureChannels(uint8_t device) {
/* -------------------------------------------------------------------------- */
    if(device >= 0 && device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {

      AnalogExpansion e = OptaController.getExpansion(device);

      if(e) {
        Serial.println("\n# Configure ANALOG expansion at index " + String(device));

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

            Serial.println("Hit return to skip this channel configuration or...");
            Serial.println("Select function for CHANNEL  n. " + String(ch) + ":>");
            
            /* ask user for a choice */
            int sel = getIntegerFromSerial();

            if(sel == -1) {
              continue;
            }
            
            /* configure the channel  as requested by user */
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
              e.beginChannelAsHighImpedance(ch);
            }
            else {
              Serial.println("\nERROR: invalid choice, channel " + String(ch) + "will configured as High Impedance");
            }
        }
      }
      else {
       Serial.println("\nERROR: could not find an Analog Expansion at index " + String(device));
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
  Serial.println("WARNING: Make sure that your Opta Analog are connected and powered up!");
  Serial.println();

  int go = 1000;
  
  /* the setup function will ask the user to select an expansion and to
   * configure it */

  do {
    Serial.println("Select the Analog expansion to be configured [0-4] or just hit return to finish: ");
    go = getIntegerFromSerial();
    if(go >= 0 && go < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
      Serial.println("Selected expansion at index " + String(go));
    }
    configureChannels(go);
  } while(go != -1);
}


/* -------------------------------------------------------------------------- */
void watchExpansion(uint8_t device) {
/* -------------------------------------------------------------------------- */
  if(device >= 0 && device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
     
     Serial.println("+ Selected Analog Expansion at index " + String(device));
     AnalogExpansion e = OptaController.getExpansion(device);
     
     if(e) {
        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {

            if(e.isChVoltageDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC VOLTAGE");
              
              /* since we can "add" an adc to a certain channel the is no
               * contraction in having a channel which is a DAC but also an ADC */
  
              if(e.isChVoltageAdc(ch)) {
                Serial.print("  Channel has additional voltage ADC -> ");
                Serial.println("Measured Voltage: " + String(e.pinVoltage(ch)) + " V");
              }
              else if(e.isChCurrentAdc(ch)) {
                Serial.print("  Channel has additional current ADC -> ");
                Serial.println("Measured Current: " + String(e.pinCurrent(ch)) + " mA");
              }
            }
            else if(e.isChCurrentDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC CURRENT");
              /* since we can "add" an adc to a certain channel the is no
               * contraction in having a channel which is a DAC but also an ADC */
  
              if(e.isChVoltageAdc(ch)) {
                Serial.print("  Channel has additional voltage ADC ->  ");
                Serial.println("Measured Voltage: " + String(e.pinVoltage(ch)) + " V");
              }
              else if(e.isChCurrentAdc(ch)) {
                Serial.print("  Channel has additional current ADC ->  ");
                Serial.println("Measured Current: " + String(e.pinCurrent(ch)) + " mA");
              }
            }
            else if(e.isChRtd(ch)) {
              Serial.print("Channel " + String(ch) + " is RTD -> ");
              Serial.println("Measured Resistance: " + String(e.getRtd(ch)) +
              " Ohms");
            }
            else if(e.isChDigitalInput(ch)) {
              Serial.print("Channel " + String(ch) + " is DIGITAL INPUT -> ");
              if(e.digitalRead(ch,true) == HIGH) {
                Serial.println("in HIGH status");
              }
              else {
                Serial.println("in LOW status");
              }
              /* since we can "add" an adc to a certain channel the is no
               * contraction in having a channel which is a Digital Input but also an ADC */
  
              if(e.isChVoltageAdc(ch)) {
                Serial.print("  Channel has additional voltage ADC -> ");
                Serial.println("Measured Voltage: " + String(e.pinVoltage(ch)) + " V");
              }
              else if(e.isChCurrentAdc(ch)) {
                Serial.print("  Channel has additional current ADC -> ");
                Serial.println("Measured Current: " + String(e.pinCurrent(ch)) + " mA");
              }
            }
            else if(e.isChHighImpedance(ch)) {
              Serial.println("Channel " + String(ch) + " is HIGH IMPEDANCE");
              /* since we can "add" an adc to a certain channel the is no
               * contraction in having a channel which is in HIGH impedence but also an ADC */
  
              if(e.isChVoltageAdc(ch)) {
                Serial.print("  Channel has additional voltage ADC -> ");
                Serial.println("Measured Voltage: " + String(e.pinVoltage(ch)) + " V");
              }
              else if(e.isChCurrentAdc(ch)) {
                Serial.print("  Channel has additional current ADC ->  ");
                Serial.println("Measured Current: " + String(e.pinCurrent(ch)) + " mA");
              }
            }
            else if(e.isChVoltageAdc(ch)) {
              Serial.print("Channel " + String(ch) + " is ADC VOLTAGE -> ");
              Serial.println("Measured Voltage: " + String(e.pinVoltage(ch)) +
              " V");
            }
            else if(e.isChCurrentAdc(ch)) {
              Serial.print("Channel " + String(ch) + " is ADC CURRENT -> ");
              Serial.println("Measured Current: " + String(e.pinCurrent(ch)) +
              " mA");
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
void addAdcToConfiguration(uint8_t device) {
/* -------------------------------------------------------------------------- */
  if(device >= 0 && device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
     
     Serial.println("+ Selected Analog Expansion at index " + String(device));
     AnalogExpansion e = OptaController.getExpansion(device);
     
     if(e) {
        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {
            bool addable = false; 
            if(e.isChVoltageDac(ch)) {
              addable = true;
              Serial.println("Channel " + String(ch) + " is DAC VOLTAGE");
            }
            else if(e.isChCurrentDac(ch)) {
              addable = true;
              Serial.println("Channel " + String(ch) + " is DAC CURRENT");
            }
            else if(e.isChDigitalInput(ch)) {
              addable = true;
              Serial.print("Channel " + String(ch) + " is DIGITAL INPUT -> ");
            }
            else if(e.isChHighImpedance(ch)) {
              addable = true;
              Serial.println("Channel " + String(ch) + " is HIGH IMPEDANCE");
            }

            if(addable) {
              Serial.println("Add an ADC measurement to channel " + String(ch) + "? [N/y]");
              char s = getCharFromSerial();
              if(s == 'y' || s == 'Y') {
                  Serial.println("Select the kind of channel:    1. VOLTAGE    2. CURRENT");
                  uint32_t st = 0;
                  st = getIntegerFromSerial();
                  if(st == 1) {
                    e.addVoltageAdcOnChannel(ch);
                  }
                  else if(st == 2) {
                    e.addCurrentAdcOnChannel(ch);
                  }
                  else {
                    Serial.println("ERROR: invalid option!");
                  }
              }
              
            }
        } 
     }
     else {
      Serial.println("ERROR: could not find an Analog Expansion at index " + String(device));
     }
  }
}
/* -------------------------------------------------------------------------- */
void changeExpansion(uint8_t device) {
/* -------------------------------------------------------------------------- */
  if(device >= 0 && device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
     Serial.println(" Selected expansion at index " + String(device));
     AnalogExpansion e = OptaController.getExpansion(device);
     if(e) {
        /* setting DAC values */
        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {

            if(e.isChVoltageDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC VOLTAGE");
              
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
            else if(e.isChCurrentDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC CURRENT");
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
        
        Serial.println("Changing PWM channels? [N/y]");
        char p = getCharFromSerial();
        
        if(p == 'y' || p == 'Y') {
          for(int ch = OA_PWM_CH_FIRST; ch <= OA_PWM_CH_LAST; ch++) {
              Serial.println("Configure PWM " + String(ch - OA_PWM_CH_FIRST) + "? [N/y]");
              char s = getCharFromSerial();
              if(s == 'y' || s == 'Y') {
                  uint32_t period = 0;
                  uint32_t pulse = 0;
                  Serial.println("Enter the new period [in micro seconds]: ");
                  period = getIntegerFromSerial();
                  Serial.println("Entered period is " + String(period) + "micro seconds");

                  Serial.println("Enter the new pulse [in micro seconds]: ");
                  period = getIntegerFromSerial();
                  Serial.println("Entered pulse is " + String(period) + "micro seconds");

                  e.setPwm(ch, period, pulse);
              }
          }
        }

        Serial.println("Changing LED status? [N/y]");
        p = getCharFromSerial();

        if(p == 'y' || p == 'Y') {
          for(int led = OA_LED_1; led < OA_LED_NUM; led++) {
            Serial.println("Changing LED number " + String(led) + "? [N/y]");
              char s = getCharFromSerial();
              if(s == 'y' || s == 'Y') {
                  Serial.println("Select the new status:    1. ON       2. OFF");
                  uint32_t st = 0;
                  st = getIntegerFromSerial();
                  if(st == 1) {
                    e.switchLedOn(led);
                  }
                  else if(st == 2) {
                    e.switchLedOff(led);
                  }
                  else {
                    Serial.println("ERROR: invalid option!");
                  }
              }
          }
        }
     }
     else {
      Serial.println("ERROR: could not find an Analog Expansion at index " + String(device));
     }
  }
}

void showExpansionInformation() {
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
  delay(2000);
}


/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  OptaController.update();
  
  static unsigned long start = millis() + 11000;
  if(millis() - start > 10000) {
    /* main menu is printed every 10 second */
    start = millis();
    Serial.println("\n****** Main menu: *******");
    Serial.println("  1.  WATCH an Analog Expansion (see input values)");
    Serial.println("  2.  CHANGE Analog Expansion outputs (set output values)");
    Serial.println("  3   UPDATE an Analog Expansion Configuration (change what channels do)");
    Serial.println("  4.  ADD ADC to certain channel (already set up with a different function)");
    Serial.println("  5.  SHOW Expansions information");
  }

  int go = getIntegerNonBlocking();
  static int device = -1;
  
  /* if the user select something... */
  if(go != -1) {
    if(go >= 1 && go <= 5) {
      if(go < 5) {
        Serial.println("    -> Select the expansion: > ");
        int dev = getIntegerFromSerial();
        device = dev;
      }
      if((device >= 0 && device < OptaController.getExpansionNum()) || go == 5) {
         if(go == 1)
            watchExpansion(device);
         else if(go == 2)
            changeExpansion(device);
         else if(go == 3)
            configureChannels(device);
         else if(go == 4)
            addAdcToConfiguration(device);
         else if(go == 5) 
            showExpansionInformation();
         else
            Serial.println("UNRECOGNIZED Option");
      }
    }
  }
  /* otherwise watch the selected expansion */
  else {
    if(device != -1) {
        watchExpansion(device);
        delay(1000);
    }
  }
}


