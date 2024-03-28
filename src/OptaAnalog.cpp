/* -------------------------------------------------------------------------- */
/* FILENAME:    OptaAnalog.cpp
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   REVISION:    0.0.1
   DESCRIPTION: Implementation of the class OptaAnalog
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "CommonCfg.h"
#include "MsgCommon.h"
#include "Protocol.h"
#include <cstdint>
#if defined ARDUINO_OPTA_ANALOG || defined ARDUINO_UNOR4_MINIMA
#include "Controller.h"
#include "OptaAnalogTypes.h"
#include "OptaCrc.h"
#include "sys/_stdint.h"

#include "OptaAnalog.h"
#include "OptaAnalogCfg.h"

#define FRAME_SIZE 4

#ifdef DEBUG_SERIAL
void dbgPrintHex(uint8_t h, bool space = false, bool nl = false) {
  Serial.print("0x");
  if (h < 0x10) {
    Serial.print("0");
  }
  Serial.print(h, HEX);
  if (space)
    Serial.print(" ");
  if (nl)
    Serial.println();
}

void dbgPrintHex(uint16_t h, bool space = false, bool nl = false) {
  Serial.print("0x");
  if (h < 0x10) {
    Serial.print("000");
  } else if (h < 0x100) {
    Serial.print("00");
  } else if (h < 0x1000) {
    Serial.print("0");
  }

  Serial.print(h, HEX);

  if (space)
    Serial.print(" ");
  if (nl)
    Serial.println();
}

void printSingleReg(OptaAnalog *oa, uint8_t device, uint8_t addr) {
  uint16_t value = 0;
  value = oa->readReg(device, addr);
  Serial.print("[");
  Serial.print(device);
  Serial.print("][");
  dbgPrintHex(addr, true);
  Serial.print("] = ");
  dbgPrintHex(value, true, true);
}

/* -------------------------------------------------------------------------- */
void printFourReg(OptaAnalog *oa, uint8_t device, uint8_t r, String desc) {
  Serial.println(desc);
  for (int i = 0; i < 4; i++) {
    printSingleReg(oa, device, r + (uint8_t)i);
  }
  Serial.println();
}
#endif
/*                        FUNCTIONS RELATED TO PWM                            */
/* -------------------------------------------------------------------------- */
/* PWM0_A - ch1 of first PWM IC    P107   [D4]   32 bit
 * PWM3_A - ch2 of first PWM IC    P111   [D7]   16 bit
 * IRQ3 - Fault of first PWM IC    P110   [D8]
 *
 * PWM2_A - ch1 of second PWM IC   P500   [D6]   16 bit
 * PWM1_A - ch2 of second PWM IC   P105   [D5]   32 bit
 * IRQ11 - Fault of second PWM IC  P501   [D9]
 *
 * PWM clock is 48 MHz
 * The maximum divider is 64
 * So the minimum frequency is 48 MHz / 64 = 750 kHz
 * The maximum period is 2^32 / 750 kHz = 5726 s for 32 bit PWM
 * The maximum period is 2^16 / 750 kHz = 87 ms for 16 bit PWM
 * */

/* -------------------------------------------------------------------------- */
void OptaAnalog::begin_pwms() {
  /* ------------------------------------------------------------------------ */
  for (int i = 0; i < OA_PWM_CHANNELS_NUM; i++) {
    pwm[i].begin();
  }
}

/* -------------------------------------------------------------------------- */
void OptaAnalog::configurePwmPeriod(uint8_t ch, uint32_t _period_us) {
  /* ------------------------------------------------------------------------ */
  if (ch >= OA_PWM_CHANNELS_NUM) {
    return;
  }

  pwm[ch].set_period_us = _period_us;
}

/* -------------------------------------------------------------------------- */
void OptaAnalog::configurePwmPulse(uint8_t ch, uint32_t _pulse_us) {
  /* ------------------------------------------------------------------------ */

  if (ch >= OA_PWM_CHANNELS_NUM) {
    return;
  }
  pwm[ch].set_pulse_us = _pulse_us;
}

