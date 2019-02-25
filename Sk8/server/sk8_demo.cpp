#include "wiringPi.h"
#include "wiringPiI2C.h"
//#include "sensors/MPU9250pi.h"
//#include "sensors/MPL3115A2pi.h"
//#include "sensors/us5881.h"
#include "SK8.h"
#include <string>
#include <iostream>
#include "bluez_server.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
int main()
{

SK8 demo;
bluez_server btServer;
btServer.init_server(3);

/*
if(!demo.initUS5881())
{
	std::cout << "\nus5881 initialzation failure\n";
}
else
{
	std::cout << "\nUS5881 online" << std::endl;
}
*/
char z;
/*
std::cout << "\nEnter 'g' to begin polling: ";
std::cin >> z;

if(z != 'g')
{
	return 1;
}
*/

//demo.initMPL3115A2();
//demo.initMPU9250();
demo.setTemperature(false);
//demo.setWheelWidth(2.5);
std::string out;
int x = 0;
bool poll = false;
while(true)
{

	if(demo.updateAll() && btServer.status)
	{

	out =  demo.frameToString(demo.createDataPacket());
//	std::cout << out << std::endl;
	btServer.write_server(out.c_str());
		
	
	}
	else
	{
//	std::cout << "\nUndefined error";
	break;
	}
	
	/*	
	if(btServer.read_server() == "go")
	{
		poll = true;
	}
	else if(btServer.read_server() == "stop")
	{
	poll = false;
	}
*/
}

return 1;
}
