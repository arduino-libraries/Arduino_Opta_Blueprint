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
#include "AnalogCommonCfg.h"
#include "AnalogExpansionAddress.h"
#include "AnalogExpansionCfg.h"
#include "OptaExpansion.h"
#include <cstdint>
#include <stdint.h>

namespace Opta {

typedef enum {
  OA_ADC_RANGE_10V,
  OA_ADC_RANGE_2_5V,
  OA_ADC_RANGE_2_5V_SELF_POWERED,
  OA_ADC_RANGE_2_5_BIPOLAR
} OaAdcRange_t;

typedef enum {
  OA_SLEW_RATE_0,  /* max vertical step 64 bit, update rate 4 kHz */
  OA_SLEW_RATE_1,  /* max vertical step 120 bit, update rate 4 kHz */
  OA_SLEW_RATE_2,  /* max vertical step 500 bit, update rate 4 kHz */
  OA_SLEW_RATE_3,  /* max vertical step 1820 bit, update rate 4 kHz */
  OA_SLEW_RATE_4,  /* max vertical step 64 bit, update rate 64 kHz */
  OA_SLEW_RATE_5,  /* max vertical step 120 bit, update rate 64 kHz */
  OA_SLEW_RATE_6,  /* max vertical step 500 bit, update rate 64 kHz */
  OA_SLEW_RATE_7,  /* max vertical step 1280 bit, update rate 64 kHz */
  OA_SLEW_RATE_8,  /* max vertical step 64 bit, update rate 150 kHz */
  OA_SLEW_RATE_9,  /* max vertical step 120 bit, update rate 150 kHz */
  OA_SLEW_RATE_10, /* max vertical step 500 bit, update rate 150 kHz */
  OA_SLEW_RATE_11, /* max vertical step 1280 bit, update rate 150 kHz */
  OA_SLEW_RATE_12, /* max vertical step 64 bit, update rate 240 kHz */
  OA_SLEW_RATE_13, /* max vertical step 120 bit, update rate 240 kHz */
  OA_SLEW_RATE_14, /* max vertical step 500 bit, update rate 240 kHz */
  OA_SLEW_RATE_15  /* max vertical step 1280 bit, update rate 240 kHz */
} OaDacSlewRate_t;

typedef enum {
  OA_DI_DEB_TIME_0,  /* No debounce time */
  OA_DI_DEB_TIME_1,  /* 0.0130 ms */
  OA_DI_DEB_TIME_2,  /* 0.0187 ms */
  OA_DI_DEB_TIME_3,  /* 0.0244 ms */
  OA_DI_DEB_TIME_4,  /* 0.0325 ms */
  OA_DI_DEB_TIME_5,  /* 0.0423 ms */
  OA_DI_DEB_TIME_6,  /* 0.0561 ms */
  OA_DI_DEB_TIME_7,  /* 0.0756 ms */
  OA_DI_DEB_TIME_8,  /* 0.1008 ms */
  OA_DI_DEB_TIME_9,  /* 0.1301 ms */
  OA_DI_DEB_TIME_10, /* 0.1805 ms */
  OA_DI_DEB_TIME_11, /* 0.2406 ms */
  OA_DI_DEB_TIME_12, /* 0.3203 ms */
  OA_DI_DEB_TIME_13, /* 0.4203 ms */
  OA_DI_DEB_TIME_14, /* 0.5602 ms */
  OA_DI_DEB_TIME_15, /* 0.7504 ms */
  OA_DI_DEB_TIME_16, /* 1.0008 ms */
  OA_DI_DEB_TIME_17, /* 1.3008 ms */
  OA_DI_DEB_TIME_18, /* 1.8008 ms */
  OA_DI_DEB_TIME_19, /* 2.4008 ms */
  OA_DI_DEB_TIME_20, /* 3.2008 ms */
  OA_DI_DEB_TIME_21, /* 4.2008 ms */
  OA_DI_DEB_TIME_22, /* 5.6008 ms */
  OA_DI_DEB_TIME_23, /* 7.5007 ms */
  OA_DI_DEB_TIME_24, /* 10.0007 ms */
  OA_DI_DEB_TIME_25, /* 13.0007 ms */
  OA_DI_DEB_TIME_26, /* 18.0007 ms */
  OA_DI_DEB_TIME_27, /* 24.0006 ms */
  OA_DI_DEB_TIME_28, /* 32.0005 ms */
  OA_DI_DEB_TIME_29, /* 42.0004 ms */
  OA_DI_DEB_TIME_30, /* 56.0003 ms */
  OA_DI_DEB_TIME_31, /* 75.0000 ms */
} OaDebounceTime_t;
/* step of 120 uA (although the manual says from 0 to 1.8mA in 0.120 uA steps
 * but with 4 bytes...)*/
typedef enum {
  OA_DI_SINK_0, /* No sink current */
  OA_DI_SINK_1, /* 120 uA */
  OA_DI_SINK_2,
  OA_DI_SINK_3,
  OA_DI_SINK_4,
  OA_DI_SINK_5,
  OA_DI_SINK_6,
  OA_DI_SINK_7,
  OA_DI_SINK_8,
  OA_DI_SINK_9,
  OA_DI_SINK_10,
  OA_DI_SINK_11,
  OA_DI_SINK_12,
  OA_DI_SINK_13,
  OA_DI_SINK_14,
  OA_DI_SINK_15,
  OA_DI_SINK_16,
  OA_DI_SINK_17,
  OA_DI_SINK_18,
  OA_DI_SINK_19,
  OA_DI_SINK_20,
  OA_DI_SINK_21,
  OA_DI_SINK_22,
  OA_DI_SINK_23,
  OA_DI_SINK_24,
  OA_DI_SINK_25,
  OA_DI_SINK_26,
  OA_DI_SINK_27,
  OA_DI_SINK_28,
  OA_DI_SINK_29,
  OA_DI_SINK_30,
  OA_DI_SINK_31, /* 1.8 mA */
} OaDiSinkCurrent_t;

class AnalogExpansion : public Expansion {
public:
  AnalogExpansion();
  ~AnalogExpansion();
  AnalogExpansion(Expansion &other);

