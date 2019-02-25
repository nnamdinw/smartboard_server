
#include <iostream>
#include "MPU9250pi.h"
#include "quaternionFilters.h"

#define DEG_TO_RAD M_PI/180
#define RAD_TO_DEG 180/M_PI

int main()
{
	int address = 0x68;
	int reg = 0x3B;
//	int data[8] = {NULL};
	uint8_t data;
	int16_t data_p;
	int error = 0;
	int whoami = 0x75; //should return 0x71
	int device;
	float selfTest[6] = {0};
	device = wiringPiI2CSetup(address);
	if(device < 0)
	{
//		error = errno();
		std::cout << "Error setting up i2c at " << address << "\nerror: " << error;
		
		 return 1;
	}
	std::cout << "\nDevice fd : " << device << std::endl;;
//	data = wiringPiI2CReadReg16(device,reg);
	data = wiringPiI2CReadReg8(device,whoami);
	if(data == 0x71)
	{
		std::cout << "\nmpu9520 online.\n";
	}
	else
	{
		std::cout << "\nmpu9520 has failed to initialize.\n";
		return 1;
	}
	
	MPU9250 imu;
	imu.setFd(device);
	data_p = imu.readTempData();
	if(data_p > -1)
	{
		std::cout << "temperature: " <<  data_p;
	}
	else
	{
		std::cout << "\nReading temp data failed..\n";
		return 1;
	}
	imu.MPU9250SelfTest(selfTest);
	if(selfTest)
	{
		std::cout << "\nX-Axis self test: acceleration trim within " << selfTest[0] << "% of factory value\n";
		std::cout << "\nY-Axis self test: acceleration trim within " << selfTest[1] << "% of factory value\n";
		std::cout << "\nZ-Axis self test: accleration trim within " << selfTest[2] << "% of factory value\n";
		std::cout << "\nX-Axis self test: Gyration trim within " << selfTest[3] << "% of factory value";
		std::cout << "\nY-Axis self test: Gyration Trim within " << selfTest[4] << "% of factory value";
		std::cout << "\nZ-Axis self test: Gyration trim within " << selfTest[5] << "% of factory value";

	}
	else
	{
		std::cout << "\n9250 self test failed\n";
		return 1;
	}

	imu.calibrateMPU9250(imu.gyroBias,imu.accelBias);
//	std::cout << "\nMPU9250 Bias\n" << "X
	imu.initMPU9250();
	imu.initAK8963(imu.magCalibration);
	std::cout << "\nAK8963 initialized for active data mode" << std::endl;

	while(true)
	{
		if(imu.readByte(MPU9250_ADDRESS,INT_STATUS) & 0X01)
		{
			imu.readAccelData(imu.accelCount); //read xyz adc values
			imu.getAres();
			imu.ax = (float)imu.accelCount[0]*imu.aRes;
			imu.ay = (float)imu.accelCount[1]*imu.aRes;
			imu.az = (float)imu.accelCount[2]*imu.aRes;

			imu.readGyroData(imu.gyroCount);
			imu.getGres();

			//calculate gyro value into actual degrees per second
			imu.gx = (float)imu.gyroCount[0]*imu.gRes;
			imu.gy = (float)imu.gyroCount[1]*imu.gRes;
			imu.gz = (float)imu.gyroCount[2]*imu.gRes;

			imu.readMagData(imu.magCount);
			imu.getMres();

			//user encironmental x axis correction is milliGauss, should be automatically calculated
			imu.magbias[0] = +470;
			imu.magbias[1] = +120.;
			imu.magbias[2] = +125.;

			//get actual magnetometer value
			imu.mx = (float)imu.magCount[0]*imu.mRes*imu.magCalibration[0] - imu.magbias[0];
			imu.my = (float)imu.magCount[1]*imu.mRes*imu.magCalibration[1] - imu.magbias[1];
			imu.mz = (float)imu.magCount[2]*imu.mRes*imu.magCalibration[2] - imu.magbias[2];
		}
		imu.updateTime();
		MahonyQuaternionUpdate(imu.ax, imu.ay, imu.az, imu.gx*DEG_TO_RAD,imu.gy*DEG_TO_RAD,imu.gz*DEG_TO_RAD,imu.my, imu.mx, imu.mz, imu.deltat);
		

		imu.delt_t = millis() - imu.count;

		if(imu.delt_t > 500)
		{
			//std::cout << "\rax = " << (int)1000*imu.ax << "\nay = " << (int)1000*imu.ay << "\naz = " << (int)1000*imu.az << std::endl;
			//std::cout << "\nmG\ngx = " << imu.gx << "\ngy = " << imu.gy << "\ngz = " << imu.gz;
			//std::cout << "\ndeg /s" << "mx = " << imu.mx << "\nmy = " << imu.my << "\nmz = " << imu.mz;
			//std::cout << "\rmG" << "\nq0 = " << *getQ() << "\nqx = " << *(getQ() + 1);
			//std::cout << "\rqy = " << (*getQ() + 2);
			//std::cout << "\rqz = " << (*getQ() + 3);
			//
//			printf("\rQ0:%3.4f\tQx1:%3.2f\tQy:%3.2f\tQz:%3.2f",*getQ(),(*getQ() + 1), (*getQ()+2), (*getQ()+3));  //print quaternion

			imu.yaw = atan2(2.0f * (*(getQ()+1) * *(getQ() + 2) + *getQ()* *(getQ() +3)),*getQ() * *getQ() + *(getQ()+1) * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) - *(getQ()+3) * *(getQ()+3));
			imu.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ() * *(getQ()+2)));
			imu.roll =atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2) * *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1) * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) + *(getQ()+3) * *(getQ()+3));	
			imu.pitch *= RAD_TO_DEG;
			imu.yaw *= RAD_TO_DEG;
			imu.yaw -= 8.5;
		       	imu.roll *= RAD_TO_DEG;

			printf("\rpitch: %3.2f \tyaw: %3.2f \troll: %3.2f\tax: %3.2f \tay: %3.2f \taz: %3.2f",imu.pitch,imu.yaw,imu.roll,imu.ax,imu.ay,imu.az);
		
		}
		
	

	}



/*	for(int i = 0; i < 8;i++)
	{
		std::cout << "Index " << i << ": " << data[i] << std::endl;
	}

*/	
	return 0;
}
