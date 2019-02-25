#include <string>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

class bluez_server {

//POSIX threads?

private:
	bdaddr_t my_bdaddr_any; 
 	bdaddr_t my_bdaddr_local;
 	sdp_session_t* session;
 	int rem_client,port,rem_socket,bad_sends;
	

public:
	bluez_server();
	void closeSocket();
	sdp_session_t *register_service(uint8_t rfcomm_channel);
	int init_server(int);
	std::string read_server();
	void write_server(const char *message);
	bool status;

};
