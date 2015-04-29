#include "MAXM8interface.h"

// Calcs and sends checksum after message.
// Msg sequence: header, header, class, ID, payload, ... payload, checkA, checkB
void MAXM8interface::sendUBX(const uint8_t message[], const int messageLength) {
	uint8_t checkA = 0;
	uint8_t checkB = 0;
	Serial3.write(g::ubx_header[0]);
	Serial3.write(g::ubx_header[1]);
	for(int i = 0; i < messageLength; i++) {
		Serial3.write(message[i]);
		checkA = checkA + message[i];
		checkB = checkB + checkA;
	}
	Serial3.write(checkA);
	Serial3.write(checkB);
	Serial3.println();
	Serial3.flush(); // Wait for buffer to empty
}

int MAXM8interface::getUbxAck(const uint8_t message[]) {
	static elapsedMicros timer;
	uint8_t checkA = 0;
	uint8_t checkB = 0;
	uint8_t ackSentence[] = {0,0,0,0,0,0,0,0,0,0};
	
	timer = 0;

	// Look for ACK message in incoming serial stream
	int byteCount = 0;
	while (timer < 2000 && byteCount < 10) {
		if (Serial3.available()) {
			uint8_t checkByte = Serial3.read();
			if (byteCount < 3) {
				if (checkByte == g::ubx_ack_header[byteCount]) {
					ackSentence[byteCount] = checkByte;
					byteCount++;
				}
				else {
					byteCount = 0;
				}
			}
			else {
				ackSentence[byteCount] = checkByte;
				byteCount++;
			}
			timer = 0;
		}
	}

	if (byteCount == 0)	{
		return -4; // Ack Error: timeout
	}
	if (byteCount < 10) {
		return -3; // Ack Error: message too short
	}
	for (int i = 2; i < 8; i++) {
		checkA = checkA + ackSentence[i];
		checkB = checkB + checkA;
	}
	if (ackSentence[8] != checkA || ackSentence[9] != checkB) {
		return -2; // Ack Error: bad checksum
	}
	if (ackSentence[6] != message[0] || ackSentence[7] != message[1]) {
		return -1; // Ack Error: response to wrong command
	}
	if (ackSentence[3] != 0x01) {
		return 0; // Message is ACK-NAK
	}
	else {
		return 1; // Reached end of checks, message is ACK-ACK
	}
}

bool MAXM8interface::sendUntilAck(const uint8_t message[], const int messageLength) {
	unsigned int tries = 0;
	int ackReturn = -5;
	do {
		sendUBX(message, messageLength);
		do {
			delay(10);
			ackReturn = getUbxAck(message);
		} while (ackReturn == -1);
		if (ackReturn == 1) return true;
		else if (ackReturn == 0) return false;
		tries++;
	} while (tries < 5);
	return false;
}

// Set UART1 to 115200 baud
// Relies on ubx_cfg_prt[1] being set to 115200
bool MAXM8interface::changeBaudRate() {
	unsigned int tries = 0;
	int ackReturn = -5;
	do {
		sendUBX(g::ubx_cfg_prt[1], 24);
		Serial3.begin(115200);
		sendUBX(g::ubx_cfg_prt[1], 24);
		do {
			delay(10);
			ackReturn = getUbxAck(g::ubx_cfg_prt[1]);
		} while (ackReturn == -1);
		if (ackReturn == 1) return true;
		else if (ackReturn == 0) return false;
		Serial3.begin(9600);
		tries++;
	} while (tries < 5);
	return false;
}

// Load settings into gps module
bool MAXM8interface::config() {
	// Check to see if GPS is configured by sending request for data
	Serial3.begin(115200);
	if (sendUntilAck(g::ubx_cfg_ant, 8))  {
		//Serial.println("GPS already configured.");
		return true; // Good to go!
	}
	
	//Serial.println("GPS is not configured.");
	// GPS did not respond at pre-configured baud rate, so try default
	Serial3.begin(9600);
	if (!sendUntilAck(g::ubx_cfg_ant, 8)) {
		//Serial.println("GPS not responding.");
		return false; // Comm error?
	}

	// GPS responded at default comm rate, so it needs configuring
	if (!changeBaudRate()) {
		//Serial.println("GPS baud rate could not be configured.");
		return false;
	}
	
	int cfgFailure = 0;

	// Start sending settings en masse
	if (!sendUntilAck(g::ubx_cfg_gnss,			48)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_itfm,			12)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_logfilt, 	16)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_nav5,			40)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_navx5, 		44)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_nmea,			24)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_odo,				24)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_pm2,				48)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_rate,			10)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_rinv,			28)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_sbas,			12)) cfgFailure++;
	if (!sendUntilAck(g::ubx_cfg_usb,			 112)) cfgFailure++;
	for (int i = 0; i < 3; i++) {
		if (!sendUntilAck(g::ubx_cfg_inf[i],	14))	cfgFailure++;
	}
	for (int i = 0; i < 50; i++) {
		if (!sendUntilAck(g::ubx_cfg_msg[i],	12))	cfgFailure++;
	}
	for (int i = 0; i < 4; i++) {
		if (i != 1) // UART1 cfg already sent and confirmed
			if (!sendUntilAck(g::ubx_cfg_prt[i], 24)) cfgFailure++;
	}
	for (int i = 0; i < 2; i++) {
		if (!sendUntilAck(g::ubx_cfg_tp5[i],	 36))	cfgFailure++;
	}

	if (cfgFailure == 0) return true;
	else  return false;
}

