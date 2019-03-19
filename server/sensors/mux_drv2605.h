#include <cstdint>
class mux_drv2605 
{
#define MUX_ADDR 0x70

private:
	int sz_entries;
	//fuint8_t MUX_ADDR;
	int* entries;
	int i2cfd;
public:
	int set(int); //Set the current mux output (0-7)
	void addEntry(int);  //add new entry to entries list
	mux_drv2605();
	void setFd(int);
	int getFd();

	
};