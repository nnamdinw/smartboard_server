#include "mux_drv2605.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cstdio>
mux_drv2605::mux_drv2605()
{
	sz_entries = 0;
	entries = nullptr;
	setFd(wiringPiI2CSetup(MUX_ADDR));

}

void mux_drv2605::addEntry(int index)
{
	sz_entries++;
	int* old = entries;
	int* temp = new int [sz_entries];
	int i = 0;
	for(; i < sz_entries - 1;i++)
	{
		temp[i] = old[i];
	}
	temp[i] = index;
	
	entries = temp;
	
	delete old;

	return;

}

int mux_drv2605::set(int index)
{
	int i = 0;
	while(i < sz_entries)
	{
		if(index == entries[i])
		{
				wiringPiI2CWrite(getFd(),1 << index);
				return wiringPiI2CRead(getFd());
		}
		i++;
	}

	printf("mux_drv2605 error: Index %d doesn't exist ",index);
	return -1;
}

void mux_drv2605::setFd(int filedescriptor)
{
	i2cfd = filedescriptor;
}

int mux_drv2605::getFd()
{
	return i2cfd;
}

