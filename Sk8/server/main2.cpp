#include <stdio.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "bluetooth/bluetooth.h"
#include "bluetooth/hci.h"
#include "bluetooth/hci_lib.h"
#include <string>
#include <sys/socket.h>
#include "bluetooth/rfcomm.h"
#include <pigpio.h>
#include "bluetooth/sdp.h"
#include "bluetooth/sdp_lib.h"

#define ALTITUDE_ADDRESS 0x60
#define IMU_ADDRESS 0x68

int readSensor(int address,int time = 50)
{
	
	int fd;
	fd = open("/dev/i2c-1",O_RDWR);

	if(fd < 0)
	{
		printf("\nError opening file :%s\n",strerror(errno));
		return 1;
	}
	if(ioctl(fd,I2C_SLAVE,address) < 0)
	{
		printf("\nioctl error: %s\n",strerror(errno));
		return 1;
	}

	unsigned char buffer[60] = {0};

	//time in sec
	int lim = 0;
	int length = 1; //number of bytes to read
	while(lim < time)
	{
		if(read(fd,buffer,length) != length)
		{
			printf("\nFailed to read from the i2c bus at address %d\n",address);
		}
		else
		{
			printf("\rData Read: %s",buffer);
		}
		
		
		
		sleep(1);
		lim += 1;
	}
	
	return 0;
}

sdp_session_t *registerService()
{
	uint32_t service_uuid_int[] = {0,0,0,0xABCD};
	uint8_t rfcomm_channel = 11;
	const char* service_name = "Skate Computer";
	const char* service_dsc = "SKATE";
	const char* service_prov = "nomde";

	uuid_t root_uuid,l2cap_uuid,rfcomm_uuid,svc_uuid;
	sdp_list_t *l2cap_list = 0,
		   *rfcomm_list = 0,
		   *root_list = 0,
		   *proto_list = 0, 
		   *access_proto_list = 0;

	sdp_data_t *channel = 0,*psm = 0;

	sdp_record_t *record = sdp_record_alloc();

	//set the general service ID
	sdp_uuid128_create (&svc_uuid,&service_uuid_int);
	sdp_set_service_id(record, svc_uuid);

	//make the service record publicly browsable
	sdp_uuid16_create(&root_uuid,PUBLIC_BROWSE_GROUP);
	root_list = sdp_list_append(0, &root_uuid);
	sdp_set_browse_groups(record, root_list);

	//set l2cap information
	sdp_uuid16_create(&l2cap_uuid,L2CAP_UUID);
	l2cap_list = sdp_list_append(0,&l2cap_uuid);
	proto_list = sdp_list_append(0, l2cap_list);

	//set rfcomm information
	sdp_uuid16_create(&rfcomm_uuid,RFCOMM_UUID);
	channel = sdp_data_alloc(SDP_UINT8,&rfcomm_channel);
	rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
	sdp_list_append(rfcomm_list, channel);
	sdp_list_append(proto_list, rfcomm_list);

	//attach protocol information to service record
	access_proto_list = sdp_list_append(0, proto_list);
	sdp_set_access_protos(record, access_proto_list);

	//set the name, provider, and d escription
	sdp_set_info_attr(record,service_name,service_prov, service_dsc);
	int err = 0;
	sdp_session_t *session = 0;


	//connect to the local SDP server, register the service record, and disconnect
	bdaddr_t tempaddr = (bdaddr_t){{0,0,0,0,0,0}};
	bdaddr_t localaddr = (bdaddr_t) {{0, 0, 0, 0xff, 0xff, 0xff}};
//	bdaddr_t localaddr = *BDADDR_LOCAL;
	session = sdp_connect(&tempaddr,&localaddr,SDP_RETRY_IF_BUSY);
	err = sdp_record_register(session,record,0);
	
	if(err < 0)
	{
		printf("\nEncountered error %d\n",err);
		return NULL;
	}

	//cleanup
	//
	sdp_data_free(channel);
	sdp_list_free(l2cap_list,0);
	sdp_list_free(rfcomm_list,0);
	sdp_list_free(root_list,0);
	sdp_list_free(access_proto_list,0);

	return session;



}



void openBtSocket()
{


	registerService();
	

	struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
	char buf[1024] = {0};
	socklen_t opt = sizeof(rem_addr);

	int s,client,bytes_read;
	//allocate socket
	s = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);


	loc_addr.rc_family = AF_BLUETOOTH;
	//create temporary bdaddr  object
	bdaddr_t temp = (bdaddr_t){{0,0,0,0,0,0}};


	loc_addr.rc_bdaddr =temp;
	loc_addr.rc_channel = (uint8_t) 1;
	bind(s,(struct sockaddr *)&loc_addr,sizeof(loc_addr));
	
	//put socket into listening mode
	listen(s,1);

	//accept the first connection
	client = accept(s,(struct sockaddr *)&rem_addr,&opt);
	ba2str(&rem_addr.rc_bdaddr,buf);
	fprintf(stderr,"accepted connection from %s\n",buf);
	memset(buf,0,sizeof(buf));

	//read data from client 
	//
	bytes_read = read(client,buf,sizeof(buf));
	if(bytes_read > 0)
	{
		printf("received [%s]\n",buf);
	}

	//close connection
	close(client);
	close(s);
	return;
}


void scanForBt()
{
inquiry_info *ii = NULL;
int max_rsp,num_rsp; 
int dev_id,sock,len,flags;
int i;
char addr[19] = {0};
char name[248] = {0};
dev_id = hci_get_route(NULL); //we pass null here because it will return default bluetooth device
sock = hci_open_dev(dev_id);
if(dev_id < 0 || sock < 0)
{
	perror("error opening socket");
	exit(1);
}
len = 8;
max_rsp = 255;
flags = IREQ_CACHE_FLUSH; //this constant will signal to bluetooth device to flush previous scan results
ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

num_rsp = hci_inquiry(dev_id,len,max_rsp,NULL,&ii,flags);
if(num_rsp < 0)
{
	perror("hci inquiry has failed..");
}
for(i = 0; i < num_rsp;i++)
{
	ba2str(&(ii+i)->bdaddr,addr);
	memset(name,0,sizeof(name));
	if(hci_read_remote_name(sock,&(ii+i)->bdaddr,sizeof(name),name,0) < 0)
	{
		strcpy(name,"[unknown]");
	}
	printf("\n%s %s", addr, name);
}
free(ii);
close(sock);
return;
}

void alert(int gpio,int level, uint32_t tick)
{
	static uint32_t lastTick = 0;
	if(lastTick)
	{
		printf("%d %.2f\n",level, (float)(tick-lastTick)/1000000.0);
	}
	else
	{
		printf("0 0.0\n");
	}
	
	lastTick = tick;
}

int main() {
int data_length;

openBtSocket();

//int lastReturn = 0;

//readSensor(IMU_ADDRESS);
//return 0;



/*
unsigned hall = 14;
int sec = 60;
if(gpioInitialise() < 0)
{
	printf("\nGPIO failed to initialize.. exiting\n");
	return 1;
}
gpioSetMode(hall,PI_INPUT);
gpioSetPullUpDown(hall,PI_PUD_UP);
gpioSetAlertFunc(hall,alert);
sleep(sec);
gpioTerminate();
*/
//scanForBt();

/*

if( registerService() != NULL)
{
	printf("Success\n");
}
else
{
	printf("Failure\n");
}
*/

return 0;


}

