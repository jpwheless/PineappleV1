#pragma once

#include "gpsConfig.h"

#define PUBX00_TIME		2		// (float)	hhmmss.ss
#define PUBX00_LAT		3		// (float)	ddmm.mmmmm
#define PUBX00_NS			4		// (char)		North or South latitude
#define PUBX00_LONG		5		// (float)	dddmm.mmmmm
#define PUBX00_EW			6		// (char)		East or West longitude
#define PUBX00_ALT		7		// (float)	altitude ref. datum ellipsoid (m)
#define PUBX00_LOCK		8		// (string)	G3 or D3 or RK = full 3D fix; 
#define PUBX00_HACC		9		// (float)	horizontal acuracy estimate (m)
#define PUBX00_VACC		10	// (float)	vertical acuracy estimate (m)
#define PUBX00_SOG		11	// (float)	speed over ground (km/hr)
#define PUBX00_COG		12	// (float)	course over ground (deg)
#define PUBX00_VVEL		13	// (float)	Vertical velocity, positive downwards (m/s)
#define PUBX00_NSAT		18	// (int)		Number of satellites used in nav solution

#define TIMEZONE			 -70000	// Relative to UTC (hours*10000)

class MAXM8interface {
private:
	String sentence;
	bool receiving;
	
	void sendUBX(const uint8_t message[], const int messageLength);
	int getUbxAck(const uint8_t message[]);
	bool sendUntilAck(const uint8_t message[], const int messageLength);
	bool changeBaudRate();
	bool config();
	bool parseSentence();

public:
	bool newData;					// Reset next time receive() is called
	bool timeout;					// Message not received (resets next time receive is called)
	bool gpsLock;					// True if 3d gps lock is maintained
	float time; 					// hhmmss.ss; 24-hour format
	float latitude;				// ddmm.mmmmm; Positive if North, negative if South
	float longitude;			// dddmm.mmmmm; Positive if West, negative if East
	float altitude;				// Meters
	float rateOfClimb;		// Meters/second; Positive for increasing altitude
	float groundSpeed;		// Meters/second;
	float groundCourse; 	// 0deg is North (duh)
	float accuracy;				// Spherical accuracy estimate
	float maxAlt;					// Max altitude gps has seen so far
	bool maxAltIncreased;	// True if the max altitude increased
	unsigned int nSats;		// number of satellites

	bool init();
	void receive();
	
};