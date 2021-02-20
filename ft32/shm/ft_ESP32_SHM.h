#ifndef FT_ESP32_SHM_H_
#define FT_ESP32_SHM_H_

#include "../shm/webData_SHM.h"
#include "../memory/CSpiffsStorage.h"

struct SHM
{ 
	/* ----- Webserver Variables ----- */
	bool commonStart = 0;
	bool commonPause = 0;
	bool commonStopp = 0;
	
	webDataSHM webData;
 
	/* ------- Queue Variables ------- */
	unsigned int digitalVal[8] = { 0 };	// Queue: RW, Web: R
	unsigned int analogVal[8] = { 0 };	// Queue: RW, Web: R
	int motorVal[4] = { 0 };	// Queue: RW, Web: R
	unsigned int ledVal[11] = { 0 };	// Queue: RW, Web: R
	unsigned int servoVal[1] = { 0 };	// Queue: RW, Web: R
	bool running = false;				// Queue: RW, Web: R

	String IPAdress = "";
	CSpiffsStorage mSpeicher;
};

#endif /* FT_ESP32_SHM_H_ */
