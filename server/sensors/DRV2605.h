#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cstdint>
#include <stdlib.h>
#include <math.h>

#define DRV2605_ADDR 0x5A

#define DRV2605_REG_STATUS 0x00
#define DRV2605_REG_MODE 0x01
#define DRV2605_MODE_INTTRIG  0x00
#define DRV2605_MODE_EXTTRIGEDGE  0x01
#define DRV2605_MODE_EXTTRIGLVL  0x02
#define DRV2605_MODE_PWMANALOG  0x03
#define DRV2605_MODE_AUDIOVIBE  0x04
#define DRV2605_MODE_REALTIME  0x05
#define DRV2605_MODE_DIAGNOS  0x06
#define DRV2605_MODE_AUTOCAL  0x07
#define DRV2605_REG_RTPIN 0x02
#define DRV2605_REG_LIBRARY 0x03
#define DRV2605_REG_WAVESEQ1 0x04
#define DRV2605_REG_WAVESEQ2 0x05
#define DRV2605_REG_WAVESEQ3 0x06
#define DRV2605_REG_WAVESEQ4 0x07
#define DRV2605_REG_WAVESEQ5 0x08
#define DRV2605_REG_WAVESEQ6 0x09
#define DRV2605_REG_WAVESEQ7 0x0A
#define DRV2605_REG_WAVESEQ8 0x0B
#define DRV2605_REG_GO 0x0C
#define DRV2605_REG_OVERDRIVE 0x0D
#define DRV2605_REG_SUSTAINPOS 0x0E
#define DRV2605_REG_SUSTAINNEG 0x0F
#define DRV2605_REG_BREAK 0x10
#define DRV2605_REG_AUDIOCTRL 0x11
#define DRV2605_REG_AUDIOLVL 0x12
#define DRV2605_REG_AUDIOMAX 0x13
#define DRV2605_REG_RATEDV 0x16
#define DRV2605_REG_CLAMPV 0x17
#define DRV2605_REG_AUTOCALCOMP 0x18
#define DRV2605_REG_AUTOCALEMP 0x19
#define DRV2605_REG_FEEDBACK 0x1A
#define DRV2605_REG_CONTROL1 0x1B
#define DRV2605_REG_CONTROL2 0x1C
#define DRV2605_REG_CONTROL3 0x1D
#define DRV2605_REG_CONTROL4 0x1E
#define DRV2605_REG_VBAT 0x21
#define DRV2605_REG_LRARESON 0x22


class DRV2605
{

private:
	int i2cfd;

public:
	DRV2605();
	bool begin();
  	//boolean begin(void);  

	void writeRegister8(uint8_t reg, uint8_t val);
	unsigned char readRegister8(uint8_t reg);
	void setWaveform(uint8_t slot, uint8_t w);
	void selectLibrary(uint8_t lib);
	void go(void);
	void stop(void);
	void setMode(uint8_t mode);
	void setRealtimeValue(uint8_t rtp);
  // Select ERM (Eccentric Rotating Mass) or LRA (Linear Resonant Actuator) vibration motor
  // The default is ERM, which is more common
  	void useERM();
  	void useLRA();
	void setFd(uint8_t);
	int getFd();
	void setFd(int);
	void readBytes(uint8_t address, uint8_t subAddress, uint8_t count,uint8_t * dest);
	uint8_t readByte(uint8_t address, uint8_t subAddress);
};