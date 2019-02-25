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
#include <string.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
#include "bluez_server.h"
bluez_server::bluez_server()
{
	 my_bdaddr_any =  {0, 0, 0, 0, 0, 0};
 	 my_bdaddr_local = {0, 0, 0, 0xff, 0xff, 0xff};
}



sdp_session_t* bluez_server::register_service(uint8_t rfcomm_channel)
{
 // Adapted from http://www.btessentials.com/examples/bluez/sdp-register.c
    uint32_t svc_uuid_int[] = {   0x01110000, 0x00100000, 0x80000080, 0xFB349B5F };
    const char *service_name = "SkateComputer";
    const char *svc_dsc = "Routing for Skateboard Sensor Data";
    const char *service_prov = "nomde";

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid, svc_class_uuid;
    sdp_list_t *l2cap_list = 0,
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0,
               *access_proto_list = 0,
               *svc_class_list = 0,
               *profile_list = 0;
    sdp_data_t *channel = 0;
    sdp_profile_desc_t profile;
    sdp_record_t record = { 0 };
    sdp_session_t *session = 0;

    // set the general service ID
    sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
    sdp_set_service_id( &record, svc_uuid );

    char str[256] = "";
    sdp_uuid2strn(&svc_uuid, str, 256);
//    printf("Registering UUID %s\n", str);

    // set the service class
    sdp_uuid16_create(&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
    svc_class_list = sdp_list_append(0, &svc_class_uuid);
    sdp_set_service_classes(&record, svc_class_list);

    // set the Bluetooth profile information
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    profile_list = sdp_list_append(0, &profile);
    sdp_set_profile_descs(&record, profile_list);

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( &record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // register the RFCOMM channel for RFCOMM sockets
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( &record, access_proto_list );

    // set the name, provider, and description
    sdp_set_info_attr(&record, service_name, service_prov, svc_dsc);

    // connect to the local SDP server, register the service record,
    // and disconnect
    session = sdp_connect(&my_bdaddr_any, &my_bdaddr_local, SDP_RETRY_IF_BUSY);
    sdp_record_register(session, &record, 0);

    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );
    sdp_list_free( svc_class_list, 0 );
    sdp_list_free( profile_list, 0 );

    return session;
}

int bluez_server::init_server(int p)
{
	port = p;
	//sdp_session_t* session = register_service(port);
	session = register_service(port);
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s, client, bytes_read = -1;
    socklen_t opt = sizeof(rem_addr);
    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
   // printf("socket() returned %d\n", s);
    rem_socket = s;
    // bind socket to port 1 of the first available
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = my_bdaddr_any;
    loc_addr.rc_channel = (uint8_t) port;
    int r;
    r = bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
   // printf("bind() on channel %d returned %d\n", port, r);

    // put socket into listening mode
    r = listen(s, 1);
   // printf("listen() returned %d\n", r);

    //sdpRegisterL2cap(port);

    // accept one connection
   // printf("calling accept()\n");
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
 //   printf("accept() returned %d\n", client);
    rem_client = client;
    ba2str( &rem_addr.rc_bdaddr, buf );
  //  fprintf(stderr, "accepted connection from %s\n", buf);
    status = true;
}

std::string bluez_server::read_server()
{
	int bytes_read;
	char buf[1024] = { 0 };
    do {
        // read data from the client
        memset(buf, 0, sizeof(buf));
        bytes_read = read(rem_client, buf, sizeof(buf));
        if( bytes_read > 0 ) {
            printf("received [%s]\n", buf);
        }
    } while (bytes_read > 0);
 
 std::string out = std::string(buf);
return out;

}

void bluez_server::write_server(const char *message)
{
	   // send data to the client
         char messageArr[1024] = { 0 };
         int bytes_sent;
         sprintf(messageArr, message);
         bytes_sent = write(rem_client, messageArr, sizeof(messageArr));
         if (bytes_sent > 0) {
		bad_sends = 0;
//                 printf("sent [%s] : %d \n", messageArr,bytes_sent);
      }
	else if(bytes_sent < 0)
	{
	bad_sends++;
//	printf("%s\n", strerror(errno));
	if(bad_sends >= 10)
	{
		status = false;
	}

	return;
	}
}

void bluez_server::closeSocket()
{
	close(rem_client);
	close(rem_socket);
	sdp_close(session);
}
