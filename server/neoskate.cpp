#include "neoskate.h"
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

  bno.setExtCrystalUse(true);


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

void neoskate::setPoll(bool a)
{
	flag = a;
	return;
}

void neoskate::setConfig(int in)
{
	configNum = in;
}
zz
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
  //buzz();
  //std::cout << "\nHi";


while(1)
{
    //buzz();

	while(flag)
	{
      //buzz();
      postPoll = true;
			switch(configNum)    
		    {

		      case 1:
		      case 2:
		      case 3:
		      case 4:
              //buzz();
		          bno.getEvent(&event);
              frameTime = time(NULL);
		          //temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
             temp = "{\"x\":\"" +std::to_string(event.orientation.x) + "\", \"y\":\"" + std::to_string(event.orientation.y) + "\",\"Z\":\"" +std::to_string(event.orientation.z) + "\",\"time\":\"" + std::to_string(frameTime)+ "\"}\n";
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
        std::ofstream out;
        out.open(logName);
        for(std::vector<std::string>::iterator it = output.begin();it != output.end();it++)
        {
          out << *it;
        }
        out.close();
        logs.push_back(logName); 
        output.clear();
        //std::cout << "wrote log: " << logName;
        postPoll = false;

      }


    

	}
    return 1;

	//cleanup
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

