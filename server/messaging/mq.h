#include "mq_includes.h"
//#include "neoskate.h"
#define SZ_COMMANDTABLE 7
class mq
{



public:
    struct skate_config
    {
      std::string mq_server;
      std::string mq_user;
      std::string mq_password;
      std::string mq_exchange;
      std::string mq_routingkey;
      std::string mq_queueName_to; // to pi from web
      std::string mq_queueName_from; //from pi to web
      std::string configVersion;
      std::string vhostName;
      int heartbeat;
    };
    void skateInterfacePoll();
    void skateInterfaceCalibrate();
    void signalPublish(std::string);
    void endPublish();
    void printMqConfig();
    int mqConsume();
    //void setFunc(int,std::function<void(int)>);
    int mqPublish();
    //void exec(int);
    int multiThread();
    void waitThreads();
    std::string getPiConfig();
    //~mq();
    mq(mq::skate_config configIn,boost::asio::io_service& ioserv_, AMQP::LibBoostAsioHandler& boosthandler/*,neoskate& nskate*/) :
      asio_service(ioserv_),
      amqp_boost_handler(boosthandler),
      amqp_TCP(&amqp_boost_handler, AMQP::Address("amqp://" + configIn.mq_user + ":" + configIn.mq_password + "@" + configIn.mq_server + "/" + configIn.vhostName)),
      amqp_channel_to(&amqp_TCP),
      amqp_channel_from(&amqp_TCP)/*,
      skateInterface(nskate)*/
      {

        //parseConfig(configDir);
        exchangeName = configIn.mq_exchange;
        queueNameTo = configIn.mq_queueName_to;
        queueNameFrom = configIn.mq_queueName_from;
        routingkey = configIn.mq_routingkey;
        s_c = configIn;
       //heartbeatTick = atoi(s_c.heartbeat.c_str());
        //asio_service = boost::asio::io_service(2);
        //amqp_boost_handler = AMQP::LibBoostAsioHandler(io_serv);
        ///amqp_TCP =  AMQP::TcpConnection(&boosthandler,AMQP::Address(amqp_connection_string));
        //amqp_channel(&amqp_TCP);

        amqp_channel_to.declareExchange(exchangeName, AMQP::direct);
        
        amqp_channel_to.declareQueue(queueNameTo);
        amqp_channel_from.declareQueue(queueNameFrom);

        amqp_channel_to.bindQueue(exchangeName, queueNameTo,routingkey);
        amqp_channel_from.bindQueue(exchangeName,queueNameFrom,routingkey);
        mustPublish = false;
        if(s_c.configVersion == "1")
        {
          skateInterface.enableLED();
        }
        if(s_c.configVersion == "0")
        {
          skateInterface.enableHaptics();
        }
        amqp_connection_string = "amqp://" + s_c.mq_user + ":" + s_c.mq_password + "@" + s_c.mq_server + "/" + s_c.vhostName;

        //onSuccess = "ok";
        //onFail = "error";

    } //pass dir to config file


    private:
      std::thread t1,t2,t3; //send, rec, poll & file io
      std::string exchangeName,queueNameTo,queueNameFrom,routingkey,amqp_connection_string;
      static const std::string onSuccess;
      static const std::string onFail;
      boost::asio::io_service& asio_service;
      AMQP::LibBoostAsioHandler& amqp_boost_handler;
      AMQP::TcpConnection amqp_TCP;
      AMQP::TcpChannel amqp_channel_to;
      AMQP::TcpChannel amqp_channel_from;
      std::string commandTable[SZ_COMMANDTABLE];
      skate_config s_c;
      int heartbeatTick;
      std::atomic<bool> poll;
      std::atomic<bool> mustPublish;
      std::string publish_message;
      void parseConfig(std::string);
      void messageParse(std::string);
      neoskate skateInterface;


};
