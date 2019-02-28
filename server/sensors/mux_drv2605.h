#include <cstdint>
class mux_drv2605 
{

private:
	int sz_entries;
	uint8_t MUX_ADDR;
	*uint8_t entries;
	int i2cfd;
public:
	void set(int); //Set the current mux output (0-7)
	void addEntry(uint8_t);  //add new entry to entries list
	mux_drv2605(uint8_t = 70);
	void setFd(int);
	int getFd();

	
}