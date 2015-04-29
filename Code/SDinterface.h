#pragma once

#include <EEPROM.h>
#include <SdFat.h>

#define CSV_BUFFER_SIZE	23
#define LOG_HEADER			"TimeElapsed,gpsLock,TimeLocal,Lat,Long,Alt,RoC,GndSpd,GndCrs,Acc,nSats,ExtTemp,BatTemp,IntTemp,Pres,PresAlt,PresRoC,BatVolt,BatCap,BatHeat,StateParachute,StateUpperCut,StateLowerCut\n"
#define CSV_TIME_ELAP		0
#define CSV_GPS_LOCK		1
#define CSV_TIME_LOCAL	2
#define CSV_LAT					3
#define CSV_LONG				4
#define CSV_ALT					5
#define CSV_ROC					6
#define CSV_GNDSPD			7
#define CSV_GNDCRS			8
#define CSV_ACC					9
#define CSV_NSATS				10
#define CSV_EXTTEMP			11
#define CSV_BATTEMP			12
#define CSV_INTTEMP			13
#define CSV_PRESSURE	 	14
#define CSV_PRES_ALT		15
#define CSV_PRES_ROC		16
#define CSV_BATVOLT			17
#define CSV_BATCAP			18
#define CSV_BATHEAT			19
#define CSV_PARACHUTE		20
#define CSV_UPPERCUT		21
#define CSV_LOWERCUT		22

class SDinterface {
private:
	SdFat sd;
	SdFile file;
	int chipSelect;
	char fileName[11];
	
public:
	String cvsBuffer[CSV_BUFFER_SIZE];
	bool cardOK;

	bool init(unsigned int chipSelect);

	void buffer(float data, int posCSV, int prec);
	void buffer(bool data, int posCSV);
	void buffer(unsigned int data, int posCSV);
	void buffer(long int data, int posCSV);
	void buffer(unsigned long int data, int posCSV);

	bool logToSD();
};