#include "messaging/mq.h"
#define BNO055_SAMPLERATE_DELAY_MS (100)


void sensorPoll(std::vector<std::string>& in)
{
  
  sensors_event_t event;
  std::cout << "\nStarting Polling Loop...\n";
  std::string temp = "";
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
 // std::vector<std::string> output;
  switch(1)    
    {

      case 1:
          while(true)
          {
          bno.getEvent(&event);
          temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
          in.push_back(temp);
          delay(BNO055_SAMPLERATE_DELAY_MS);
          }

          break;
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
            std::cout << "\nUniplimented";
            break;
      case 7:
            //update log struct in memory
            //write output vector to disk
            break;
    }
}

void buzz()
{
  int index = 0;
  DRV2605 hap;
  mux_drv2605 mux;
  mux.addEntry(1);
  mux.addEntry(2);


  //setup
  mux.set(1);
  hap.begin();
  hap.selectLibrary(1);
  hap.setMode(DRV2605_MODE_INTTRIG);

  mux.set(2);
  hap.begin();
  hap.selectLibrary(1);
  hap.setMode(DRV2605_MODE_INTTRIG);
    //wiringPiSetup();

  uint8_t effect = 1;
  mux.set(index);
//    effect = 12;
  hap.setWaveform(0,effect);
  hap.setWaveform(1,0);
  hap.go();
  delay(100);
  return;
}
mq::skate_config parseConfig(std::string name)
{
	//checks immediate dir for config file
  //Config file is Newline delimieted text file with the following lines
  /*  


      1- rabbitmqurl
      2- rabbitmq username
      3- rabbitmq password
      4- rabbitmq routing-key
      5- rabbitmq exchangeName
      6- rabbitmq queueName
      7- piConfig? (haptics or not, build id)
  */
  mq::skate_config s_c;
  std::ifstream in(name);
  std::string temp = "";
  std::vector<std::string> output;
  char delim = '\n';
  int fileLen = 0;
  in.seekg(0,in.end);
  fileLen = in.tellg();
  in.seekg(0,in.beg);

  if(fileLen != 0)
  {

    while(std::getline(in,temp,delim))
    {
      output.push_back(temp);
    }

      if(output.size() == 9)
      {
        s_c.mq_server = output[0];
        s_c.mq_user = output[1];
        s_c.mq_password = output[2];
        s_c.mq_routingkey = output[3];
        s_c.mq_exchange = output[4];
        s_c.mq_queueName = output[5];
        s_c.configVersion = output[6];
        s_c.vhostName = output[7];
        std::istringstream(output[8]) >> s_c.heartbeat;
       // amqp_connection_string = "amqp://" + s_c.mq_user + ":" + s_c.mq_password + "@" + s_c.mq_server + "/" + s_c.vhostName;

        std::cout << "\nConfig Parse Succesfull" << std::endl;
      }
      else
      {
        std::cout << "\nConfig Parse Unsuccesfull" << std::endl << "Too few newlines, need 8 received " << output.size() <<  std::endl << 
        " File size of " << fileLen << std::endl;
        exit(-1);

      }

  
  }
  else
  {
    std::cout << "\nError: Could not open file at " << name << std::endl;
    exit(-1);
  }
  return s_c;
}
int main()
{
  std::vector<std::string> log;
  //std::function<void(int)> bf = buzz;
  boost::asio::io_service mqservice(2);
  AMQP::LibBoostAsioHandler handler(mqservice);

  //bf = buzz;
  mq messageq(parseConfig("alphaconfig.conf"),mqservice,handler);
  //messageq.setFunc(0,buzz(0));

  std::thread t[2];
  std::cout << "\nSpawning rabbitmq listener...\n";
  messageq.mqConsume();


  //      threads[1] = std::thread(sensorPoll,configParam,messageFlag);


  return 0;
}