/* -------------------------------------------------------------------------- */
void OptaAnalog::updatePwm(uint8_t ch) {
  /* ------------------------------------------------------------------------ */
  if (ch >= OA_PWM_CHANNELS_NUM) {
    return;
  }
  if (pwm[ch].set_period_us == 0 && pwm[ch].active) {
    pwm[ch].pwm.suspend();
    pwm[ch].active = false;
  } else {
    if (pwm[ch].set_pulse_us < pwm[ch].set_period_us) {
      if (pwm[ch].set_period_us != pwm[ch].period_us) {
        pwm[ch].pwm.period_us(pwm[ch].set_period_us);
      }
      if (pwm[ch].set_pulse_us != pwm[ch].pulse_us) {
        pwm[ch].pwm.pulseWidth_us(pwm[ch].set_pulse_us);
      }
      pwm[ch].period_us = pwm[ch].set_period_us;
      pwm[ch].pulse_us = pwm[ch].set_pulse_us;
      if (!pwm[ch].active) {
#ifdef ARDUINO_OPTA_ANALOG
        pwm[ch].pwm.resume();
        pwm[ch].active = true;
#endif
      }
    }
  }
}
/* -------------------------------------------------------------------------- */
void OptaAnalog::suspendPwm(uint8_t ch) {
  /* ------------------------------------------------------------------------ */
  if (ch >= OA_PWM_CHANNELS_NUM) {
    return;
  }

  if (pwm[ch].active) {
    pwm[ch].pwm.suspend();
    pwm[ch].active = false;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void OptaAnalog::setLedStatus(uint8_t i) {
  PinStatus hw_st = (led_status & (1 << i)) ? HIGH : LOW;
  /* this take advantage from the definition of LED position in the Opta
   * Analog variant */
  digitalWrite(OA_VARIANT_FIST_LED_POS + i, hw_st);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void OptaAnalog::updateLedStatus() {
  for (int i = 0; i < OA_NUMBER_OF_LEDS; i++) {
    setLedStatus(i);
  }
}

/*                            CONSTRUCTOR */

/* Note: PWM_x are defined in the variant of OPTA Analog, they are defined
 * in an order so that PWM_0 correspond to PWM ch 0 which is the leftmost on
 * the Opta analog connector, PWM_1 is the second Pin and so on (but take
 * into account that third and fourth pins are ground) To obtain the correct
 * correspondent is necessary that PWM_x are used "in order" from 0 to 3 in
 * the Constructor here below */

/* --------------------------------------------------------------------------
 */
OptaAnalog::OptaAnalog()
    : pwm{CfgPwm(PWM_0), CfgPwm(PWM_1), CfgPwm(PWM_2), CfgPwm(PWM_3)},
      led_status(0), rtd_update_time(1000) {
  expansion_type = EXPANSION_OPTA_ANALOG;
  /* ------------------------------------------------------------------------ */
}

/* BEGIN */
/* -------------------------------------------------------------------------- */
void OptaAnalog::begin() {
  /* ------------------------------------------------------------------------ */

  Module::begin();
  SPI.begin();

#ifdef ARDUINO_OPTA_ANALOG
  /* chip select set up in variant */

  /* pwm initialization */
  begin_pwms();

  /* GPO initialization  */
  configureGpo(0, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(0);
  configureGpo(1, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(1);
  configureGpo(2, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(2);
  configureGpo(3, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(3);
  configureGpo(4, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(4);
  configureGpo(5, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(5);
  configureGpo(6, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(6);
  configureGpo(7, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(7);

#else
#ifdef DEBUG_SERIAL
  Serial.println("*** OPTA ANALOG TEST VERSION, runs on UNO R4 MINIMA ***");
  Serial.println("-------------------------------------------------------");
#endif
  /* chip select */
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  delay(PRE_AN_DEV_RESET_TIME);
  pinMode(DIO_RESET, OUTPUT);
  digitalWrite(DIO_RESET, LOW);
  delay(AN_DEV_RESET_TIME);
  digitalWrite(DIO_RESET, HIGH);
  delay(POST_AN_DEV_RESET_TIME);

  configureGpo(0, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(0);
  configureGpo(1, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(1);
  configureGpo(2, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(2);
  configureGpo(3, GPO_HIGH_IMP, GPO_LOW);
  updateGpo(3);

#endif
}

#ifdef DEBUG_UPDATE_FW
/* used to recognize sw version with led blinking during fw update tests*/
void OptaAnalog::debug_with_leds() {
  static unsigned long start = millis();
  static bool status = true;

  if (millis() - start > 2000) {
    start = millis();

    if (status) {
      set_led_on(0);
      status = false;
    } else {
      set_led_off(0);
      status = true;
    }
  }
}
#endif
void new_i2c_address_obtained(void *ptr) {
  OptaAnalog *oa = (OptaAnalog *)ptr;
  if (ptr != nullptr) {
    if (oa->getI2CAddress() >= OPTA_CONTROLLER_FIRST_AVAILABLE_ADDRESS &&
        oa->getI2CAddress() < OPTA_CONTROLLER_FIRST_TEMPORARY_ADDRESS) {
      oa->swAnalogDevReset();
      oa->suspendPwm(0);
      oa->suspendPwm(1);
      oa->suspendPwm(2);
      oa->suspendPwm(3);
    }
  }
}
void OptaAnalog::update() {

#ifdef DEBUG_ADC_CONVERTER
  int sel = -1;
  char num[10] = {0};
  if (Serial.available()) {
    int pos = 0;
    while (Serial.available()) {
      char ch = Serial.read();
      Serial.print("+ ");
      Serial.println(ch);
      if (ch >= 0x30 && ch <= 0x39 && pos < 10) {
        if (pos == 0) {
          sel = ch - 0x30;
        } else {
          sel *= 10;
          sel += ch - 0x30;
        }

        pos++;
      }
    }
  }

  if (sel != -1) {
    Serial.println("Selezione = " + String(sel));
    if (sel >= 0 && sel <= 7) {
      if (adc[sel].en_conversion) {
        configureAdcEnable(sel, false);

      } else {
        configureAdcEnable(sel, true);
      }
    }

    if (adc_ch_mask_0_last != adc_ch_mask_0) {
      adc_ch_mask_0_last = adc_ch_mask_0;
      stop_adc(0, false);
      start_adc(0, false);

      uint16_t r = 0;
      read_direct_reg(0, 0x2E, r);

      Serial.print("Alert status (0) = ");
      Serial.println(r, HEX);

      r = 0;
      read_direct_reg(0, 0x23, r);

      Serial.print("ADC CONTROL (0) = ");
      Serial.println(r, HEX);

      r = 0;
      read_direct_reg(1, 0x23, r);

      Serial.print("ADC CONTROL (1) = ");
      Serial.println(r, HEX);

      while (!Serial.available()) {
      }

      while (Serial.available()) {
        Serial.read();
      }
    }

    if (adc_ch_mask_1_last != adc_ch_mask_1) {
      adc_ch_mask_1_last = adc_ch_mask_1;
      stop_adc(1, false);
      start_adc(1, false);

      uint16_t r = 0;
      read_direct_reg(1, 0x2E, r);
      Serial.print("Alert status (1) = ");
      Serial.println(r, HEX);

      r = 0;
      read_direct_reg(0, 0x23, r);
      Serial.print("ADC CONTROL (0) = ");
      Serial.println(r, HEX);

      r = 0;
      read_direct_reg(1, 0x23, r);
      Serial.print("ADC CONTROL (1) = ");
      Serial.println(r, HEX);

      while (!Serial.available()) {
      }
      while (Serial.available()) {
        Serial.read();
      }
    }
  }

  updateAdc(false);
  Serial.println("\nch 0 = " + String(adc[0].conversion));
  Serial.println("ch 1 = " + String(adc[1].conversion));
  Serial.println("ch 2 = " + String(adc[2].conversion));
  Serial.println("ch 3 = " + String(adc[3].conversion));
  Serial.println("ch 4 = " + String(adc[4].conversion));
  Serial.println("ch 5 = " + String(adc[5].conversion));
  Serial.println("ch 6 = " + String(adc[6].conversion));
  Serial.println("ch 7 = " + String(adc[7].conversion));

  delay(1000);
#endif

  Module::update();
#ifdef DEBUG_UPDATE_FW
  debug_with_leds();
  updateLedStatus();
#endif

  // unsigned long time = millis();
  static unsigned long last_rtd_update = millis();

  // MEMO: always first update DAC and then rtd
  // This ensure synchronization
  for (int i = 0; i < OA_AN_CHANNELS_NUM; i++) {
    updateDac(i);
  }
  if (update_dac_using_LDAC) {
    update_dac_using_LDAC = false;
    toggle_ldac();
  }

  if (millis() - last_rtd_update > rtd_update_time) {
    static int i = 0;
    updateRtd(i);
    i++;
    if (i >= OA_AN_CHANNELS_NUM) {
      i = 0;
      last_rtd_update = millis();
    }
  }

  for (int i = 0; i < OA_PWM_CHANNELS_NUM; i++) {
    updatePwm(i);
  }

  /* handle start and stop of adc on device 0*/
  if (adc_ch_mask_0_last != adc_ch_mask_0) {
    adc_ch_mask_0_last = adc_ch_mask_0;
    stop_adc(0, false);
    start_adc(0, false);
  }

  /* handle start and stop of adc on device 1 */
  if (adc_ch_mask_1_last != adc_ch_mask_1) {
    adc_ch_mask_1_last = adc_ch_mask_1;
    stop_adc(1, false);
    start_adc(1, false);
  }

  updateDinReadings();
  updateAdc(false);
  updateAdcDiagnostics();
  updateLedStatus();
  update_live_status();

  // Serial.println("Update time: " + String(millis() - time));
}

/* ---------------------------------------------------------------------
 * SPI communication functions
 * --------------------------------------------------------------------- */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* set the offset for channels register */
uint8_t OptaAnalog::get_add_offset(uint8_t ch) {
  uint8_t rv = 0;
  if (ch < OA_AN_CHANNELS_NUM) {
#ifdef ARDUINO_UNOR4_MINIMA
    rv = ch;
#else
    if (ch == 0) {
      rv = 1;
    } else if (ch == 1) {
      rv = 0;
    } else if (ch == 2) {
      rv = 0;
    } else if (ch == 3) {
      rv = 1;
    } else if (ch == 4) {
      rv = 2;
    } else if (ch == 5) {
      rv = 3;
    } else if (ch == 6) {
      rv = 2;
    } else if (ch == 7) {
      rv = 3;
    }
#endif
  }
  return rv;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint8_t OptaAnalog::get_dummy_channel(uint8_t ch) {
  if (ch == 0 || ch == 1 || ch == 6 || ch == 7) {
    /* those channels belongs to device 0 */
    return OA_DUMMY_CHANNEL_DEVICE_0;
  } else if (ch == 2 || ch == 3 || ch == 4 || ch == 5) {
    return OA_DUMMY_CHANNEL_DEVICE_1;
  }
  return ch;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint8_t OptaAnalog::get_device(uint8_t ch) {
  if (ch == OA_DUMMY_CHANNEL_DEVICE_0) {
    return 0;
  } else if (ch == OA_DUMMY_CHANNEL_DEVICE_1) {
    return 1;
  } else if (ch == 0 || ch == 1 || ch == 6 || ch == 7) {
    /* those channels belongs to device 0 */
    return 0;
  } else {
    return 1;
  }
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::write_reg(uint8_t addr, uint16_t value, uint8_t ch) {
  uint8_t ad = addr + get_add_offset(ch);
  uint8_t device = get_device(ch);
  write_direct_reg(device, ad, value);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::read_reg(uint8_t addr, uint16_t &value, uint8_t ch) {
  /* Bit 8 of read register: Determines the content of the MSBs in the
   * SPI read frame. When this bit is set to 0, the READBACK_ADDR is
   * returned in bits, Bits[30:24] (the MSB is not shown) of any
   * subsequent SPI read. When this bit is set to 1, the ADC_RDY bit,
   * alert flags, and the four digital input outputs are returned
   * in Bits[30:24] of any subsequent SPI read. */
  uint8_t ad = addr + get_add_offset(ch);
  uint8_t device = get_device(ch);
  return read_direct_reg(device, ad, value);
}

void OptaAnalog::write_direct_reg(uint8_t device, uint8_t addr,
                                  uint16_t value) {

  com_buffer[0] = addr;
  com_buffer[1] = value >> 8;
  com_buffer[2] = value & 0xFF;
  com_buffer[3] = OptaCrc8::calc(com_buffer, 3, 0);

#if defined DEBUG_SERIAL && defined DEBUG_SPI_COMM
  Serial.print("TX: ");
  for (int i = 0; i < 4; i++) {
    if (com_buffer[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(com_buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif

#ifdef ARDUINO_UNOR4_MINIMA
  digitalWrite(CS, LOW);
#else
  if (device == 0) {
    digitalWrite(SPI_CS_1, LOW);
  } else if (device == 1) {
    digitalWrite(SPI_CS_2, LOW);
  }
  delay(1);
#endif
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE1));
  SPI.transfer(com_buffer, 4);
  SPI.endTransaction();

#if defined DEBUG_SERIAL && defined DEBUG_SPI_COMM
  Serial.print("RX: ");
  for (int i = 0; i < 4; i++) {
    if (com_buffer[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(com_buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif

#ifdef ARDUINO_UNOR4_MINIMA
  digitalWrite(CS, HIGH);

#else
  if (device == 0) {
    digitalWrite(SPI_CS_1, HIGH);
  } else if (device == 1) {
    digitalWrite(SPI_CS_2, HIGH);
  }
#endif
}

bool OptaAnalog::read_direct_reg(uint8_t device, uint8_t addr,
                                 uint16_t &value) {
  write_direct_reg(device, OPTA_AN_READ_SELECT, (addr | 0x100));
  write_direct_reg(device, OPTA_AN_NOP, OPTA_AN_NOP);
  if (OptaCrc8::verify(com_buffer[3], com_buffer, 3)) {
    value = com_buffer[2] | ((uint16_t)com_buffer[1] << 8);
    return true;
  }
  return false;
}
/* ################################################################### */
/* CONFIGURE CHANNEL FUNCTIONs                                         */
/* ################################################################### */

void OptaAnalog::configureFunction(uint8_t ch, CfgFun_t f) {
  if (ch < OA_AN_CHANNELS_NUM) {
    // Serial.println("Configure function: ch " + String(ch) + " function " +
    // String((int)f));
    fun[ch] = f;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

CfgFun_t OptaAnalog::getFunction(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    return fun[ch];
  }
  return CH_FUNC_UNDEFINED;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void OptaAnalog::sendFunction(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    uint8_t v = 0;
    CfgFun_t f = fun[ch];
    switch (f) {
    case CH_FUNC_ADC:
      v = CH_ADC;
      break;
    case CH_FUNC_VOLTAGE_OUTPUT:
      v = CH_VO;
      break;
    case CH_FUNC_CURRENT_OUTPUT:
      v = CH_CO;
      break;
    case CH_FUNC_VOLTAGE_INPUT:
      v = CH_VI;
      break;
    case CH_FUNC_CURRENT_INPUT_EXT_POWER:
      v = CH_CI_EP;
      break;
    case CH_FUNC_CURRENT_INPUT_LOOP_POWER:
      v = CH_CI_LP;
      break;
    case CH_FUNC_RESISTANCE_MEASUREMENT:
      v = CH_RM;
      break;
    case CH_FUNC_DIGITAL_INPUT:
      v = CH_DI;
      break;
    case CH_FUNC_DIGITAL_INPUT_LOOP_POWER:
      v = CH_DI_LP;
      break;
    default:
      v = CH_ADC;
      break;
    }

    write_reg(OA_REG_FUNC_SETUP, v, ch);
    // Serial.println("Send funtion ch " + String(ch) + " value " +
    // String(v));
  }
}

/* ##########################################################################
 */
/*                            ADC FUNCTIONs */
/* ##########################################################################
 */

void OptaAnalog::configureAdcMux(uint8_t ch, CfgAdcMux_t m) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].mux = m;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcRange(uint8_t ch, CfgAdcRange_t r) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].range = r;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcPullDown(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].en_pull_down = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcRejection(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].en_rejection = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcDiagnostic(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].en_conversion_diagnostic = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcDiagRejection(uint8_t ch, bool en) {
  en_adc_diag_rej[ch] = en;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcEnable(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].en_conversion = en;
    if (ch == 0 || ch == 1 || ch == 6 || ch == 7) {
      if (en) {
        adc_ch_mask_0 |= (1 << ch);
      } else {
        adc_ch_mask_0 &= ~(1 << ch);
      }
    } else if (ch == 2 || ch == 3 || ch == 4 || ch == 5) {
      if (en) {
        adc_ch_mask_1 |= (1 << ch);
      } else {
        adc_ch_mask_1 &= ~(1 << ch);
      }
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcMovingAverage(uint8_t ch, uint8_t ma) {
  if (ch < OA_AN_CHANNELS_NUM) {
    adc[ch].mov_average_req = ma;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Send the ADC configuration for CHANNEL ch to the Analog Device */
void OptaAnalog::sendAdcConfiguration(uint8_t ch) {

  uint16_t reg_cfg = 0;
  if (ch < OA_AN_CHANNELS_NUM) {

    if (adc[ch].mux == CFG_ADC_INPUT_NODE_IOP_AGND_SENSE) {

      ADC_CFG_SET_AD_SENSE(reg_cfg);
    } else if (adc[ch].mux == CFG_ADC_INPUT_NODE_100OHM_R) {

      ADC_CFG_SET_100_OHM(reg_cfg);
    }

    switch (adc[ch].range) {
    case CFG_ADC_RANGE_10V:

      ADC_CFG_SET_RANGE(reg_cfg, ADC_CFG_RANGE_10V);
      break;

    case CFG_ADC_RANGE_2_5V_RTD:

      ADC_CFG_SET_RANGE(reg_cfg, ADC_CFG_RANGE_2_5V_RTD);
      break;
    case CFG_ADC_RANGE_2_5V_LOOP:

      ADC_CFG_SET_RANGE(reg_cfg, ADC_CFG_RANGE_2_5V_LOOP);
      break;
    case CFG_ADC_RANGE_2_5V_BI:

      ADC_CFG_SET_RANGE(reg_cfg, ADC_CFG_RANGE_2_5V_BI);
      break;
    default:

      ADC_CFG_SET_RANGE(reg_cfg, ADC_CFG_RANGE_10V);
      break;
    }

    if (adc[ch].en_pull_down) {
      ADC_CFG_ENABLE_PULL_DOWN(reg_cfg);
    } else {
      ADC_CFG_DISABLE_PULL_DOWN(reg_cfg);
    }

    if (adc[ch].en_rejection) {
      ADC_CFG_ENABLE_REJECT(reg_cfg);
    } else {
      ADC_CFG_DISABLE_REJECT(reg_cfg);
    }

    write_reg(OA_REG_ADC_CONFIG, reg_cfg, ch);
  }
}

/* -------------------------------------------------------- */
bool OptaAnalog::is_adc_started(uint8_t device) {
  uint16_t r = 0;
  read_direct_reg(device, OA_ADC_CONV_CTRL, r);
  uint16_t v = (r & (3 << 8)) >> 8;
  if (v == 1 || v == 2) {
    return true;
  }
  return false;
}

/* -------------------------------------------------------- */
void OptaAnalog::stop_adc(uint8_t device, bool power_down) {
  /* stop adc 1 by writing stop in the ADC conversion control */
  uint16_t stop_value = 0;
  if (power_down) {
    stop_value = STOP_CONTINUOUS_MODE_AND_OFF;
  } else {
    stop_value = STOP_CONTINUOUS_MODE_AND_ON;
  }

  uint16_t r = 0;
  if (is_adc_started(device)) {
    do {
      write_direct_reg(device, OPTA_AN_ADC_CONV_CTRL, stop_value);
      read_direct_reg(device, OA_LIVE_STATUS, r);
    } while (r & ADC_BUSY_MASK);
  }
}

bool OptaAnalog::adc_enable_channel(uint8_t ch, uint16_t &reg) {
  if (adc[ch].en_conversion) {
    ENABLE_ADC_CONVERSION(reg, get_add_offset(ch));
    if (adc[ch].en_conversion_diagnostic) {
      ENABLE_ADC_DIAG_CONVERSION(reg, ch);
    }
    return true;
  }
  return false;
}
/* -------------------------------------------------------- */
void OptaAnalog::start_adc(uint8_t device, bool single_acquisition) {
  uint16_t reg_cfg = 0;
  bool start = false;

  if (device == OA_AN_DEVICE_0) {
    start |= adc_enable_channel(OA_CH_0, reg_cfg);
    start |= adc_enable_channel(OA_CH_1, reg_cfg);
    start |= adc_enable_channel(OA_CH_6, reg_cfg);
    start |= adc_enable_channel(OA_CH_7, reg_cfg);
    if (en_adc_diag_rej[OA_AN_DEVICE_0]) {
      ENABLE_DIAG_REJECTION(reg_cfg);
    }
  } else if (device == OA_AN_DEVICE_1) {
    start |= adc_enable_channel(OA_CH_2, reg_cfg);
    start |= adc_enable_channel(OA_CH_3, reg_cfg);
    start |= adc_enable_channel(OA_CH_4, reg_cfg);
    start |= adc_enable_channel(OA_CH_5, reg_cfg);
    if (en_adc_diag_rej[OA_AN_DEVICE_1]) {
      ENABLE_DIAG_REJECTION(reg_cfg);
    }
  }

  if (single_acquisition) {
    SET_ADC_START_STOP(reg_cfg, START_SINGLE_CONVERSION);
  } else {
    SET_ADC_START_STOP(reg_cfg, START_CONTINUOUS_CONVERSION);
  }
  if (start) {

    uint16_t r = 0;

    write_direct_reg(device, OA_LIVE_STATUS, ADC_DATA_READY);

    do {
      write_direct_reg(device, OA_ADC_CONV_CTRL, reg_cfg);
      read_direct_reg(device, OA_LIVE_STATUS, r);
    } while ((r & ADC_BUSY_MASK) == 0);
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::stopAdc(bool power_down /*= false*/) {
  if (is_adc_started(OA_AN_DEVICE_0)) {
    stop_adc(OA_AN_DEVICE_0, power_down);
  }
#ifdef ARDUINO_OPTA_ANALOG
  if (is_adc_started(OA_AN_DEVICE_1)) {
    stop_adc(OA_AN_DEVICE_1, power_down);
  }

#endif
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::startAdc(bool single) {
  start_adc(0, single);
#ifdef ARDUINO_OPTA_ANALOG
  start_adc(1, single);
#endif
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::update_adc_value(uint8_t ch) {
  uint16_t read_value = 0;

  if (adc[ch].en_conversion) {

    if (read_reg(OA_REG_ADC_RESULT, read_value, ch)) {
      adc[ch].conversion = read_value;
      if (adc[ch].mov_average_req > 0) {
        if (adc[ch].average_samples < adc[ch].mov_average_req) {
          adc[ch].average_samples++;
        }
        if (adc[ch].average_samples == 1) {
          adc[ch].average = (double)read_value;
        } else {
          double v = adc[ch].average_samples;
          adc[ch].average -= (adc[ch].average / v);
          adc[ch].average += ((double)read_value / v);
        }
      }
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
bool OptaAnalog::is_adc_updatable(uint8_t device, bool wait_for_conversion) {
  bool rv = false;
  uint16_t r = 0;
  /* this function verify if the ADC data are ready to be read */

  if (is_adc_started(device)) {
    /* obviously it makes sense only if the ADC is started */
    if (wait_for_conversion) {
      /* if wait_for_conversion is true, then the function is blocking
       * and wait until the ADC has finished the conversion */
      do {
        read_direct_reg(device, OA_LIVE_STATUS, r);
        delay(1);
      } while ((r & ADC_DATA_READY) == 0);
      rv = true;
    } else {
      read_direct_reg(device, OA_LIVE_STATUS, r);
      if (r & ADC_DATA_READY) {
        rv = true;
        write_direct_reg(device, OA_LIVE_STATUS, ADC_DATA_READY);
      }
    }
  }
  return rv;
}

void OptaAnalog::updateAdc(bool wait_for_conversion /*= false*/) {

  if (is_adc_updatable(OA_AN_DEVICE_0, wait_for_conversion)) {
    update_adc_value(OA_CH_0);
    update_adc_value(OA_CH_1);
    update_adc_value(OA_CH_6);
    update_adc_value(OA_CH_7);
    write_reg(OA_LIVE_STATUS, ADC_DATA_READY, OA_DUMMY_CHANNEL_DEVICE_0);
  }

#ifdef ARDUINO_OPTA_ANALOG
  if (is_adc_updatable(OA_AN_DEVICE_1, wait_for_conversion)) {
    update_adc_value(OA_CH_2);
    update_adc_value(OA_CH_3);
    update_adc_value(OA_CH_4);
    update_adc_value(OA_CH_5);
    write_reg(OA_LIVE_STATUS, ADC_DATA_READY, OA_DUMMY_CHANNEL_DEVICE_1);
  }

#endif
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::update_adc_diagnostic(uint8_t ch) {
  uint16_t read_value = 0;
  if (adc[ch].en_conversion_diagnostic) {
    if (read_reg(OA_REG_DIAG_RESULT, read_value, ch)) {
      adc[ch].diag_conversion = read_value;
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::updateAdcDiagnostics() {
  /* READ ALL THE ADC CONVERSIONs FOR DEVICE 0 */
  if (is_adc_started(OA_AN_DEVICE_0)) {
    update_adc_diagnostic(OA_CH_0);
    update_adc_diagnostic(OA_CH_1);
    update_adc_diagnostic(OA_CH_6);
    update_adc_diagnostic(OA_CH_7);
  }
  /* READ ALL THE ADC CONVERSIONs FOR DEVICE 1 */

#ifdef ARDUINO_OPTA_ANALOG
  if (is_adc_started(OA_AN_DEVICE_1)) {
    update_adc_diagnostic(OA_CH_2);
    update_adc_diagnostic(OA_CH_3);
    update_adc_diagnostic(OA_CH_4);
    update_adc_diagnostic(OA_CH_5);
  }
#endif
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint16_t OptaAnalog::getAdcDiagValue(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    return adc[ch].diag_conversion;
  }
  return 0;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint16_t OptaAnalog::getAdcValue(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    return adc[ch].conversion;
  }
  return 0;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcForCurrent(uint8_t ch, bool en_rej,
                                        bool set_function /*= true*/,
                                        bool loop /*= false*/) {
  /* configure the channel function */
  if (set_function) {
    configureFunction(ch, CH_FUNC_CURRENT_INPUT_EXT_POWER);
    sendFunction(ch);
  }
  /* Set the channel ADC with the following
   * configuration*/
  configureAdcMux(ch, CFG_ADC_INPUT_NODE_100OHM_R);
  if (loop) {
    configureAdcRange(ch, CFG_ADC_RANGE_2_5V_LOOP);
  } else {
    configureAdcRange(ch, CFG_ADC_RANGE_2_5V_RTD);
  }
  configureAdcPullDown(ch, false);
  configureAdcRejection(ch, en_rej);
  configureAdcEnable(ch, true);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAdcForVoltage(uint8_t ch, bool en_rej,
                                        bool use_pulldown,
                                        bool set_function /*=true*/) {
  if (set_function) {
    configureFunction(ch, CH_FUNC_VOLTAGE_INPUT);
    sendFunction(ch);
  }
  /* Set the channel ADC with the following
   * configuration*/
  configureAdcMux(ch, CFG_ADC_INPUT_NODE_IOP_AGND_SENSE);
  configureAdcRange(ch, CFG_ADC_RANGE_10V);
  configureAdcPullDown(ch, use_pulldown);
  configureAdcRejection(ch, en_rej);
  configureAdcEnable(ch, true);
}

/* ##################################################################### */
/*                         RTD FUNCTIONs                                 */
/* ##################################################################### */

void OptaAnalog::configureRtd(uint8_t ch, bool use_3_w, float current_mA) {
  if (ch < OA_AN_CHANNELS_NUM) {
    rtd[ch].is_rtd = true;
    rtd[ch].set_measure_current(current_mA);

    adc[ch].en_conversion = true;
#ifdef ARDUINO_UNOR4_MINIMA
    rtd[ch].use_3_wires = false;
    configureFunction(ch, CH_FUNC_RESISTANCE_MEASUREMENT);
#else
    if ((ch == 0 || ch == 1) && use_3_w) {
      rtd[ch].use_3_wires = true;
      configureFunction(ch, CH_FUNC_CURRENT_OUTPUT);
    } else {
      rtd[ch].use_3_wires = false;
      configureFunction(ch, CH_FUNC_RESISTANCE_MEASUREMENT);
    }
#endif
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::sendRtdConfiguration(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    if (rtd[ch].is_rtd) {
      sendFunction(ch);
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

float OptaAnalog::getRtdValue(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    if (rtd[ch].is_rtd) {
      return (float)rtd[ch].RTD;
    }
  }
  return 0;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* in this function there are some call to the Module::update() function
 * this is not related to the update RTD function but is due to the fact
 * that the RTD update for 3 wire is extremely low (around 800 ms)
 * this could lead to problem in detecting a "reset" of other expansion
 * when they pull down the detect out pin so the Module::update is called
 * here */
void OptaAnalog::updateRtd(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    if (rtd[ch].is_rtd) {
      if (rtd[ch].use_3_wires) {
        /* =============================================== */
        /* PART 1 - Set and measure the excitation current */
        /* =============================================== */
#ifdef ARDUINO_OPTA_ANALOG
        /* switch SENSEHF to RSENSE
         * LOW is correct for measuring current */
        // if (ch == 0) {
        digitalWrite(DIO_RTD_SWITCH_2, LOW);
        //} else {
        digitalWrite(DIO_RTD_SWITCH_1, LOW);
        //}
#endif
        delay(5);
        configureDacValue(ch, rtd[ch].current_value);
        updateDacValue(ch, true);
        /* measure the current provided by the
         * "current" DAC */
        configureAdcForCurrent(ch, true, // no rejection
                               false,    // no function set
                               true);    // self loop powered
        sendAdcConfiguration(ch);
        stopAdc(); // program configuration
        startAdc();
        updateAdc(true);

        rtd[ch].set_i_excite(adc[ch].conversion);

        Module::update();
        /* ===============================================
         */
        /* PART 2 - measure the voltage on the RDT
         * this will determine R_RDT + 2*RL so the
         * resistance of the RDT plus the resistance of
         * the 2 wires   */
        /* ===============================================
         */
        configureAdcForVoltage(ch, true, // no rejection
                               false,    // no pulldows
                               false);   // no function set
        // "correct" the configuration respect to the
        // default one
        configureAdcRange(ch, CFG_ADC_RANGE_2_5V_RTD); // range
        sendAdcConfiguration(ch);
        stopAdc(); // program configuration

        startAdc();
        updateAdc(true);
        rtd[ch].set_adc_RTD_2RL(adc[ch].conversion);

        Module::update();

        /* =============================================== */
        /* PART 3 - measure the voltage on the RDT
         * this will determine R_RDT + RL so the resistance
         * of the RDT plus the resistance of the 2 wires   */
        /* =============================================== */
#ifdef ARDUINO_OPTA_ANALOG
        // if (ch == 0) {
        digitalWrite(DIO_RTD_SWITCH_2, HIGH);
        // Serial.println("channel 0");
        //} else {
        digitalWrite(DIO_RTD_SWITCH_1, HIGH);
        // Serial.println("channel 1");
        //}
#endif
        delay(5);
        configureAdcForCurrent(ch, false, // no rejection
                               false,     // no function set
                               false);    // self loop powered
        sendAdcConfiguration(ch);
        stopAdc(); // program configuration
        startAdc();
        updateAdc(true);

        Module::update();
        /* This uses a formula to convert ADC value to
         * excitement current
         */
        rtd[ch].set_adc_RTD_RL(adc[ch].conversion);

        rtd[ch].calc_RTD();
      } else {
        /* use 2 wire measurement */
        startAdc();

        updateAdc(false);
        rtd[ch].set(adc[ch].conversion);
      }
    }
  }
}

/* ###################################################################### */
/*                         DAC FUNCTIONs                                  */
/* ###################################################################### */

void OptaAnalog::configureDacCurrLimit(uint8_t ch, CfgOutCurrLim_t cl) {
  if (ch < OA_AN_CHANNELS_NUM) {
    dac[ch].current_limit = cl;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDacUseSlew(uint8_t ch, CfgOutSlewRate_t sr,
                                     CfgOutSlewStep_t r) {
  if (ch < OA_AN_CHANNELS_NUM) {
    dac[ch].enable_slew = true;
    dac[ch].slew_rate = sr;
    dac[ch].slew_step = r;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDacDisableSlew(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    dac[ch].enable_slew = false;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDacUseReset(uint8_t ch, uint16_t value) {
  if (ch < OA_AN_CHANNELS_NUM) {
    dac[ch].enable_clear = true;
    dac[ch].reset_value = value;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDacDisableReset(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    dac[ch].enable_clear = false;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::sendDacConfiguration(uint8_t ch) {
  uint16_t cfg = 0;
  if (ch < OA_AN_CHANNELS_NUM) {

    if (dac[ch].current_limit == OUT_CURRENT_LIMIT_7_5mA) {
      cfg |= CURRENT_LIMIT_7_5mA;
    } else { /* nothing to do */
    }

    if (dac[ch].enable_clear) {
      ENABLE_CLEAR(cfg);
    } else { /* nothing to do */
    }

    if (dac[ch].enable_slew) {
      ENABLE_SLEW(cfg);
    } else { /* nothing to do */
    }

    switch (dac[ch].slew_rate) {
    case OUT_SLEW_RATE_4k:
      SET_SLEW_RATE(cfg, SLEW_RATE_4k);
      break;
    case OUT_SLEW_RATE_64k:
      SET_SLEW_RATE(cfg, SLEW_RATE_64k);
      break;
    case OUT_SLEW_RATE_150k:
      SET_SLEW_RATE(cfg, SLEW_RATE_150k);
      break;
    case OUT_SLEW_RATE_240k:
      SET_SLEW_RATE(cfg, SLEW_RATE_240k);
      break;
    default:
      SET_SLEW_RATE(cfg, SLEW_RATE_4k);
      break;
    }

    switch (dac[ch].slew_step) {
    case OUT_SLEW_STEP_64:
      SET_SLEW_STEP(cfg, SLEW_STEP_64);
      break;
    case OUT_SLEW_STEP_120:
      SET_SLEW_STEP(cfg, SLEW_STEP_120);
      break;
    case OUT_SLEW_STEP_500:
      SET_SLEW_STEP(cfg, SLEW_STEP_500);
      break;
    case OUT_SLEW_STEP_1820:
      SET_SLEW_STEP(cfg, SLEW_STEP_1820);
      break;
    default:
      SET_SLEW_STEP(cfg, SLEW_STEP_64);
      break;
    }
    write_reg(OA_REG_DAC_CONFIG, cfg, ch);
    if (dac[ch].enable_clear) {
      write_reg(OA_REC_DAC_CLEAR_CODE, dac[ch].reset_value, ch);
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDacValue(uint8_t ch, uint16_t value) {
  if (ch < OA_AN_CHANNELS_NUM) {
    if (value <= OA_MAX_DAC_VALUE) {
      dac[ch].set_value = value;
    }
  }
}
void OptaAnalog::toggle_ldac() {
  digitalWrite(LDAC1, LOW);
  digitalWrite(LDAC2, LOW);
  delay(100);
  digitalWrite(LDAC1, HIGH);
  digitalWrite(LDAC2, HIGH);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::updateDacValue(uint8_t ch, bool toggle /*= true*/) {
  if (ch < OA_AN_CHANNELS_NUM) {
    if (dac[ch].value != dac[ch].set_value) {
      write_reg(OA_REG_DAC_CODE, dac[ch].set_value, ch);
      // Serial.println("DAC ch " + String(ch) + " value " +
      //                String(dac[ch].set_value));
      dac[ch].value = dac[ch].set_value;

      if (toggle) {
        toggle_ldac();
      }
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::resetDacValue(uint8_t ch) {
  // TODO: implement
  (void)ch;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::is_dac_used(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    return (fun[ch] == CH_FUNC_VOLTAGE_OUTPUT ||
            fun[ch] == CH_FUNC_CURRENT_OUTPUT);
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::updateDac(uint8_t ch) {
  if (is_dac_used(ch)) {
    updateDacPresentValue(ch);
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::updateDacPresentValue(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    /* Opta Analog DAC ACTIVE code (0x1E -> 0x21) */
    read_reg(OA_REG_DAC_ACTIVE, dac[ch].present_value, ch);
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

uint16_t OptaAnalog::getDacCurrentValue(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    return dac[ch].present_value;
  }
  return 0;
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* ######################################################################## */
/*                            DIN FUNCTIONs */
/* ######################################################################## */

void OptaAnalog::configureDinFilterCompIn(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    din[ch].filter_input_comparator = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinInvertCompOut(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    din[ch].invert_comparator_output = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinEnableComp(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    din[ch].enable_comparator = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinDebounceSimple(uint8_t ch, bool en) {
  if (ch < OA_AN_CHANNELS_NUM) {
    din[ch].debounce_mode_simple = en;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinScaleComp(uint8_t ch, bool en) {
  uint8_t d = get_device(ch);
  di_scaled[d] = en;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinCompTh(uint8_t ch, uint8_t v) {
  if (v >= 32) { // only 5 bits available
    v = 31;
  }
  uint8_t d = get_device(ch);
  di_th[d] = v;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinCurrentSink(uint8_t ch, uint8_t v) {
  if (v >= 16) { // only 4 bits available
    v = 15;
  }
  if (ch < OA_AN_CHANNELS_NUM) {
    din[ch].current_sink = v;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureDinDebounceTime(uint8_t ch, uint8_t v) {
  if (v >= 32) { // only 5 bits available
    v = 31;
  }
  if (ch < OA_AN_CHANNELS_NUM) {
    din[ch].debounce_time = v;
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::sendDinConfiguration(uint8_t ch) {
  uint16_t reg_cfg = 0;
  if (ch < OA_AN_CHANNELS_NUM) {
    if (din[ch].filter_input_comparator) {
      FILTER_COMPARATOR(reg_cfg);
    }

    if (din[ch].invert_comparator_output) {
      INVERT_COMPARATOR(reg_cfg);
    }

    if (din[ch].enable_comparator) {
      ENABLE_COMPARATOR(reg_cfg);
    }

    reg_cfg |= (din[ch].current_sink << DIN_SINK_BIT_START);

    if (din[ch].debounce_mode_simple) {
      SET_DEBOUNCE_MODE_COUNTER(reg_cfg);
    }

    DEBOUNCE_TIME(reg_cfg, din[ch].debounce_time);

    write_reg(OA_REG_DIN_CONFIG, reg_cfg, ch);

    reg_cfg = 0;

    uint8_t d = get_device(ch);
    if (!di_scaled[d]) {
      reg_cfg = TH_FIXED_TO_16V;
    }

    COMPARATOR_TH(reg_cfg, di_th[d]);

    write_reg(OA_REG_DIN_THRESH, reg_cfg, get_dummy_channel(ch));
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::updateDinReadings() {
  uint16_t read_value = 0;
  digital_ins = 0;
  /* since channel 0 belongs to the first device we use
   * this trick to use get_device function on channel 0
   */

  read_reg(OA_REG_DIN_COMP_OUT, read_value, OA_DUMMY_CHANNEL_DEVICE_0);
#ifdef ARDUINO_UNOR4_MINIMA
  digital_ins |= (read_value & 0x0F);
#else
  digital_ins |= (read_value & 0x2) >> 1;
  digital_ins |= (read_value & 0x1) << 1;
  digital_ins |= (read_value & 0xC) << 4;
#endif
  /* since channel 2 belongs to the second device we
   * use this trick to use get_device function on
   * channel 2 */

  read_reg(OA_REG_DIN_COMP_OUT, read_value, OA_DUMMY_CHANNEL_DEVICE_1);
  digital_ins |= (read_value & 0xF) << 2;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::getDinValue(uint8_t ch) {
  if (ch < OA_AN_CHANNELS_NUM) {
    if (digital_ins & (1 << ch)) {
      return true;
    }
  }
  return false;
}

/* ####################################################################### */
/*                         GPO FUNCTIONs                                   */
/* ####################################################################### */

/* GPO FUNCTIONS ARE AVAILABLE ONLY FOR TestaLog ana not for OPTA ANALOG */

void OptaAnalog::configureGpo(uint8_t ch, CfgGpo_t f, GpoState_t state) {
#ifndef ARDUINO_OPTA_ANALOG
  if (ch < OA_GPO_NUM) {
    gpo[ch].cfg = f;
    gpo[ch].state = state;
  }
#else
  (void)ch;
  (void)f;
  (void)state;
#endif
}

/* CONFIGURE the GPO function for the CHANNEL ch */
void OptaAnalog::updateGpo(uint8_t ch) {
#ifndef ARDUINO_OPTA_ANALOG
  uint16_t cfg = 0;
  if (ch < OA_GPO_NUM) {
    switch (gpo[ch].cfg) {
    case GPO_PULL_DOWN:
      cfg = GPO_SELECT_PULL_DOWN;
      break;
    case GPO_DRIVEN_SINGLE:
      cfg = GPO_SELECT_DRIVE_VIA_CONFIG;
      break;
    case GPO_DRIVEN_PARALLEL:
      cfg = GPO_SELECT_DRIVE_VIA_PARALLEL;
      break;
    case GPO_OUT_DEB_COMP:
      cfg = GPO_SELECT_DEBOUNCE_COMP;
      break;
    case GPO_HIGH_IMP:
      cfg = GPO_SELECT_HIGH_IMPEDENCE;
      break;
    default:
      cfg = GPO_SELECT_HIGH_IMPEDENCE;
      break;
    }
    if (gpo[ch].state == GPO_HIGH) {
      GPO_SELECT_HIGH(cfg);
    } else {
      /* nothing to do, already reset */
    }
    write_reg(OA_REG_GPO_CONFIG, cfg, ch);
  }
#else
  (void)ch;
#endif
}

void OptaAnalog::digitalWriteAnalog(uint8_t ch, GpoState_t s) {
#ifndef ARDUINO_OPTA_ANALOG
  if (ch < OA_GPO_NUM) {
    gpo[ch].cfg = GPO_DRIVEN_SINGLE;
    gpo[ch].state = s;
  }
  updateGpo(ch);
#else
  (void)ch;
  (void)s;
#endif
}

void OptaAnalog::digitalParallelWrite(GpoState_t a, GpoState_t b, GpoState_t c,
                                      GpoState_t d) {
#ifndef ARDUINO_OPTA_ANALOG
  uint16_t cfg = 0;
  if (a == GPO_HIGH) {
    PARALLEL_HIGH_CH_A(cfg);
  }
  if (b == GPO_HIGH) {
    PARALLEL_HIGH_CH_B(cfg);
  }
  if (c == GPO_HIGH) {
    PARALLEL_HIGH_CH_C(cfg);
  }
  if (d == GPO_HIGH) {
    PARALLEL_HIGH_CH_D(cfg);
  }
  write_reg(OPTA_AN_GPO_PARALLEL, cfg,
            -1); // TODO check if -1 is correct
#else
  (void)a;
  (void)b;
  (void)c;
  (void)d;
#endif
}

void OptaAnalog::swAnalogDevReset() {
#ifdef ARDUINO_OPTA_ANALOG
  stop_adc(0, false);
  stop_adc(1, false);
  digitalWrite(DIO_RESET_1, LOW);
  digitalWrite(DIO_RESET_2, LOW);
  delay(50);
  digitalWrite(DIO_RESET_1, HIGH);
  digitalWrite(DIO_RESET_2, HIGH);
  delay(100);
  adc_ch_mask_0 = 0;
  adc_ch_mask_1 = 0;
  adc_ch_mask_0_last = 0;
  adc_ch_mask_1_last = 0;

  // write_direct_reg(0, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_RESET_1);
  // write_direct_reg(0, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_RESET_2);
  // write_direct_reg(1, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_RESET_1);
  // write_direct_reg(1, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_RESET_2);
#else
  digitalWrite(DIO_RESET_1, LOW);
  delay(50);
  digitalWrite(DIO_RESET_1, HIGH);
  delay(100);
  // write_direct_reg(0, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_RESET_1);
  // write_direct_reg(0, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_RESET_2);
#endif
}

void OptaAnalog::sychLDAC() {
#ifdef ARDUINO_OPTA_ANALOG
  write_direct_reg(0, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_LDAC);
  write_direct_reg(1, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_LDAC);
#else
  write_direct_reg(0, OPTA_AN_CMD_REGISTER, OPTA_AN_KEY_LDAC);
#endif
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::is_adc_busy(uint8_t ch) {
  update_live_status(ch);
  if (ch == OA_DUMMY_CHANNEL_DEVICE_0) {
    return (bool)((state[OA_AN_DEVICE_0] & ADC_BUSY_MASK));
  } else if (ch == OA_DUMMY_CHANNEL_DEVICE_0) {
    return (bool)((state[OA_AN_DEVICE_1] & ADC_BUSY_MASK));
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::update_live_status(uint8_t ch) {
  if (ch == OA_DUMMY_CHANNEL_DEVICE_0) {
    read_reg(OA_LIVE_STATUS, state[OA_AN_DEVICE_0], ch);
  } else if (ch == OA_DUMMY_CHANNEL_DEVICE_1) {
    read_reg(OA_LIVE_STATUS, state[OA_AN_DEVICE_1], ch);
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::update_live_status() {
  update_live_status(OA_DUMMY_CHANNEL_DEVICE_0);
#ifndef ARDUINO_UNOR4_MINIMA
  update_live_status(OA_DUMMY_CHANNEL_DEVICE_1);
#endif
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* the live status register contains:
 * - bit 14 -> adc data ready
 * - bit 13 -> adc busy
 * - bit 10..12 -> channel being converted
 * This register is 1 for each device
 * The purpose of this function is to verify if the adc conversion is finished
 * for THAT channel (ch) */
bool OptaAnalog::is_adc_conversion_finished(uint8_t ch) {
  update_live_status(get_dummy_channel(ch));
  // depending on the channel the device is different
  // and we select the device in this way (see
  // get_device function)
  uint8_t _ch = get_device(ch);
  // there is a state for each device
  if (state[_ch] & ADC_DATA_READY) {
    // here we get the channel being converted from the
    // live status register
    uint16_t ch_used = (state[_ch] & ADC_DATA_READY);
    ch_used = ch_used >> 10;
    // but we need to to convert "our" channel (from 0
    // to 7) to "device" channel (from 0 to 3)
    if (get_add_offset(ch) == ch_used) {
      write_reg(OA_LIVE_STATUS, ADC_DATA_READY, get_dummy_channel(ch));
      return true;
    }
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::update_alert_mask(int8_t ch) {
  // TODO: implement
  (void)ch;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::update_alert_status() {
  read_reg(OA_ALERT_STATUS, state[OA_AN_DEVICE_0], OA_DUMMY_CHANNEL_DEVICE_0);
#ifndef ARDUINO_UNOR4_MINIMA
  read_reg(OA_ALERT_STATUS, state[OA_AN_DEVICE_1], OA_DUMMY_CHANNEL_DEVICE_1);
#endif
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void OptaAnalog::configureAlertMaskRegister(uint8_t device, uint16_t alert) {
  // TODO: implement
  (void)device;
  (void)alert;
}

/* ####################################################################### */
/*                     PARSING FUNCTIONs                                   */
/* ####################################################################### */

bool OptaAnalog::parse_setup_di_channel() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_CH_DI, LEN_OA_CH_DI,
                          OA_CH_DI_LEN)) {
    uint8_t ch = rx_buffer[OA_CH_DI_CHANNEL_POS];

    if (ch < OA_AN_CHANNELS_NUM) {
      rtd[ch].is_rtd = false;
    }

    configureFunction(ch, CH_FUNC_DIGITAL_INPUT);
    sendFunction(ch);
    if (rx_buffer[OA_CH_DI_FILTER_COMP_POS] == OA_ENABLE) {
      configureDinFilterCompIn(ch, true);
    } else {
      configureDinFilterCompIn(ch, false);
    }
    if (rx_buffer[OA_CH_DI_INVERT_COMP_POS] == OA_ENABLE) {
      configureDinInvertCompOut(ch, true);
    } else {
      configureDinInvertCompOut(ch, false);
    }
    if (rx_buffer[OA_CH_DI_ENABLE_COMP_POS] == OA_ENABLE) {
      configureDinEnableComp(ch, true);
    } else {
      configureDinEnableComp(ch, false);
    }
    if (rx_buffer[OA_CH_DI_DEBOUNCE_SIMPLE_POS] == OA_ENABLE) {
      configureDinDebounceSimple(ch, true);
    } else {
      configureDinDebounceSimple(ch, false);
    }
    if (rx_buffer[OA_CH_DI_SCALE_COMP_POS] == OA_ENABLE) {
      configureDinScaleComp(get_dummy_channel(ch), true);
    } else {
      configureDinScaleComp(get_dummy_channel(ch), false);
    }
    configureDinCompTh(get_dummy_channel(ch), rx_buffer[OA_CH_DI_COMP_TH_POS]);

    configureDinCurrentSink(ch, rx_buffer[OA_CH_DI_CURR_SINK_POS]);
    configureDinDebounceTime(ch, rx_buffer[OA_CH_DI_DEBOUNCE_TIME_POS]);
    sendDinConfiguration(ch);

    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);

    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_setup_dac_channel() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_CH_DAC, LEN_OA_CH_DAC,
                          OA_CH_DAC_LEN)) {
    uint8_t ch = rx_buffer[OA_CH_DAC_CHANNEL_POS];

    if (ch < OA_AN_CHANNELS_NUM) {
      rtd[ch].is_rtd = false;
    }

    if (rx_buffer[OA_CH_DAC_TYPE_POS] == OA_VOLTAGE_DAC) {
      configureFunction(ch, CH_FUNC_VOLTAGE_OUTPUT);
    } else if (rx_buffer[OA_CH_DAC_TYPE_POS] == OA_CURRENT_DAC) {
      configureFunction(ch, CH_FUNC_CURRENT_OUTPUT);
    }

    sendFunction(ch);

    if (rx_buffer[OA_CH_DAC_LIMIT_CURRENT_POS] == OA_ENABLE) {
      configureDacCurrLimit(ch, OUT_CURRENT_LIMIT_7_5mA);

    } else {
      configureDacCurrLimit(ch, OUT_CURRENT_LIMIT_30mA);
    }
    if (rx_buffer[OA_CH_DAC_ENABLE_SLEW_POS] == OA_ENABLE) {
      switch (rx_buffer[OA_CH_DAC_SLEW_RATE_POS]) {
      case OA_SLEW_RATE_0:
        configureDacUseSlew(ch, OUT_SLEW_RATE_4k, OUT_SLEW_STEP_64);
        break;
      case OA_SLEW_RATE_1:
        configureDacUseSlew(ch, OUT_SLEW_RATE_4k, OUT_SLEW_STEP_120);
        break;
      case OA_SLEW_RATE_2:
        configureDacUseSlew(ch, OUT_SLEW_RATE_4k, OUT_SLEW_STEP_500);
        break;
      case OA_SLEW_RATE_3:
        configureDacUseSlew(ch, OUT_SLEW_RATE_4k, OUT_SLEW_STEP_1820);
        break;
      case OA_SLEW_RATE_4:
        configureDacUseSlew(ch, OUT_SLEW_RATE_64k, OUT_SLEW_STEP_64);
        break;
      case OA_SLEW_RATE_5:
        configureDacUseSlew(ch, OUT_SLEW_RATE_64k, OUT_SLEW_STEP_120);
        break;
      case OA_SLEW_RATE_6:
        configureDacUseSlew(ch, OUT_SLEW_RATE_64k, OUT_SLEW_STEP_500);
        break;
      case OA_SLEW_RATE_7:
        configureDacUseSlew(ch, OUT_SLEW_RATE_64k, OUT_SLEW_STEP_1820);
        break;
      case OA_SLEW_RATE_8:
        configureDacUseSlew(ch, OUT_SLEW_RATE_150k, OUT_SLEW_STEP_64);
        break;
      case OA_SLEW_RATE_9:
        configureDacUseSlew(ch, OUT_SLEW_RATE_150k, OUT_SLEW_STEP_120);
        break;
      case OA_SLEW_RATE_10:
        configureDacUseSlew(ch, OUT_SLEW_RATE_150k, OUT_SLEW_STEP_500);
        break;
      case OA_SLEW_RATE_11:
        configureDacUseSlew(ch, OUT_SLEW_RATE_150k, OUT_SLEW_STEP_1820);
        break;
      case OA_SLEW_RATE_12:
        configureDacUseSlew(ch, OUT_SLEW_RATE_240k, OUT_SLEW_STEP_64);
        break;
      case OA_SLEW_RATE_13:
        configureDacUseSlew(ch, OUT_SLEW_RATE_240k, OUT_SLEW_STEP_120);
        break;
      case OA_SLEW_RATE_14:
        configureDacUseSlew(ch, OUT_SLEW_RATE_240k, OUT_SLEW_STEP_500);
        break;
      case OA_SLEW_RATE_15:
        configureDacUseSlew(ch, OUT_SLEW_RATE_240k, OUT_SLEW_STEP_1820);
        break;
      }
    } else {
      configureDacDisableSlew(ch);
    }
    sendDacConfiguration(ch);
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_setup_rtd_channel() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_CH_RTD, LEN_OA_CH_RTD,
                          OA_CH_RTD_LEN)) {
    uint8_t ch = rx_buffer[OA_CH_RTD_CHANNEL_POS];
    Float_u v;
    for (int i = 0; i < 4; i++) {
      v.bytes[i] = rx_buffer[OA_CH_RTD_CURRENT_POS + i];
    }
    if (rx_buffer[OA_CH_RTD_3WIRE_POS] == OA_ENABLE) {
      configureRtd(ch, true, v.value);
    } else {
      configureRtd(ch, false, v.value);
    }
    sendRtdConfiguration(ch);
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_setup_adc_channel() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_CH_ADC, LEN_OA_CH_ADC,
                          OA_CH_ADC_LEN)) {

    bool configure_function = true;

    if (rx_buffer[OA_CH_ADC_ADDING_ADC_POS] == OA_ENABLE) {
      configure_function = false;
    }

    uint8_t ch = rx_buffer[OA_CH_ADC_CHANNEL_POS];

    if (ch < OA_AN_CHANNELS_NUM) {
      rtd[ch].is_rtd = false;
    }

    if (rx_buffer[OA_CH_ADC_TYPE_POS] == OA_VOLTAGE_ADC) {
      if (configure_function) {
        configureFunction(ch, CH_FUNC_VOLTAGE_INPUT);
      }
      configureAdcMux(ch, CFG_ADC_INPUT_NODE_IOP_AGND_SENSE);
      configureAdcRange(ch, CFG_ADC_RANGE_10V);
      if (rx_buffer[OA_CH_ADC_PULL_DOWN_POS] == OA_ENABLE) {
        configureAdcPullDown(rx_buffer[OA_CH_ADC_CHANNEL_POS], true);
      } else if (rx_buffer[OA_CH_ADC_PULL_DOWN_POS] == OA_DISABLE) {
        configureAdcPullDown(rx_buffer[OA_CH_ADC_CHANNEL_POS], false);
      }

    } else if (rx_buffer[OA_CH_ADC_TYPE_POS] == OA_CURRENT_ADC) {
      if (configure_function) {
        configureFunction(ch, CH_FUNC_CURRENT_INPUT_EXT_POWER);
      }

      configureAdcMux(ch, CFG_ADC_INPUT_NODE_100OHM_R);
      configureAdcRange(ch, CFG_ADC_RANGE_2_5V_RTD);
      configureAdcPullDown(ch, false);
    }

    if (rx_buffer[OA_CH_ADC_REJECTION_POS] == OA_ENABLE) {
      configureAdcRejection(ch, true);
      configureAdcDiagRejection(get_dummy_channel(ch), true);
    } else if (rx_buffer[OA_CH_ADC_REJECTION_POS] == OA_DISABLE) {
      configureAdcRejection(ch, false);
      configureAdcDiagRejection(get_dummy_channel(ch), false);
    }
    if (rx_buffer[OA_CH_ADC_DIAGNOSTIC_POS] == OA_ENABLE) {
      configureAdcDiagnostic(ch, true);
    } else if (rx_buffer[OA_CH_ADC_DIAGNOSTIC_POS] == OA_DISABLE) {
      configureAdcDiagnostic(ch, false);
    }
    configureAdcMovingAverage(ch, rx_buffer[OA_CH_ADC_MOVING_AVE_POS]);
    if (configure_function) {
      sendFunction(ch);
    }
    sendAdcConfiguration(ch);
    configureAdcEnable(ch, true);

    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);

    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_get_rtd_value() {
  if (checkGetMsgReceived(rx_buffer, ARG_OA_GET_RTD, LEN_OA_GET_RTD,
                          OA_GET_RTD_LEN)) {
    uint8_t ch = rx_buffer[OA_CH_RTD_CHANNEL_POS];

    tx_buffer[ANS_OA_GET_RTD_CHANNEL_POS] = ch;
    if (ch < OA_AN_CHANNELS_NUM) {
      Float_u _rtd;
      _rtd.value = rtd[ch].RTD;
      for (int i = 0; i < 4; i++) {
        tx_buffer[ANS_OA_GET_RTD_VALUE_POS + i] = _rtd.bytes[i];
      }
    } else {
      for (int i = 0; i < 4; i++) {
        tx_buffer[ANS_OA_GET_RTD_VALUE_POS + i] = 0;
      }
    }
    prepareGetAns(tx_buffer, ANS_ARG_OA_GET_RTD, ANS_LEN_OA_GET_RTD,
                  ANS_OA_GET_RTD_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_set_rtd_update_rate() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_SET_RTD_UPDATE_TIME,
                          LEN_OA_SET_RTD_UPDATE_TIME,
                          OA_SET_RTD_UPDATE_TIME_LEN)) {
    uint16_t rate = rx_buffer[OA_SET_RTD_UPDATE_TIME_POS];
    rate += (rx_buffer[OA_SET_RTD_UPDATE_TIME_POS + 1] << 8);
    rtd_update_time = rate;
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_set_dac_value() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_SET_DAC, LEN_OA_SET_DAC,
                          OA_SET_DAC_LEN)) {

    uint8_t ch = rx_buffer[OA_SET_DAC_CHANNEL_POS];

    uint16_t value = rx_buffer[OA_SET_DAC_VALUE_POS];
    value += (rx_buffer[OA_SET_DAC_VALUE_POS + 1] << 8);

    if (rx_buffer[OA_SET_DAC_UPDATE_VALUE] == 1) {
      update_dac_using_LDAC = true;
    } else {
      update_dac_using_LDAC = false;
    }

    configureDacValue(ch, value);
    updateDacValue(ch, false);
    /* value are sent to the analog device during
     * update function */
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_set_all_dac_value() {

  Serial.println();
  if (checkSetMsgReceived(rx_buffer, ARG_OA_SET_ALL_DAC, LEN_OA_SET_ALL_DAC,
                          OA_SET_ALL_DAC_LEN)) {

    update_dac_using_LDAC = true;
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_get_adc_value() {
  if (checkGetMsgReceived(rx_buffer, ARG_OA_GET_ADC, LEN_OA_GET_ADC,
                          OA_GET_ADC_LEN)) {
    uint8_t ch = rx_buffer[OA_CH_ADC_CHANNEL_POS];
    tx_buffer[ANS_OA_ADC_CHANNEL_POS] = ch;
    if (ch < OA_AN_CHANNELS_NUM) {
      uint16_t value = 0;
      if (adc[ch].mov_average_req > 0) {
        value = (uint16_t)adc[ch].average;
      } else {
        value = adc[ch].conversion;
      }

      tx_buffer[ANS_OA_ADC_VALUE_POS] = (uint8_t)(value & 0xFF);
      tx_buffer[ANS_OA_ADC_VALUE_POS + 1] = (uint8_t)((value & 0xFF00) >> 8);
    } else {
      tx_buffer[ANS_OA_ADC_VALUE_POS] = 0;
      tx_buffer[ANS_OA_ADC_VALUE_POS + 1] = 0;
    }
    prepareGetAns(tx_buffer, ANS_ARG_OA_GET_ADC, ANS_LEN_OA_GET_ADC,
                  ANS_OA_GET_ADC_LEN);
    return true;
  }
  return false;
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_get_all_adc_value() {
  if (checkGetMsgReceived(rx_buffer, ARG_OA_GET_ALL_ADC, LEN_OA_GET_ALL_ADC,
                          OA_GET_ADC_ALL_LEN)) {

    const int s = ANS_OA_ADC_GET_ALL_VALUE_POS;
    for (int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {

      uint16_t value = 0;
      if (adc[ch].mov_average_req > 0) {
        value = (uint16_t)adc[ch].average;

      } else {
        value = adc[ch].conversion;
      }

      tx_buffer[s + 2 * ch] = (uint8_t)(value & 0xFF);
      tx_buffer[s + 2 * ch + 1] = (uint8_t)((value & 0xFF00) >> 8);
    }

    prepareGetAns(tx_buffer, ANS_ARG_OA_GET_ALL_ADC, ANS_LEN_OA_GET_ALL_ADC,
                  ANS_OA_GET_ADC_ALL_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_set_pwm_value() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_SET_PWM, LEN_OA_SET_PWM,
                          OA_SET_PWM_LEN)) {
    uint8_t ch = rx_buffer[OA_SET_PWM_CHANNEL_POS];

    uint32_t value = rx_buffer[OA_SET_PWM_PERIOD_POS];
    value += (rx_buffer[OA_SET_PWM_PERIOD_POS + 1] << 8);
    value += (rx_buffer[OA_SET_PWM_PERIOD_POS + 2] << 16);
    value += (rx_buffer[OA_SET_PWM_PERIOD_POS + 3] << 24);

    configurePwmPeriod(ch, value);

    value = rx_buffer[OA_SET_PWM_PULSE_POS];
    value += (rx_buffer[OA_SET_PWM_PULSE_POS + 1] << 8);
    value += (rx_buffer[OA_SET_PWM_PULSE_POS + 2] << 16);
    value += (rx_buffer[OA_SET_PWM_PULSE_POS + 3] << 24);

    configurePwmPulse(ch, value);
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_get_di_value() {
  if (checkGetMsgReceived(rx_buffer, ARG_OA_GET_DI, LEN_OA_GET_DI,
                          OA_GET_DI_LEN)) {
    tx_buffer[ANS_OA_GET_DI_VALUE_POS] = digital_ins;
    prepareGetAns(tx_buffer, ANS_ARG_OA_GET_DI, ANS_LEN_OA_GET_DI,
                  ANS_OA_GET_DI_LEN);
    return true;
  }
  return false;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

bool OptaAnalog::parse_set_led() {
  if (checkSetMsgReceived(rx_buffer, ARG_OA_SET_LED, LEN_OA_SET_LED,
                          OA_SET_LED_LEN)) {
    led_status = rx_buffer[OA_SET_LED_VALUE_POS];
    prepareSetAns(tx_buffer, ANS_ARG_OA_ACK, ANS_LEN_OA_ACK, ANS_ACK_OA_LEN);
    return true;
  }
  return false;
}
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int OptaAnalog::parse_rx() {
  /* call base version because here are handled assign
     addresses messages NOTE: this must be done for
     every other expansion type derived from Module */

  int rv = Module::parse_rx();

  if (rv != -1) {
    /* no need for other msg parsing (message parsed in
     * base version) */
    return rv;
  }

  rv = -1;

  /* Parse OPTA ANALOG I2C messages */
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
  Serial.print("*** ANALOG PARSING MESSAGE: ");
#endif

  if (parse_get_adc_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("get   ADC channel");
#endif
#ifdef BP_USE_CRC
    rv = ANS_OA_GET_ADC_LEN_CRC;
#else
    rv = ANS_OA_GET_ADC_LEN;
#endif
  } else if (parse_get_all_adc_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("get   ADC channel (ALL version)");
#endif
#ifdef BP_USE_CRC
    rv = ANS_OA_GET_ADC_ALL_LEN_CRC;
#else
    rv = ANS_OA_GET_ADC_ALL_LEN;
#endif
  } else if (parse_get_rtd_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("get   RTD value");
#endif
#ifdef BP_USE_CRC
    rv = ANS_OA_GET_RTD_LEN_CRC;
#else
    rv = ANS_OA_GET_RTD_LEN;
#endif
  } else if (parse_get_di_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("get   DI value");
#endif
#ifdef BP_USE_CRC
    rv = ANS_OA_GET_DI_LEN_CRC;
#else
    rv = ANS_OA_GET_DI_LEN;
#endif
  } else if (parse_set_led()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("set   LED status");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_setup_adc_channel()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("begin ADC channel");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_set_dac_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("set   DAC value");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_set_rtd_update_rate()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("begin RTD CURRENT");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_set_pwm_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("set PWM value");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_setup_rtd_channel()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("begin RTD channel");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_setup_dac_channel()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("begin DAC channel");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_setup_di_channel()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("begin DI channel");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else if (parse_set_all_dac_value()) {
#if defined DEBUG_SERIAL && defined DEBUG_ANALOG_PARSE_MESSAGE
    Serial.println("set all DAC values");
#endif
    rv = CTRL_ANS_OA_LEN;
  } else {
    Serial.println(" !!! MESSAGE UNKNOWN !!!");
  }
  return rv;
}

#ifdef DEBUG_SERIAL
void print_function(uint8_t v) {
  switch (v) {
  case 0:
    Serial.println("Function: HIGH IMPEDENCE");
    break;
  case 1:
    Serial.println("Function: DAC VOLTAGE [V] -->");
    break;
  case 2:
    Serial.println("Function: DAC CURRENT [I] -->");
    break;
  case 3:
    Serial.println("Function: ADC VOLTAGE [V] <--");
    break;
  case 4:
    Serial.println("Function: ADC CURRENT [I] <-- "
                   "(externally powered)");
    break;
  case 5:
    Serial.println("Function: ADC CURRENT [I] <-- "
                   "(loop powered)");
    break;
  case 6:
    Serial.println("Function: RESISTANCE [R]");
    break;
  case 7:
    Serial.println("Function: DIGITAL INPUT [D] - (logic)");
    break;
  case 8:
    Serial.println("Function: DIGITAL INPUT [D] - "
                   "(loop powered)");
    break;
  }
}

void print_adc_configuration(uint16_t v) {
  if (v & 1) {
    Serial.println("- MUX is 100 ohm resistor");
  } else {
    Serial.println("- MUX is IOP / AGND_SENSE");
  }

  if (v & 4) {
    Serial.println("- PULL DOWN enabled");
  } else {
    Serial.println("- PULL DOWN disabled");
  }

  if (v & 8) {
    Serial.println("- REJECTION disabled");
  } else {
    Serial.println("- REJECTION enabled");
  }

  uint8_t m = (v & (7 << 5)) >> 5;

  if (m == 0) {
    Serial.println("- RANGE 0-10V");
  } else if (m == 1) {
    Serial.println("- RANGE 2.5V RTD and externally powered");
  } else if (m == 2) {
    Serial.println("- RANGE 2.5V loop powered");
  } else if (m == 3) {
    Serial.println("- RANGE 2.5V voltage output mode");
  }
}

void print_adc_control(uint16_t v, int d) {
  int ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 1 : 2;
  if (v & 1) {
    Serial.println("- Ch " + String(ch) + " ADC conversion enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC conversion DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 0 : 3;
  if (v & 2) {
    Serial.println("- Ch " + String(ch) + " ADC conversion enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC conversion DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 6 : 4;
  if (v & 4) {
    Serial.println("- Ch " + String(ch) + " ADC conversion enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC conversion DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 7 : 5;
  if (v & 8) {
    Serial.println("- Ch " + String(ch) + " ADC conversion enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC conversion DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 1 : 2;
  if (v & 16) {
    Serial.println("- Ch " + String(ch) + " ADC diag enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC diag DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 0 : 3;
  if (v & 32) {
    Serial.println("- Ch " + String(ch) + " ADC diag enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC diag DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 6 : 4;
  if (v & 64) {
    Serial.println("- Ch " + String(ch) + " ADC diag enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC diag DISABLED");
  }
  ch = (d == OA_DUMMY_CHANNEL_DEVICE_0) ? 7 : 5;
  if (v & 128) {
    Serial.println("- Ch " + String(ch) + " ADC diag enabled");
  } else {
    Serial.println("- Ch " + String(ch) + " ADC diag DISABLED");
  }

  if (v & (1 << 10)) {
    Serial.println("- DIAGNOSTIC REJECTION enabled");
  } else {
    Serial.println("- DIAGNOSTIC REJECTION DISABLED");
  }

  uint8_t m = (v & (3 << 8)) >> 8;

  if (m == 0) {
    Serial.println("- ADC STOPPED AND POWERED UP");
  } else if (m == 1) {
    Serial.println("- ADC STARTED SINGLE CONVERSION");
  } else if (m == 2) {
    Serial.println("- ADC STARTED CONTINUOS");
  } else if (m == 3) {
    Serial.println("- ADC STOPPED AND POWERED DOWN");
  }
}

void print_output_configuration(uint16_t v) {
  if (v & 1) {
    Serial.println("- OUTPUT Current Limit 7.5mA");
  } else {
    Serial.println("- OUTPUT Current Limit 29mA");
  }
  if (v & 2) {
    Serial.println("- OUTPUT Clear function ENABLED");
  } else {
    Serial.println("- OUTPUT Clear function DISABLED");
  }
  if ((v & (1 << 6)) && (v & (1 << 7) == 0)) {
    Serial.println("- OUTPUT Slave rate ENABLED");
  } else {
    Serial.println("- OUTPUT Slave rate DISABLED");
  }
  uint8_t m = (v & (3 << 2)) >> 2;

  if (m == 0) {
    Serial.println("- OUTPUT Slew rate time step 4 kHz");
  } else if (m == 1) {
    Serial.println("- OUTPUT Slew rate time step 64 kHz");
  } else if (m == 2) {
    Serial.println("- OUTPUT Slew rate time step 150 kHz");
  } else if (m == 3) {
    Serial.println("- OUTPUT Slew rate time step 240 kHz");
  }
  m = (v & (3 << 4)) >> 4;

  if (m == 0) {
    Serial.println("- OUTPUT Slew rate increment 64 bit");
  } else if (m == 1) {
    Serial.println("- OUTPUT Slew rate increment 120 bit");
  } else if (m == 2) {
    Serial.println("- OUTPUT Slew rate increment 500 bit");
  } else if (m == 3) {
    Serial.println("- OUTPUT Slew rate increment 1820 bit");
  }
}

void print_digital_input_configuration(uint16_t v1, uint16_t v2) {
  uint8_t dt = v1 & 0x1F;
  switch (dt) {
  case 0:
    Serial.println("- DIN debounce time NONE");
    break;
  case 1:
    Serial.println("- DIN debounce time 13us");
    break;
  case 2:
    Serial.println("- DIN debounce time 18.7us");
    break;
  case 3:
    Serial.println("- DIN debounce time 24.4us");
    break;
  case 4:
    Serial.println("- DIN debounce time 32.5us");
    break;
  case 5:
    Serial.println("- DIN debounce time 42.3us");
    break;
  case 6:
    Serial.println("- DIN debounce time 56.1us");
    break;
  case 7:
    Serial.println("- DIN debounce time 75.6us");
    break;
  case 8:
    Serial.println("- DIN debounce time 100.8us");
    break;
  case 9:
    Serial.println("- DIN debounce time 130.1us");
    break;
  case 10:
    Serial.println("- DIN debounce time 180.5us");
    break;
  case 11:
    Serial.println("- DIN debounce time 240.6us");
    break;
  case 12:
    Serial.println("- DIN debounce time 320.3us");
    break;
  case 13:
    Serial.println("- DIN debounce time 420.3us");
    break;
  case 14:
    Serial.println("- DIN debounce time 560.2us");
    break;
  case 15:
    Serial.println("- DIN debounce time 750.4us");
    break;
  case 16:
    Serial.println("- DIN debounce time 1ms");
    break;
  case 17:
    Serial.println("- DIN debounce time 1.8ms");
    break;
  case 18:
    Serial.println("- DIN debounce time 2.4ms");
    break;
  case 19:
    Serial.println("- DIN debounce time 3.2ms");
    break;
  case 20:
    Serial.println("- DIN debounce time 4.2ms");
    break;
  case 21:
    Serial.println("- DIN debounce time 5.6ms");
    break;
  case 22:
    Serial.println("- DIN debounce time 7.5ms");
    break;
  case 23:
    Serial.println("- DIN debounce time 10ms");
    break;
  case 24:
    Serial.println("- DIN debounce time 13ms");
    break;
  case 25:
    Serial.println("- DIN debounce time 18ms");
    break;
  case 26:
    Serial.println("- DIN debounce time 18ms");
    break;
  case 27:
    Serial.println("- DIN debounce time 24ms");
    break;
  case 28:
    Serial.println("- DIN debounce time 32ms");
    break;
  case 29:
    Serial.println("- DIN debounce time 42ms");
    break;
  case 30:
    Serial.println("- DIN debounce time 56ms");
    break;
  case 31:
    Serial.println("- DIN debounce time 75ms");
    break;
  }
  if (v1 & 32) {
    Serial.println("- DIN debounce mode INTEGRATOR");
  } else {
    Serial.println("- DIN debounce mode SIMPLE");
  }

  uint8_t isink = (v1 & (0xF << 6)) >> 6;
  Serial.print("- DIN current sink ");
  Serial.print(isink * 120);
  Serial.println("uA");
  if (v1 & 0x1000) {
    Serial.println("- DIN comparator ENABLED");
  } else {
    Serial.println("- DIN comparator DISABLED");
  }
  if (v1 & 0x2000) {
    Serial.println("- DIN comparator output INVERTED");
  } else {
    Serial.println("- DIN comparator output not inverted");
  }
  if (v1 & 0x4000) {
    Serial.println("- DIN input is FILTERED");
  } else {
    Serial.println("- DIN input is not FILTERED");
  }
  if (v2 & 1) {
    Serial.println("- DIN threshold is fixed at 16V");
  } else {
    Serial.println("- DIN threshold moves with VDD");
  }
}

void OptaAnalog::debugDacFunction(uint8_t ch) {
  Serial.print("   -> ");
  uint16_t reg = 0;
  read_reg(0x12, reg, ch);
  print_output_configuration(reg);
}

int OptaAnalog::debugChannelFunction(uint8_t ch) {
  Serial.print(" ########## CHANNEL ");
  Serial.print(ch);
  Serial.print(" ");
  uint16_t reg = 0;
  if (!read_reg(0x01, reg, ch)) {
    Serial.println("ERROR while reading");
  }
  print_function((reg & 0xF));
  return (reg & 0xF);
}
void OptaAnalog::debugAdcConfiguration(uint8_t ch) {
  Serial.print("   -> ");
  uint16_t reg = 0;
  read_reg(0x05, reg, ch);
  print_adc_configuration(reg);
}
void OptaAnalog::debugDiConfiguration(uint8_t ch) {
  Serial.print("   -> ");
  uint16_t r2 = 0;
  uint16_t reg = 0;
  read_reg(0x09, reg, ch);
  read_reg(0x22, r2, get_dummy_channel(ch));
  print_digital_input_configuration(reg, r2);
}

void OptaAnalog::displayOaDebugInformation() {
  /* This function is for debug only and list all the
   * settings for the analog device */
  uint16_t reg = 0;
  Serial.println("\n**********************************"
                 "****************");
  Serial.println("*               DEVICE "
                 "configuration             *");
  Serial.println("************************************"
                 "**************\n");
  for (int i = 0; i < OA_AN_CHANNELS_NUM; i++) {
    int ch_function = debugChannelFunction(i);
    if (ch_function == 0 || ch_function == 3 || ch_function == 4 ||
        ch_function == 5) {
      /* HIGH IMPEDENCE (ADC works) */
      debugAdcConfiguration(i);
    } else if (ch_function == 1 || ch_function == 2) {
      debugDacFunction(i);
    } else if (ch_function == 6) {
      /* resistance */
    } else if (ch_function == 7 || ch_function == 8) {
      debugDiConfiguration(i);
      Serial.println("+++++++++++++++++++++++++++++++++++++++");
      debugAdcConfiguration(i);
    }
  }
#ifdef ARDUINO_OPTA_ANALOG
  read_reg(0x23, reg, OA_DUMMY_CHANNEL_DEVICE_0);
  print_adc_control(reg, OA_DUMMY_CHANNEL_DEVICE_0);
  read_reg(0x23, reg, OA_DUMMY_CHANNEL_DEVICE_1);
  print_adc_control(reg, OA_DUMMY_CHANNEL_DEVICE_1);
#else
  read_reg(0x23, reg, OA_DUMMY_CHANNEL_DEVICE_0);
  print_adc_control(reg, OA_DUMMY_CHANNEL_DEVICE_0);
#endif
}
#endif
#endif
