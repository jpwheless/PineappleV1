#pragma once

#define RED 					255,0,0
#define ORANGE  			255,127,0
#define YELLOW 				255,255,0
#define GREEN 				0,255,0
#define CYAN					0,255,255
#define BLUE 					0,0,255
#define PURPLE 				255,0,255

class RGBinterface {
private:
	unsigned int pinRed;
	unsigned int pinGreen;
	unsigned int pinBlue;
	bool sinkDrive;

public:
	void init(unsigned int pinRed, unsigned int pinGreen, unsigned int pinBlue, bool sinkDrive);
	void color(int r, int g, int b);
};