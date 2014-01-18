/*
    ADXL345 Triple Axis Accelerometer. Output for ADXL345_processing.pde
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-akcelerometr-adxl345.html
    GIT: https://github.com/jarzebski/Arduino-ADXL345
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <ADXL345.h>

ADXL345 accelerometer;

void setup(void) 
{
  Serial.begin(115200);

  // Initialize ADXL345
  if (!accelerometer.begin())
  {
    delay(500);
  }

  // Set measurement range
  // +/-  2G: ADXL345_RANGE_2G
  // +/-  4G: ADXL345_RANGE_4G
  // +/-  8G: ADXL345_RANGE_8G
  // +/- 16G: ADXL345_RANGE_16G
  accelerometer.setRange(ADXL345_RANGE_16G);
}

void loop(void) 
{
  // Read normalized values
  Vector norm = accelerometer.readNormalize();

  // Low Pass Filter to smooth out data. 0.1 - 0.9
  Vector filtered = accelerometer.lowPassFilter(norm, 0.15);

  // Calculate Pitch & Roll
  int pitch = -(atan2(norm.XAxis, sqrt(norm.YAxis*norm.YAxis + norm.ZAxis*norm.ZAxis))*180.0)/M_PI;
  int roll  = (atan2(norm.YAxis, norm.ZAxis)*180.0)/M_PI;

  // Calculate Pitch & Roll (Low Pass Filter)
  int fpitch = -(atan2(filtered.XAxis, sqrt(filtered.YAxis*filtered.YAxis + filtered.ZAxis*filtered.ZAxis))*180.0)/M_PI;
  int froll  = (atan2(filtered.YAxis, filtered.ZAxis)*180.0)/M_PI;

  // Output
  Serial.print(pitch);
  Serial.print(":");
  Serial.print(roll);
  Serial.print(":");

  // Output (filtered)
  Serial.print(fpitch);
  Serial.print(":");
  Serial.print(froll);
  Serial.println();
}
