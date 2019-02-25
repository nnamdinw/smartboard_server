#include "SK8.h"
#include <string>
#include <iostream>
struct sk8_packet;

bool SK8::updateAll()
{
bool result = true;

result &= updateIMU9250();
//result &= updateMPL3115A2();
//temperature = 0;
//altitude = 0;
result &= updateUS5881();
timestamp = millis();
return result;
}

bool SK8::updateUS5881()
{
	bool result = false;
	if(!US5881.poll())
	{
		rpm = US5881.getRPM();
		wheelspeed = US5881.getWheelSpeed('m');
		result = true;
	}
	else
	{
		//must be no movement assuming init was succesful. set rpm to zero
		rpm = 0;
		wheelspeed = 0;
	}

	return true;
}

bool SK8::updateMPL3115A2()
{
	//currently there's no way to tell if a reading has "failed" for this class -  maybe we'll add that later
	altitude = mpl3115a2.readAltitudeFt();
	if(temperatureEnabled)
	{
		temperature = mpl3115a2.readTempF();
	}
	else
	{
	temperature = 0.0;
	}
	return true;
}

bool SK8::updateIMU9250()
{
	bool result = false;
	if(imu9250.poll())
	{
		ax = imu9250.ax;
		ay = imu9250.ay;
		az = imu9250.az;
//		pitch = imu9250.pitch;
//		yaw = imu9250.yaw;
//		roll = imu9250.roll;
		q0 = imu9250.q0;
		q1 = imu9250.q1;
		q2 = imu9250.q2;
		q3 = imu9250.q3;
		result = true;
	}
	else
	{
		std::cout << "\nMPU9250 error";
	}

	return result;
}
	
SK8::sk8_packet SK8::createDataPacket()
{
++index;
sk8_packet frame;
frame.index =index;
frame.timestamp = timestamp; //updated in update<Sensor>() functions
frame.ax = ax;
frame.ay = ay;
frame.az = az;
//ame.pitch = pitch;
//frame.yaw = yaw;
//frame.roll = roll;
frame.q0 = q0;
frame.q1 = q1;
frame.q2 = q2;
frame.q3 = q3;
frame.rpm = rpm;
frame.wheelspeed = wheelspeed;
frame.altitude = altitude;
frame.temperature = temperature;

return frame;
}
std::string SK8::frameToString(sk8_packet frame)
{
	std::string out = "";
	/*
		Format as follows:
		
		 {
		"version":1.0,
		"contents":"data": //other posibilities include 'selfTest,error,etc.'
		"data",{
		"ax":,
		"ay":,
		"az":,
		"pitch":,
		"yaw":,
		"roll":,
		"rpm":,
		"wheelspeed":,
		"altitude":,
		"temperature":,
		"index":,
		"timestamp":
		}
		}


	
	*/

packet_version = 2; //updated to use quaternion instead of Euler
/*
	out = "{ \"Version\":" + std::to_string(packet_version) + ",\"contents\":data\",\"data\":{\"ax\":" + std::to_string(ax) + ",\"ay\":" + std::to_string(ay) + ",\"az\":" + std::to_string(az) + ",\"pitch\":" +std::to_string( pitch) + ",\"yaw\":" + std::to_string(yaw) + ",\"roll\":" +std::to_string(roll) + ",\"rpm\":" + std::to_string(rpm) + ",\"wheelspeed\":" + std::to_string(wheelspeed) + ",\"altitude\":" +std::to_string( altitude) + ",\"temperature\":" +std::to_string( temperature) + ",\"index\":" +  std::to_string(index) + ",\"timestamp\":" +std::to_string( timestamp) + "} }";
*/


  out = "{ \"Version\":" + std::to_string(packet_version) + ",\"contents\":\"data\",\"data\":{\"ax\":" + std::to_string(ax) + ",\"ay\":" + std::to_string(ay) + ",\"az\":" + std::to_string(az) + ",\"q0\":" +std::to_string(q0) + ",\"q1\":" + std::to_string(q1) + ",\"q2\":" +std::to_string(q2) +",\"q3\":" + std::to_string(q3) + ",\"rpm\":" + std::to_string(rpm) + ",\"wheelspeed\":" + std::to_string(wheelspeed) + ",\"altitude\":" +std::to_string( altitude) + ",\"temperature\":" +std::to_string( temperature) + ",\"index\":" +  std::to_string(index) + ",\"timestamp\":" +std::to_string( timestamp) + "} }";
return out;
}	
void SK8::setTemperature(bool a)
{
	temperatureEnabled = a;	
	return;	
}

bool SK8::initUS5881()
{
	US5881.setPinID(14); //hardcoded for now cuz breadboard ain't changin
	if(!US5881.init())
	{
		std::cout << "\nus5881 init failure";
		US5881_status = false;
	}
	else
	{
	
		US5881_status = true;
		US5881.setWheelDiameter(2.5);
		
	}
	return US5881_status; 
}

void SK8::initMPL3115A2()
{
	mpl3115a2.setModeAltimeter();
	mpl3115a2.setOversampleRate(7);
	MPL3115A2_status = true;
}	

void SK8::initMPU9250()
{
	imu9250.calibrateMPU9250(imu9250.gyroBias,imu9250.accelBias);
	imu9250.initMPU9250();
	imu9250.initAK8963(imu9250.magCalibration);
	IMU9250_status = true;
}

SK8::SK8()
{
	altitude = 0;
	temperature = 0;
	index = 0;
	initMPU9250();
	initMPL3115A2();
//	US5881.setPinID(14);
	initUS5881();
}

