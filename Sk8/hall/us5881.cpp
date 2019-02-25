#include "us5881.h"
#include <wiringPi.h>
#include <math.h>

bool us5881::init()
{
	//init using broadcom numbering	
	int _status = 0;
	if(!pinID)
	{
		return false;
	}
	else
	{
		_status = wiringPiSetupGpio();
		if(_status < 0)
		{
			status = false;
			return false;
		}
	}
	pinMode(getPinID(),INPUT);
	pullUpDnControl(getPinID(),PUD_DOWN);
	status = true;
	return true;
}

int us5881::getPinID()
{
 	return pinID;
}

void us5881::setPinID(int a)
{
	pinID = a;
	 return;
}

void us5881::setWheelDiameter(float a)
{
	wheelDiameter = a;
	return;
}

float us5881::getWheelDiameter()
{
	return wheelDiameter;
}

float us5881::getRPM()
{	
	if(status && firstVal)
	{	
	long delta_t = t1 - t0;
	float delta_tMinutes =  (float)delta_t / 1000000 / 60;
	
	return  (1/delta_tMinutes);
	}
	else
	{
	return 0;
	}
}

float us5881::getWheelSpeed(char a = 'z')
{
	float temp,ratio,circumference = M_PI * getWheelDiameter();
	int conversion;
	switch(a)
	{
	
	default:
	//mph case
	conversion = 5280;
	ratio = conversion/circumference; //this many revolutions per mile
	temp = getRPM() / ratio; //miles per minute
	temp *= 60; //mph
	}

	return temp;
					
}

int us5881::getDigitalValue()
{
	int out = -1;
	if(status)
	{
		out = digitalRead(getPinID());
	}
	return out;
}

bool us5881::poll()
{
	if(status)
	{
		val = getDigitalValue();
		if(val == 1)
		{
			if(!pass)
			{	
				pass = true;
			}
		}
		else
		{
			if(pass)
			{
			if(!firstVal)
			{
			firstVal = true;
			}
			t0 = t1;
			t1 = micros();
			pass = false;
			return true;
			}
		}
	}
	return false;
}


us5881::us5881()
{
	//default construc
	t0 = 0;
	t1 = 0;
	val = 0;
	pinID = 0;
	pass = false;
	firstVal = false;
}	



