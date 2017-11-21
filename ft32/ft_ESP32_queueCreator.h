#ifndef FT_ESP32_QUEUECREATOR_H
#define FT_ESP32_QUEUECREATOR_H

#include <string.h>
#include <Arduino.h>

using namespace std;

struct commonElement
{
	unsigned char ID = 0;	//'M', 'A', 'D', ...
	commonElement* nextElement;	//nächstes reguläres Element der Kette
	commonElement* prevElement;	//vorhergehendes reguläres Element der Kette
	commonElement* jumpElement = NULL;	//zeigt bei: If -> EndIf, While -> EndWhile+1, EndWhile -> While
	unsigned char type = 0;	//Art des Inputs "A"/"D" bei If und (normalem) While, "Z" bei Zählschleifen
	unsigned char portNr = 0;	//0..7 bei Input (auch in If/While), 0..3 bei Aktor
	unsigned char compare_direction = 0;	//Vergleichsoperator bei If/While, Richtung Motor
	unsigned int val_pwm_timems_loop = 0;	//Vgl-Wert If/While, PWM Motor/Lampe, Wartezeit [ms], Wert Zählschleife
	unsigned int time_s = 0;	//Zeitwert in Sekunden (nur von Wartezeit benutzt)
	unsigned short rootID = 0;	//fortlaufende ID für If/While (um die EndIf und EndWhile korrekt zuordnen zu können)
};

//Fkt zur Prüfung eines Stringelements auf ein bestimmtes Zeichen
//notwendig für Debugging während Entwicklung
void checkChar(bool& errorFlag, int& errorID, String& uestring, int& strCounter, char zeichen);

//Fkt: an Fischertechnik/ESP32 angepasstes String-to-int
int stoi_ft(String& uestring, int& strCounter);

//Fkt zur Erstellung der Queue aus dem Uebergabestring
bool queue_creator(commonElement*& startPtr, commonElement*& endPtr, String& uebergabestr);

//Fkt zur Löschung der Queue (mit Ausnahme von startPtr und endPtr)
void queue_delete(commonElement*& startPtr, commonElement*& endPtr);

#endif