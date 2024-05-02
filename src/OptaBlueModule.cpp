/* -------------------------------------------------------------------------- */
/* FILENAME:
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   REVISION:    0.0.1
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */
#ifndef ARDUINO_OPTA
#include "Arduino.h"

#include "OptaBlueModule.h"
#include "boot.h"
#ifdef MODULE_USE_FLASH_MEMORY
#include "EEPROM.h"
#endif

Module *OptaExpansion = nullptr;

/* -------------------------------------------------------------------------- */
/* Module RX event: callback called when Module receive from Controller on I2C*/
/* -------------------------------------------------------------------------- */
void receive_event(int n) {

#if defined DEBUG_SERIAL && defined DEBUG_RX_MODULE_ENABLE
  Serial.println();
  Serial.print("- start RX: ");
#endif

  if (OptaExpansion != nullptr) {
    OptaExpansion->setRxNum(0);
    for (int i = 0; i < n && i < OPTA_I2C_BUFFER_DIM; i++) {
      int r = Wire.read();

#if defined DEBUG_SERIAL && defined DEBUG_RX_MODULE_ENABLE
      Serial.print(r, HEX);
      Serial.print(" ");
#endif

      OptaExpansion->rx((uint8_t)r, i);
      OptaExpansion->setRxNum(i + 1);
    }
#if defined DEBUG_SERIAL && defined DEBUG_RX_MODULE_ENABLE
    Serial.println("[end RX]");
#endif
    /* parse received message */
    OptaExpansion->setTxNum(OptaExpansion->parse_rx());
  }
}

/* -------------------------------------------------------------------------- */
/* Module Request event: callback called when Module receive a request from I2C
   Controller                                                                 */
/* -------------------------------------------------------------------------- */
void request_event() {
#if defined DEBUG_SERIAL && defined DEBUG_TX_MODULE_ENABLE
  Serial.println();
  Serial.print("- start TX: ");
  for (int i = 0; i < OptaExpansion->getTxNum(); i++) {
    Serial.print(*(OptaExpansion->txPrt() + i), HEX);
    Serial.print(" ");
  }
#endif

  if (OptaExpansion != nullptr) {
    Wire.write(OptaExpansion->txPrt(), OptaExpansion->getTxNum());
  }

  uint8_t *ck = OptaExpansion->txPrt();
  if (*(ck + BP_ARG_POS) == ARG_REBOOT) {
    OptaExpansion->setRebootSent();
  }

#if defined DEBUG_SERIAL && defined DEBUG_TX_MODULE_ENABLE
  Serial.println("[end TX]");
#endif
}

/* -------------------------------------------------------------------------- */
/* SETTER / GETTER functions                                                  */
/* -------------------------------------------------------------------------- */

/* set the tx_buffer @ position pos with value v */
void Module::tx(uint8_t v, int pos) {
  if (pos >= 0 && pos < OPTA_I2C_BUFFER_DIM) {
    tx_buffer[pos] = v;
  }
}

/* get value of tx_buffer @ position pos */
uint8_t Module::tx(int pos) {
  if (pos >= 0 && pos < OPTA_I2C_BUFFER_DIM) {
    return tx_buffer[pos];
  }
  return 0;
}

/* get value of rx buffer @ position pos */
uint8_t Module::rx(int pos) {
  if (pos >= 0 && pos < OPTA_I2C_BUFFER_DIM) {
    return rx_buffer[pos];
  }
  return 0;
}

/* set rx_buffer @ position pos with value v */
void Module::rx(uint8_t v, int pos) {
  if (pos >= 0 && pos < OPTA_I2C_BUFFER_DIM) {
    rx_buffer[pos] = v;
  }
}

/* set the number of bytes received into the rx_buffer */
void Module::setRxNum(uint8_t n) { rx_num = n; }
/* get the number of bytes received into the rx_buffer */
uint8_t Module::getRxNum() { return rx_num; }
/* set the number of bytes to be transmitted in the tx_buffer */
void Module::setTxNum(uint8_t n) { tx_num = n; }
/* get the number of bytes to be transmitted in the tx_buffer */
uint8_t Module::getTxNum() { return tx_num; }
/* returns the pointer to the tx buffer */
uint8_t *Module::txPrt() { return ans_buffer; }

