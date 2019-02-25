#include <wiringPi.h>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include "us5881.h"
using namespace std;

const int us1881 = 14;
/*
 *The US1881 is a latching hall sensor, this means that the magnet will trigger AND remain triggered once a magnet has passed
 and won't 'untrigger' until a magnet of oppiste polarity and strength has passed
 * 
 */


/*
 *
 * The US5881 is a standard hall effect sensor. Without any magnetic field detected, or a North Pole magnet detected - the device will output LOW
 *
 * Once a Magnet's South Pole has been detected, the device will ouput HIGH
 *
 * Unlike the latching sensor, the LOW output will persist for only as long as the magnet is nearby. Once it leaves the output will go back to LOW
 */



//const int us5881 = 14;
static long countPm = 0;
static long tMin = 0;
static int ticks = 0;

float getRpm(long a, long b)
{
	//a is t0, b is t1
	float delta_t =a-b;
	float actualRotations = 0.0;
	
	
	if(delta_t >= 60000000)
	{
		cout << "\nWheels spinning at or slower than 1 rotation per minute\n";
		return 0;
	}
	//convert microsecond delta to minutes
	delta_t = delta_t / 1000000 / 60;
	actualRotations = 1/delta_t;
	return actualRotations;
}
	


int main()
{
	

	//initializing wiringPi using Broadcom numbering of GPIO ports
	
us5881 mag;
mag.setPinID(14);
if(!mag.init())	
{
	cout << "\nInit error..\n";
	return 1;
}	

	while(true)		
	{

	if(mag.poll())
	{
		printf("\n%3.2f",mag.getRPM());
	}
	
		
/*	
		if(digitalRead(us5881) == 1)
		{
			if(!pass)
			{
//				cout << "\nMagnet Detected";
				pass = true;
//				count++;
//				t = micros();
			}

		}
		else
		{
			if(pass)
			{
				t1 = t;
				t = micros();
//				cout << "\nNo magnet detected";
				pass = false;
				ticks++;
				cout << "\nRotations per minute: " << getRpm(t,t1);
			}
		}


	*/	
	
		
		

	}
	return 0;
}


		

