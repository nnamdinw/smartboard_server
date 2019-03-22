#include <atomic>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "sensors/Adafruit_Sensor.h"
#include "sensors/Adafruit_BNO055.h"
#include "sensors/utility/imumaths.h"
#include "sensors/DRV2605.h"
#include "sensors/mux_drv2605.h"
#define BNO055_SAMPLERATE_DELAY_MS (100)

class neoskate {



private:
	int buzzTogg;
	Adafruit_BNO055 bno;
	DRV2605 hap;
	mux_drv2605 mux;
	//int configNum;
	int packet_version;
	//frame data
	int index;
	long timestamp;
	float ax,ay,az,pitch,yaw,roll,rpm,wheelspeed,altitude,temperature,q0,q1,q2,q3;
	std::atomic<bool> flag;
	std::atomic<int> configNum;
	std::vector<std::string> logs;

public:

	struct sk8_packet {
	        int index;
	        long timestamp;
	        float x;
	        float y;
	        float z;
	        float ax;
	        float ay;
	        float q0;
	        float az;
	        float q1;
	        float q2;
			float q3;
	        float rpm;
	        float wheelspeed;
	        float altitude;
	        float temperature;
	};

	//sk8_packet createDataPacket();
	int poll();
	void buzz();
	//std::string frameToString(sk8_packet);
	neoskate();
	void setPoll(bool);
	void setConfig(int);
};
