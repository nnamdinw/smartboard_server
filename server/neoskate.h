#include <atomic>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include "sensors/Adafruit_Sensor.h"
#include "sensors/Adafruit_BNO055.h"
#include "sensors/utility/imumaths.h"
#include "sensors/DRV2605.h"
#include "sensors/mux_drv2605.h"
#include "sensors/utility/imumaths.h"

#define BNO055_SAMPLERATE_DELAY_MS (100)

class neoskate {



private:
	bool hasLED,needsCalibration,pollStream,calibrating;
	int errorState;
	static const std::string logDir;
	static const std::string configDir;
	void saveCalData(adafruit_bno055_offsets_t&);
	std::fstream fileio;
	int buzzTogg;
	//bool newPoll;
	Adafruit_BNO055 bno;
	DRV2605 hap;
	mux_drv2605 mux;
	int led1pin,led2pin;
	int packet_version;
	//frame data
	int index;
	long timestamp;
	float ax,ay,az,pitch,yaw,roll,rpm,wheelspeed,altitude,temperature,q0,q1,q2,q3;
	std::atomic<bool> flag,newPoll,newFrame;
	std::atomic<int> configNum;
	std::vector<std::string> logs;
	std::string calibOutput,pollFrame;

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
	bool isCalibrating();
	void setCalibrating(bool);
	std::string getFrame();
	bool newPollData();
	bool getStreamStatus();
	std::string getCalibrationProgress();
	void togglePollStream();
	std::string getPollFrame();
	bool getCalibrationStatus();
	void calibrateBNO055();
	void enableLED();
	void setErrorState(int);
	void indicate();
	void setLED(int,bool);
	void initLED(int,int);
	void printCalData();
	void updateCalOutput();
	int poll();
	void buzz();
	bool isNewPoll(); //tru if theres new poll logged
	void setNewPoll(bool);
	//std::string frameToString(sk8_packet);
	neoskate();
	bool pollStatus();
	void enableHaptics();
	void setPoll(bool);
	void setConfig(int);
	int getSzLogs();
	std::string getLogs(); //return json of saved logs
	std::string fetchLog(int); //read in log from disk and do something 
};