  static Expansion *makeExpansion();
  static std::string getProduct();
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
  /* get Pwm period in micro seconds */
  uint32_t getPwmPeriod(uint8_t ch);
  /* get Pwm pulse in micro seconds */
  uint32_t getPwmPulse(uint8_t ch);

  float getPwmFreqHz(uint8_t ch);
  float getPwmPulsePerc(uint8_t ch);

  /* get adc converter bits of channel ch (if ch is configured as ADC)*/
  uint16_t getAdc(uint8_t ch, bool update = true);

  /* - if the pin is a VOLTAGE DAC (output) returns the Voltage set as output (but not 
     directly measured) 
     - if the pin is a VOLTAGE ADC (input) returns the Voltage measure by the Adc
     - if the pin is a CURRENT DAC with addition VOLTAGE ADC it returns the Voltage
     measured by the ADC
     - 0.0 otherwise

     Value is intended in Volts
   */
  float pinVoltage(uint8_t ch, bool update = true);

   /* - if the pin is a CURRENT DAC (output) returns the VCurrent set as output (but not 
     directly measured) 
     - if the pin is a CURRENT ADC (input) returns the Current measured by the Adc
     - if the pin is a VOLTAGE DAC with addition VOLTAGE ADC it returns the Current
     measured by the ADC
     - 0.0 otherwise

     Value is intended in milli Ampere
   */
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
  bool isChRtd2Wires(uint8_t ch);
  bool isChRtd3Wires(uint8_t ch);
  bool isChHighImpedance(uint8_t ch);

protected:
  bool verify_address(unsigned int add) override;
  

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
