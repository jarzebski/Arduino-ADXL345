/*
    ADXL345 Triple Axis Accelerometer. Activity & Inactivity
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
  Serial.begin(9600);

  // Initialize ADXL345
  Serial.println("Initialize ADXL345");

  if (!accelerometer.begin())
  {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    delay(500);
  }

  // Values for Activity and Inactivity detection
  accelerometer.setActivityThreshold(2.0);    // Recommended 2 g
  accelerometer.setInactivityThreshold(2.0);  // Recommended 2 g
  accelerometer.setTimeInactivity(5);         // Recommended 5 s

  // Set activity detection only on X,Y,Z-Axis
  accelerometer.setActivityXYZ(1);         // Check activity on X,Y,Z-Axis
  // or
  // accelerometer.setActivityX(1);        // Check activity on X_Axis
  // accelerometer.setActivityY(1);        // Check activity on Y-Axis
  // accelerometer.setActivityZ(1);        // Check activity on Z-Axis

  // Set inactivity detection only on X,Y,Z-Axis
  accelerometer.setInactivityXYZ(1);       // Check inactivity on X,Y,Z-Axis
  // or
  // accelerometer.setInactivityX(1);      // Check inactivity on X_Axis
  // accelerometer.setInactivityY(1);      // Check inactivity on Y-Axis
  // accelerometer.setInactivityZ(1);      // Check inactivity on Z-Axis

  // Select INT 1 for get activities
  accelerometer.useInterrupt(ADXL345_INT1);

  // Check settings
  checkSetup();
}

void checkSetup()
{
  Serial.print("Activity Threshold = "); Serial.println(accelerometer.getActivityThreshold());
  Serial.print("Inactivity Threshold = "); Serial.println(accelerometer.getInactivityThreshold());
  Serial.print("Time Inactivity = "); Serial.println(accelerometer.getTimeInactivity());

  Serial.print("Look activity on axis = "); 
  if (accelerometer.getActivityX()) { Serial.print(" X "); }
  if (accelerometer.getActivityY()) { Serial.print(" Y "); }
  if (accelerometer.getActivityZ()) { Serial.print(" Z "); }
  Serial.println();

  Serial.print("Look inactivity on axis = "); 
  if (accelerometer.getInactivityX()) { Serial.print(" X "); }
  if (accelerometer.getInactivityY()) { Serial.print(" Y "); }
  if (accelerometer.getInactivityZ()) { Serial.print(" Z "); }
  Serial.println();  
}

void loop(void) 
{
  // Read values for activities
  delay(50);
  Vector norm = accelerometer.readNormalize();

  // Read activities
  Activites activ = accelerometer.readActivites();

  if (activ.isActivity)
  {
    Serial.println("Activity Detected");
  }

  if (activ.isInactivity)
  {
    Serial.println("Inactivity Detected");
  }
}

