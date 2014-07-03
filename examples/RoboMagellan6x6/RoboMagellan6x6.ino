#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include <MINDSi.h>
#include "Compass.h"
#include "MPU.h"
#include "DroneUtils.h"
#include "GreatCircle.h"
#include "HLAverage.h"
#include "wiring_private.h"

//resync is sketchy

const uint8_t VoltagePin  = 67;
const uint8_t LEDpin[]    = {25, 26, 27}; //blue, yellow, red
const uint8_t PingPin[]	  = {A4, A1, A0, A2, A3}; //left to right
const uint8_t ServoPin[]  = {12, 11,  8};//drive, steer, backS; APM 1,2,3 resp.
const uint8_t RadioPin[]  = {2, 3}; //drive, steer
const int SCHEDULE_DELAY  = 25;
const int STEERTHROW	  = 30; //degrees from far turn to center
const int REVTHROW		  = 20;
const int FWDSPEED		  = 115;
const int REVSPEED        = 75;
const int STOP_TIME       = 1500;//time to coast to stop
const int DANGER_TIME     = STOP_TIME+800; //time to backup after last sighting
const uint16_t warn[]     = {850, 1050, 2150, 1050, 850};
const double POINT_RADIUS = .001; //in miles, margin for error in rover location
const double PING_CALIB   = 1500.l;
const double pAngle[5]    = { 79.27l, 36.83l, 0.l, -36.83l, -79.27l};
const double dPlsb        = 4.f/float(0xffff); //dps Per leastSigBit for gyro

HardwareSerial *CommSerial = &Serial;
NMEA			nmea(Serial1);
CommManager		manager(CommSerial);
Point			location(0,0);
HLA				lowFilter (600000, 0);//10 minutes
HLA				highFilter(    10, 0);//10 milliseconds
Servo			servo[3]; //drive, steer, backSteer
//scheduler, navigation, obstacle, stop
uint32_t uTime = 0, nTime = 0, oTime = 0, sTime = 0;
uint32_t gpsHalfTime = 0, gpsTime = 0;
 int32_t Ax,Ay,Az; //used in accelerometer calculations
uint16_t ping[5] = {20000,20000,20000,20000,20000};
uint8_t  sIter, pIter; //iterators for scheduler and ping
double   pathHeading; //all headings are Clockwise=+, -179 to 180, 0=north
double   trueHeading;
double   gyroHalf; //Gyro Heading halfway between gps points
double   distance;
double   pitch=0, roll=0; //stores the observed angle from horizontal
double   eulerPitch, eulerRoll; //the rotations to get to our current state
boolean  stop = true;
boolean  backDir;

voidFuncPtr schedule[] = {
							extrapPosition,
							checkPing,
							readAccelerometer,
							reportLocation,
							};

void setup() {
	Serial1.begin(38400);
	CommSerial->begin(9600);
	InitMPU();
	pinMode(40, OUTPUT); digitalWrite(40, HIGH);
	for(int i=0; i<3; i++) pinMode(LEDpin[i], OUTPUT);
	for(int i=0; i<3; i++) servo[i].attach(ServoPin[i]);
	output(90,90);

	sendGPSMessage(0x06, 0x01, 0x0003, GPRMC_On);
	sendGPSMessage(0x06, 0x17, 0x0004, CFG_NMEA);
	sendGPSMessage(0x06, 0x00, 0x0014, CFG_PRT);
	sendGPSMessage(0x06, 0x24, 0x0024, Pedestrian_Mode);

	delay(1000);
	calibrateGyro(); //this also takes one second

	getRadio(RadioPin[0]);//start radio interrupts

	manager.sendDataMessage(Protocol::DATA_DISTANCE, (double)0); //clear buffer
	manager.sendDataMessage(Protocol::DATA_DISTANCE, (double)0); //of bad msgs
	manager.requestResync();

	uTime = millis();
}

void loop(){
	manager.update();
	updateGPS();
	updateGyro();
	if(uTime <= millis()){
		uTime += SCHEDULE_DELAY;
		schedule[sIter]();
		sIter++;
		sIter = sIter%(sizeof(schedule)/sizeof(*schedule));
		navigate();
	}
}

