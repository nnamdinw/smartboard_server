#include "mq.h"

void mq::parseConfig(std::string name)
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
   		  //std::istringstream(s_c.heartbeat) >> heartbeatTick;
        amqp_connection_string = "amqp://" + s_c.mq_user + ":" + s_c.mq_password + "@" + s_c.mq_server + "/" + s_c.vhostName;

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
  return;

}

 void mq::messageParse(std::string msg)
 {

	  size_t pos = 0;
	  int configParam = 0;
	  bool* messageFlag = NULL;
  /*
      Pi_Control,Start_Poll/Stop_Poll/No_Poll,Config Preset,[Optional Input]
  */
  pos = msg.find("Pi_Control");
  if(pos != std::string::npos)
  { 

      if((pos = msg.find("Set_Config_")) != std::string::npos)
      {

          std::string temp = msg.substr(pos,pos+1);
          configParam = atoi(temp.c_str());
          std::cout << "\nStarting sensor poll using config: " << configParam; 
          poll = true;
         // threads[1] = std::thread(sensorPoll,configParam,messageFlag);

      }

    if(msg.find("Start_Poll") != std::string::npos)
    { 
      //*messageFlag = true;
      skateInterface.togglePoll();
      std::cout << "\nStart Poll message received\n";
    }
    if (msg.find("Stop_Poll") != std::string::npos)
    {
      //*messageFlag = false;
      std::cout << "\nPoll Stopping..\n";
      skateInterface.togglePoll();
//      threads[1] = std::thread(sensorPoll,configParam,messageFlag);

    }
    else if (msg.find("buzz_0") != std::string::npos)
    {
   //   buzz(0);
      //exec(0);
      skateInterface.buzz();
    }
    else if (msg.find("buzz_1") != std::string::npos)
    {
   //   buzz(1);
          skateInterface.buzz();

    }


  }
  return;
}
void mq::signalPublish(std::string in)
{
  mustPublish = true;
  publish_message = in;
}

void mq::endPublish()
{
  mustPublish = false;
}
int mq::multiThread()
{
  std::thread t1(&mq::mqConsume,this);
  std::thread t2(&mq::mqPublish,this);
  //std::thread t3(&neoskate::poll,this->skateInterface);

  t1.join();
  t2.join();
 // t3.join();
}
int mq::mqConsume()
{
  auto startCb = [&](const std::string &consumertag) 
  {

    std::cout << "consume operation started" << std::endl;  
  };

// callback function that is called when the consume operation failed
  auto errorCb = [&](const char *message) 
  {

      std::cout << "consume operation failed with error " << std::string(message) << std::endl;
  };

// callback operation when a message was received
  auto messageCb = [&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) 
  {

      std::cout << "message received" << ": " << message.body() << std::endl;
      //Parse Message
      amqp_channel.ack(deliveryTag);

     // std::cout << (std::string)message.body();
      messageParse(std::string(message.body()));
      // acknowledge the message
  };

    amqp_channel.consume(queueName)
    .onReceived(messageCb)
    .onSuccess(startCb)
    .onError(errorCb);
     return asio_service.run();
}

int mq::mqPublish()
{
    while(mustPublish)
    {
          amqp_channel.publish(exchangeName,routingkey,publish_message);
          amqp_channel.commitTransaction();
          mustPublish = false;
    }

    return asio_service.run();

}

void mq::setFunc(int index, std::function<void(int)> func)
{
  //b_f = func;
}

void mq::exec(int index)
{
  //b_f();
  return;
}