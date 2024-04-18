/* -------------------------------------------------------------------------- */
/* FILENAME:    Controller.cpp
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   DESCRIPTION: Implementation file for class Controller to be used on OPTA to
                control the I2C and the address assignment procedure to other
                expansions used with OPTA
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "Controller.h"


#ifdef ARDUINO_OPTA
// #define DEBUG_COMM_TIMEOUT

#define INC_WITH_MAX(x, M) (x = (++x >= M) ? 0 : x)
#define DEC_WITH_MAX(x, M) (x = (--x < 0) ? (M - 1) : x)

// namespace Opta {

/* -------------------------------------------------------------------------- */
/* Controller constructor (performs some initialization)                      */
/* -------------------------------------------------------------------------- */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* CONSTRUCTOR */
Controller::Controller()
    : rx_buffer{0}, tx_buffer{0}, rx_num(0),
      tmp_address(OPTA_CONTROLLER_FIRST_TEMPORARY_ADDRESS), tmp_num_of_exp(0),
      address(OPTA_CONTROLLER_FIRST_AVAILABLE_ADDRESS), num_of_exp(0),
      failed_i2c_comm(nullptr) {
  for (int i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
    expansions[i] = nullptr;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* DESTRUCTOR */
Controller::~Controller() {
  for (int i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
    if (expansions[i] != nullptr) {
      delete expansions[i];
      expansions[i] = nullptr;
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint8_t Controller::send(int add, int device, ExpansionType_t type, int n,
                         int r) {
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
    if (type == exp_type[device] && add == exp_add[device]) {
      if (n > 0) {

#ifdef DEBUG_CONTROLLER_I2C_MSG
        Serial.print("CTRL TX: ");
        for (int i = 0; i < n; i++) {
          Serial.print(tx_buffer[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
#endif
        _send(add, n, r);

        if (r > 0) {
          if (wait_for_device_answer(device, r)) {
            return SEND_RESULT_OK;
          }
          return SEND_RESULT_COMM_TIMEOUT;
        }
        return SEND_RESULT_OK;
      }
      return SEND_RESULT_NO_DATA_TO_TRANSMIT;
    }
    return SEND_RESULT_WRONG_EXPANSION_ATTRIBUTES;
  }

  return SEND_RESULT_WRONG_EXPANSION_INDEX;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void Controller::setTx(uint8_t value, uint8_t pos) {
  if (pos < OPTA_I2C_BUFFER_DIM) {
    tx_buffer[pos] = value;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint8_t Controller::getRx(uint8_t pos) {
  if (pos < OPTA_I2C_BUFFER_DIM) {
    return rx_buffer[pos];
  }
  return 0;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

Expansion *Controller::getExpansionPtr(int device) {
  
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
    if (expansions[device] != nullptr) {
      return expansions[device];
    } else {
      switch (exp_type[device]) {
      case EXPANSION_OPTA_DIGITAL_MEC:
        expansions[device] = new DigitalMechExpansion();
        setExpStartUpCb(DigitalExpansion::startUp);
        break;
      case EXPANSION_OPTA_DIGITAL_STS:
        expansions[device] = new DigitalStSolidExpansion();
        setExpStartUpCb(DigitalExpansion::startUp);
        break;
      case EXPANSION_OPTA_ANALOG:
        expansions[device] = new AnalogExpansion();
        setExpStartUpCb(AnalogExpansion::startUp);
        break;

      case EXPANSION_DIGITAL_INVALID:
        expansions[device] = new DigitalExpansion();
        setExpStartUpCb(DigitalExpansion::startUp);
        break;
      case EXPANSION_UNOR4_DISPLAY:
        expansions[device] = new R4DisplayExpansion();
        break;

      }
      if (expansions[device] != nullptr) {
        expansions[device]->setIndex(device);
        expansions[device]->setType(exp_type[device]);
        expansions[device]->setI2CAddress(exp_add[device]);
        expansions[device]->setCtrl(this);
      }
    }
  }
  return expansions[device];
}

Expansion dummyExp;

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

Expansion &Controller::getExpansion(int device) {
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
    Expansion *exp = getExpansionPtr(device);
    if (exp != nullptr) {
      return *exp;
    }
  }
  dummyExp.setController(this);
  return dummyExp;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

ExpansionType_t Controller::getExpansionType(uint8_t i) {
  if (i < num_of_exp) {
    return exp_type[i];
  }
  return EXPANSION_NOT_VALID;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint8_t Controller::getExpansionNum() { return num_of_exp; }

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint8_t Controller::getExpansionI2Caddress(uint8_t i) {
  if (i < num_of_exp) {
    return exp_add[i];
  }
  return OPTA_MODULE_INVALID_ADDRESS;
}

/* ------------------------------------------------------------------------ */
/* Controller begin */
/* ------------------------------------------------------------------------ */
void Controller::begin() {

#ifdef DEBUG_SERIAL
  pinMode(LED_RESET, OUTPUT);
  Serial.begin(115200);
  delay(2000);
  Serial.println("[LOG]: Controller begin");
#endif
  /* initalize the controller detect pin as a input pullup */
  pinMode(OPTA_CONTROLLER_DETECT_PIN, INPUT_PULLUP);
  /* initialize the controller as I2C MASTER */
  Wire.begin();

  _send(OPTA_CONTROLLER_FIRST_AVAILABLE_ADDRESS, msg_opta_reset(), 0);
  delay(OPTA_CONTROLLER_SETUP_INIT_DELAY);
  checkForExpansions();
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
int Controller::analogReadOpta(uint8_t device, uint8_t pin,
                               bool update /*= true*/) {
  DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
  if (ptr != nullptr) {
    return ptr->analogRead(pin, update);
  }
  return -1;
  // TODO (MAYBE): add analog
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void Controller::setExpStartUpCb(reset_exp_f f) {
  bool found = false;
  for (unsigned int i = 0; i < reset_cbs.size(); i++) {
    if (reset_cbs[i].fnc == f) {
      found = true;
      break;
    }
  }
  if (!found) {
    ResetCb rc;
    rc.fnc = f;
    reset_cbs.push_back(rc);
  }
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
bool Controller::getFwVersion(uint8_t device, uint8_t &major, uint8_t &minor,
                              uint8_t &release) {
  Expansion *ptr = getExpansionPtr(device);
  if (ptr != nullptr) {
    return ptr->getFwVersion(major, minor, release);
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
PinStatus Controller::digitalReadOpta(uint8_t device, uint8_t pin,
                                      bool update) {
  DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
  if (ptr != nullptr) {
    return ptr->digitalRead(pin, update);
  }

  // TODO (MAYBE): add analog

  return LOW;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool Controller::rebootExpansion(uint8_t device) {
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {

#ifdef BP_USE_CRC
    uint8_t req = ANS_REBOOT_LEN_CRC;
#else
    uint8_t req = ANS_REBOOT_LEN;
#endif

    _send(exp_add[device], msg_opta_reboot(), req);

    if (wait_for_device_answer(OPTA_BLUE_UNDEFINED_DEVICE_NUMBER, req)) {
      if (parse_opta_reboot()) {
        delay(OPTA_CONTROLLER_DELAY_AFTER_REBOOT);
        return true;
      }
    } else {
#ifdef DEBUG_COMM_TIMEOUT
      Serial.println("ERR C");
      for (int i = 0; i < ANS_REBOOT_LEN_CRC; i++) {
        Serial.print(rx_buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
#endif
    }
  }
  delay(OPTA_CONTROLLER_DELAY_AFTER_REBOOT);
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
void Controller::digitalWriteOpta(uint8_t device, uint8_t pin, PinStatus st,
                                  bool update) {
  DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
  if (ptr != nullptr) {
    return ptr->digitalWrite(pin, st, update);
  }

  // TODO (MAYBE): add analog
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
bool Controller::updateDigitalsOut(uint8_t device) {
  DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
  if (ptr != nullptr) {
    ptr->updateDigitalOutputs();
    return true;
  }
  return false;
  // TODO (MAYBE): add analog
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
void Controller::updateDigitalsIn(uint8_t device) {
  DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
  if (ptr != nullptr) {
    return ptr->updateDigitalInputs();
  }
  // TODO (MAYBE): add analog
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
void Controller::updateDigitals(uint8_t device) {
  updateDigitalsIn(device);
  updateDigitalsOut(device);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
void Controller::updateAnalogsIn(uint8_t device) {
  DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
  if (ptr != nullptr) {
    return ptr->updateAnalogInputs();
  }
  // TODO (MAYBE): add analog
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// REVISED
void Controller::updateAnalogs(uint8_t device) { updateAnalogsIn(device); }

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* function to be called periodically to update all the data to and from
   expansions */
void Controller::update() {

#if defined DEBUG_SERIAL
  static unsigned long int start_debug = millis();
  static bool flag = true;
  if (millis() - start_debug > 500) {
    start_debug = millis();
    if (flag) {
      digitalWrite(LED_RESET, HIGH);
      flag = false;
    } else {
      digitalWrite(LED_RESET, LOW);
      flag = true;
    }
  }
#endif

  static unsigned long int start = millis();
  if (millis() - start > OPTA_CONTROLLER_UPDATE_RATE) {
    start = millis();

    checkForExpansions();
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* this function checks the status of the detect pin and debounce it for LOW
   status i.e. it returns true if the PIN is low and remains LOW for a
   certain time */
bool Controller::is_detect_low() {
  PinStatus detect_st = digitalRead(OPTA_CONTROLLER_DETECT_PIN);

  if (detect_st == LOW) {
    int num = 0;

    while (detect_st == LOW && num < OPTA_CONTROLLER_DEBOUNCE_LOW_NUMBER) {
      detect_st = digitalRead(OPTA_CONTROLLER_DETECT_PIN);
      if (detect_st == HIGH) {
        break;
      }
      delay(OPTA_CONTROLLER_DEBOUNCE_LOW_TIME);
      num++;
    }
  }

  return (detect_st == LOW);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* this function checks the status of the detect pin and debounce it for LOW
   status i.e. it returns true if the PIN is low and remains LOW for a
   certain time */
bool Controller::is_detect_high() {
  PinStatus detect_st = digitalRead(OPTA_CONTROLLER_DETECT_PIN);

  if (detect_st == HIGH) {
    int num = 0;

    while (detect_st == LOW && num < OPTA_CONTROLLER_DEBOUNCE_UP_TIME) {
      detect_st = digitalRead(OPTA_CONTROLLER_DETECT_PIN);
      if (detect_st == LOW) {
        break;
      }
      delay(OPTA_CONTROLLER_DEBOUNCE_UP_TIME);
      num++;
    }
  }

  return (detect_st == HIGH);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* function that performs the actual assign address process
   it has to be called ciclycally in the loop to support hot-plug expansion
   attachment */
void Controller::checkForExpansions() {
  if (digitalRead(OPTA_CONTROLLER_DETECT_PIN) == HIGH) {
    return;
  }

  bool enter_while = is_detect_low();
  /* PAY ATTENTION:
     The condition to ENTER into the while loop is that the DETECT pin is
     LOW BUT the condition to exit is that the DETECT pin is HIGH this exit
     condition is performed using a break statemente within the while loop
     This way I am sure to catch the transition LOW --> HIGH in a correct
     way the enter_while is no more updated */

  /* * FIX Controller not re-assigning address when reset *
     the tmp_exp_add is filled like a circular queue so that in any number
     of temporary address can be used  but only the last 10 value are stored
     - this initialization is useful to exit as soon as we meet 0*/
  if (enter_while) {
    for (int i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
      tmp_exp_add[i] = 0;
    }
    num_of_exp = 0;
  }

  while (enter_while) {
#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.println("[LOG]:  - DETECT pin is LOW (expansions without address)");
    Serial.println("        - Sending SET address message");
#endif

    _send(OPTA_DEFAULT_SLAVE_I2C_ADDRESS, msg_set_address(tmp_address), 0);

    delay(OPTA_CONTROLLER_DELAY_AFTER_SET_ADDRESS);

#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.println("        - Sending GET address message");
#endif

#ifdef BP_USE_CRC
    uint8_t req = ANS_MSG_ADDRESS_AND_TYPE_LEN_CRC;
#else
    uint8_t req = ANS_MSG_ADDRESS_AND_TYPE_LEN;
#endif

    /* send the request and waits for 4 bytes answer */
    _send(tmp_address, msg_get_address_and_type(), req);

#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.print("        - Receiving answer from device: ");
#endif

    /* * FIX Controller not re-assigning address when reset *
       when the address is correcly received as answer to the previous the
       parse_address_and_type function increase in a circular way
       tmp_num_of_exp so that tmp_exp_add array is filled in a circular way
     */

    if (wait_for_device_answer(OPTA_BLUE_UNDEFINED_DEVICE_NUMBER, req)) {
      parse_address_and_type(tmp_address);
    }

#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.print("TEMPORARY Number of expansions found ");
    Serial.println(tmp_num_of_exp);
    for (int i = 0; i < tmp_num_of_exp; i++) {
      Serial.print("Expansion index ");
      Serial.print(i);
      Serial.print(" address ");
      Serial.println(tmp_exp_add[i], HEX);
    }

    delay(1000);
#endif

    /* EXIT FROM LOOP */
    if (is_detect_high()) {
      break;
    }
  }

  /* since the enter_while is no more updated I can use it here to check for
     the transition LOW -> HIGH*/

  if (enter_while) {

#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.println("[LOG]:  - TRANSITION of detect from LOW to HIGH");
#endif
    /* reset the available address for an other possible assign address
     * process
     */
    tmp_address = OPTA_CONTROLLER_FIRST_TEMPORARY_ADDRESS;

    /* here all the slave known so far get new addresses starting from
       OPTA_DEFAULT_SLAVE_I2C_ADDRESS in this way addresses are reassigned
       starting from the device closer to the controller so the first device
       on the right of the Controller gets OPTA_DEFAULT_SLAVE_I2C_ADDRESS
       the second one OPTA_DEFAULT_SLAVE_I2C_ADDRESS + 1 and so on all the
       job is done inside the parse_rx address
    */
    address = OPTA_CONTROLLER_FIRST_AVAILABLE_ADDRESS;
    num_of_exp = 0;

    /* * FIX Controller not re-assigning address when reset *
       Since now the tmp_exp_add array is filled in circular way we need to
       find the index of the array with the maximum value (this is always
       the address of the expansion closest to the controller and the
       re-assignment of address must begin from this expansion that will
       take the first address and the first position in the array of
       expansion) */

    int initial_value = -1;
    uint8_t max_value = 0x00;
    for (int i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
      if (tmp_exp_add[i] > max_value) {
        max_value = tmp_exp_add[i];
        initial_value = i;
      }
    }

    /* * FIX Controller not re-assigning address when reset *
       tmp_num_of_exp will be decreased in circular way (when it becomes
       lower than 0 it will be assigned to OPTA_CONTROLLER_MAX_EXPANSION_NUM
       - 1 using DEC_WITH_MAX macro that does that) */
    tmp_num_of_exp = initial_value;

    /* * FIX Controller not re-assigning address when reset *
       In case of unwanted expansion reset (btw this is fixed also in the
       expansion so that is a case should never happen) it can happen that
       some temporary addresses is not working properly, this is fine...
       just try and then skip the address, the attemps is used to to so
        */
    int attempts = 0;

    bool remain_in_while_loop = true;
    do {
      _send(tmp_exp_add[tmp_num_of_exp], msg_set_address(address), 0);

      delay(OPTA_CONTROLLER_DELAY_AFTER_SET_ADDRESS);

#ifdef BP_USE_CRC
      uint8_t req = ANS_MSG_ADDRESS_AND_TYPE_LEN_CRC;
#else
      uint8_t req = ANS_MSG_ADDRESS_AND_TYPE_LEN;
#endif

      _send(address, msg_get_address_and_type(), req);

      /* * FIX Controller not re-assigning address when reset *
      when the address is correcly received as answer to the previous the
      parse_address_and_type function decrease in a circular way
      (see DEC_WITH_MAS tmp_num_of_exp)
      after 3 failed attemps the address is skipped and then the
      tmp_num_of_exp is decreased in the else branch */

      if (wait_for_device_answer(OPTA_BLUE_UNDEFINED_DEVICE_NUMBER, req)) {
        if (parse_address_and_type(address)) {
          remain_in_while_loop = (tmp_num_of_exp != initial_value);
        }
        attempts = 0;
      } else {
        attempts++;
        if (attempts > 2) {
          DEC_WITH_MAX(tmp_num_of_exp, OPTA_CONTROLLER_MAX_EXPANSION_NUM);
          Serial.println(tmp_num_of_exp);
          remain_in_while_loop = (tmp_num_of_exp != initial_value);
        } else {
          remain_in_while_loop = true;
        }
      }
    } while (tmp_exp_add[tmp_num_of_exp] != 0 && remain_in_while_loop);

#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.print("FINAL Number of expansions found ");
    Serial.println(num_of_exp);
    for (int i = 0; i < num_of_exp; i++) {
      Serial.print("Expansion index ");
      Serial.print(i);
      Serial.print(" address ");
      Serial.println(exp_add[i], HEX);
    }

    delay(1000);
#endif
    /* give some time to analog to reset Analog Devices */
    delay(250);

    /* at this point all the expansion have the final address, time to
       make room for data from and to each expansion */
    for (int i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
      if (expansions[i] != nullptr) {
        delete expansions[i];
        expansions[i] = nullptr;
      }
    }

    /* IMPORTANT: rest tmp_num_of_exp to 0 so it is ready for an other
       possible assign address process */
    tmp_num_of_exp = 0;

    /* once the assign address is finished try to set up default  */
    for (unsigned int i = 0; i < reset_cbs.size(); i++) {
      reset_cbs[i].call(this);
    }

#if defined DEBUG_SERIAL && defined DEBUG_ASSIGN_ADDRESS_CONTROLLER
    Serial.println();
    Serial.println("[LOG]: ***** REASSING ADDRESSES FINISHED ***** ");
    Serial.print("number of expansions found ");
    Serial.println(num_of_exp);
    for (int i = 0; i < num_of_exp; i++) {
      Serial.print(i + 1);
      Serial.print(") address ");
      Serial.print(exp_add[i]);
      Serial.print(", type ");
      if (exp_type[i] == EXPANSION_NOT_VALID) {
        Serial.println("EXPANSION_NOT_VALID");
      } else if (exp_type[i] == EXPANSION_OPTA_DIGITAL_MEC ||
                 exp_type[i] == EXPANSION_OPTA_DIGITAL_STS) {
        Serial.println("EXPANSION_OPTA_DIGITAL");
      } else {
        Serial.println("expansion code unknown!!!!!!");
      }
    }
#endif
  }

  else {
  }
}

void Controller::setFailedCommCb(CommErr_f f) { failed_i2c_comm = f; }

/* #################################################################### */
/* -------------- message preparation functions ---------------------- */
/* #################################################################### */

uint8_t Controller::msg_opta_reboot() {
  tx_buffer[REBOOT_1_POS] = REBOOT_1_VALUE;
  tx_buffer[REBOOT_2_POS] = REBOOT_2_VALUE;
  return prepareSetMsg(tx_buffer, ARG_REBOOT, LEN_REBOOT, REBOOT_LEN);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* prepare the message in the tx buffer to get opta digital analog in */
uint8_t Controller::msg_opta_reset() {
  tx_buffer[BP_PAYLOAD_START_POS] = CONTROLLER_RESET_CODE;
  return prepareSetMsg(tx_buffer, ARG_CONTROLLER_RESET, LEN_CONTROLLER_RESET,
                       CONTROLLER_RESET_LEN);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* prepare the message in the tx buffer to set an address to an expansion */
uint8_t Controller::msg_set_address(uint8_t add) {
  tx_buffer[BP_PAYLOAD_START_POS] = add; /* the value to be set */
  return prepareSetMsg(tx_buffer, ARG_ADDRESS, LEN_ADDRESS,
                       MSG_SET_ADDRESS_LEN);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* prepare the message in the tx buffer to get address and type from
 * expansion*/
uint8_t Controller::msg_get_address_and_type() {
  return prepareGetMsg(tx_buffer, ARG_ADDRESS_AND_TYPE, LEN_ADDRESS_AND_TYPE,
                       MSG_GET_ADDRESS_AND_TYPE_LEN);
}

/* #######################################################################
 */
/* ---------------------- parse message functions ---------------------- */
/* #######################################################################
 */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void Controller::resetRxBuffer() { memset(rx_buffer, 0, OPTA_I2C_BUFFER_DIM); }

bool Controller::parse_opta_reboot() {
  if (checkAnsSetReceived(rx_buffer, ANS_ARG_REBOOT, ANS_LEN_REBOOT,
                          ANS_REBOOT_LEN)) {

    if (rx_buffer[ANS_REBOOT_CODE_POS] == ANS_REBOOT_CODE) {
      return true;
    } else {
#ifdef DEBUG_COMM_TIMEOUT
      Serial.println("ERR A");
      for (int i = 0; i < ANS_REBOOT_LEN_CRC; i++) {
        Serial.print(rx_buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
#endif
    }
  } else {
#ifdef DEBUG_COMM_TIMEOUT
    Serial.println("ERR B");
    for (int i = 0; i < ANS_REBOOT_LEN_CRC; i++) {
      Serial.print(rx_buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
#endif
  }
  resetRxBuffer();
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* parse the answer to the message get address and type from the slave */
bool Controller::parse_address_and_type(int slave_address) {
  if (checkAnsGetReceived(rx_buffer, ANS_ARG_ADDRESS_AND_TYPE,
                          ANS_LEN_ADDRESS_AND_TYPE,
                          ANS_MSG_ADDRESS_AND_TYPE_LEN)) {
#if defined DEBUG_SERIAL && defined DEBUG_MSG_PARSE_ADDRESS_AND_TYPE
    Serial.println("        - Received answer to get address request");
#endif

    if (rx_buffer[BP_PAYLOAD_START_POS] == slave_address) {
      if (slave_address == tmp_address) {
#if defined DEBUG_SERIAL && defined DEBUG_MSG_PARSE_ADDRESS_AND_TYPE
        Serial.print("        - Received address is CORRECT: 0x");
        Serial.println(rx_buffer[BP_PAYLOAD_START_POS], HEX);
#endif
        tmp_address++;

        tmp_exp_add[tmp_num_of_exp] = slave_address;
        tmp_exp_type[tmp_num_of_exp] =
            (ExpansionType_t)rx_buffer[BP_PAYLOAD_START_POS + 1];

#if defined DEBUG_SERIAL && defined DEBUG_MSG_PARSE_ADDRESS_AND_TYPE
        Serial.print("        - Type of device attached: ");
        if (tmp_exp_type[tmp_num_of_exp] == EXPANSION_NOT_VALID) {
          Serial.println("EXPANSION_NOT_VALID");
        } else if (tmp_exp_type[tmp_num_of_exp] == EXPANSION_OPTA_DIGITAL_MEC) {
          Serial.println("EXPANSION_OPTA_DIGITAL_MEC");
        } else if (tmp_exp_type[tmp_num_of_exp] == EXPANSION_OPTA_DIGITAL_STS) {
          Serial.println("EXPANSION_OPTA_DIGITAL_STS");
        } else {
          Serial.println("expansion code unknown!!!!!!");
        }
#endif

        INC_WITH_MAX(tmp_num_of_exp, OPTA_CONTROLLER_MAX_EXPANSION_NUM);
      } else if (slave_address == address) {
        if (num_of_exp < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
          exp_add[num_of_exp] = slave_address;
          exp_type[num_of_exp] =
              (ExpansionType_t)rx_buffer[BP_PAYLOAD_START_POS + 1];  
          num_of_exp++;
        }
        DEC_WITH_MAX(tmp_num_of_exp, OPTA_CONTROLLER_MAX_EXPANSION_NUM);
        address++;
      }
    } else {
#if defined DEBUG_SERIAL && defined DEBUG_MSG_PARSE_ADDRESS_AND_TYPE
      Serial.print("        - Received address is WRONG: 0x");
      Serial.println(rx_buffer[BP_PAYLOAD_START_POS], HEX);
#endif
    }
    return true;
  }
  return false;
}

/* ##################################################################### */
/* ---------------------- send receive message functions --------------- */
/* ##################################################################### */

/* send to address add n bytes from tx_buffer
   if r is > 0 then it issues a request from the slave for r bytes */
void Controller::_send(int add, int n, int r) {
#if defined DEBUG_SERIAL && defined DEBUG_TX_CONTROLLER_ENABLE
  Serial.print("- TX to device 0x");
  Serial.print(add, HEX);
#endif

  Wire.beginTransmission(add);

/* Send */
#if defined DEBUG_SERIAL && defined DEBUG_TX_CONTROLLER_ENABLE
  Serial.print(" bytes: ");
#endif

  for (int i = 0; i < n && i < OPTA_I2C_BUFFER_DIM; i++) {

#if defined DEBUG_SERIAL && defined DEBUG_TX_CONTROLLER_ENABLE
    Serial.print("0x");
    Serial.print(tx_buffer[i], HEX);
    Serial.print(" ");
#endif

    Wire.write(tx_buffer[i]);
  }

  Wire.endTransmission();

#if defined DEBUG_SERIAL && defined DEBUG_TX_CONTROLLER_ENABLE
  Serial.println();
#endif

  /* Request */
  if (r > 0) {
#if defined DEBUG_SERIAL && defined DEBUG_TX_CONTROLLER_ENABLE
    Serial.print("-> REQUESTED ");
    Serial.print(r);
    Serial.println(" bytes");
#endif
    Wire.requestFrom(add, r);
  } else {
#if defined DEBUG_SERIAL && defined DEBUG_TX_CONTROLLER_ENABLE
    Serial.println();
#endif
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* when an answer is requested from expansion this function wait for an
   answer and put it into the rx buffer
   return true if an answer is available */
bool Controller::wait_for_device_answer(uint8_t device, uint8_t wait_for) {
  uint8_t rx = 0;
  unsigned long int start = millis();
  while (millis() - start < OPTA_CONTROLLER_WAIT_REQUEST_TIMEOUT &&
         rx < wait_for) {
    while (Wire.available()) {
      uint8_t rec = Wire.read();
      if (rx < OPTA_I2C_BUFFER_DIM) {
        rx_buffer[rx++] = rec;
      }
    }
  }

  rx_num = rx;

  if (rx_num == wait_for) {
    return true;
  } else {
#ifdef DEBUG_COMM_TIMEOUT
    Serial.println("COMMUNICATION TIMEOUT");
    Serial.println("wait_for " + String(wait_for));
    Serial.println("rx_num " + String(rx_num));
#endif
  }

  /* call callback function if nothing has been received */
  if (failed_i2c_comm != nullptr &&
      device < OPTA_BLUE_UNDEFINED_DEVICE_NUMBER) {
    failed_i2c_comm(device, tx_buffer[BP_ARG_POS]);
  }

  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void Controller::setProductionData(uint8_t device, uint8_t *data,
                                   uint8_t dlen) {
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
    if (exp_type[device] == EXPANSION_OPTA_DIGITAL_MEC ||
        exp_type[device] == EXPANSION_OPTA_DIGITAL_STS) {
      DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
      if (ptr != nullptr) {
        return ptr->setProductData(data, dlen);
      }
    }
  }
}
void Controller::setOdMechanical(uint8_t device) {
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
    if (exp_type[device] == EXPANSION_OPTA_DIGITAL_MEC ||
        exp_type[device] == EXPANSION_OPTA_DIGITAL_STS) {
      DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
      if (ptr != nullptr) {
        return ptr->setIsMechanical();
      }
    }
  }
}
void Controller::setOdStateSolid(uint8_t device) {
  if (device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
    if (exp_type[device] == EXPANSION_OPTA_DIGITAL_MEC ||
        exp_type[device] == EXPANSION_OPTA_DIGITAL_STS) {
      DigitalExpansion *ptr = (DigitalExpansion *)getExpansionPtr(device);
      if (ptr != nullptr) {
        return ptr->setIsStateSolid();
      }
    }
  }
}

Controller OptaController;

//} // namespace Opta

#endif // ARDUINO_OPTA
