#include "neoskate.h"
neoskate::neoskate()
{
  configNum = -1;
  bno = Adafruit_BNO055(55);
  flag = false;
  mux.addEntry(1);
  mux.addEntry(2);

  mux.set(1);
  hap.begin();
  hap.selectLibrary(1);
  hap.setMode(DRV2605_MODE_INTTRIG);

  mux.set(2);
  hap.begin();
  hap.selectLibrary(1);
  hap.setMode(DRV2605_MODE_INTTRIG);
}

void neoskate::togglePoll()
{
	flag = !flag;
	return;
}

void neoskate::setConfig(int in)
{
	configNum = -1;
}

int neoskate::poll()
{
  int now;		
  if(configNum	< 0)
  {
  	//error out
  	//How can i have an error bubble all the way to the web client from here
  	//maybe some centralized error class
  	//higher order methods will periodically check and post errors to rabbitmq or something
  	return -1;
  }

  std::string temp = "";
  sensors_event_t event;
  std::vector<std::string> output;

	while(flag)
	{

			switch(configNum)    
		    {

		      case 1:
		      case 2:
		      case 3:
		      case 4:
		          bno.getEvent(&event);
		          temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
		          output.push_back(temp);
		          delay(BNO055_SAMPLERATE_DELAY_MS);
		   		  break;
		   	 default:
		   	 break;
		    }


	}
		now = time(NULL);
		std::string logName = std::to_string(now) + "_configType_"+ std::to_string(configNum) +".log";
		std::ofstream out;
		out.open(logName);
		for(std::vector<std::string>::iterator it = output.begin();it != output.end();it++)
		{
			out << *it;
		}
		out.close();
		logs.push_back(logName); 
		output.clear();

	//cleanup
}

void neoskate::buzz()
{
  uint8_t effect = 1;
  mux.set(0);
//    effect = 12;
  hap.setWaveform(0,effect);
  hap.setWaveform(1,0);
  hap.go();
  delay(100);
}

