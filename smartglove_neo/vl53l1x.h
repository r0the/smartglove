#pragma once

#include <Arduino.h>

class VL53L1X
{
  public:

    // register addresses from API vl53l1x_register_map.h
    enum regAddr : uint16_t
    {
      SOFT_RESET                                                                 = 0x0000,
      I2C_SLAVE__DEVICE_ADDRESS                                                  = 0x0001,
      OSC_MEASURED__FAST_OSC__FREQUENCY                                          = 0x0006,
      VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND                                      = 0x0008,
      VHV_CONFIG__INIT                                                           = 0x000B,
      ALGO__PART_TO_PART_RANGE_OFFSET_MM                                         = 0x001E,
      MM_CONFIG__OUTER_OFFSET_MM                                                 = 0x0022,
      DSS_CONFIG__TARGET_TOTAL_RATE_MCPS                                         = 0x0024,
      PAD_I2C_HV__EXTSUP_CONFIG                                                  = 0x002E,
      GPIO__TIO_HV_STATUS                                                        = 0x0031,
      SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS                                  = 0x0036,
      SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS                                = 0x0037,
      ALGO__CROSSTALK_COMPENSATION_VALID_HEIGHT_MM                               = 0x0039,
      ALGO__RANGE_IGNORE_VALID_HEIGHT_MM                                         = 0x003E,
      ALGO__RANGE_MIN_CLIP                                                       = 0x003F,
      ALGO__CONSISTENCY_CHECK__TOLERANCE                                         = 0x0040,
      CAL_CONFIG__VCSEL_START                                                    = 0x0047,
      PHASECAL_CONFIG__TIMEOUT_MACROP                                            = 0x004B,
      PHASECAL_CONFIG__OVERRIDE                                                  = 0x004D,
      DSS_CONFIG__ROI_MODE_CONTROL                                               = 0x004F,
      SYSTEM__THRESH_RATE_HIGH                                                   = 0x0050,
      SYSTEM__THRESH_RATE_LOW                                                    = 0x0052,
      DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT                                  = 0x0054,
      DSS_CONFIG__APERTURE_ATTENUATION                                           = 0x0057,
      MM_CONFIG__TIMEOUT_MACROP_A                                                = 0x005A, // added by Pololu for 16-bit accesses
      MM_CONFIG__TIMEOUT_MACROP_B                                                = 0x005C, // added by Pololu for 16-bit accesses
      RANGE_CONFIG__TIMEOUT_MACROP_A                                             = 0x005E, // added by Pololu for 16-bit accesses
      RANGE_CONFIG__VCSEL_PERIOD_A                                               = 0x0060,
      RANGE_CONFIG__TIMEOUT_MACROP_B                                             = 0x0061, // added by Pololu for 16-bit accesses
      RANGE_CONFIG__VCSEL_PERIOD_B                                               = 0x0063,
      RANGE_CONFIG__SIGMA_THRESH                                                 = 0x0064,
      RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS                                = 0x0066,
      RANGE_CONFIG__VALID_PHASE_HIGH                                             = 0x0069,
      SYSTEM__INTERMEASUREMENT_PERIOD                                            = 0x006C,
      SYSTEM__GROUPED_PARAMETER_HOLD_0                                           = 0x0071,
      SYSTEM__SEED_CONFIG                                                        = 0x0077,
      SD_CONFIG__WOI_SD0                                                         = 0x0078,
      SD_CONFIG__WOI_SD1                                                         = 0x0079,
      SD_CONFIG__INITIAL_PHASE_SD0                                               = 0x007A,
      SD_CONFIG__INITIAL_PHASE_SD1                                               = 0x007B,
      SYSTEM__GROUPED_PARAMETER_HOLD_1                                           = 0x007C,
      SD_CONFIG__QUANTIFIER                                                      = 0x007E,
      SYSTEM__SEQUENCE_CONFIG                                                    = 0x0081,
      SYSTEM__GROUPED_PARAMETER_HOLD                                             = 0x0082,
      SYSTEM__INTERRUPT_CLEAR                                                    = 0x0086,
      SYSTEM__MODE_START                                                         = 0x0087,
      RESULT__RANGE_STATUS                                                       = 0x0089,
      PHASECAL_RESULT__VCSEL_START                                               = 0x00D8,
      RESULT__OSC_CALIBRATE_VAL                                                  = 0x00DE,
      FIRMWARE__SYSTEM_STATUS                                                    = 0x00E5,
      IDENTIFICATION__MODEL_ID                                                   = 0x010F,
    };

    enum DistanceMode { Short, Medium, Long, Unknown };

    enum RangeStatus : uint8_t
    {
      RangeValid                =   0,

      // "sigma estimator check is above the internal defined threshold"
      // (sigma = standard deviation of measurement)
      SigmaFail                 =   1,

      // "signal value is below the internal defined threshold"
      SignalFail                =   2,

      // "Target is below minimum detection threshold."
      RangeValidMinRangeClipped =   3,

      // "phase is out of bounds"
      // (nothing detected in range; try a longer distance mode if applicable)
      OutOfBoundsFail           =   4,

      // "HW or VCSEL failure"
      HardwareFail              =   5,

      // "The Range is valid but the wraparound check has not been done."
      RangeValidNoWrapCheckFail =   6,

      // "Wrapped target, not matching phases"
      // "no matching phase in other VCSEL period timing."
      WrapTargetFail            =   7,

      // "Internal algo underflow or overflow in lite ranging."
   // ProcessingFail            =   8: not used in API

      // "Specific to lite ranging."
      // should never occur with this lib (which uses low power auto ranging,
      // as the API does)
      XtalkSignalFail           =   9,