/* -------------------------------------------------------------------------- */
/* Handle reset                                                               */
/* -------------------------------------------------------------------------- */
void Module::reset() {
  Wire.end();

#ifdef MODULE_USE_RGB_STATUS_LED
  digitalWrite(OPTA_LED_BLUE, LED_RGB_ON);
  digitalWrite(OPTA_LED_RED, LED_RGB_OFF);
  digitalWrite(OPTA_LED_GREEN, LED_RGB_OFF);
#endif

  /* put address to invalid */
  address = OPTA_MODULE_INVALID_ADDRESS;

  /* DETECT_IN (toward Controller) as Output */
  pinMode(DETECT_IN, OUTPUT);
  /* put detect in pin to LOW -> signal the MODULE wants an address */
  digitalWrite(DETECT_IN, LOW);

  /* DETECT_OUT (toward other Module) as output*/
  pinMode(DETECT_OUT, OUTPUT);
  digitalWrite(DETECT_OUT, LOW);
  delay(OPTA_MODULE_DETECT_OUT_LOW_TIME);

  pinMode(DETECT_OUT, INPUT_PULLUP);

  while (is_detect_out_low()) {
  }

#ifdef MODULE_USE_RGB_STATUS_LED
  digitalWrite(OPTA_LED_BLUE, LED_RGB_OFF);
#endif
  /* put I2C address to the default one */
  Wire.begin(OPTA_DEFAULT_SLAVE_I2C_ADDRESS);
}

/* -------------------------------------------------------------------------- */
/* Module constructor (does nothing)                                          */
/* -------------------------------------------------------------------------- */
Module::Module()
    : address(OPTA_MODULE_INVALID_ADDRESS), rx_num(0), reboot_required(false),
      reset_required(false), ans_buffer(nullptr),
      expansion_type(EXPANSION_NOT_VALID), reboot_sent(0) {}

/* -------------------------------------------------------------------------- */
/* Return true if the module has obtained an address from the controller      */
/* -------------------------------------------------------------------------- */
bool Module::addressAcquired() {
  return ((address != OPTA_MODULE_INVALID_ADDRESS));
}

/* -------------------------------------------------------------------------- */
/* Module begin                                                               */
/* -------------------------------------------------------------------------- */
void Module::begin() {
#ifdef MODULE_USE_RGB_STATUS_LED
  pinMode(OPTA_LED_RED, OUTPUT);
  pinMode(OPTA_LED_BLUE, OUTPUT);
  pinMode(OPTA_LED_GREEN, OUTPUT);
  digitalWrite(OPTA_LED_RED, LED_RGB_OFF);
  digitalWrite(OPTA_LED_BLUE, LED_RGB_OFF);
  digitalWrite(OPTA_LED_GREEN, LED_RGB_OFF);
#endif

#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  delay(2000);
  // while(!Serial) { }
  Serial.println("[Log]: START");
#endif
  reset();

  /* Set up callbacks */
  Wire.onReceive(receive_event);
  Wire.onRequest(request_event);
}

/* -------------------------------------------------------------------------- */
/* Module begin                                                               */
/* -------------------------------------------------------------------------- */
void Module::update() {

  updatePinStatus();

  if (reset_required) {
    reset();
    reset_required = false;
  }

  if (reboot_required && reboot_sent > 0) {
    while ((millis() - reboot_sent) < WAIT_FOR_REBOOT) {
    }
    reboot_required = false;
    reboot_sent = 0;
    goBootloader();
  }

#if defined DEBUG_SERIAL && defined DEBUG_EXPANSION_PRINT_ADDRESS
  static unsigned long int start = millis();

  if (millis() - start > 1000) {
    start = millis();
    Serial.print("ADDRESS ");
    Serial.println(address);
  }
#endif
}

