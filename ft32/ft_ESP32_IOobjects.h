#ifndef FT_ESP32_IOOBJECTS_H
#define FT_ESP32_IOOBJECTS_H

#include <Arduino.h>

//Festlegen Anzahl Ports
constexpr size_t MOTOR_QTY = 4;	//Anzahl der Motoren
constexpr size_t LAMP_QTY = 4;	//Anzahl der Lampen
constexpr size_t DAIN_QTY = 8;	//Anzahl der Eingänge (digital/analog)

//Zuweisung Ports
const int PORT_M_DIR[MOTOR_QTY] = { 14, 13, 4, 15 };	//Output-Ports Motor-Richtung
const int PORT_M_PWM[MOTOR_QTY] = { 12, 17, 16, 2 };	//Output-Ports Motor-Drehzahl

const int PORT_L_PWM[LAMP_QTY] = { 12, 17, 16, 2 };	//Output-Ports Lampe

const int PORT_IN[DAIN_QTY] = { 39, 34, 35, 32, 33, 25, 26, 27 };	//Input-Ports Ditital/Analog


class Motor
{
public:
	Motor();	//standardkonstruktor, setzt alles auf 0;
	Motor(unsigned int motorNr);	//Konstruktor, Motor-Nr (0..3), weist zu: Port-Nr für PWM (27,12,16,2), Port-Nr für Richtung (14,13,04,15)
	void setValues(bool, unsigned int);	//neue Motorwerte setzen (Aktiv, Richtung, Drehzahl)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
private:
	unsigned int mMotorNr;	//Motornummer 0..3, wird bei erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mPortNrDir;	//Portnr für Richtung, wird bei Erstellung des Objekts zugewiesen
	bool mRechtslauf;	//Drehrichtung: rechts = ture, links = false
	unsigned int mDrehzahl;	//aktuelle Geschwindigkeit (von 0 bis 8)

};

class Lampe
{
public:
	Lampe();
	Lampe(unsigned int);
	void setValues(unsigned int);	//neue Lampenwerte setzen (Aktiv, Helligkeit)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
private:
	unsigned int mLampeNr;
	unsigned int mHelligkeit;
};

class Encoder
{
public:

private:

};

class DigitalAnalogIn
{
public:
	DigitalAnalogIn();
	DigitalAnalogIn(unsigned int);
	unsigned int getValueAnalog();
	unsigned int getValueDigital();
private:
	unsigned int mInputNummer;
	unsigned int mInputPortNr;
};

#endif