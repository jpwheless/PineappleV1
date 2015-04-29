#include "BatterySense.h"
#include "GPOinterface.h"
#include "Heater.h"
#include "LEDinterface.h"
#include "LMT84LPinterface.h"
#include "LowerCutDown.h"
#include "MAXM8interface.h"
#include "MS5607interface.h"
#include "Parachute.h"
#include "PushButton.h"
#include "Rainbow.h"
#include "RGBinterface.h"
#include "SDinterface.h"
#include "UpperCutDown.h"

using namespace std;

// TODO:
// 	Write IMU Code
// 	Add caveats for gps accuracy

////////////////////
//Pin Definitions //
////////////////////
#define SD_CS			2
#define RGB_R			3
#define RGB_B			4
#define RGB_G			5
#define GPS_EXINT	6
#define GPS_TX		7
#define GPS_RX		8
#define LED_2			9
#define PUSH_BUT	10
#define SD_MOSI		11
#define SD_MISO		12
#define SD_SCK		13
#define THERM_2		14
#define THERM_1		15
#define BAT_VSNS	16
#define IMU_INT		17
#define I2C_SDA		18
#define I2C_SLCK	19
#define GPO_2			20
#define GPO_3			21
#define GPO_4			22
#define GPO_1			23

///////////////
// Constants //
///////////////
#define V_REF			2.500 // Nominal voltage of high-accuracy reference (±0.08%)
#define ADC_MAX		4095.0
#define ADC_RES		12
#define PWM_RES		11
#define PWM_MAX		2047.0
#define PWM_FREQ	23437

#define BAT_VSNS_MULT	2.005

#define PWM_A_PIN 5
#define PWM_B_PIN 3
#define PWM_C_PIN 25

#define MS5607_ADDR		0x77 // 6 MSBs of address. B111011x is complete address, with x being ~CSB
#define MPU9250_ADDR	0x69 // 6 MSBs of address. B111011x is complete address, with x being CSB

//////////////////
// I2C Commands //
//////////////////
#define MS5607_RST		0x1E
#define MS5607_PCONV	0x48 // 4096 OSR (9.04ms)
#define MS5607_TCONV	0x58 // 4096 OSR (9.04ms)
#define MS5607_AREAD	0x00
#define MS5607_PREAD	0xA0 // Address of first PROM

//////////////////////
// Global Variables //
//////////////////////
SDinterface sdCard;
LMT84LPinterface thermBat;
LMT84LPinterface thermExt;
MS5607interface barometer;
//MPU9250interface inertial;
MAXM8interface gps;
PushButton button;
GPOinterface parachuteNich;
GPOinterface upperCutNich;
GPOinterface lowerCutNich;
BatterySense battery;
RGBinterface rgb;
LEDinterface led;
Heater batteryHeat;
Rainbow rainbowLED;
LowerCutDown lowerCutDown;
UpperCutDown upperCutDown;
Parachute parachute;

////////////////////
// Main Functions //
////////////////////

void updateClassStatesGPS() {
	parachute.belowMaxAlt = (gps.altitude < gps.maxAlt - 304.8);
	upperCutDown.maxAltIncreased = gps.maxAltIncreased;

	if (gps.altitude >= 3048.0) { // 10k ft
		lowerCutDown.aboveArmAlt = true;
		upperCutDown.aboveArmAlt = true;
		parachute.belowDeployAlt = false;
	}
	else {
		lowerCutDown.aboveArmAlt = false;
		upperCutDown.aboveArmAlt = false;
		parachute.belowDeployAlt = true;
	}

	lowerCutDown.aboveCutAlt = (gps.altitude >= 21336.0); // 70k ft

	if (gps.rateOfClimb <= -7.0) {
		upperCutDown.falling = true;
		parachute.falling = true;
	}
	else {
		upperCutDown.falling = false;
		parachute.falling = false;
	}

	// Format: ddmm.mmmmm, dddmm.mmmmm (latitude, longitude)
	// Lower right corner: 4628.15314, -11703.70902
	// Upper left  corner: 4746.63872, -11951.79980
	if (gps.latitude  >= 4628.15314  && gps.latitude  <= 4746.63872
	 && gps.longitude >= -11951.79980 && gps.longitude <= -11703.70902) {
		upperCutDown.outsideBoundaries = false;
	}
	else upperCutDown.outsideBoundaries = true;
}

void updateClassStatesBaro() {
	parachute.belowMaxAlt = (barometer.altitude < barometer.maxAlt - 304.8);

	if (barometer.altitude >= 3048.0) { // 10k ft
		lowerCutDown.aboveArmAlt = true;
		upperCutDown.aboveArmAlt = true;
		parachute.belowDeployAlt = false;
	}
	else {
		lowerCutDown.aboveArmAlt = false;
		upperCutDown.aboveArmAlt = false;
		parachute.belowDeployAlt = true;
	}

	lowerCutDown.aboveCutAlt = (barometer.altitude >= 21336.0); // 70k ft

	if (barometer.rateOfClimb <= -7.0) {
		upperCutDown.falling = true;
		parachute.falling = true;
	}
	else {
		upperCutDown.falling = false;
		parachute.falling = false;
	}

	// Not detectable with barometer
	upperCutDown.outsideBoundaries = false;
}