      // "1st interrupt when starting ranging in back to back mode. Ignore
      // data."
      // should never occur with this lib
      SynchronizationInt         =  10, // (the API spells this "syncronisation")

      // "All Range ok but object is result of multiple pulses merging together.
      // Used by RQL for merged pulse detection"
   // RangeValid MergedPulse    =  11: not used in API

      // "Used by RQL as different to phase fail."
   // TargetPresentLackOfSignal =  12:

      // "Target is below minimum detection threshold."
      MinRangeFail              =  13,

      // "The reported range is invalid"
   // RangeInvalid              =  14: can't actually be returned by API (range can never become negative, even after correction)

      // "No Update."
      None                      = 255,
    };

    struct RangingData
    {
      uint16_t range_mm;
      RangeStatus range_status;
      float peak_signal_count_rate_MCPS;
      float ambient_count_rate_MCPS;
    };

    RangingData ranging_data;

    uint8_t last_status; // status of last I2C transmission

    VL53L1X();

    void setAddress(uint8_t new_addr);
    uint8_t getAddress() { return address; }

    bool init(bool io_2v8 = true);

    void writeReg(uint16_t reg, uint8_t value);
    void writeReg16Bit(uint16_t reg, uint16_t value);
    void writeReg32Bit(uint16_t reg, uint32_t value);
    uint8_t readReg(regAddr reg);
    uint16_t readReg16Bit(uint16_t reg);
    uint32_t readReg32Bit(uint16_t reg);

    bool setDistanceMode(DistanceMode mode);
    DistanceMode getDistanceMode() { return distance_mode; }

    bool setMeasurementTimingBudget(uint32_t budget_us);
    uint32_t getMeasurementTimingBudget();

    void startContinuous(uint32_t period_ms);
    void stopContinuous();
    uint16_t read(bool blocking = true);
    uint16_t readRangeContinuousMillimeters(bool blocking = true) { return read(blocking); } // alias of read()

    // check if sensor has new reading available
    // assumes interrupt is active low (GPIO_HV_MUX__CTRL bit 4 is 1)
    bool dataReady() { return (readReg(GPIO__TIO_HV_STATUS) & 0x01) == 0; }

    static const char * rangeStatusToString(RangeStatus status);

    void setTimeout(uint16_t timeout) { io_timeout = timeout; }
    uint16_t getTimeout() { return io_timeout; }
    bool timeoutOccurred();

  private:

    // The Arduino two-wire interface uses a 7-bit number for the address,
    // and sets the last bit correctly based on reads and writes
    static const uint8_t AddressDefault = 0x29;

    // value used in measurement timing budget calculations
    // assumes PresetMode is LOWPOWER_AUTONOMOUS
    //
    // vhv = LOWPOWER_AUTO_VHV_LOOP_DURATION_US + LOWPOWERAUTO_VHV_LOOP_BOUND
    //       (tuning parm default) * LOWPOWER_AUTO_VHV_LOOP_DURATION_US
    //     = 245 + 3 * 245 = 980
    // TimingGuard = LOWPOWER_AUTO_OVERHEAD_BEFORE_A_RANGING +
    //               LOWPOWER_AUTO_OVERHEAD_BETWEEN_A_B_RANGING + vhv
    //             = 1448 + 2100 + 980 = 4528
    static const uint32_t TimingGuard = 4528;

    // value in DSS_CONFIG__TARGET_TOTAL_RATE_MCPS register, used in DSS
    // calculations
    static const uint16_t TargetRate = 0x0A00;

    // for storing values read from RESULT__RANGE_STATUS (0x0089)
    // through RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_LOW
    // (0x0099)
    struct ResultBuffer
    {
      uint8_t range_status;
    // uint8_t report_status: not used
      uint8_t stream_count;
      uint16_t dss_actual_effective_spads_sd0;
   // uint16_t peak_signal_count_rate_mcps_sd0: not used
      uint16_t ambient_count_rate_mcps_sd0;
   // uint16_t sigma_sd0: not used
   // uint16_t phase_sd0: not used
      uint16_t final_crosstalk_corrected_range_mm_sd0;
      uint16_t peak_signal_count_rate_crosstalk_corrected_mcps_sd0;
    };

    // making this static would save RAM for multiple instances as long as there
    // aren't multiple sensors being read at the same time (e.g. on separate
    // I2C buses)
    ResultBuffer results;

    uint8_t address;

    uint16_t io_timeout;
    bool did_timeout;
    uint16_t timeout_start_ms;

    uint16_t fast_osc_frequency;
    uint16_t osc_calibrate_val;

    bool calibrated;
    uint8_t saved_vhv_init;
    uint8_t saved_vhv_timeout;

    DistanceMode distance_mode;

    // Record the current time to check an upcoming timeout against
    void startTimeout() { timeout_start_ms = millis(); }

    // Check if timeout is enabled (set to nonzero value) and has expired
    bool checkTimeoutExpired() {return (io_timeout > 0) && ((uint16_t)(millis() - timeout_start_ms) > io_timeout); }

    void setupManualCalibration();
    void readResults();
    void updateDSS();
    void getRangingData();

    static uint32_t decodeTimeout(uint16_t reg_val);
    static uint16_t encodeTimeout(uint32_t timeout_mclks);
    static uint32_t timeoutMclksToMicroseconds(uint32_t timeout_mclks, uint32_t macro_period_us);
    static uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_us, uint32_t macro_period_us);
    uint32_t calcMacroPeriod(uint8_t vcsel_period);

    // Convert count rate from fixed point 9.7 format to float
    float countRateFixedToFloat(uint16_t count_rate_fixed) { return (float)count_rate_fixed / (1 << 7); }
};
