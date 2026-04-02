/* -------------------------------------------------------------------------- */
/* FILE NAME:   updateExpansionFw.ino
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240115
   DESCRIPTION: 
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "OptaBlue.h"
#include "utility/BossaOpta.h"
#include "fwUpdateDigital.h"
#include "fwUpdateAnalog.h"


/* if this is defined the sketch will ask for a confirmation via serial to 
 * actually perform the fw update */
#define ASK_FOR_FW_UPDATE 

#ifndef ARDUINO_ARCH_ZEPHYR
using namespace mbed;

UART BossaSerial(D14, D13, NC, NC);
#else
#define BossaSerial Serial2
#endif

/* opta_analog_fw_update contains the fw update for analog expansion 
 * opta_digital_fw_update_contains the fw update for digital expansion
 * fourth from last bit contains the fw type 
 * last 3 bytes contains the version of the FW expansion */

static uint32_t oa_fw_size = sizeof(opta_analog_fw_update) - 4;
static unsigned char oa_type = opta_analog_fw_update[oa_fw_size];
static unsigned char oa_M = opta_analog_fw_update[oa_fw_size + 1];
static unsigned char oa_m = opta_analog_fw_update[oa_fw_size + 2];
static unsigned char oa_r = opta_analog_fw_update[oa_fw_size + 3];
static unsigned int  oa_version = (oa_M << 16) | (oa_m << 8) | oa_r;

static uint32_t od_fw_size = sizeof(opta_digital_fw_update) - 4;
static unsigned char od_type = opta_digital_fw_update[od_fw_size];
static unsigned char od_M = opta_digital_fw_update[od_fw_size + 1];
static unsigned char od_m = opta_digital_fw_update[od_fw_size + 2];
static unsigned char od_r = opta_digital_fw_update[od_fw_size + 3];
static unsigned int  od_version = (od_M << 16) | (od_m << 8) | od_r;


/* print the expansion type */
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
  else {
    Serial.print("Unknown!");
  }
}

void printVersion(unsigned char M, unsigned char m, unsigned char r) {
   Serial.print(M);
   Serial.print('.');
   Serial.print(m);
   Serial.print('.');
   Serial.print(r);
   Serial.println();
}

typedef enum {
   NOT_UPDATABLE,
   UPDATABLE,
   UPDATED_DECLINED
} UpdateStatus_t;

static UpdateStatus_t update_status[OPTA_CONTROLLER_MAX_EXPANSION_NUM];

bool update_finished() {
   uint8_t n = OptaController.getExpansionNum();
   for(uint8_t i = 0; i < n; i++) {
      if(update_status[i] == UPDATABLE) {
         return false;
      }
   }
   return true;
}

void init_update_status() {
   for(uint8_t i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
      update_status[i] = UPDATABLE;
   }
}


static uint8_t exp_num = 0;


/* verify if the device is updatable */
bool isUpdatable(int device) {
   bool rv = false;
   uint8_t M,m,r;
   Serial.print("\nDevice n. ");
   Serial.print(device);
   Serial.print(" type ");
   ExpansionType_t type = OptaController.getExpansionType(device);
   printExpansionType(type);

   if(OptaController.getFwVersion(device,M,m,r)) {
      Serial.print(" Current FW version: ");
      printVersion(M,m,r);
      unsigned int current_version = (M << 16) | (m << 8) | r;
      if(EXPANSION_OPTA_DIGITAL_MEC == type || EXPANSION_OPTA_DIGITAL_STS == type) {
         if(od_version > current_version ) {
            rv = true;
         }
      } else if(EXPANSION_OPTA_ANALOG == type) {
         if(oa_version > current_version ) {
            rv = true;
         }
      }
   } else {
      Serial.println("WARNING: unable to get current FW version!");
   }

   if(rv) {
      Serial.println("Device is Updatable");
   }
   else {
      Serial.println("Device is already updated to the last FW version");
   }
  
   return rv;
}


bool ask_for_confirmation(String &msg) {

   Serial.println(msg);
   
   bool first = true;;
   char ans = 'Y';
   while(!Serial.available()) {
      
   }
   while(Serial.available()) {
      if(first) {
         ans = (char)Serial.read();
         first = false;
      }else {
         Serial.read();
      }
   }
   if(ans == 'Y' || ans == 'y') {
      return true;
   }
   else {
     return false;
   }
}


bool check_expansions() {
   uint8_t n = OptaController.getExpansionNum();
   if(n == 0) {
      return false;
   } else if (n != exp_num) {
      return false;
   }
   
   for(uint8_t i = 0; i < n; i++) {
      ExpansionType_t type = OptaController.getExpansionType(i);
      if(type == EXPANSION_NOT_VALID || 
         type == EXPANSION_DIGITAL_INVALID) {
         return false;
      }
   }
   return true;
}