void bufferLogData() {
	if (gps.newData || gps.timeout) {
		barometer.measure(); // Start barometer (Main loop call does not reset flag)

		sdCard.buffer(float(millis())/1000.0, CSV_TIME_ELAP, 1);
		sdCard.buffer(gps.gpsLock, CSV_GPS_LOCK);
		sdCard.buffer(gps.time, CSV_TIME_LOCAL, 1);
		sdCard.buffer(gps.latitude, CSV_LAT, 5);
		sdCard.buffer(gps.longitude, CSV_LONG, 5);
		sdCard.buffer(gps.altitude, CSV_ALT, 2);
		sdCard.buffer(gps.rateOfClimb, CSV_ROC, 2);
		sdCard.buffer(gps.groundSpeed, CSV_GNDSPD, 2);
		sdCard.buffer(gps.groundCourse, CSV_GNDCRS, 1);
		sdCard.buffer(gps.accuracy, CSV_ACC, 2);
		sdCard.buffer(gps.nSats, CSV_NSATS);
		sdCard.buffer(thermExt.celsius, CSV_EXTTEMP, 2);
		sdCard.buffer(thermBat.celsius, CSV_BATTEMP, 2);
		sdCard.buffer(barometer.celsius, CSV_INTTEMP, 2);
		sdCard.buffer(barometer.kPa, CSV_PRESSURE, 2);
		sdCard.buffer(barometer.altitude, CSV_PRES_ALT, 2);
		sdCard.buffer(barometer.rateOfClimb, CSV_PRES_ROC, 2);
		sdCard.buffer(battery.voltage, CSV_BATVOLT, 2);
		sdCard.buffer(battery.getCap(), CSV_BATCAP, 2);
		sdCard.buffer(batteryHeat.dutyCycle, CSV_BATHEAT, 2);
		sdCard.buffer(parachute.state, CSV_PARACHUTE);
		sdCard.buffer(upperCutDown.state, CSV_UPPERCUT);
		sdCard.buffer(lowerCutDown.state, CSV_LOWERCUT);

		for (int i = 0; i < CSV_BUFFER_SIZE; i++) {
			Serial.print(sdCard.cvsBuffer[i]);
			Serial.print(",");
		}
		Serial.println();

		sdCard.logToSD();
	}
}

void updateDisplayLights() {
	static elapsedMillis lightTimer = 0;

	if (lightTimer >= 1000) {
		lightTimer = 0;
		if (gps.gpsLock) rgb.color(GREEN);
		else rgb.color(ORANGE);
	}
	else if (lightTimer >= 100) {
		rgb.color(0, 0, 0);
	}

	/*
	rainbowLED.cycle();
	if (lightToggle && blinkTimer > 200) {
		led.write(0);
		if (gps.gpsLock) rgb.color(0, 0, 0);
	}
	blinkTimer = 0;
	lightToggle = true;
	led.write(255);
	if (gps.gpsLock) rgb.color(0, 127, 0);
	if (button.fell) {
		rainbowLED.off();
	}
	else if (button.rose) {
		rainbowLED.on();
	}
	*/
}

String printState(unsigned int state) {
	switch (state) {
		case S_MONITORING:
			return "Monitoring";
			break;
		case S_ARMED:
			return "Armed";
			break;
		case S_PREPPING:
			return "Prepping";
			break;
		case S_EXECUTING:
			return "Executing";
			break;
		case S_COMPLETE:
			return "Complete";
			break;
	}
	return "";
}

void setup() {
	Serial.begin(115200);

	analogWriteFrequency(PWM_A_PIN, PWM_FREQ); // Ideal frequency for 11-bit pwm
	analogWriteFrequency(PWM_B_PIN, PWM_FREQ);
	analogWriteFrequency(PWM_C_PIN, PWM_FREQ);
	analogWriteResolution(PWM_RES);
	analogReadResolution(ADC_RES);

	Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);

	thermBat.init(THERM_1, 1.0, 1.0176, -2.0087);
	thermExt.init(THERM_2, 1.0, 1.0176, -2.0087);
	parachuteNich.init(GPO_2, 0.9);
	upperCutNich.init(GPO_1, 1.1);
	lowerCutNich.init(GPO_4, 1.2);
	batteryHeat.init(GPO_3, &thermBat, 5.0, 1.0, 0.5);
	battery.init(BAT_VSNS, BAT_VSNS_MULT, 1.0);
	button.init(PUSH_BUT, true, 8000);
	rgb.init(RGB_R, RGB_G, RGB_B, true);
	rainbowLED.init(&rgb);
	led.init(LED_2);
	lowerCutDown.init(&lowerCutNich, &battery);
	upperCutDown.init(&upperCutNich, &battery, &lowerCutDown);
	parachute.init(&parachuteNich, &battery, &upperCutDown);

	if (!gps.init()) while(1) {
		Serial.println("GPS initialization failed.");
		rgb.color(127,0,0);
		delay(500);
		rgb.color(0,0,0);
		delay(500);
	}
	if (!barometer.init(MS5607_ADDR)) while(1) {
		Serial.println("Barometer initialization failed.");
		rgb.color(127,0,0);
		delay(500);
		rgb.color(0,0,0);
		delay(500);
	}
	if (!sdCard.init(SD_CS)) while(1) {
		Serial.println("SD card initialization failed.");
		rgb.color(127,0,0);
		delay(500);
		rgb.color(0,0,0);
		delay(500);
	}
}

void loop() {

	// Sensors
	gps.receive();
	battery.read();
	batteryHeat.update();
	if (!barometer.measComplete) barometer.measure();
	thermExt.read();
	button.read();

	// Data source logic
	if (gps.newData) {
		if (gps.gpsLock) updateClassStatesGPS();
		else updateClassStatesBaro();
	}
	else if (gps.timeout) {
		updateClassStatesBaro();
	}

	//Nichrome classes
	parachute.update();
	upperCutDown.update();
	lowerCutDown.update();

	// Data logging
	bufferLogData();

	updateDisplayLights();
}