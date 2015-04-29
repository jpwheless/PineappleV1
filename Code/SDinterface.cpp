#include "SDinterface.h"

bool SDinterface::init(unsigned int chipSelect) {
	this->chipSelect = chipSelect;
	// Get old file number
	uint16_t oldFileNumber = 0;
	oldFileNumber |= EEPROM.read(1); // LSB
	oldFileNumber |= EEPROM.read(0) << 8; // MSB

	// Increment. Will overflow eventually (after 65,535 new files), but that should be fine.
	uint16_t fileNumber = oldFileNumber + 1;

	if (!sd.begin(chipSelect, SPI_FULL_SPEED)) return false;

	String s = String(fileNumber) + ".csv";
	s.toCharArray(fileName, 11);

	if(!file.open(fileName, O_CREAT | O_WRITE | O_APPEND | O_TRUNC)) return false;
	file.print(LOG_HEADER);
	file.close();

	EEPROM.write(1, fileNumber & 0xff); // LSB
	EEPROM.write(0, (fileNumber >> 8) & 0xff); // MSB

	cardOK = true;
	return true;
}


void SDinterface::buffer(float data, int posCSV, int prec) { // Add values to buffer
	char tempBuffer[15];
	switch(prec) {
		default:
		case 1:
			sprintf(tempBuffer, "%.1f", data);
			break;
		case 2:
			sprintf(tempBuffer, "%.2f", data);
			break;
		case 3:
			sprintf(tempBuffer, "%.3f", data);
			break;
		case 4:
			sprintf(tempBuffer, "%.4f", data);
			break;
		case 5:
			sprintf(tempBuffer, "%.5f", data);
			break;
		case 6:
			sprintf(tempBuffer, "%.6f", data);
			break;
	}
	cvsBuffer[posCSV] = String(tempBuffer);
}
void SDinterface::buffer(bool data, int posCSV) { // Add values to buffer
	cvsBuffer[posCSV] = (data) ? "T" : "F";
}
void SDinterface::buffer(unsigned int data, int posCSV) { // Add values to buffer
	cvsBuffer[posCSV] = String(data);
}
void SDinterface::buffer(long int data, int posCSV) { // Add values to buffer
	cvsBuffer[posCSV] = String(data);
}
void SDinterface::buffer(unsigned long int data, int posCSV) { // Add values to buffer
	cvsBuffer[posCSV] = String(data);
}

bool SDinterface::logToSD() { // Write buffer to sd card as a line of CSVs
	if (file.open(fileName, O_CREAT | O_WRITE | O_APPEND)) {
		for (int i = 0; i < CSV_BUFFER_SIZE; i++) {
    	file.print(cvsBuffer[i]);
    	if (i < CSV_BUFFER_SIZE - 1) file.print(",");
		}
		file.print("\n");
		cardOK = file.close();
		return cardOK;
	}
	else return false;
}