#include "neoskate.h"

const std::string neoskate::logDir = "logs/";
neoskate::neoskate()
{
  buzzTogg = 1;
  configNum = -1;
  bno = Adafruit_BNO055();
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    std::cout << "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!";
  }

  delay(1000);

  /* Display the current temperature */

  //bno.setExtCrystalUse(true);


  flag = false;
  newPoll = false;
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

void neoskate::setPoll(bool a)
{
	flag = a;
	return;
}

void neoskate::setConfig(int in)
{
	configNum = in;
}

std::string neoskate::getLogs()
{
  //std::string output = "{\"logs\":[{";
  std::string output = "{\"logs\":[";
  for(int i =0; i < logs.size();i++)
  {
    //output += "\"" + std::to_string(i) + "\":" +  "\"" + logs.at(i) + "\"";
    output += "{\"index\":\"" + std::to_string(i) + "\",\"filename\":\"" + logs.at(i) + "\"}";
    if(i < logs.size() - 1)
    {
      output += ",";
    }
  }
  output 
  += "]}";
  return output;
}
bool neoskate::pollStatus()
{
  return flag;
}
bool neoskate::isNewPoll()
{
  return newPoll;
}

void neoskate::setNewPoll(bool in)
{
  newPoll = in;
}
int neoskate::poll()
{
  if(configNum	< 0)
  {
  	//error out
  	//How can i have an error bubble all the way to the web client from here
  	//maybe some centralized error class
  	//higher order methods will periodically check and post errors to rabbitmq or something
  	//return -1;
  }
  unsigned int frameTime;
  std::string temp = "";
  sensors_event_t event;
  std::vector<std::string> output;
  bool postPoll = false;
  bool firstLoop = false;
  std::chrono::steady_clock::time_point start;
  std::chrono::milliseconds sampleTime;
  //buzz();
  //std::cout << "\nHi";


while(1)
{
    //buzz();
	while(flag)
	{
      //buzz();
      if(!firstLoop)
      {
        postPoll = true;
        setNewPoll(true);
        start = std::chrono::steady_clock::now();
        firstLoop = true;

      }

			switch(configNum)    
		    {

		      case 1:
		      case 2:
		      case 3:
		      case 4:
              //buzz();

		          bno.getEvent(&event);
              //frameTime = time(NULL);
              sampleTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - start);

		          //temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
              temp = "{\"x\":\"" +std::to_string(event.orientation.x) + "\", \"y\":\"" + std::to_string(event.orientation.y) + "\",\"Z\":\"" +std::to_string(event.orientation.z) + "\",\"time\":\"" + std::to_string(sampleTime.count()) + "\"}\n";
		          output.push_back(temp);
		          delay(BNO055_SAMPLERATE_DELAY_MS);
		   		  break;
		   	 default:
		   	 break;
		    }

      }

      if(postPoll)
      {
        frameTime = time(NULL);
        std::string logName = std::to_string(frameTime) + "_configType_"+ std::to_string(configNum) +".log";
        fileio.open(logDir + logName, std::fstream::out);
        for(std::vector<std::string>::iterator it = output.begin();it != output.end();it++)
        {
          fileio << *it;
        }
        fileio.close();
        logs.push_back(logName); 
        output.clear();
        //std::cout << "wrote log: " << logName;
        postPoll = false;
        firstLoop = false;
      }


    

	}
    return 1;

}
 int neoskate::getSzLogs()
 {
  return logs.size();
 }
std::string neoskate::fetchLog(int index)
{
        //std::ofstream out;
        std::string out = ""; 
        std::string contents = "";
        fileio.open(logDir + logs.at(index),std::fstream::in | std::fstream::app);
        while(std::getline(fileio,contents))
          {
            //fileio << out;
            out += contents;
          }
       
        //std::cout << out;
        fileio << out;
        fileio.close();
        return out;

}

void neoskate::buzz()
{

  if(buzzTogg == 1)
  {
  	  mux.set(buzzTogg);
  	  buzzTogg++;
  }
  else if(buzzTogg == 2)
  {
  	  mux.set(buzzTogg);
  	  buzzTogg--;
  }

  uint8_t effect = 12;
  //effect = 12;
  hap.setWaveform(0,effect);
  hap.setWaveform(1,0);
  hap.go();
  delay(100);
}

