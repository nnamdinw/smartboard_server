/**
 *  LibBoostAsio.cpp
 * 
 *  Test program to check AMQP functionality based on Boost's asio io_service.
 * 
 *  @author Gavin Smith <gavin.smith@coralbay.tv>
 *
 *  Compile with g++ -std=c++14 libboostasio.cpp -o boost_test -lpthread -lboost_system -lamqpcpp
 */

/**
 *  Dependencies
 */
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>


#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>


bool parse();

int main()
{

	 boost::asio::io_service service(2);
	 //boost::asio::io_service::work work(service);
//	 service.run();

    // handler for libev
    AMQP::LibBoostAsioHandler handler(service);
	

	AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    
    // we need a channel too
    AMQP::TcpChannel channel(&connection);
    channel.declareExchange("my-exchange", AMQP::fanout);
    channel.declareQueue("my-queue");

    channel.bindQueue("my-exchange", "my-queue", "my-routing-key");



	auto startCb = [](const std::string &consumertag) {

    std::cout << "consume operation started" << std::endl;
};

// callback function that is called when the consume operation failed
auto errorCb = [](const char *message) {

    std::cout << "consume operation failed" << std::endl;
};

// callback operation when a message was received
auto messageCb = [&channel](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {

    std::cout << "message received" << ": " << message.body() << std::endl;
    //std::cout << (std::string)message.body();

    // acknowledge the message
    channel.ack(deliveryTag);
};

// start consuming from the queue, and install the callbacks
	std::cout << "\nAwaiting messages on queue...\n:";
	channel.consume("my-queue")
    .onReceived(messageCb)
    .onSuccess(startCb)
    .onError(errorCb);

    return service.run();


}