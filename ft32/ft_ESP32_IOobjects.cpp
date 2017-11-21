#include "ft_ESP32_IOobjects.h"

/*
//Prototypen zur Simulation f�r VisualStudio
void ledcAttachPin(int, int) {}
void ledcSetup(int, int, int) {}
void ledcWrite(int, int) {}
void pinMode(int, int) {}
void digitalWrite(int, int) {}
unsigned int digitalRead(unsigned int) { return 0; }
unsigned int analogRead(unsigned int) { return 0; }
void delay(double) {}
//Ende der Prototypen f�r VS
*/

Motor::Motor()
{
	mMotorNr = 0;
	mPortNrPWM = 0;
	mPortNrDir = 0;
	mRechtslauf = true;
	mDrehzahl = 0;
}

Motor::Motor(unsigned int motorNr)
{
	mMotorNr = motorNr;
	mPortNrPWM = PORT_M_PWM[motorNr];
	mPortNrDir = PORT_M_DIR[motorNr];
	mRechtslauf = true;
	mDrehzahl = 0;

	//Zuweisen PWM-Generator zu Port
	//pinMode(mPortNrPWM, OUTPUT);	//Port f�r PWM
	ledcAttachPin(mPortNrPWM, motorNr);	//Port ID, PWM-Generator Nr.
	ledcSetup(motorNr, 12000, 8);	// 12 kHz PWM, 8-bit resolution (0..255)
	pinMode(mPortNrDir, OUTPUT);	//Port f�r Richtungsangabe
	
}

void Motor::setValues(bool rechtslauf, unsigned int drehzahl)
{
	mRechtslauf = rechtslauf;
	mDrehzahl = drehzahl;
	
	//Serial.begin(9600);
	Serial.print("Motor ");
	Serial.print(mMotorNr);
	Serial.print(" dreht in Richtung ");
	Serial.print(mRechtslauf);
	Serial.print(" mit Drehzahl ");
	Serial.println(mDrehzahl);

	
	//Berechnen der PWM-Werte
	int drehzahl_pwm;
	if (mDrehzahl < 1)
	{
		drehzahl_pwm = 0;
	}
	else if (mDrehzahl >7)
	{
		drehzahl_pwm = 255;
	}
	else
	{
		drehzahl_pwm = mDrehzahl * 256 / 8;
	}

	//Zuweisen der Richtung an den richtigen Port entsprechend der Motornr.
	if (mRechtslauf)
	{
		digitalWrite(mPortNrDir, HIGH);
	}
	else
	{
		digitalWrite(mPortNrDir, LOW);
		//!!! Unbedingt im Datenblatt des Motortreibers nachsehen, wie PWM und Richtung zusammenh�ngen
		//drehzahl_pwm = 255 - drehzahl_pwm;	//wenn der Motor r�ckw�rts l�uft, ist die pwm invertiert (255 = min, 0 = max)
	}

	//Zuweisen des PWM-Werts an den richtigen Port entsprechend der Motornr.
	ledcWrite(mMotorNr, drehzahl_pwm);
	
	Serial.print("Dir: ");
	Serial.print(mPortNrDir);
	Serial.print(" PWM: ");
	Serial.print(mPortNrPWM);
	Serial.print(" Val: ");
	Serial.println(drehzahl_pwm);
}

void Motor::reRun()
{
	setValues(mRechtslauf, mDrehzahl);
}

Lampe::Lampe()
{
	mLampeNr = 0;
	mHelligkeit = 0;
}

Lampe::Lampe(unsigned int lampeNr)
{
	mLampeNr = lampeNr;
	mHelligkeit = 0;

	//Zuweisen des PWM-Generators an einen Port entsprechend der Lampennummer...
}

void Lampe::setValues(unsigned int helligkeit)
{
	//Serial.begin(9600);
	mHelligkeit = helligkeit;
	Serial.print("Lampe ");
	Serial.print(mLampeNr);
	Serial.print(" leuchtet mit Helligkeit ");
	Serial.println(mHelligkeit);
	
	
	//Berechnen der PWM - Werte
	int helligkeit_pwm;
	if (mHelligkeit > 7)
	{
		helligkeit_pwm = 255;
	}
	else if (mHelligkeit < 1)
	{
		helligkeit_pwm = 0;
	}
	else
	{
		helligkeit_pwm = mHelligkeit * 256 / 8;
	}

	//Zuweisen des PWM-Werts an den richtigen Port entsprechend der Lampennummer
	ledcWrite(mLampeNr, helligkeit_pwm);
}

void Lampe::reRun()
{
	setValues(mHelligkeit);
}

DigitalAnalogIn::DigitalAnalogIn()
{
	mInputNummer = 0;
}

DigitalAnalogIn::DigitalAnalogIn(unsigned int inputNummer)
{
	mInputNummer = inputNummer;
	mInputPortNr = PORT_IN[mInputNummer];
	//pinMode(mInputPortNr, INPUT_PULLUP);
}

unsigned int DigitalAnalogIn::getValueDigital()
{
	pinMode(mInputPortNr, INPUT_PULLUP);
	bool eingabe = !digitalRead(mInputPortNr);
	return (unsigned int) eingabe;
}

unsigned int DigitalAnalogIn::getValueAnalog()
{
	pinMode(mInputPortNr, INPUT);
	unsigned int eingabe = analogRead(mInputPortNr);
	return eingabe;
}
