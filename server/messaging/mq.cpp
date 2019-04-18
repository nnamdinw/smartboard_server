#include "mq.h"

const std::string mq::onSuccess = "publish_ok";
const std::string mq::onFail = "publish_error";
//const enum commands = {}

void mq::printMqConfig()
{
  std::cout << "- Connection String: " << amqp_connection_string << std::endl;
  std::cout << "- Routing Key: " << routingkey << std::endl;
  std::cout << "- Queue Name to: " << queueNameTo << std::endl;
  std::cout << "- Queue Name from: " << queueNameFrom << std::endl;
  std::cout << "- Error Level: " << skateInterface.getErrorState();
  return;
}

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
        amqp_connection_string = "amqp://" + s_c.mq_user + ":" + s_c.mq_password + "@" + s_c.mq_server + "/" + s_c.vhostName;

        //std::cout << "\nConfig Parse Succesfull" << std::endl;
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
  return;

}
void mq::closeChannels()
{
   amqp_channel_to.close();
  amqp_channel_from.close();
}
 void mq::messageParse(std::string msg)
 {

	  size_t pos = 0;
	  int param = 0;
	  bool* messageFlag = NULL;

  /*
      Pi_Control,Start_Poll/Stop_Poll/No_Poll,Config Preset,[Optional Input]
  */
  pos = msg.find("Pi_Control");
  if(pos != std::string::npos)
  { 

      if((pos = msg.find("Set_Config_")) != std::string::npos)
      {

          std::string temp;
          temp = msg.substr(msg.length() - 1);
          //std::cout << "Parsed message " << temp << std::endl;
          param = atoi(temp.c_str());
          skateInterface.setConfig(param);
          //poll = true;
         // threads[1] = std::thread(sensorPoll,configParam,messageFlag);

      }

    if(msg.find("Start_Poll") != std::string::npos)
    { 
      //*messageFlag = true;
      skateInterface.setPoll(true);
      //std::cout << "\nStart Poll message received\n";
    }
    if (msg.find("Stop_Poll") != std::string::npos)
    {
      //*messageFlag = false;
      if(skateInterface.isNewPoll())
      {
        //signalPublish("Pi_Message_Logs");
        signalPublish(skateInterface.getLogs());
      }
      //std::cout << "\nPoll Stopping..\n";
      skateInterface.setPoll(false);
      skateInterface.setNewPoll(false); //reset flag cuz we've pulled new logs

//      threads[1] = std::thread(sensorPoll,configParam,messageFlag);

    }
    if (msg.find("Stream_Status") != std::string::npos)
    {

      signalPublish("Pi_Message_StreamEnabled_" + skateInterface.getStreamStatus());

    }
    if (msg.find("Enable_Stream") != std::string::npos)
    {
      //std::cout << "\nstream start message received.. " << std::endl;
      skateInterface.setPollStream(true);

    }
    if (msg.find("Disable_Stream") != std::string::npos)
    {
      //std::cout << "\nstream stop message received.. " << std::endl;

      skateInterface.setPollStream(false);

    }
    if (msg.find("Calibrate_IMU") != std::string::npos)
    {

      //skateInterfaceCalibrate();
      std::thread temp (&mq::skateInterfaceCalibrate,this);
      temp.detach();
      signalPublish("Pi_Message_CalibrationComplete");

    }
    if (msg.find("Send_Calibration_Status") != std::string::npos)
    {

      signalPublish("Pi_Message_Cal_" + skateInterface.getCalibrationProgress());

    }
    else if (msg.find("indicate") != std::string::npos)
    {
   //   buzz(0);
      //exec(0);
      skateInterface.indicate();
    }
    else if (msg.find("buzz_1") != std::string::npos)
    {
   //   buzz(1);
          skateInterface.buzz();

    }
     else if (msg.find("Request_Log") != std::string::npos)
     {
          std::string temp;
          temp = msg.substr(msg.length() - 1);
          //std::cout << "Parsed message " << temp << std::endl;
          param = atoi(temp.c_str());
          if(param < skateInterface.getSzLogs())
          {
            //ok seems good send log over rabbitmq
            signalPublish("Pi_Message_Log_Data_" + skateInterface.fetchLog(param));
            //signalPublish();

          }
          else
          {
              signalPublish("Pi_Message_Error_Log_DoesNotExist");

          }
     }

     else if (msg.find("Ping") != std::string::npos)
    {
   //   buzz(1);
        signalPublish("Pong");
        //endPublish();

    } 

     else if (msg.find("Send_Config_") != std::string::npos)
    {
   //   buzz(1);
        signalPublish(getPiConfig());
        //endPublish();
    } 

    else if(msg.find("Terminate") != std::string::npos)
    { 
      closeChannels();
      exit(1);
    }



  }
  return;
}

