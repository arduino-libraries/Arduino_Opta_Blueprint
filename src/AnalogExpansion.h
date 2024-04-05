/* -------------------------------------------------------------------------- */
/* FILE NAME:   AnalogExpansion.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240208
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef ANALOGEXPANSION
#define ANALOGEXPANSION
#include "AnalogExpansionAddress.h"
#include "AnalogExpansionCfg.h"
#include "CommonCfg.h"
#include "Expansion.h"
#include <cstdint>
#include <stdint.h>

namespace Opta {

class AnalogExpansion : public Expansion {
public:
  AnalogExpansion();
  ~AnalogExpansion();
  AnalogExpansion(Expansion &other);

  /*
   * Begin channel functions
   */

  /* ----------------------------- As ADC ------------------------------ */
  /* Set the function for the channel 'ch' to ADC
   * the parameter 'type' select the kind of ADC:
   * - OA_VOLTAGE_ADC -> voltage measurement
   * - OA_CURRENT_ADC -> current measurement
   * depending on this parameter some configuration of ADC are "blocked":
   * 1. OA_VOLTAGE_ADC
   *    - MUX is between IOP and AGND_SENSE
   *    - RANGE is 0-10V
   *    - PULL DOWN is configurable via pull down parameter
   * 2. OA_CURRENT_ADC
   *    - MUX is on the 100R resistor
   *    - RANGE is 0-2.5V volt externally powered
   *    - PULL DOWN is DISABLE (the parameter 'pull_down' is disregarded)
   *
   * Note: in case OA_ADC_NOT_USED is used as 'type' parameter this function
   * does nothing (since it makes no sense to "begin" a channel as ADC and tell
   * the ADC is not used). This value is used when the channel is configured for
   * "other" than ADC (typically DAC or DI) and the ADC function is used "in
   * parallel" to the primary function to say there is no need of additional ADC
   * configuration*/
  void beginChannelAsAdc(uint8_t ch, OaAdcType_t type, bool pull_down,
                         bool rejection, bool diagnostic, uint8_t ma);
  void addAdcOnChannel(uint8_t ch, OaAdcType_t type, bool pull_down,
                       bool rejection, bool diagnostic, uint8_t ma);
  void addVoltageAdcOnChannel(uint8_t ch);
  void addCurrentAdcOnChannel(uint8_t ch);
  static void addAdcOnChannel(Controller &ctrl, uint8_t device, uint8_t ch,
                              OaAdcType_t type, bool pull_down, bool rejection,
                              bool diagnostic, uint8_t ma);

  static void addVoltageAdcOnChannel(Controller &ctrl, uint8_t device,
                                     uint8_t ch);
  static void addCurrentAdcOnChannel(Controller &ctrl, uint8_t device,
                                     uint8_t ch);
  void beginChannelAsVoltageAdc(uint8_t ch);
  void beginChannelAsCurrentAdc(uint8_t ch);

  static void beginChannelAsAdc(Controller &ctrl, uint8_t device, uint8_t ch,
                                OaAdcType_t type, bool pull_down,
                                bool rejection, bool diagnostic, uint8_t ma);
  static void beginChannelAsVoltageAdc(Controller &ctrl, uint8_t device,
                                       uint8_t ch);
  static void beginChannelAsCurrentAdc(Controller &ctrl, uint8_t device,
                                       uint8_t ch);
  /* ------------------------  As Digital input ------------------------- */
  /* Digital input setup function:
   * - filter: true -> use low pass filtered input, false -> use unfiltered
   * input
   * - invert: true -> invert the logic of the comparator (high input means
   * low output)
   * - simple_deb -> true -> use a simple counter as debounce filter, false
   * -> use an integrative algorithm as debounce filter
   * - sink_cur -> set the sink current sunk by the DI (OA_DI_SINK_0 - no
   * current
   *   ---> OA_DI_SINK_31 = 1.8 mA)
   * - deb_time -> see enumerative comments for timing
   * - scale -> true the comparator is scaled with Vcc, false -> the
   *   comparator use "fixed" voltage 16 V
   * - th -> threshold to be used by the comparator
   * - Vcc -> the actual Vcc in the scale is true (this is disregarded if
   * scale is false)
   */
  void beginChannelAsDigitalInput(uint8_t ch, bool filter, bool invert,
                                  bool simple_deb, OaDiSinkCurrent_t sink_cur,
                                  OaDebounceTime_t deb_time, bool scale,
                                  float th, float Vcc);
  static void beginChannelAsDigitalInput(Controller &ctrl, uint8_t device,
                                         uint8_t ch, bool filter, bool invert,
                                         bool simple_deb,
                                         OaDiSinkCurrent_t sink_cur,
                                         OaDebounceTime_t deb_time, bool scale,
                                         float th, float Vcc);
  void beginChannelAsDigitalInput(uint8_t ch);
  static void beginChannelAsDigitalInput(Controller &ctrl, uint8_t device,
                                         uint8_t ch);
  /* ----------------------------- As RTD -------------------------------- */
  void beginChannelAsRtd(uint8_t ch, bool use_3_wires, float current);
  static void beginChannelAsRtd(Controller &ctrl, uint8_t device, uint8_t ch,
                                bool use_3_wires, float current);

  /* since RTD is a slow function (especially in case of 3 wire) this function
   * allow the user to set the update time (in ms) i.e. the time the RTD values
   * are updated */
  void beginRtdUpdateTime(uint16_t time);
  static void beginRtdUpdateTime(Controller &ctrl, uint8_t device,
                                 uint16_t time);
  /* ------------------------------ As DAC ------------------------------- */
  void beginChannelAsDac(uint8_t ch, OaDacType_t type, bool limit_current,
                         bool enable_slew, OaDacSlewRate_t sr);
  static void beginChannelAsDac(Controller &ctrl, uint8_t device, uint8_t ch,
                                OaDacType_t type, bool limit_current,
                                bool enable_slew, OaDacSlewRate_t sr);
  void beginChannelAsVoltageDac(uint8_t ch);
  void beginChannelAsCurrentDac(uint8_t ch);
  static void beginChannelAsVoltageDac(Controller &ctrl, uint8_t device,
                                       uint8_t ch);
  static void beginChannelAsCurrentDac(Controller &ctrl, uint8_t device,
                                       uint8_t ch);

  void beginChannelAsHighImpedance(uint8_t ch);

  /*
   * Set /get channel functions
   */
  /* period and pulse in micro seconds */
  void setPwm(uint8_t ch, uint32_t period, uint32_t pulse);
  /* get adc converter bits of channel ch (if ch is configured as ADC)*/
  uint16_t getAdc(uint8_t ch, bool update = true);
  /* get adc value as Volts (if ch is configured as voltage ADC, otherwise 0.0)
   */
  float pinVoltage(uint8_t ch, bool update = true);
  /* get adc value as milli Ampere (if ch is configured as current ADC,
   * otherwise 0.0)*/
  float pinCurrent(uint8_t ch, bool update = true);
  /* set dac converter bits of channels ch (if ch is configured as DAC) */
  void setDac(uint8_t ch, uint16_t value, bool update = true);
  /* set the dac as Volts (range 0-11V) if ch is configured as voltage DAC */
  void pinVoltage(uint8_t ch, float voltage, bool update = true);
  /* set the dac as milli Amperes (range 0-25 mA) if ch is configured as current
   * DAC*/
  void pinCurrent(uint8_t ch, float current, bool update = true);
  /* get RTD value as Ohms */
  float getRtd(uint8_t ch);
  void switchLedOn(uint8_t pin, bool update = true);
  void switchLedOff(uint8_t pin, bool update = true);
  void updateLeds();

  /* if ch is configured as DAC the value (max value 8191) is set as
   * DAC converter bits (period is disregarded in this case)
   * if ch is a PWM channel then value is pulse and period is period as
   * for the setPwm function */
  void analogWrite(uint8_t ch, uint32_t value, uint32_t period = 0,
                   bool update = true);

  PinStatus digitalRead(int pin, bool update = false);
  int analogRead(int pin, bool update = true);

  void updateDigitalInputs();
  void updateAnalogInputs();
  void updateAnalogOutputs();

  unsigned int execute(uint32_t what) override;
  void write(unsigned int address, unsigned int value) override;
  bool read(unsigned int address, unsigned int &value) override;

  /* static start up function, this function must contains the code
   * needed to initialize the harware correctly for ALL the possible
   * expansions present (up to 10)
   * This is done to support hot plug, as soon the expansions are
   * discovered by the controller, the controller itself will call
   * this function as a callback set up correctly the expansion */
  static void startUp(Controller *ptr);

  void setProductData(uint8_t *data, uint8_t len);

  bool isChDac(uint8_t ch);
  bool isChAdc(uint8_t ch);
  bool isChVoltageDac(uint8_t ch);
  bool isChCurrentDac(uint8_t ch);
  bool isChVoltageAdc(uint8_t ch);
  bool isChCurrentAdc(uint8_t ch);
  bool isChDigitalInput(uint8_t ch);
  bool isChRtd(uint8_t ch);
  bool isChHighImpedance(uint8_t ch);

protected:
  bool verify_address(unsigned int add) override;
  unsigned int i2c_transaction(uint8_t (AnalogExpansion::*prepare)(),
                               bool (AnalogExpansion::*parse)(), int rx_bytes);

  static OaChannelCfg cfgs[OPTA_CONTROLLER_MAX_EXPANSION_NUM];

  uint8_t msg_begin_adc();
  uint8_t msg_begin_di();
  uint8_t msg_begin_dac();
  uint8_t msg_begin_rtd();
  uint8_t msg_set_rtd_time();
  uint8_t msg_begin_high_imp();
  bool parse_oa_ack();
  bool adc_registers_defined();
  bool di_registers_defined();
  bool dac_registers_defined();
  bool rtd_registers_defined();

  uint8_t msg_set_pwm();
  uint8_t msg_get_adc();
  bool parse_ans_get_adc();
  uint8_t msg_set_dac();
  uint8_t msg_get_rtd();
  bool parse_ans_get_rtd();
  uint8_t msg_get_di();
  bool parse_ans_get_di();
  uint8_t msg_set_led();
  uint8_t msg_set_all_dac();

  uint8_t msg_get_all_ai();
  bool parse_ans_get_all_ai();
};

} // namespace Opta
#endif // ARDUINO_OPTA
