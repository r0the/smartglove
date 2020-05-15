#pragma once

#include <Arduino.h>

class VL53L1X
{
  public:


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
    uint8_t readReg(uint16_t reg);
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
    bool dataReady();
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
