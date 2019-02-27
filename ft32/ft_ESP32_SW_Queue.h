#ifndef FT_ESP32_SW_QUEUE_H
#define FT_ESP32_SW_QUEUE_H

#include <string.h> //für String-Operationen
#include <Arduino.h>  //für String-Operationen
#include <array>  //Library erlaubt Zugriffssicherung bei Arrays
//#include "ft_ESP32_queueCreator.h"  //Aufbauen der Queue
#include "ft_ESP32_SHM.h" //gemeinsamer Speicher (SHared Memory)
#include "ft_ESP32_IOobjects.h" //Klassen der Ausgänge (Hardwaretreiber)
#include "ft_ESP32_queueWorker.h" //abarbeiten der Queue

using namespace std;  //zum Handling der Arrays (für Zugriffssicherung)

struct commonElement
{
	unsigned char ID = 0;	//'M', 'A', 'D', ...
	commonElement* nextElement;	//naechstes regulaeres Element der Kette
	commonElement* prevElement;	//vorhergehendes regulaeres Element der Kette
	commonElement* jumpElement = NULL;	//zeigt bei: If -> Else+1/EndIf+1, Else -> EndIf+1, While -> EndWhile+1, EndWhile -> While
	unsigned char type = 0;	//Art des Inputs "A"/"D" bei If und (normalem) While, "Z" bei Zaehlschleifen
	unsigned char portNr = 0;	//0..7 bei Input (auch in If/While), 0..3 bei Aktor
	unsigned char compare_direction = 0;	//Vergleichsoperator bei If/While, Richtung Motor
	unsigned int val_pwm_timems_loop = 0;	//Vgl-Wert If/While, PWM Motor/Lampe, Wartezeit [ms], Wert Zaehlschleife
	unsigned int time_s = 0;	//Zeitwert in Sekunden (nur von Wartezeit benutzt)
	unsigned short rootID = 0;	//fortlaufende ID fuer If/While (um die EndIf und EndWhile korrekt zuordnen zu koennen)
};

class SW_queue
{
public:
	SW_queue();
	~SW_queue();
	void SW_work(SHM* mSHM);
	void queueCreator();// commonElement*& startPtr, commonElement*& endPtr, String& uebergabestr);
	void queueWorker(SHM* mSHM);
	void queueDelete();// commonElement*& startPtr, commonElement*& endPtr);
	void setBoardType(bool pMaxi);

	int stoi_ft(String& uestring, int& strCounter);
	void checkChar(int& strCounter, char zeichen);
	void printErrors();

private:
	array<Motor, MOTOR_QTY> mMotorArray;  //Anlegen aller Motoren in einen Array
	array<Lampe, LAMP_QTY> mLampeArray; //Anlegen aller Lampen in einen Array
	array<CServoMotor, SERVO_QTY> mServoArray;	//Anlegen aller Servos in einen Array
	array<DigitalAnalogIn, DAIN_QTY> mDAIn; //Anlegen aller Eingänge in einen Array
	
	String uebergabestr = "";

	commonElement* startPtr;  //Halbstatisches Startobjekt der Queue
	commonElement* endPtr;  //Halbstatisches Endobjekt der Queue
	
	bool qCreateError = false;	//Fehlerflag für Erstellung der Queue
	int qCreateErrorID = 0;
	bool qWorkError = false;	//Fehlerflag für fehlerhaft ablaufende Queue
	int qWorkErrorID = 0;
};



#endif
