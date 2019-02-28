
#include "sensors/Adafruit_Sensor.h"
#include "sensors/Adafruit_BNO055.h"
#include "sensors/utility/imumaths.h"
#include <cstdio>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.

   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3-5V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
   2015/AUG/27  - Added calibration and system status helpers
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55);

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  printf("------------------------------------\n");
  printf  ("Sensor:       %d\n",sensor.name);
  printf  ("Driver Ver:   %d\n",sensor.version);
  printf  ("Unique ID:    %d\n",sensor.sensor_id);
  printf  ("Max Value:    %d\n",sensor.max_value);
  printf  ("Min Value:    %d\n",sensor.min_value);
  printf  ("Resolution:   %d",sensor.resolution);
  printf("------------------------------------\n");
  printf("");
  delay(500);
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void displaySensorStatus(void)
{
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  printf("");
  printf("System Status %d",system_status);
  printf("Self Test:     %d",self_test_results);
  printf("System Error:  %d",system_error);
  printf("");
  delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  printf("\t");
  if (!system)
  {
    printf("! ");
  }
  //TODO: Remove Arduino Specifc lines and classify this file. 
  /* Display the individual values */
  printf("Sys:%d\n",system);
  printf(" G:%d\n",gyro);
  printf(" A:%d\n",accel);
  printf(" M:%d\n",mag);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup()
{
  printf("Orientation Sensor Test\n");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    printf("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Optional: Display current status */
  displaySensorStatus();

  bno.setExtCrystalUse(true);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop()
{
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  /* Display the floating point data */
  printf("%3.2f,",event.orientation.x);
  printf(" %3.2f,",event.orientation.y);
  printf(" %3.2f\n",event.orientation.z);

  /* Optional: Display calibration status */
  //displayCalStatus();

  /* Optional: Display sensor status (debug only) */
  //displaySensorStatus();

  /* New line for the next sample */
  //printf("\n");

  /* Wait the specified delay before requesting nex data */
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

int main()
{
  setup();
  while(true)
  {
    loop();
  }
}