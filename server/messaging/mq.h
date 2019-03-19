#include "mq_includes.h"
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
      std::string mq_queueName;
      std::string configVersion;
      std::string vhostName;
      int heartbeat;
    };

    void printMqConfig();
    int mqConsume();
    void setFunc(int,std::function<void(int)>);
    int mqPublish(std::string);
    void exec(int);
    mq(mq::skate_config configIn,boost::asio::io_service& ioserv_, AMQP::LibBoostAsioHandler& boosthandler) :
      asio_service(ioserv_),
      amqp_boost_handler(boosthandler),
      amqp_TCP(&amqp_boost_handler, AMQP::Address("amqp://" + configIn.mq_user + ":" + configIn.mq_password + "@" + configIn.mq_server + "/" + configIn.vhostName)),
      amqp_channel(&amqp_TCP)
      {

        //parseConfig(configDir);
        exchangeName = configIn.mq_exchange;
        queueName = configIn.mq_queueName;
        routingkey = configIn.mq_routingkey;
        
        //s_c = config
       //heartbeatTick = atoi(s_c.heartbeat.c_str());
        //asio_service = boost::asio::io_service(2);
        //amqp_boost_handler = AMQP::LibBoostAsioHandler(io_serv);
        ///amqp_TCP =  AMQP::TcpConnection(&boosthandler,AMQP::Address(amqp_connection_string));
        //amqp_channel(&amqp_TCP);

        amqp_channel.declareExchange(exchangeName, AMQP::fanout);
        amqp_channel.declareQueue(queueName);
        amqp_channel.bindQueue(exchangeName, queueName,routingkey);

    } //pass dir to config file


    private:
      std::string exchangeName,queueName,routingkey,amqp_connection_string;
      boost::asio::io_service& asio_service;
      AMQP::LibBoostAsioHandler& amqp_boost_handler;
      AMQP::TcpConnection amqp_TCP;
      AMQP::TcpChannel amqp_channel;
      std::string commandTable[SZ_COMMANDTABLE];
      skate_config s_c;
      int heartbeatTick;
      std::atomic<bool> poll;
      std::function<void(int)> b_f;
     // void parseCommands(std::string);
      void parseConfig(std::string);
      void messageParse(std::string);

};