// Returns true if sentence parsed
bool MAXM8interface::parseSentence() {
	int sentenceEnd = sentence.length() - 1;
	bool sentenceValid = false;
	int valStart = 0;
	int valEnd = 0;
	int valIndex = 0;
	String value;
	
	valEnd = sentence.indexOf(',', valStart);
	if (valEnd > 0) {
		if (valEnd + 3 <= sentenceEnd) valEnd += 3; // Skip comma and include msgID
		value = sentence.substring(valStart, valEnd);
		if (value == "$PUBX,00") {
			sentenceValid = true;
			valStart = valEnd + 1; // Skip the next comma delimiter
			valIndex += 2;
		}
	}

	if (sentenceValid) {
		while (valEnd < sentenceEnd) {
			valEnd = sentence.indexOf(',', valStart); // index of comma
			if (valEnd < 0) valEnd = sentenceEnd; // indexOf returns -1 if no token found

			value = sentence.substring(valStart, valEnd); // Start inclusive, end exclusive
		
			switch(valIndex) {
				case PUBX00_TIME: // Format: hhmmss.s
					time = value.toFloat() + TIMEZONE;
					if (time < 0) time += 240000;
					else if (time > 240000) time -= 240000;
					if ((time - 10000*int(time/10000.0)) > 6000.0) time -= 4000.0;
					break;
				case PUBX00_LAT:
					latitude = value.toFloat();
					break;
				case PUBX00_NS:
					// Represent southern latitude as negative
					if (value == "S") latitude = -latitude;
					break;
				case PUBX00_LONG:
					longitude = value.toFloat();
					break;
				case PUBX00_EW:
					// Represent eastern latitude as negative
					if (value == "W") longitude = -longitude;
					break;
				case PUBX00_ALT:
					altitude = value.toFloat();
					break;
				case PUBX00_LOCK:
					if (value == "G3" || value == "D3" || value == "RK") gpsLock = true;
					else gpsLock = false;
					break;
				case PUBX00_HACC:
					accuracy = value.toFloat();
					break;
				case PUBX00_VACC: // This comes after PUBX00_HACC
					accuracy = sqrt(pow(accuracy, 2.0) + pow(value.toFloat(), 2.0));
					break;
				case PUBX00_SOG:
					groundSpeed = 0.2777778*value.toFloat(); // km/hr to m/s
					break;
				case PUBX00_COG:
					groundCourse = value.toFloat();
					break;
				case PUBX00_VVEL:
					rateOfClimb = -value.toFloat(); // Need to change sign
					break;
				case PUBX00_NSAT:
					nSats = value.toInt();
					break;
			}

			valStart = valEnd + 1;
			valIndex ++;
		}

		if (gpsLock && maxAlt < altitude - accuracy) {
			maxAlt = altitude - accuracy;
			maxAltIncreased = true;
		}
		else maxAltIncreased = false;

		sentence = "";
		return true;
	}
	else {
		sentence = "";
		return false;
	}
}

bool MAXM8interface::init() {
	receiving = false;
	newData = false;
	timeout = false;
	sentence.reserve(256);

	return config();
}

void MAXM8interface::receive() {
	static elapsedMicros timerSrl = 0;
	static elapsedMillis timerMsg = 0;
	newData = false;
	timeout = false;

	if (receiving) {
		while(Serial3.available()) {
			sentence += char(Serial3.read());
			timerSrl = 0;
		}
		if (timerSrl >= 1000) { // 9600 baud is 833 us/byte
			receiving = false;
			if (parseSentence()) {
				newData = true;
				timerMsg = 0;
			}
			else {
				newData = false;
			}
		}
	}
	else if(Serial3.available()) {
		sentence += char(Serial3.read());
		receiving = true;
		timerSrl = 0;
	}

	if (timerMsg > 700 && !receiving) {
		timerMsg = 0;
		timeout = true;
		gpsLock = false;
	}
}
