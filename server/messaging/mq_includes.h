

#include "../neoskate.h"
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
//#include <vector>
//#include <string>
#include <thread>