/* -------------------------------------------------------------------------- */
bool Module::parse_set_address() {
  /* ------------------------------------------------------------------------ */
  if (checkSetMsgReceived(rx_buffer, ARG_ADDRESS, LEN_ADDRESS,
                          MSG_SET_ADDRESS_LEN)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
bool Module::parse_get_address_and_type() {
  /* ---------------------------------------------------------------------- */
  if (checkGetMsgReceived(rx_buffer, ARG_ADDRESS_AND_TYPE, LEN_ADDRESS_AND_TYPE,
                          MSG_GET_ADDRESS_AND_TYPE_LEN)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
bool Module::parse_reset_controller() {
  /* ---------------------------------------------------------------------- */
  if (checkSetMsgReceived(rx_buffer, ARG_CONTROLLER_RESET, LEN_CONTROLLER_RESET,
                          CONTROLLER_RESET_LEN)) {
    if (rx_buffer[BP_PAYLOAD_START_POS] == CONTROLLER_RESET_CODE) {
      return true;
    }
  }
  return false;
}

/* ------------------------------------------------------------------------ */
int Module::prepare_ans_get_version() {
  /* ---------------------------------------------------------------------- */
  tx_buffer[GET_VERSION_MAJOR_POS] = getMajorFw();
  tx_buffer[GET_VERSION_MINOR_POS] = getMinorFw();
  tx_buffer[GET_VERSION_RELEASE_POS] = getReleaseFw();
  return prepareGetAns(tx_buffer, ANS_ARG_GET_VERSION, ANS_LEN_GET_VERSION,
                       ANS_GET_VERSION_LEN);
}

/* ------------------------------------------------------------------------ */
bool Module::parse_get_version() {
  /* ----------------------------------------------------------------------
   */
  if (checkGetMsgReceived(rx_buffer, ARG_GET_VERSION, LEN_GET_VERSION,
                          GET_VERSION_LEN)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
int Module::prepare_ans_reboot() {
  /* ----------------------------------------------------------------------
   */
  tx_buffer[ANS_REBOOT_CODE_POS] = ANS_REBOOT_CODE;
  return prepareSetAns(tx_buffer, ANS_ARG_REBOOT, ANS_LEN_REBOOT,
                       ANS_REBOOT_LEN);
}

/* ------------------------------------------------------------------------ */
bool Module::parse_reboot() {
  /* ----------------------------------------------------------------------
   */
  if (checkSetMsgReceived(rx_buffer, ARG_REBOOT, LEN_REBOOT, REBOOT_LEN)) {
    if (rx_buffer[REBOOT_1_POS] == REBOOT_1_VALUE &&
        rx_buffer[REBOOT_2_POS] == REBOOT_2_VALUE) {
      return true;
    }
  }
  return false;
}

/* ------------------------------------------------------------------------ */
int Module::prepare_ans_get_address_and_type() {
  /* ----------------------------------------------------------------------
   */
  tx_buffer[BP_PAYLOAD_START_POS] = address;
  tx_buffer[BP_PAYLOAD_START_POS + 1] = expansion_type;
  return prepareGetAns(tx_buffer, ANS_ARG_ADDRESS_AND_TYPE,
                       ANS_LEN_ADDRESS_AND_TYPE, ANS_MSG_ADDRESS_AND_TYPE_LEN);
}

__WEAK void new_i2c_address_obtained(void *ptr) { (void)ptr; }

/* ------------------------------------------------------------------------ */
/* Parse the content of the rx_buffer
   this function only consider the messages related to addressing process
   so that it return the size of answer to be transmitted or -1 if it cannot
   handle the message
   Please note that this function MUST be called in all derived classes in
   order to manage correctly the addressing process */
/* ------------------------------------------------------------------------ */
int Module::parse_rx() {
  /* ----------------------------------------------------------------------
   */
  /* set address message */
  if (parse_set_address()) {
    pinMode(DETECT_OUT, INPUT_PULLUP);
    delay(1);
    /* accept the address only if detect out is HIGH */
    if (digitalRead(DETECT_OUT) == HIGH) {
      address = rx_buffer[BP_PAYLOAD_START_POS];
      setAddress(address);
    }
    return 0;
  }
  /* get address and type message */
  else if (parse_get_address_and_type()) {
    int rv = prepare_ans_get_address_and_type();
    ans_buffer = tx_buffer;
    new_i2c_address_obtained(this);
    return rv;
  }
  /* reset control message */
  else if (parse_reset_controller()) {
    reset_required = true;
    return 0;
  }
  /* get fw version */
  else if (parse_get_version()) {
    int rv = prepare_ans_get_version();
    ans_buffer = tx_buffer;
    return rv;
  }
  /* REBOOT --> update FW */
  else if (parse_reboot()) {
    int rv = prepare_ans_reboot();
    reboot_required = true;
    ans_buffer = tx_buffer;
    return rv;
  }
  /* FLASH WRITE */
  else if (parse_set_flash()) {
    return 0;
  }
  /* FLASH READ */
  else if (parse_get_flash()) {
    int rv = prepare_ans_get_flash();
    ans_buffer = tx_buffer;
    return rv;
  }

  return -1;
}

/* ------------------------------------------------------------------------ */
bool Module::is_detect_in_low() {
  /* ----------------------------------------------------------------------
   */
  pinMode(DETECT_IN, INPUT_PULLUP);
  delay(1);
  PinStatus dtcin = digitalRead(DETECT_IN);
  if (dtcin == LOW) {
    int num = 0;
    while (dtcin == LOW && num < OPTA_MODULE_DEBOUNCE_NUMBER_IN) {
      dtcin = digitalRead(DETECT_IN);
      if (dtcin == HIGH) {
        break;
      }
      delay(OPTA_MODULE_DEBOUNCE_TIME_IN);
      num++;
    }
  }
  return (dtcin == LOW);
}

/* ------------------------------------------------------------------------ */
bool Module::is_detect_out_low() {
  /* ----------------------------------------------------------------------
   */

  pinMode(DETECT_OUT, INPUT_PULLUP);
  delay(1);
  PinStatus dtcout = digitalRead(DETECT_OUT);

  if (dtcout == LOW) {
    int num = 0;
    while (
        dtcout == LOW &&
        num <
            OPTA_MODULE_DEBOUNCE_NUMBER_OUT) { // OPTA_DIGITAL_DEBOUNCE_NUMBER)
                                               // {
      dtcout = digitalRead(DETECT_OUT);
      if (dtcout == HIGH) {
        break;
      }
      delay(OPTA_MODULE_DEBOUNCE_TIME_OUT);
      num++;
    }
  }
  return (dtcout == LOW);
}

/* ------------------------------------------------------------------------ */
bool Module::is_detect_out_high() {
  /* ----------------------------------------------------------------------
   */

  pinMode(DETECT_OUT, INPUT_PULLUP);
  delay(1);
  PinStatus dtcout = digitalRead(DETECT_OUT);

  int num = 0;
  while (dtcout == HIGH && num < OPTA_MODULE_DEBOUNCE_NUMBER_OUT) {
    dtcout = digitalRead(DETECT_OUT);
    if (dtcout == LOW) {
      break;
    }
    delay(OPTA_MODULE_DEBOUNCE_TIME_OUT);
    num++;
  }
  return (dtcout == HIGH);
}
/* ------------------------------------------------------------------------ */
void Module::updatePinStatus() {
  /* ----------------------------------------------------------------------
   */
#if defined DEBUG_SERIAL && defined DEBUG_UPDATE_PIN_ENABLE
  Serial.print("- UPDATE PIN ");
#endif

  if (!addressAcquired()) {
#if defined DEBUG_SERIAL && defined DEBUG_UPDATE_PIN_ENABLE
    Serial.println("ADDRESS not ACQUIRED");
#endif
#ifdef MODULE_USE_RGB_STATUS_LED
    digitalWrite(OPTA_LED_RED, LED_RGB_ON);
    digitalWrite(OPTA_LED_GREEN, LED_RGB_OFF);
#endif
    /* DETECT_IN (toward Controller) as Output */
    pinMode(DETECT_IN, OUTPUT);
    /* put detect in pin to LOW -> signal the MODULE wants an address */
    digitalWrite(DETECT_IN, LOW);
  } else {
#if defined DEBUG_SERIAL && defined DEBUG_UPDATE_PIN_ENABLE
    Serial.print("ADDRESS ACQUIRED ");
    Serial.println(address, HEX);
#endif

#ifdef MODULE_USE_RGB_STATUS_LED
    digitalWrite(OPTA_LED_RED, LED_RGB_OFF);
    digitalWrite(OPTA_LED_GREEN, LED_RGB_ON);
#endif

    if (is_detect_in_low()) {
      reset_required = true;
    } else if (is_detect_out_low()) {
      reset_required = true;
    } else {
      pinMode(DETECT_OUT, INPUT_PULLUP);
      digitalWrite(DETECT_IN, HIGH);
    }
  }
}

/* ------------------------------------------------------------------------ */
bool Module::parse_set_flash() {
  /* ----------------------------------------------------------------------
   */
  if (checkSetMsgReceived(rx_buffer, ARG_SAVE_IN_DATA_FLASH,
                          LEN_SAVE_IN_DATA_FLASH, SAVE_DATA_LEN)) {

#ifdef MODULE_USE_FLASH_MEMORY
    uint16_t add = rx_buffer[SAVE_ADDRESS_1_POS];
    add += (rx_buffer[SAVE_ADDRESS_2_POS] << 8);
    uint8_t d = rx_buffer[SAVE_DIMENSION_POS];

#ifdef DEBUG_FLASH
    Serial.println("SAVE Flash: address " + String(add) + " dim " + String(d));
#endif

    uint8_t data[32];
    memset((uint8_t *)data, 0, 32);
    for (int i = 0; i < d; i++) {
#ifdef DEBUG_FLASH
      Serial.print(rx_buffer[SAVE_DATA_INIT_POS + i], HEX);
      Serial.print(" ");
#endif
      data[i] = rx_buffer[SAVE_DATA_INIT_POS + i];
    }
#ifdef DEBUG_FLASH
    Serial.println();
#endif
    EEPROM.put(add, data);
#endif
    return true;
  } else {
    return false;
  }
}

/* ------------------------------------------------------------------------ */
bool Module::parse_get_flash() {
  /* ----------------------------------------------------------------------
   */
  if (checkGetMsgReceived(rx_buffer, ARG_GET_DATA_FROM_FLASH,
                          LEN_GET_DATA_FROM_FLASH, READ_DATA_LEN)) {
    flash_add = rx_buffer[READ_ADDRESS_1_POS];
    flash_add += (rx_buffer[READ_ADDRESS_2_POS] << 8);
    flash_dim = rx_buffer[READ_DATA_DIM_POS];
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
int Module::prepare_ans_get_flash() {
  /* ----------------------------------------------------------------------
   */
  uint8_t data[32] = {0};

#ifdef MODULE_USE_FLASH_MEMORY
  EEPROM.get(flash_add, data);
#endif
  tx_buffer[ANS_GET_DATA_ADDRESS_1_POS] = (uint8_t)(flash_add & 0xFF);
  tx_buffer[ANS_GET_DATA_ADDRESS_2_POS] = (uint8_t)((flash_add & 0xFF00) >> 8);
  tx_buffer[ANS_GET_DATA_DIMENSION_POS] = flash_dim;
#ifdef DEBUG_FLASH
  Serial.println("READ Flash: address " + String(flash_add) + " dim " +
                 String(flash_dim));
#endif
  for (int i = 0; i < 32; i++) {
    if (i < flash_dim) {
      tx_buffer[ANS_GET_DATA_DATA_INIT_POS + i] = data[i];
#ifdef DEBUG_FLASH
      Serial.print(tx_buffer[ANS_GET_DATA_DATA_INIT_POS + i], HEX);
      Serial.print(" ");
#endif
    } else {
      tx_buffer[ANS_GET_DATA_DATA_INIT_POS + i] = 0;
    }
  }
#ifdef DEBUG_FLASH
  Serial.println();
#endif

  return prepareGetAns(tx_buffer, ANS_ARG_GET_DATA_FROM_FLASH,
                       ANS_LEN_GET_DATA_FROM_FLASH, ANS_GET_DATA_LEN);
}
/* ------------------------------------------------------------------------ */
void Module::end() {}
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* Give the device a new slave address (add) */
/* ------------------------------------------------------------------------ */
void Module::setAddress(uint8_t add) {
  Wire.end();
  Wire.begin(add);
}
#endif
