#include "neoskate.h"

const std::string neoskate::logDir = "logs/";
const std::string neoskate::configDir = "config/";
neoskate::neoskate()
{ 
  //bn055 calibratino is 11 entry struct of ints
  buzzTogg = 1;
  configNum = -1;
  bno = Adafruit_BNO055();
  std::vector<std::string> cal;
  std::string temp = "";
  if(!bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS))
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    std::cout << "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!";
  }
  fileio.open(configDir + "bno05.conf", std::fstream::app | std::fstream::in);
      if(fileio.is_open())
      {
          while(std::getline(fileio,temp))
          {
            cal.push_back(temp);
          }
          if(cal.size() == 11)
          {
            int i = 0;
            adafruit_bno055_offsets_t savedConfig;
            savedConfig.accel_offset_x = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.accel_offset_y = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.accel_offset_z = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.mag_offset_x = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.mag_offset_y = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.mag_offset_z = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.gyro_offset_x = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.gyro_offset_y = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.gyro_offset_z = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.accel_radius = (int16_t)std::stoi(cal.at(i));
            i++;
            savedConfig.mag_radius = (int16_t)std::stoi(cal.at(i));
            bno.setSensorOffsets(savedConfig);
          }
          else
          {
            std::cout << "\nInvalid config file..";
            std::cout << "\nContains " << cal.size() << " lines.";
          }

        fileio.close();
      }
      
      if(cal.size() != 11)
      {


        std::cout << "\nSensor offsets file not found.. Calibrating.\n";
        sensors_event_t event;
        bno.getEvent(&event);
        while (!bno.isFullyCalibrated())
        {
            bno.getEvent(&event);
            /* Optional: Display calibration status */
            printCalData();
            /* New line for the next sample */
            std::cout << std::endl;

            /* Wait the specified delay before requesting new data */
            delay(BNO055_SAMPLERATE_DELAY_MS);
        }
        std::cout << "\nCalibration Complete.. Writing to disk at /config/bno05.conf\n";

        adafruit_bno055_offsets_t newCalib;
        bno.getSensorOffsets(newCalib);
        saveCalData(newCalib);

      }
std::cout << "\nCal data loaded.";
printCalData();

  delay(1000);

  bno.setExtCrystalUse(true);


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
void neoskate::saveCalData(adafruit_bno055_offsets_t &in)
{
  std::string out = "";

  out += std::to_string(in.accel_offset_x) + '\n'; 
  out += std::to_string(in.accel_offset_y) + '\n';  
  out += std::to_string(in.accel_offset_z) + '\n';  
  out += std::to_string(in.mag_offset_x) + '\n'; 
  out += std::to_string(in.mag_offset_y) + '\n'; 
  out += std::to_string(in.mag_offset_z) + '\n'; 
  out += std::to_string(in.gyro_offset_x) + '\n'; 
  out += std::to_string(in.gyro_offset_y) + '\n'; 
  out += std::to_string(in.gyro_offset_z) + '\n'; 
  out += std::to_string(in.accel_radius) + '\n'; 
  out += std::to_string(in.mag_radius);
  
  fileio.open(configDir + "bno05.conf", std::fstream::out);
    
  fileio << out;
        
  fileio.close();
}
void neoskate::printCalData()
{
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    std::cout << "\nSystem: " << std::to_string(system)
              << "\nGyro : " << std::to_string(gyro)
              << "\nAccel: " << std::to_string(accel)
              << "\nMag: " << std::to_string(mag);
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
  imu::Quaternion quat;
  imu::Vector<3> accel;
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
        start = std::chrono::steady_clock::now();
        firstLoop = true;

      }

			switch(configNum)    
		    {

		      case 1:
		          bno.getEvent(&event);
              sampleTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - start);
		          //temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
              temp = "{\"x\":\"" +std::to_string(event.orientation.x) + "\", \"y\":\"" + std::to_string(event.orientation.y) + "\",\"Z\":\"" +std::to_string(event.orientation.z) + "\",\"time\":\"" + std::to_string(sampleTime.count()) + "\"}\n";
		          output.push_back(temp);
		          delay(BNO055_SAMPLERATE_DELAY_MS);
		   		  break;
          case 2:
              quat = bno.getQuat();
              sampleTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - start);
              temp = "{\"qW\":\"" + std::to_string(quat.w()) + "\", \"qX\":\"" + std::to_string(quat.x()) + "\",\"qY\":\"" +std::to_string(quat.y()) + "\",\"qZ\":\"" + std::to_string(quat.z()) + "\",\"time\":\"" +std::to_string(sampleTime.count()) + "\"}\n";
              output.push_back(temp);
              delay(BNO055_SAMPLERATE_DELAY_MS);
              break;
          case 3:
              bno.getEvent(&event);
              accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
              sampleTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - start);
              //temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
              temp = "{\"x\":\"" +std::to_string(event.orientation.x) + "\", \"y\":\"" + std::to_string(event.orientation.y) + "\",\"Z\":\"" +std::to_string(event.orientation.z) + "\",\"aX\":\"" +std::to_string(accel.x()) + "\", \"aY\":\"" + std::to_string(accel.y()) + "\",\"aZ\":\"" +std::to_string(accel.z()) + "\",\"time\":\"" + std::to_string(sampleTime.count()) + "\"}\n";
              output.push_back(temp);
              delay(BNO055_SAMPLERATE_DELAY_MS);
              break;
          case 4:
              quat = bno.getQuat();
              accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
              sampleTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - start);
              temp = "{\"qW\":\"" + std::to_string(quat.w()) + "\", \"qX\":\"" + std::to_string(quat.x()) + "\",\"qY\":\"" +std::to_string(quat.y()) + "\",\"qZ\":\"" + std::to_string(quat.z()) + "\",\"aX\":\"" +std::to_string(accel.x()) + "\", \"aY\":\"" + std::to_string(accel.y()) + "\",\"aZ\":\"" +std::to_string(accel.z()) + "\",\"time\":\"" +std::to_string(sampleTime.count()) + "\"}\n";
              output.push_back(temp);
              delay(BNO055_SAMPLERATE_DELAY_MS);
              break;
		   	 default:
              //
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
        setNewPoll(true);

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

