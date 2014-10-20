/*
ADXL345.cpp - Class file for the ADXL345 Triple Axis Accelerometer Arduino Library.

Version: 1.1.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include "ADXL345.h"

bool ADXL345::begin()
{
    f.XAxis = 0;
    f.YAxis = 0;
    f.ZAxis = 0;

    Wire.begin();

    // Check ADXL345 REG DEVID
    if (fastRegister8(ADXL345_REG_DEVID) != 0xE5)
    {
        return false;
    }

    // Enable measurement mode (0b00001000)
    writeRegister8(ADXL345_REG_POWER_CTL, 0x08);

    clearSettings();

    return true;
}

// Set Range
void ADXL345::setRange(adxl345_range_t range)
{
  // Get actual value register
  uint8_t value = readRegister8(ADXL345_REG_DATA_FORMAT);

  // Update the data rate
  // (&) 0b11110000 (0xF0 - Leave HSB)
  // (|) 0b0000xx?? (range - Set range)
  // (|) 0b00001000 (0x08 - Set Full Res)
  value &= 0xF0;
  value |= range;
  value |= 0x08;

  writeRegister8(ADXL345_REG_DATA_FORMAT, value);
}

// Get Range
adxl345_range_t ADXL345::getRange(void)
{
    return (adxl345_range_t)(readRegister8(ADXL345_REG_DATA_FORMAT) & 0x03);
}

// Set Data Rate
void ADXL345::setDataRate(adxl345_dataRate_t dataRate)
{
    writeRegister8(ADXL345_REG_BW_RATE, dataRate);
}

// Get Data Rate
adxl345_dataRate_t ADXL345::getDataRate(void)
{
    return (adxl345_dataRate_t)(readRegister8(ADXL345_REG_BW_RATE) & 0x0F);
}

// Low Pass Filter
Vector ADXL345::lowPassFilter(Vector vector, float alpha)
{
    f.XAxis = vector.XAxis * alpha + (f.XAxis * (1.0 - alpha));
    f.YAxis = vector.YAxis * alpha + (f.YAxis * (1.0 - alpha));
    f.ZAxis = vector.ZAxis * alpha + (f.ZAxis * (1.0 - alpha));
    return f;
}

// Read raw values
Vector ADXL345::readRaw(void)
{
    r.XAxis = readRegister16(ADXL345_REG_DATAX0);
    r.YAxis = readRegister16(ADXL345_REG_DATAY0);
    r.ZAxis = readRegister16(ADXL345_REG_DATAZ0);
    return r;
}

// Read normalized values
Vector ADXL345::readNormalize(float gravityFactor)
{
    readRaw();

    // (4 mg/LSB scale factor in Full Res) * gravity factor
    n.XAxis = r.XAxis * 0.004 * gravityFactor;
    n.YAxis = r.YAxis * 0.004 * gravityFactor;
    n.ZAxis = r.ZAxis * 0.004 * gravityFactor;

    return n;
}

// Read scaled values
Vector ADXL345::readScaled(void)
{
    readRaw();

    // (4 mg/LSB scale factor in Full Res)
    n.XAxis = r.XAxis * 0.004;
    n.YAxis = r.YAxis * 0.004;
    n.ZAxis = r.ZAxis * 0.004;

    return n;
}

void ADXL345::clearSettings(void)
{
    setRange(ADXL345_RANGE_2G);
    setDataRate(ADXL345_DATARATE_100HZ);

    writeRegister8(ADXL345_REG_THRESH_TAP, 0x00);
    writeRegister8(ADXL345_REG_DUR, 0x00);
    writeRegister8(ADXL345_REG_LATENT, 0x00);
    writeRegister8(ADXL345_REG_WINDOW, 0x00);
    writeRegister8(ADXL345_REG_THRESH_ACT, 0x00);
    writeRegister8(ADXL345_REG_THRESH_INACT, 0x00);
    writeRegister8(ADXL345_REG_TIME_INACT, 0x00);
    writeRegister8(ADXL345_REG_THRESH_FF, 0x00);
    writeRegister8(ADXL345_REG_TIME_FF, 0x00);

    uint8_t value;

    value = readRegister8(ADXL345_REG_ACT_INACT_CTL);
    value &= 0b10001000;
    writeRegister8(ADXL345_REG_ACT_INACT_CTL, value);

    value = readRegister8(ADXL345_REG_TAP_AXES);
    value &= 0b11111000;
    writeRegister8(ADXL345_REG_TAP_AXES, value);
}

// Set Tap Threshold (62.5mg / LSB)
void ADXL345::setTapThreshold(float threshold)
{
    uint8_t scaled = constrain(threshold / 0.0625f, 0, 255);
    writeRegister8(ADXL345_REG_THRESH_TAP, scaled);
}

// Get Tap Threshold (62.5mg / LSB)
float ADXL345::getTapThreshold(void)
{
    return readRegister8(ADXL345_REG_THRESH_TAP) * 0.0625f;
}

// Set Tap Duration (625us / LSB)
void ADXL345::setTapDuration(float duration)
{
    uint8_t scaled = constrain(duration / 0.000625f, 0, 255);
    writeRegister8(ADXL345_REG_DUR, scaled);
}

// Get Tap Duration (625us / LSB)
float ADXL345::getTapDuration(void)
{
    return readRegister8(ADXL345_REG_DUR) * 0.000625f;
}

// Set Double Tap Latency (1.25ms / LSB)
void ADXL345::setDoubleTapLatency(float latency)
{
    uint8_t scaled = constrain(latency / 0.00125f, 0, 255);
    writeRegister8(ADXL345_REG_LATENT, scaled);
}

// Get Double Tap Latency (1.25ms / LSB)
float ADXL345::getDoubleTapLatency()
{
    return readRegister8(ADXL345_REG_LATENT) * 0.00125f;
}

// Set Double Tap Window (1.25ms / LSB)
void ADXL345::setDoubleTapWindow(float window)
{
    uint8_t scaled = constrain(window / 0.00125f, 0, 255);
    writeRegister8(ADXL345_REG_WINDOW, scaled);
}

// Get Double Tap Window (1.25ms / LSB)
float ADXL345::getDoubleTapWindow(void)
{
    return readRegister8(ADXL345_REG_WINDOW) * 0.00125f;
}

// Set Activity Threshold (62.5mg / LSB)
void ADXL345::setActivityThreshold(float threshold)
{
    uint8_t scaled = constrain(threshold / 0.0625f, 0, 255);
    writeRegister8(ADXL345_REG_THRESH_ACT, scaled);
}

// Get Activity Threshold (65.5mg / LSB)
float ADXL345::getActivityThreshold(void)
{
    return readRegister8(ADXL345_REG_THRESH_ACT) * 0.0625f;
}

// Set Inactivity Threshold (65.5mg / LSB)
void ADXL345::setInactivityThreshold(float threshold)
{
    uint8_t scaled = constrain(threshold / 0.0625f, 0, 255);
    writeRegister8(ADXL345_REG_THRESH_INACT, scaled);
}

// Get Incactivity Threshold (65.5mg / LSB)
float ADXL345::getInactivityThreshold(void)
{
    return readRegister8(ADXL345_REG_THRESH_INACT) * 0.0625f;
}

// Set Inactivity Time (s / LSB)
void ADXL345::setTimeInactivity(uint8_t time)
{
    writeRegister8(ADXL345_REG_TIME_INACT, time);
}

// Get Inactivity Time (s / LSB)
uint8_t ADXL345::getTimeInactivity(void)
{
    return readRegister8(ADXL345_REG_TIME_INACT);
}

// Set Free Fall Threshold (65.5mg / LSB)
void ADXL345::setFreeFallThreshold(float threshold)
{
    uint8_t scaled = constrain(threshold / 0.0625f, 0, 255);
    writeRegister8(ADXL345_REG_THRESH_FF, scaled);
}

// Get Free Fall Threshold (65.5mg / LSB)
float ADXL345::getFreeFallThreshold(void)
{
    return readRegister8(ADXL345_REG_THRESH_FF) * 0.0625f;
}

// Set Free Fall Duratiom (5ms / LSB)
void ADXL345::setFreeFallDuration(float duration)
{
    uint8_t scaled = constrain(duration / 0.005f, 0, 255);
    writeRegister8(ADXL345_REG_TIME_FF, scaled);
}

// Get Free Fall Duratiom
float ADXL345::getFreeFallDuration()
{
    return readRegister8(ADXL345_REG_TIME_FF) * 0.005f;
}

void ADXL345::setActivityX(bool state)
{
    writeRegisterBit(ADXL345_REG_ACT_INACT_CTL, 6, state);
}

bool ADXL345::getActivityX(void)
{
    return readRegisterBit(ADXL345_REG_ACT_INACT_CTL, 6);
}

void ADXL345::setActivityY(bool state)
{
    writeRegisterBit(ADXL345_REG_ACT_INACT_CTL, 5, state);
}

bool ADXL345::getActivityY(void)
{
    return readRegisterBit(ADXL345_REG_ACT_INACT_CTL, 5);
}

void ADXL345::setActivityZ(bool state)
{
    writeRegisterBit(ADXL345_REG_ACT_INACT_CTL, 4, state);
}

bool ADXL345::getActivityZ(void)
{
    return readRegisterBit(ADXL345_REG_ACT_INACT_CTL, 4);
}

void ADXL345::setActivityXYZ(bool state)
{
    uint8_t value;

    value = readRegister8(ADXL345_REG_ACT_INACT_CTL);

    if (state)
    {
	value |= 0b00111000;
    } else
    {
	value &= 0b11000111;
    }

    writeRegister8(ADXL345_REG_ACT_INACT_CTL, value);
}


void ADXL345::setInactivityX(bool state) 
{
    writeRegisterBit(ADXL345_REG_ACT_INACT_CTL, 2, state);
}

bool ADXL345::getInactivityX(void)
{
    return readRegisterBit(ADXL345_REG_ACT_INACT_CTL, 2);
}

void ADXL345::setInactivityY(bool state)
{
    writeRegisterBit(ADXL345_REG_ACT_INACT_CTL, 1, state);
}

bool ADXL345::getInactivityY(void)
{
    return readRegisterBit(ADXL345_REG_ACT_INACT_CTL, 1);
}

void ADXL345::setInactivityZ(bool state)
{
    writeRegisterBit(ADXL345_REG_ACT_INACT_CTL, 0, state);
}

bool ADXL345::getInactivityZ(void)
{
    return readRegisterBit(ADXL345_REG_ACT_INACT_CTL, 0);
}

void ADXL345::setInactivityXYZ(bool state)
{
    uint8_t value;

    value = readRegister8(ADXL345_REG_ACT_INACT_CTL);

    if (state)
    {
	value |= 0b00000111;
    } else
    {
	value &= 0b11111000;
    }

    writeRegister8(ADXL345_REG_ACT_INACT_CTL, value);
}

void ADXL345::setTapDetectionX(bool state)
{
    writeRegisterBit(ADXL345_REG_TAP_AXES, 2, state);
}

bool ADXL345::getTapDetectionX(void)
{
    return readRegisterBit(ADXL345_REG_TAP_AXES, 2);
}

void ADXL345::setTapDetectionY(bool state)
{
    writeRegisterBit(ADXL345_REG_TAP_AXES, 1, state);
}

bool ADXL345::getTapDetectionY(void)
{
    return readRegisterBit(ADXL345_REG_TAP_AXES, 1);
}

void ADXL345::setTapDetectionZ(bool state)
{
    writeRegisterBit(ADXL345_REG_TAP_AXES, 0, state);
}

bool ADXL345::getTapDetectionZ(void)
{
    return readRegisterBit(ADXL345_REG_TAP_AXES, 0);
}

void ADXL345::setTapDetectionXYZ(bool state)
{
    uint8_t value;

    value = readRegister8(ADXL345_REG_TAP_AXES);

    if (state)
    {
	value |= 0b00000111;
    } else
    {
	value &= 0b11111000;
    }

    writeRegister8(ADXL345_REG_TAP_AXES, value);
}


void ADXL345::useInterrupt(adxl345_int_t interrupt)
{
    if (interrupt == 0)
    {
	writeRegister8(ADXL345_REG_INT_MAP, 0x00);
    } else
    {
	writeRegister8(ADXL345_REG_INT_MAP, 0xFF);
    }

    writeRegister8(ADXL345_REG_INT_ENABLE, 0xFF);
}

Activites ADXL345::readActivites(void)
{
    uint8_t data = readRegister8(ADXL345_REG_INT_SOURCE);

    a.isOverrun = ((data >> ADXL345_OVERRUN) & 1);
    a.isWatermark = ((data >> ADXL345_WATERMARK) & 1);
    a.isFreeFall = ((data >> ADXL345_FREE_FALL) & 1);
    a.isInactivity = ((data >> ADXL345_INACTIVITY) & 1);
    a.isActivity = ((data >> ADXL345_ACTIVITY) & 1);
    a.isDoubleTap = ((data >> ADXL345_DOUBLE_TAP) & 1);
    a.isTap = ((data >> ADXL345_SINGLE_TAP) & 1);
    a.isDataReady = ((data >> ADXL345_DATA_READY) & 1);

    data = readRegister8(ADXL345_REG_ACT_TAP_STATUS);

    a.isActivityOnX = ((data >> 6) & 1);
    a.isActivityOnY = ((data >> 5) & 1);
    a.isActivityOnZ = ((data >> 4) & 1);
    a.isTapOnX = ((data >> 2) & 1);
    a.isTapOnY = ((data >> 1) & 1);
    a.isTapOnZ = ((data >> 0) & 1);

    return a;
}

// Write byte to register
void ADXL345::writeRegister8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(ADXL345_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
        Wire.write(value);
    #else
        Wire.send(reg);
        Wire.send(value);
    #endif
    Wire.endTransmission();
}

// Read byte to register
uint8_t ADXL345::fastRegister8(uint8_t reg)
{
    uint8_t value;
    Wire.beginTransmission(ADXL345_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.requestFrom(ADXL345_ADDRESS, 1);
    #if ARDUINO >= 100
        value = Wire.read();
    #else
        value = Wire.receive();
    #endif;
    Wire.endTransmission();

    return value;
}

// Read byte from register
uint8_t ADXL345::readRegister8(uint8_t reg)
{
    uint8_t value;
    Wire.beginTransmission(ADXL345_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(ADXL345_ADDRESS);
    Wire.requestFrom(ADXL345_ADDRESS, 1);
    while(!Wire.available()) {};
    #if ARDUINO >= 100
        value = Wire.read();
    #else
        value = Wire.receive();
    #endif;
    Wire.endTransmission();

    return value;
}

// Read word from register
int16_t ADXL345::readRegister16(uint8_t reg)
{
    int16_t value;
    Wire.beginTransmission(ADXL345_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(ADXL345_ADDRESS);
    Wire.requestFrom(ADXL345_ADDRESS, 2);
    while(!Wire.available()) {};
    #if ARDUINO >= 100
        uint8_t vla = Wire.read();
        uint8_t vha = Wire.read();
    #else
        uint8_t vla = Wire.receive();
        uint8_t vha = Wire.receive();
    #endif;
    Wire.endTransmission();

    value = vha << 8 | vla;

    return value;
}

void ADXL345::writeRegisterBit(uint8_t reg, uint8_t pos, bool state)
{
    uint8_t value;
    value = readRegister8(reg);

    if (state)
    {
	value |= (1 << pos);
    } else 
    {
	value &= ~(1 << pos);
    }

    writeRegister8(reg, value);
}

bool ADXL345::readRegisterBit(uint8_t reg, uint8_t pos)
{
    uint8_t value;
    value = readRegister8(reg);
    return ((value >> pos) & 1);
}