void updateTask() {
   static uint8_t current_expansion = 0;
   
   static unsigned long start = millis();
   if(millis() - start > 500) {
      start = millis();
      if(update_finished()) {
         Serial.println("FW UPDATE COMPLETE!");

         for(uint8_t i = 0; i < OptaController.getExpansionNum(); i++) {
            if(update_status[i] == UPDATED_DECLINED) {
               Serial.print(" Updated was skipped for expansion: ");
               Serial.println(i);
            }
         }

         Serial.println("If you wish to perform the update againg, reset the Opta Controller");
         delay(1500);
      } else if(check_expansions() && current_expansion < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
            if(isUpdatable(current_expansion)) {
               unsigned char *fw = nullptr;
               uint32_t sz = 0;

               ExpansionType_t type = OptaController.getExpansionType(current_expansion);
               if(EXPANSION_OPTA_DIGITAL_MEC == type || EXPANSION_OPTA_DIGITAL_STS == type) {
                  fw = (unsigned char *)opta_digital_fw_update;
                  sz = od_fw_size;
               } else if(EXPANSION_OPTA_ANALOG == type) {
                  fw = (unsigned char *)opta_analog_fw_update;
                  sz = oa_fw_size;
               }
               
               if(sz == 0 || fw == nullptr) {
                  return;
               }

               Serial.print("REBOOTING expansion: ");
                  
               if(BOSSA.begin(BossaSerial,&OptaController,current_expansion)) {
                  Serial.println("BOSSA correctly initialized");
                  #ifdef ASK_FOR_FW_UPDATE
                  String msg = "Expansion ";
                  msg += String(current_expansion);
                  msg += " will be now updated... Proceed? [Y/n]";

                  if(ask_for_confirmation(msg)) {
                     Serial.println("Starting FW Update");
                     if(BOSSA.flash(fw,sz)) {
                        Serial.println("UPDATE successfully performed... reset board");
                        BOSSA.reset();
                        update_status[current_expansion] = NOT_UPDATABLE;
                     }
                     else {
                        BOSSA.reset();
                     }
                  }
                  else {
                     update_status[current_expansion] = UPDATED_DECLINED;
                     Serial.println("FW will NOT be updated!");
                     BOSSA.reset();
                    
                  }
                  #else
                  if(BOSSA.flash(fw,sz)) {
                     update_status[current_expansion] = NOT_UPDATABLE;
                     Serial.println("UPDATE successfully performed... reset board");
                     BOSSA.reset();
                  }
                  #endif
               }
               else {
                  Serial.println("FAILED to initialize BOSSA...");
                  BOSSA.reset();
               }
            }
            else {
               update_status[current_expansion] = NOT_UPDATABLE;
            }
         current_expansion++;
         if(current_expansion >= exp_num) {
            current_expansion = 0;
         }
      } ////
      else {
         Serial.println("[ERROR]: Wrong number of expansions or invalid type");
         Serial.println("[!!!]: The FW Update is canceled. Please execute the following procedure");
         Serial.println("       1. Power off all the expansions");
         Serial.println("       2. Wait some time (15/20 seconds)");
         Serial.println("       3. Power on the expansions again");
         Serial.println("       4. Check connections and power then reset the Opta Controller to try again");
         Serial.println();
         delay(1500);
      }
   }
}



/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */    
  init_update_status();
  
  Serial.begin(115200);
  while(!Serial) {
  }

  Serial.println("\n**** OPTA Expansions Updater sketch ****\n");

  Serial.print("This update will update\n  - OPTA ** DIGITAL ** to version: ");
  printVersion(od_M, od_m, od_r);

  Serial.print("  - OPTA ** ANALOG ** to version: ");
  printVersion(oa_M, oa_m, oa_r);

  OptaController.begin();

  exp_num = OptaController.getExpansionNum();

  Serial.print("- Found ");
  Serial.print(exp_num);
  Serial.println(" expansions");

  if(exp_num == 0) {
      while(1) {
         Serial.println("[!!!]: The FW Update will NOT be executed.");
         Serial.println("       No expansions to update were detected.");
         Serial.println("       Check connections and power then reset the Opta Controller to try again");
         Serial.println();
         delay(2000);
      }
  }

  while(Serial.available()) {
      Serial.read();
  }


  String msg = "Is the numbe of detected expansions correct? [Y/N]";
  if(ask_for_confirmation(msg) == false) {
      while(1) {
         Serial.println("[ERROR]: the number of detected expansion is not correct");
         Serial.println("         Please check the connections");
         Serial.println("         Are all the expansion correctly powered with DC power (12/24V)?");
         Serial.println();
         Serial.println("[!!!]: The FW Update will NOT be executed.");
         Serial.println("       Check connections and power then reset the Opta Controller to try again");
         Serial.println();
         delay(2000);
      }
  }


}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  /* update function will be called each OPTA_CONTROLLER_UPDATE_RATE ms */
  OptaController.update();
  updateTask();

  
}