std::string mq::getPiConfig()
{
  std::string jsonOut = "{\"name\":\"" + s_c.mq_user+ "\", \"config\":\"" + s_c.configVersion + "\",\"calibration\":\"" + std::to_string(skateInterface.getCalibrationStatus()) + "\"}";
  return jsonOut;

  /*
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
  {"name":"s_c.mq_user", "config":configVersion}
    

  */

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
void mq::skateInterfacePoll()
{
  //std::cout << "Launching poll thread";

  if(skateInterface.poll() == -1)
  {
    signalPublish("Pi_Message_PollError");
  }
  else
  {
    signalPublish("Pi_Message_PollEnd");
  }
    std::cout << "Ending poll thread";

}

void mq::skateInterfaceCalibrate()
{
  skateInterface.setCalibrating(true);
  skateInterface.calibrateBNO055();
  skateInterface.setCalibrating(false);

}
int mq::multiThread()
{


  t1 = std::thread(&mq::mqConsume,this);

  t2 = std::thread(&mq::mqPublish,this);

  t3 = std::thread(&mq::skateInterfacePoll,this);

  t2.detach();
  t3.detach();
  t1.join();


}
int mq::mqConsume()
{
  auto startCb = [&](const std::string &consumertag) 
  {
          std::cout << "consume operation started" << std::endl;  
          skateInterface.setErrorState(1);
  };

// callback function that is called when the consume operation failed
  auto errorCb = [&](const char *message) 
  {

      std::cout << "consume operation failed with error " << std::string(message) << std::endl;
      skateInterface.setErrorState(-1);
  };

// callback operation when a message was received
  auto messageCb = [&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) 
  {
      //std::cout << "message received" << ": " << message.body() << std::endl;
          // acknowledge the message
      amqp_channel_to.ack(deliveryTag);
     // std::cout << (std::string)message.body();
            //Parse Message
      messageParse(std::string(message.body(),message.bodySize()));
  };

    amqp_channel_to.consume(queueNameTo)
    .onReceived(messageCb)
    .onSuccess(startCb)
    .onError(errorCb);
     return asio_service.run();
}

void mq::waitThreads()
{



 // amqp_channel_to.close();
  //amqp_channel_from.close();

}
int mq::mqPublish()
{
  bool ok = true;
  publish_message = "Pi_Hello";
  while(1)
  {
    if(mustPublish || ok)
    {     
          ok = false;
          amqp_channel_from.startTransaction();
          amqp_channel_from.publish(exchangeName,routingkey,publish_message);
          amqp_channel_from.commitTransaction().onSuccess([]() {
            //std::cout << onSuccess << std::endl;  
          })
          .onError([](const char* message){
          std::cout << onFail << std::endl;         });
          mustPublish = false;
    }
    if(skateInterface.isCalibrating())
    {
          amqp_channel_from.startTransaction();
          amqp_channel_from.publish(exchangeName,routingkey,"Pi_Message_CalibrationFrame_" + skateInterface.getCalibrationProgress());
          amqp_channel_from.commitTransaction().onSuccess([]() {
               })
          .onError([](const char* message){
          std::cout << onFail << std::endl;         });
    }
    if(skateInterface.getStreamStatus() && skateInterface.newPollData())
    {
          amqp_channel_from.startTransaction();
          amqp_channel_from.publish(exchangeName,routingkey,"Pi_Message_Poll_Frame_" + skateInterface.getFrame());
          amqp_channel_from.commitTransaction().onSuccess([]() {
               })
          .onError([](const char* message){
          std::cout << onFail << std::endl;         });
    }


  }

  return 1;
}
