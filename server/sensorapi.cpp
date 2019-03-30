
#include "sensors/Adafruit_Sensor.h"
#include "sensors/Adafruit_BNO055.h"
#include "sensors/utility/imumaths.h"
#include "sensors/DRV2605.h"
#include "sensors/mux_drv2605.h"
#include <cstdio>
#include <cstdlib>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <fstream>
#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <amqpcpp/linux_tcp.h>
#include <mutex>
#include <vector>
#include <string>
#include <thread>
std::mutex mtx;
std::thread threads[2];

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (330)

Adafruit_BNO055 bno = Adafruit_BNO055(55);

struct skate_config
{
  std::string mq_server;
  std::string mq_user;
  std::string mq_password;
  std::string mq_exchange;
  std::string mq_routingkey;
  std::string mq_queueName;
  std::string configVersion;
  std::string vhostName;
  std::string heartbeat;
}s_c;

int server_time;

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/


void buzz(int index)
{
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
}
void getConfig(std::string name)
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

  std::ifstream in (name);
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
        s_c.heartbeat = output[8];
        std::cout << "\nConfig Parse Succesfull" << std::endl;

      }
      else
      {
        std::cout << "\nConfig Parse Unsuccesfull" << std::endl << "Too few newlines, need 6 received " << output.size() <<  std::endl << 
        " File size of " << fileLen << std::endl;
        exit(-1);

      }

  
  }
  else
  {
    std::cout << "\nError: Could not open file at " << name << std::endl;
  }
  return;

}

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  printf("------------------------------------\n");
  printf  ("Sensor:       %d\n",sensor.name);
  printf  ("Driver Ver:   %d\n",sensor.version);
  printf  ("Unique ID:    %d\n",sensor.sensor_id);
  printf  ("Max Value:    %d\n",sensor.max_value);
  printf  ("Min Value:    %d\n",sensor.min_value);
  printf  ("Resolution:   %d",sensor.resolution);
  printf("------------------------------------\n");
  printf("");
  delay(500);
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void displaySensorStatus(void)
{
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  printf("");
  printf("System Status %d",system_status);
  printf("Self Test:     %d",self_test_results);
  printf("System Error:  %d",system_error);
  printf("");
  delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  printf("\t");
  if (!system)
  {
    printf("! ");
  }
  //TODO: Remove Arduino Specifc lines and classify this file. 
  /* Display the individual values */
  printf("Sys:%d\n",system);
  printf(" G:%d\n",gyro);
  printf(" A:%d\n",accel);
  printf(" M:%d\n",mag);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void sensorPoll(int config,bool * flag)
{
  std::cout << "\nStarting Polling Loop...\n";
  std::string temp = "";
  sensors_event_t event;
//  Adafruit_BNO055 bno = Adafruit_BNO055(55);

  std::vector<std::string> output;
  switch(config)    
    {

      case 1:
          while(*flag)
          {
          bno.getEvent(&event);
          temp = "(" + std::to_string(event.orientation.x) + "," + std::to_string(event.orientation.y) + "," + std::to_string(event.orientation.z) + ")";
          output.push_back(temp);
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

void setup()
{
  printf("Orientation Sensor Test\n");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    printf("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Optional: Display current status */
  displaySensorStatus();

  bno.setExtCrystalUse(true);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void messageParse(std::string msg)
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
    if(msg.find("Start_Poll") != std::string::npos)
    { 
      *messageFlag = true;
      if((pos = msg.find("Config:")) != std::string::npos)
      {

          std::string temp = msg.substr(pos,pos+1);
          configParam = atoi(temp.c_str());
          std::cout << "\nStarting sensor poll...\n"; 
         // threads[1] = std::thread(sensorPoll,configParam,messageFlag);

      }
    }
    else if (msg.find("Stop_Poll") != std::string::npos)
    {
      *messageFlag = false;
      std::cout << "\nPoll Stopping..\n";
//      threads[1] = std::thread(sensorPoll,configParam,messageFlag);

    }
    else if(msg.find("No_Poll") != std::string::npos)
    {


      if((pos = msg.find("Config:")) != std::string::npos)
      {

          std::string temp = msg.substr(pos,pos+1);
          configParam = atoi(temp.c_str());

      }
      if((pos = msg.find("Optional_Param")) != std::string::npos)
      {



      }
    }
    else if (msg.find("buzz 0") != std::string::npos)
    {
      buzz(0);
    }
    else if (msg.find("buzz 1") != std::string::npos)
    {
      buzz(1);
    }


  }
  return;
}




int mqLoop(int choice)
{

  /*
  std::string exchangeName = "skate_exchange";
  std::string queueName = "command_queue";
  std::string routingkey = "piAlpha";
  std::string amqp_connection_string = "amqp://piAlpha:258654as@192.168.1.109/";
*/
  /*  


      1- rabbitmqurl
      2- rabbitmq username
      3- rabbitmq password
      4- rabbitmq routing-key
      5- rabbitmq exchangeName
      6- rabbitmq queueName
      7- piConfig? (haptics or not, build id)
      8- vhost
      9- heartbeat refresh in ms
  */  std::cout << "\t\t Init RabbitMq Interface\t\t\n";
  int heartbeat = 0;
  std::string exchangeName = s_c.mq_exchange;
  std::string queueName = s_c.mq_queueName;
  std::string routingkey = s_c.mq_routingkey;
  std::string amqp_connection_string = "amqp://" + s_c.mq_user + ":" + s_c.mq_password + "@" + s_c.mq_server + "/" + s_c.vhostName;
  heartbeat = atoi(s_c.heartbeat.c_str());
  bool isLive = true;
  std::vector<std::string> output;
  std::string temp;
  boost::asio::io_service service(2);
    // handler for libev
  AMQP::LibBoostAsioHandler handler(service);
  AMQP::TcpConnection connection(&handler, AMQP::Address(amqp_connection_string));
   std::cout << "FD: " <<  connection.fileno() << std::endl;
    // we need a channel too
  AMQP::TcpChannel channel(&connection);
  channel.declareExchange(exchangeName, AMQP::fanout);
  channel.declareQueue(queueName);
  channel.bindQueue(exchangeName, queueName,routingkey);

  std::cout << "- Connection String: " << amqp_connection_string << std::endl;
  std::cout << "- Routing Key: " << routingkey << std::endl;
  std::cout << "- Queue Name: " << queueName << std::endl;

  switch(choice)
  {


    case 1:
    if(time(NULL) > server_time + heartbeat)
    {
    server_time = time(NULL);
    channel.publish(exchangeName,routingkey,"Hi");
    channel.commitTransaction();
    //connection.close();
    }
 
    break;

    // create a temporary queue
    /*
    channel.declareQueue(AMQP::exclusive).onSuccess([&connection](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
        
        // report the name of the temporary queue
        std::cout << "declared queue " << name << std::endl;
        
        // now we can close the connection
        connection.close();
    });
    */
    case 2:
  auto startCb = [](const std::string &consumertag) 
  {

    std::cout << "consume operation started" << std::endl;  
  };

// callback function that is called when the consume operation failed
  auto errorCb = [](const char *message) 
  {

      std::cout << "consume operation failed with error " << std::string(message) << std::endl;
  };

// callback operation when a message was received
  auto messageCb = [&channel](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) 
  {

      std::cout << "message received" << ": " << message.body() << std::endl;
      //Parse Message
      channel.ack(deliveryTag);

     // std::cout << (std::string)message.body();
      messageParse(std::string(message.body()));
      // acknowledge the message
  };

    channel.consume(queueName)
    .onReceived(messageCb)
    .onSuccess(startCb)
    .onError(errorCb);
     return service.run();
    break;
}




}



int main()
{

  getConfig("alphaconfig.conf");
  server_time = time(NULL);
  int deltaTime = 0;
  std::cout << "\nSpawning rabbitmq listener...\n";
  threads[0] = std::thread(mqLoop,2);
  threads[1] = std::thread(mqLoop,1);
  threads[0].join();
  threads[1].join();


  //listenLoop();
 // std::thread run_thread([&]{ listenLoop(); });
  //boost::thread run_thread(boost::bind(&boost::asio::io_service::run, boost::ref(listenLoop())));




}