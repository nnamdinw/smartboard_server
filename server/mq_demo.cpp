#include "messaging/mq.h"
//#define BNO055_SAMPLERATE_DELAY_MS (330)


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

  size_t configSz = 10;
  if(fileLen != 0)
  {

    while(std::getline(in,temp,delim))
    {
      output.push_back(temp);
    }

      if(output.size() == configSz)
      {
        s_c.mq_server = output[0];
        s_c.mq_user = output[1];
        s_c.mq_password = output[2];
        s_c.mq_routingkey = output[3];
        s_c.mq_exchange = output[4];
        s_c.mq_queueName_to = output[5];
        s_c.mq_queueName_from = output[6];
        s_c.configVersion = output[7];
        s_c.vhostName = output[8];
        std::istringstream(output[9]) >> s_c.heartbeat;
        //std::istringstream(s_c.heartbeat) >> heartbeatTick;
        //amqp_connection_string = "amqp://" + s_c.mq_user + ":" + s_c.mq_password + "@" + s_c.mq_server + "/" + s_c.vhostName;

        std::cout << "\nConfig Parse Succesfull" << std::endl;
      }
      else
      {
        std::cout << "\nConfig Parse Unsuccesfull" 
        << std::endl 
        << "Wrong number of newlines, need " 
        << (configSz)
        << " received "
        << output.size()
        <<  std::endl 
        << " File size of "
        << fileLen
        << std::endl;
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
//  neoskate hardwareInterface;

  //std::function<void(int)> bf = buzz;
  boost::asio::io_service mqservice(4);
  AMQP::LibBoostAsioHandler handler(mqservice);

  //bf = buzz;
  mq messageq(parseConfig("/home/pi/config.conf"),mqservice,handler);
//  mq messageq(parseConfig("alphaconfig.conf"),mqservice,handler,hardwareInterface);
  messageq.printMqConfig();
  std::cout << "\nSpawning rabbitmq handler...\n";
  messageq.multiThread();
  //messageq.endThreads();
  //      threads[1] = std::thread(sensorPoll,configParam,messageFlag);


  return 0;
}
