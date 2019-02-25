
class us5881 
{

private:
	int t0,t1,val,pinID;
	float wheelDiameter;
	bool status,pass,firstVal;
	int getDigitalValue();
public:
	int getPinID();
	void setPinID(int);
	void setWheelDiameter(float);
	float getWheelDiameter();
	bool init();
	float getRPM();
	float getWheelSpeed(char);
	bool poll();	//returns true if theres a new value
	us5881();

};
