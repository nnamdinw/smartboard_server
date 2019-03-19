#include "sensors/DRV2605.h"
#include "sensors/mux_drv2605.h"
#include <iostream>

int main()
{
	DRV2605 hap;
	mux_drv2605 mux;
	mux.addEntry(1);
	mux.addEntry(2);


	//setup
	mux.set(1);
	hap.begin();
	hap.selectLibrary(1);
	hap.setMode(DRV2605_MODE_INTTRIG);

	mux.set(2);
	hap.begin();
	hap.selectLibrary(1);
	hap.setMode(DRV2605_MODE_INTTRIG);
  	//wiringPiSetup();

	uint8_t effect = 1;
	/*
	while(effect < 117)
	{
		std::cout << "\nNow playing effect: " << (int)effect << std::endl;
		hap.setWaveform(0,effect);
		hap.setWaveform(1,0);
		hap.go();
		delay(500);
		effect++;
	}
	*/
	int userin = 0;
	while(true)
	{

		std::cout << "\nChoose MUX output 0-7: ";
		std::cin >> userin;

	
		std::cout << "Status: " << mux.set(userin) << std::endl;
//		effect = 12;
		hap.setWaveform(0,effect);
		hap.setWaveform(1,0);
		hap.go();
		delay(100);

	}


	
	return 0;

}
