#pragma once

#include <Arduino.h>
#include <Wire.h>

class I2CDevice {
public:
    I2CDevice(uint8_t address) : 
      _address(address),
      _lastStatus(0) {
    }
protected:
    inline void beginTransmission() const {
        Wire.beginTransmission(_address);
    }

    inline uint8_t lastStatus() const { return _lastStatus; }
  
    inline void write8(uint8_t data) const {
        Wire.write(data);
    }

    inline void write16(uint16_t data) const {
        Wire.write(static_cast<uint8_t>((data >> 8) & 0xFF));
        Wire.write(static_cast<uint8_t>(data & 0xFF));
    }

    inline void write32(uint32_t data) const {
        Wire.write(static_cast<uint8_t>((data >> 24) & 0xFF));
        Wire.write(static_cast<uint8_t>((data >> 16) & 0xFF));
        Wire.write(static_cast<uint8_t>((data >> 8) & 0xFF));
        Wire.write(static_cast<uint8_t>(data & 0xFF));
    }
    
    inline void endTransmission() {
        _lastStatus = Wire.endTransmission();
    }
    
    inline void requestData(uint8_t bytes) const {
        Wire.requestFrom(_address, bytes);
    }
    
    inline uint8_t read8() const {
        return Wire.read();
    }
    
    inline uint16_t read16() const {
        uint16_t value  = static_cast<uint16_t>(Wire.read()) << 8;
        value |= Wire.read();
        return value;
    }
private:
    uint8_t _address;
    uint8_t _lastStatus;
};



class VL53L1X : public I2CDevice {
public:
    enum DistanceMode { Short, Medium, Long, Unknown };

    VL53L1X(uint8_t address);
    bool init(bool io_2v8 = true);
    inline void setTimeout(uint16_t timeout) { _timeout = timeout; }
    inline uint16_t getTimeout() const { return _timeout; }
    bool setDistanceMode(DistanceMode distanceMode);
    inline DistanceMode distanceMode() const { return _distanceMode; }
    bool writeMeasurementTimingBudget(uint32_t budget_us);
    uint32_t readMeasurementTimingBudget();

    /*
     * Start continuous ranging measurements, with the given inter-measurement
     * period in milliseconds determining how often the sensor takes a measurement.
     */    
    void startContinuous(uint32_t period_ms);

    /*
     * Stop continuous measurements
     */
    void stopContinuous();

    /*
     * check if sensor has new reading available
     * assumes interrupt is active low (GPIO_HV_MUX__CTRL bit 4 is 1)
     */
    bool dataReady();

    void updateDSS();
    void readResults();
    void getRangingData();


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



    void changeAddress(uint8_t address);
    uint8_t address() { return _address; }


    uint16_t read(bool blocking = true);

    // check if sensor has new reading available
    // assumes interrupt is active low (GPIO_HV_MUX__CTRL bit 4 is 1)

    bool timeoutOccurred();

private:
    uint8_t _address;
    DistanceMode _distanceMode;
    uint16_t _timeout;

    void writeReg(uint16_t reg, uint8_t value);
    void writeReg16Bit(uint16_t reg, uint16_t value);
    void writeReg32Bit(uint16_t reg, uint32_t value);
    uint8_t readReg(uint16_t reg);
    uint16_t readReg16Bit(uint16_t reg);


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

    bool did_timeout;
    uint16_t timeout_start_ms;

    uint16_t fast_osc_frequency;
    uint16_t osc_calibrate_val;

    bool calibrated;
    uint8_t saved_vhv_init;
    uint8_t saved_vhv_timeout;


    // Record the current time to check an upcoming timeout against
    void startTimeout() { timeout_start_ms = millis(); }

    // Check if timeout is enabled (set to nonzero value) and has expired
    bool checkTimeoutExpired() {return (_timeout > 0) && ((uint16_t)(millis() - timeout_start_ms) > _timeout); }

    void setupManualCalibration();

    uint32_t calcMacroPeriod(uint8_t vcsel_period);

    // Convert count rate from fixed point 9.7 format to float
    float countRateFixedToFloat(uint16_t count_rate_fixed) { return (float)count_rate_fixed / (1 << 7); }
};
