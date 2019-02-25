#include "sensors/us5881.h"
#include "sensors/MPL3115A2pi.h"
#include "sensors/MPU9250pi.h"
#include <string>
class SK8 {



private:

bool MPL3115A2_status;
bool US5881_status;
bool IMU9250_status;
bool temperatureEnabled;
us5881 US5881;
MPL3115A2 mpl3115a2;
MPU9250 imu9250;
int packet_version;
//frame data
int index;
long timestamp;
float ax,ay,az,pitch,yaw,roll,rpm,wheelspeed,altitude,temperature,q0,q1,q2,q3;




	

public:


/*

struct sk8_packet {
	int index;
	long timestamp;
	float ax;
	float ay;
	float az;
	float pitch;
	float yaw;
	float roll;
	float rpm;
	float wheelspeed;
	float altitude;
	float temperature;
};
*/

struct sk8_packet {
        int index;
        long timestamp;
        float ax;
        float ay;
        float az;
        float q0;
        float q1;
        float q2;
	float q3;
        float rpm;
        float wheelspeed;
        float altitude;
        float temperature;
};

sk8_packet createDataPacket();
bool updateAll();
bool updateIMU9250();
bool updateMPL3115A2();
bool updateUS5881();
std::string frameToString(sk8_packet);
void setTemperature(bool);
bool initUS5881();
void initMPL3115A2();
void initMPU9250();
SK8();
};
