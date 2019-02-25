#include "MPL3115A2pi.h"
#include <wiringPi.h>
#include <iostream>
#include <math.h>
#include <cstdlib>

using namespace std;

int main()
{

MPL3115A2 height_sensor;


cout << "\nAltimeter Demo\n";
height_sensor.setModeActive();
height_sensor.setModeAltimeter();
height_sensor.setOversampleRate(7);
height_sensor.enableEventFlags();

cout << "\nCurrent height above sea level: " << height_sensor.readAltitudeFt() << "\n";
cout << "\nCurrent temperature: " << height_sensor.readTempF() << endl;
return 0;
}


