/*
    ADXL345 Triple Axis Accelerometer. RGB LED Demo.
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-akcelerometr-adxl345.html
    GIT: https://github.com/jarzebski/Arduino-ADXL345
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <ADXL345.h>

ADXL345 accelerometer;

int RedPin = 4;
int GreenPin = 3;
int BluePin = 2;

long RedTime;
long GreenTime;
long BlueTime;
long DTTime;

void setup(void) 
{
  Serial.begin(9600);

  pinMode(RedPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  digitalWrite(RedPin, LOW);
  digitalWrite(BluePin, LOW);
  digitalWrite(GreenPin, LOW);

  // Initialize ADXL345
  Serial.println("Initialize ADXL345");

  if (!accelerometer.begin())
  {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    delay(500);
  }

  // Values for Free Fall detection
  accelerometer.setFreeFallThreshold(0.35); // Recommended 0.3 -0.6 g
  accelerometer.setFreeFallDuration(0.1);   // Recommended 0.1 s

  // Values for Activity and Inactivity detection
  accelerometer.setActivityThreshold(1.2);    // Recommended 1.2 g
  accelerometer.setInactivityThreshold(1.2);  // Recommended 1.2 g
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

  // Set tap detection on Z-Axis
  accelerometer.setTapDetectionX(0);       // Don't check tap on X-Axis
  accelerometer.setTapDetectionY(0);       // Don't check tap on Y-Axis
  accelerometer.setTapDetectionZ(1);       // Check tap on Z-Axis
  // or
  // accelerometer.setTapDetectionXYZ(1);  // Check tap on X,Y,Z-Axis

  accelerometer.setTapThreshold(2.5);      // Recommended 2.5 g
  accelerometer.setTapDuration(0.02);      // Recommended 0.02 s
  accelerometer.setDoubleTapLatency(0.10); // Recommended 0.10 s
  accelerometer.setDoubleTapWindow(0.30);  // Recommended 0.30 s

  // Select INT 1 for get activities
  accelerometer.useInterrupt(ADXL345_INT1);

}

void loop(void) 
{
  long time = micros();

  if ((time - RedTime) > 300000) digitalWrite(RedPin, LOW);
  if ((time - BlueTime) > 300000) digitalWrite(BluePin, LOW);

  // Read values for activities
  delay(50);

  Vector norm = accelerometer.readNormalize();

  // Read activities
  Activites activ = accelerometer.readActivites();

  if (activ.isFreeFall)
  {
    for (int i = 0; i <= 4; i++)
    {
	digitalWrite(RedPin, HIGH);
	digitalWrite(BluePin, HIGH);
	delay(100);
	digitalWrite(RedPin, LOW);
	digitalWrite(BluePin, LOW);
	delay(100);
    }

    delay(200);

    return;
  }

  if (activ.isDoubleTap)
  {
    digitalWrite(RedPin, HIGH);
    RedTime = micros();
  } else
  if (activ.isTap)
  {
    digitalWrite(BluePin, HIGH);
    BlueTime = micros();
  }

  if (activ.isInactivity)
  {
    digitalWrite(GreenPin, HIGH);
    GreenTime = micros();
  }

  if (activ.isActivity)
  {
    digitalWrite(GreenPin, LOW);
  }
}
