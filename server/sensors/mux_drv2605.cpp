#include "mux_drv2605.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cstdio>
mux_drv2605::mux_drv2605(uint8_t addr)
{
	MUX_ADDR = addr;
	sz_entries = 0;
	entries = nullptr;
	setFd(wiringPiI2CSetup(MUX_ADDR));

}

void mux_drv2605::addEntry(uint8_t index)
{
	sz_entries++;
	*uint8_t old = entries;
	*uint8_t temp = new uint8_t [sz_entries];
	int i = 0;
	for(; i < sz_entries - 1;i++)
	{
		temp[i] = old[i];
	}
	temp[i] = index;
	
	aentries = temp;
	
	delete old;

	return;

}

void mux_drv2605::set(int index)
{
	int i = 0;
	while(i < sz_entries)
	{
		if(index == entries[i])
		{
				wiringPiI2CWrite(getFd(),index);
				return;
		}
	}

	printf("mux_drv2605 error: Index %d doesn't exist ",index);
	return;
}

void mux_drv2605::setFd(int filedescriptor)
{
	i2cfd = filedescriptor;
}

int mux_drv2605::getFd()
{
	return i2cfd;
}

