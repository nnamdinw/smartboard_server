#include "neoskate.h"
#include <iostream>

int main()
{
	neoskate pi;
	int userpin = 0;
	std::string userin = "";
	std::cout << "\nEnter pin: ";
	std::cin >> userpin;
	pi.initLED(userpin,userpin);
	std::cout << "\nEnter H for High, L for Low: ";
	std::cin >> userin;

	if(userin == "H" || userin == "h")
	{
		pi.setLED(1,TRUE);
	}
	if(userin == "L" || userin == "l")
	{
		pi.setLED(1,FALSE);
	}
}