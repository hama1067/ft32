#ifndef FT_ESP32_SW_QUEUE_H
#define FT_ESP32_SW_QUEUE_H

#include <string.h> //für String-Operationen
#include <Arduino.h>  //für String-Operationen
#include <array>  //Library erlaubt Zugriffssicherung bei Arrays
#include "ft_ESP32_queueCreator.h"  //Aufbauen der Queue
#include "ft_ESP32_SHM.h" //gemeinsamer Speicher (SHared Memory)
#include "ft_ESP32_IOobjects.h" //Klassen der Ausgänge (Hardwaretreiber)
#include "ft_ESP32_queueWorker.h" //abarbeiten der Queue

using namespace std;  //zum Handling der Arrays (für Zugriffssicherung)

class SW_queue
{
public:
	SW_queue();
	~SW_queue();
	void SW_work(SHM* mSHM);
  void setBoardType(bool pMaxi);
private:
	array<Motor, MOTOR_QTY> mMotorArray;  //Anlegen aller Motoren in einen Array
	array<Lampe, LAMP_QTY> mLampeArray; //Anlegen aller Lampen in einen Array
	array<DigitalAnalogIn, DAIN_QTY> mDAIn; //Anlegen aller Eingänge in einen Array
	
	commonElement* startPtr;  //Halbstatisches Startobjekt der Queue
	commonElement* endPtr;  //Halbstatisches Endobjekt der Queue
	
	bool qCreateError = false;  //Fehlerflag für Erstellung der Queue
	bool qWorkError = false;  //Fehlerflag für Fehlerhaft ablaufende Queue
};

#endif