void navigate(){
	if (isRadioOn(RadioPin[0])) {
		output(getRadio(RadioPin[0]), getRadio(RadioPin[1]));
	} else if (oTime != 0) {
		//Back Up
		if(sTime == 0){
			output(90, 90);
			sTime = millis();
			backDir = ping[0]<ping[4];
		} else if(sTime+STOP_TIME < millis()){
			if(backDir) output(REVSPEED, 90-REVTHROW);
			else 		output(REVSPEED, 90+REVTHROW);
		}

		if(oTime+DANGER_TIME < millis()){
			sTime = 0;
			oTime = 0;
		}
	} else {
		//drive based on pathHeading and side ping sensors
		double x,y;
		double angularError = trunkAngle(pathHeading - trueHeading);
		double outputAngle = atan( angularError*PI/180 )*(2*STEERTHROW/PI);

		x = cos(toRad(outputAngle));
		y = sin(toRad(outputAngle));
		for(int i=0; i<5; i++){
			double tmp = ping[i]/PING_CALIB;
			tmp *= tmp;
			x += cos(toRad(pAngle[i]))/tmp;
			y += sin(toRad(pAngle[i]))/tmp;
		}

		outputAngle = toDeg(atan2(y,x))+90;
		bound(double(90-STEERTHROW), outputAngle, double(90+STEERTHROW));

		//slow down near waypoints
		int speed = (distance*5280.l);//distance in feet;
		speed *= speed;
		speed += 96;
		speed = min(FWDSPEED, speed);

		if(stop) output(90 ,90);
		else     output(speed, outputAngle);
	}
}

void updateGPS(){
	nmea.update();
	if(nmea.newData()){
		location = nmea.getLocation();
		updateWaypoint();
		syncHeading();
		positionChanged();
		gpsTime = millis();
	}
}

void updateWaypoint(){
	if(manager.numWaypoints() >= 1 && !nmea.getWarning()){
		stop = false;
		distance = calcDistance(manager.getTargetWaypoint(), location);
		if(distance > POINT_RADIUS) return;

		if(manager.getTargetIndex() < manager.numWaypoints()-1)
			manager.advanceTargetIndex();
		else if (manager.loopWaypoints())
			manager.setTargetIndex(0);
		else
			stop = true;
	}
}

void updateGyro(){
	double dt = lowFilter.millisSinceUpdate();
	double Gz = MPU_Gz();
	lowFilter.update(Gz);
	highFilter.update(Gz-lowFilter.get());
	trueHeading = trunkAngle(trueHeading + dt*(highFilter.get())*dPlsb);

	if(gpsHalfTime < millis() && gpsHalfTime!=0){
		gyroHalf = trueHeading;
		gpsHalfTime = 0;
	}

}

void syncHeading(){
	if(!nmea.getWarning() && nmea.getCourse()!=0){
		trueHeading = trunkAngle(nmea.getCourse() + trueHeading - gyroHalf);
		gpsHalfTime = millis()+(millis()-gpsTime)/2;
	}
	else if(stop) trueHeading = pathHeading;
}

void checkPing(){
	ping[pIter] = getPing(PingPin[pIter]);
	pIter++;
	pIter = pIter%5;
	if(ping[pIter] < warn[pIter]) oTime = millis();
}

void extrapPosition(){
/*	float dT = millis()-nTime;
	if(dT < 1000 && !nmea.getWarning()){ //ignore irrational values
		//3600000 = milliseconds per hour
		float dTraveled = nmea.getGroundSpeed()*dT/3600000.f;
		location = extrapPosition(location, trueHeading, dTraveled);
	}
	positionChanged();*/
}

void positionChanged(){
	nTime = millis();
	if(manager.numWaypoints() <= 0) return;

	pathHeading = calcHeading(location, manager.getTargetWaypoint());
	distance = calcDistance(manager.getTargetWaypoint(), location);
}

void readAccelerometer(){
	Ax = MPU_Ax();
	Ay = MPU_Ay();
	Az = MPU_Az();
	pitch = atan2(sqrt(Ax*Ax+Az*Az), Ay);
	roll  = atan2(sqrt(Ay*Ay+Az*Az), -Ax);
	eulerPitch = atan2(Ax, Az);
	eulerRoll = atan2( float(Ay), float(Ax)/sin(eulerPitch) );
}

void reportLocation(){
	float voltage = float(analogRead(67)/1024.l*5.l*10.1l);
	manager.sendDataMessage(Protocol::DATA_LATITUDE, location.degLatitude());
	manager.sendDataMessage(Protocol::DATA_LONGITUDE, location.degLongitude());
	manager.sendDataMessage(Protocol::DATA_ROLL, roll*180/PI);
	manager.sendDataMessage(Protocol::DATA_PITCH, pitch*180/PI);
	manager.sendDataMessage(Protocol::DATA_HEADING, trueHeading);
	manager.sendDataMessage(Protocol::DATA_SPEED, trunkAngle(nmea.getCourse()));
	manager.sendDataMessage(Protocol::DATA_DISTANCE, voltage);//nmea.getCourse());//distance);
}

void calibrateGyro(){ //takes one second
	double tmp = 0;
	for(int i=0; i<100; i++){
		double Gz = MPU_Gz();
		tmp += Gz/100;
		delay(10);
	}
	lowFilter.set(tmp);
}

void output(uint8_t drive, uint8_t steer){
	servo[0].write(drive);
	servo[1].write(steer);
	servo[2].write(180-steer);
}
