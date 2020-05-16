/*
 * Copyright (C) 2020 by Stefan Rothe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "i2cdevice.h"

class VL53L1X : public I2CDevice {
public:
    enum DistanceMode { Short, Medium, Long, Unknown };

    VL53L1X(uint8_t address);
    bool init(bool io_2v8 = true);
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

    uint16_t readInput();

    void changeAddress(uint8_t address);
    uint8_t address() { return _address; }

private:
    uint8_t _address;
    uint16_t _ambientCountRateMCPS;
    DistanceMode _distanceMode;
    uint16_t _oscFastFrequency;
    uint16_t _oscCalibrateVal;
    uint16_t _peakSignalCountRateMCPS;
    uint32_t _rangeMM;
    bool _measurementValid;
    uint8_t _rangeStatus;
    uint16_t _spadCount;
    uint8_t _streamCount;
    

    void writeReg(uint16_t reg, uint8_t value);
    void writeReg16Bit(uint16_t reg, uint16_t value);
    void writeReg32Bit(uint16_t reg, uint32_t value);
    uint8_t readReg(uint16_t reg);
    uint16_t readReg16Bit(uint16_t reg);

    bool calibrated;
    uint8_t saved_vhv_init;
    uint8_t saved_vhv_timeout;

    void updateDSS();
    void readResults();

    void setupManualCalibration();
    
    uint32_t calcMacroPeriod(uint8_t vcsel_period);
};
