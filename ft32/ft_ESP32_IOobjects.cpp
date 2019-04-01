/*
Ausgangstreiber für ESP32-Fischertechnik-Anbindung
Autor: Johannes Marquart
*/
#include "ft_ESP32_IOobjects.h"
#include <Wire.h>
#include <Arduino.h>
//#include <SparkFunSX1509.h>

SX1509 sx1509Object;        //i2c SDA = PIN 21, SCL = PIN 22
int initRecCalls = 0;  
int PORT_M_PWM[MOTOR_QTY];// = {};//Output-Pins Motor-Drehzahl
int PORT_L_PWM[LAMP_QTY]; //Output-Pins Lampe, werden hier über den selben Treiber angesteuert
int PORT_IN[DAIN_QTY]; //Input-Pins Ditital/Analog
bool ISMAXI = false;

/*
bool initExtension()
{
    delay(1500);    //etwas warten, damit I2C möglich ist (ansonsten Kommunikationsprobleme)
    if (!sx1509Object.begin(SX1509_I2C_ADDRESS))	//starten des SX1509 mit SX1509-I²C-Adresse, wenn false (Fehler erkannt -> kein SX1509 angeschlossen)
    {
		Serial.println("[io] SX1509-Object could not be initialized.");
		return false;  
    }
    else	//wenn true (SX1509 angeschlossen)
    {
		Serial.println("[io] SX1509-Object initialized.");  
    }
	sx1509Object.clock(INTERNAL_CLOCK_2MHZ, 4);	//Einrichten der Frequenz? Warum nicht 'oben'?
	return true;
}
*/

CheckMaxiExtension::CheckMaxiExtension()
{
  mAddress = SX1509_I2C_ADDRESS;
  mBoard = "";
}

CheckMaxiExtension::CheckMaxiExtension(byte address)
{
  mAddress = address;
  mBoard = "";
}


bool CheckMaxiExtension::CheckMaxi()
{
	delay(1500);    //etwas warten, damit I2C möglich ist (ansonsten Kommunikationsprobleme)
	if (!sx1509Object.begin(SX1509_I2C_ADDRESS))	//starten des SX1509 mit SX1509-I²C-Adresse, wenn false (Fehler erkannt -> kein SX1509 angeschlossen)
	{
		Serial.println("[io] SX1509-Object could not be initialized.");

		mBoard = "MINI";
		ISMAXI = false;
		Serial.println("[io] mini board activated");
		//Zuweisung Ports    
		for (int i = 0; i < MOTOR_QTY; i++)
		{
			PORT_M_PWM[i] = MINI_PORT_M_PWM[i];
		}
		for (int i = 0; i < LAMP_QTY; i++)
		{
			PORT_L_PWM[i] = MINI_PORT_L_PWM[i];
		}
		for (int i = 0; i < DAIN_QTY; i++)
		{
			PORT_IN[i] = MINI_PORT_IN[i];
		}
		return false;
	}
	else	//wenn true (SX1509 angeschlossen)
	{
		Serial.println("[io] SX1509-Object initialized.");
		sx1509Object.clock(INTERNAL_CLOCK_2MHZ, 4);	//Einrichten der Frequenz

		Serial.println("[io] maxi board activated");
		mBoard = "MAXI";
		ISMAXI = true;
		//Zuweisung Ports    
		for (int i = 0; i < MOTOR_QTY; i++)
		{
			PORT_M_PWM[i] = MAXI_PORT_M_PWM[i];
		}
		for (int i = 0; i < LAMP_QTY; i++)
		{
			PORT_L_PWM[i] = MAXI_PORT_L_PWM[i];
		}
		for (int i = 0; i < DAIN_QTY; i++)
		{
			PORT_IN[i] = MAXI_PORT_IN[i];
		}
		return true;
	}
}

Motor::Motor()
{
	//Abschalten des Motortreibers, welcher von diesem Objekt versorgt wird.
	//Evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_M_INH, OUTPUT);
	digitalWrite(PIN_M_INH, LOW);

	mMotorNr = 0;
	mPortNrPWM = 0;
	mPortNrDir = 0;
	mRechtslauf = true;
	mDrehzahl = 0;
}


Motor::Motor(unsigned int motorNr)
{
	//Abschalten des Motortreibers, evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_M_INH, OUTPUT);
	digitalWrite(PIN_M_INH, LOW);
  
	//Initialisieren des Motorobjektes
	mMotorNr = motorNr;
	mRechtslauf = true;
	mDrehzahl = 0;

//  if (ISMAXI)
//  {
//    mPortNrDir = SX1509_PORT_M_DIR[mMotorNr];
//  }
//  else
//  {
//    mPortNrDir = PORT_M_DIR[mMotorNr];
//  }

//  mPortNrPWM = PORT_M_PWM[mMotorNr];
//    //Zuweisen PWM-Generator zu Pin. Generator 0,2,4,6 für Drehzahl
//  ledcAttachPin(mPortNrPWM, mMotorNr*2);  //Pin-Nr für Drehzahl, PWM-Generator Nr
//  ledcSetup(mMotorNr*2, 21700, 8);  //PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
//  ledcWrite(mMotorNr*2, 0); //frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
    
//    // Zuweisen der Direction 
//  if (ISMAXI)
//  {
//    sx1509Object.digitalWrite(mPortNrDir, 1);
//  }
//  else
//  {
//    //Zuweisen PWM-Generator zu Pin. Generator 1,3,5,7 für Richtung
//    ledcAttachPin(mPortNrDir, (mMotorNr*2)+1);  //Pin-Nr für Richtungsangabe, PWM-Generator Nr
//    ledcSetup(mMotorNr*2+1, 21700, 8);
//    ledcWrite(mMotorNr*2+1,255);  //frühzeitiges Definieren des Dir-Pins
//  }
}


void Motor::setMaxi(bool pMaxi)
{
	mPortNrPWM = PORT_M_PWM[mMotorNr];
	if (pMaxi)
	{
		//Zuweisen PWM-Generator zu Pin. Generator 0, 1, 2, 3 für Drehzahl
		ledcAttachPin(mPortNrPWM, mMotorNr);  //Pin-Nr für Drehzahl, PWM-Generator Nr
		ledcSetup(mMotorNr, 21700, 8);  //PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
		ledcWrite(mMotorNr, 0); //frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
		mPortNrDir = SX1509_PORT_M_DIR[mMotorNr];
		sx1509Object.digitalWrite(mPortNrDir, 1);
	}
	else
	{
		//Zuweisen PWM-Generator zu Pin. Generator 0,2,4,6 für Drehzahl
		ledcAttachPin(mPortNrPWM, mMotorNr*2);  //Pin-Nr für Drehzahl, PWM-Generator Nr
		ledcSetup(mMotorNr*2, 21700, 8);  //PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
		ledcWrite(mMotorNr*2, 0); //frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
		mPortNrDir = PORT_M_DIR[mMotorNr];
		//Zuweisen PWM-Generator zu Pin. Generator 1,3,5,7 für Richtung
		ledcAttachPin(mPortNrDir, (mMotorNr*2)+1);  //Pin-Nr für Richtungsangabe, PWM-Generator Nr
		ledcSetup((mMotorNr*2)+1, 21700, 8);
		ledcWrite((mMotorNr*2)+1, 255);  //frühzeitiges Definieren des Dir-Pins
	}
}
 

void Motor::setValues(bool rechtslauf, unsigned int drehzahl)
{
  mRechtslauf = rechtslauf;
  mDrehzahl = drehzahl;

  digitalWrite(PIN_M_INH, LOW);
  
  //Serial.begin(9600); -> sollte in der aufrufenden Instanz schon initialisiert sein
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

  //Zuweisen der Richtung an den richtigen Pin entsprechend der Motornr.
  if (mRechtslauf)
  {
    //digitalWrite(mPortNrDir, HIGH);
    if (ISMAXI)
    {
      sx1509Object.digitalWrite(mPortNrDir, 1);  //Richtungspin wird auf HIGH - Rechtslauf gesetzt
    }
    else
    {
      ledcWrite(mMotorNr*2+1, 255); //Generator für Richtung wird auf max. (255) gesetzt
    }
  }
  else
  {
    //digitalWrite(mPortNrDir, LOW);
    if (ISMAXI)
    {
      sx1509Object.digitalWrite(mPortNrDir, 0);  //Richtungspin wird auf LOW - Linkslauf gesetzt
    }
    else
    {
      ledcWrite(mMotorNr*2+1, 0); //Generator für Richtung wird auf 0 gesetzt
    }
    //!!! Unbedingt im Datenblatt des Motortreibers nachsehen, wie PWM und Richtung zusammenhängen !!!
   
    drehzahl_pwm = 255 - drehzahl_pwm;  //wenn der Motor rückwärts läuft, ist die PWM invertiert (255 = min, 0 = max)
  }

  //Zuweisen des PWM-Werts an den richtigen Generator entsprechend der Motornr.
  if (ISMAXI)
  {
    ledcWrite(mMotorNr, drehzahl_pwm);
  }
  else
  {
    ledcWrite(mMotorNr*2, drehzahl_pwm);
  }
  
  digitalWrite(PIN_M_INH, HIGH);  //Einschalten Motortreiber
  
  Serial.print("Dir: ");
  Serial.print(mPortNrDir);
  Serial.print(" PWM: ");
  Serial.print(mPortNrPWM);
  Serial.print(" Val: ");
  Serial.println(drehzahl_pwm);
}

void Motor::reRun()
{
  if(mDrehzahl > 0)
  {
    setValues(mRechtslauf, mDrehzahl);
  }
}

CServoMotor::CServoMotor()
{
	mMotorNr = 0;
	mPortNrPWM = 0;
	mMinDuty = 4;
	mMaxDuty = 11;
	mRelDuty = 0;
}

CServoMotor::CServoMotor(unsigned int motorNr, unsigned int dutyCycle)
{
	//init of servomotor-object
	mMotorNr = motorNr;
	//mPinNrPWM = PIN_M_PWM[motorNr];
	mMinDuty = 4;	//minimal real DutyCycle in %
	mMaxDuty = 11;	//maximal real DutyCycle in %
	mRelDuty = dutyCycle < 100 ? dutyCycle : 100;	//limiting maximum value to 100(%)
	mLedcChannel = 4;	//channel 4 - servo only used at mini-board -> only channels 0..3 are used by motors
}

void CServoMotor::setMaxi(bool pMaxi)
{
	mPortNrPWM = 13;// PORT_M_PWM[mMotorNr];
	if (pMaxi)
	{
		//Servomotoren funktionieren noch nicht am Maxi-Board
	}
	else
	{
		//Zuweisen PWM-Generator zu Pin. Generator 0,2,4,6 für Drehzahl
		ledcAttachPin(mPortNrPWM, mLedcChannel);  //Pin-Nr für Drehzahl, PWM-Generator Nr
		
		ledcSetup(mLedcChannel, 50, 12);  //PWM-Generator Nr, 50 Hz PWM, 12-bit resolution (0..255)
		ledcWrite(mLedcChannel, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);	//early set of pin -> servo moves to given duty-cycle
	}
}

void CServoMotor::setValues(unsigned int dutyCycle)
{
	if (!ISMAXI)
	{
		mRelDuty = dutyCycle < 100 ? dutyCycle : 100;	//limiting maximum value to 100(%)
		Serial.print("Servo ");
		Serial.print(mMotorNr);
		Serial.print(" auf Pos ");
		Serial.println(mRelDuty);
		ledcWrite(mLedcChannel, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);	//calculate real duty-cycle for servos: 0%rel = 4%, 100%rel = 11%
	}
}

void CServoMotor::reRun()
{
	if (!ISMAXI)
	{
		ledcWrite(mLedcChannel, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);
	}
}

Lampe::Lampe()
{
  //Abschalten des Lampentreibers, welcher von diesem Objekt versorgt wird.
  //Evtl. noch undefinierte Pins können so kein falsches Signal an den Lampentreiber geben
  pinMode(PIN_L_INH, OUTPUT);
  digitalWrite(PIN_L_INH, LOW);
  
  mLampeNr = 0;
  mPortNrPWM = 0;
  mHelligkeit = 0;
}

Lampe::Lampe(unsigned int lampeNr)
{
  //Abschalten des Motortreibers, evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
  pinMode(PIN_L_INH, OUTPUT);
  digitalWrite(PIN_L_INH, LOW);
  
  //Initialisieren des Lampenobjektes
  mLampeNr = lampeNr;


    //Folgender Abschnitt erlaubt es pro 'pololu a4990 dual-motor-driver' 4 Lampen unabhängig voneinander anzusteuern
    // Update:
   // Wichtiger Hinweis. Es nach dem Code des WS17/18 waren 8 Möglichliche Lampen implementiert.'
   // Für das Mini-Board ist jedoch nur ein Treiber Vorhanden, also nur 4 Lampen
   // Für das Maxi-Board liegen die DIR-Ausgänge auf dem Extension-Board, daher werden sie nicht verwendet. Es können jedoch alle vier PWM-Ausgänge verwendet werden.
  
//  if (ISMAXI)
//  {
//        mPortNrPWM = PORT_L_PWM[lampeNr];
//  }
//  else
//  {
//        if(mLampeNr % 2 == 0) //Lampen 0 und 2 werden durch Pins der PWM-Reihe angesteuert
//        {
//          mPortNrPWM = PORT_L_PWM[mLampeNr/2];
//        }
//        else  //Lampen 1 und 3 werden durch Pins der DIR-Reihe angesteuert
//        {
//          mPortNrPWM = PORT_M_DIR[mLampeNr/2];
//        }
//  }
//  
//  mHelligkeit = 0;
//
//  //Zuweisen des PWM-Generators an einen Port entsprechend der Lampennummer...
//  ledcAttachPin(mPortNrPWM, mLampeNr);  //Pin-Nr, PWM-Generator Nr
//  ledcSetup(mLampeNr, 21700, 8);  //PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
//
//  // Ausgang ausschalten
//  ledcWrite(mLampeNr, 0); //frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
  
}


void Lampe::setMaxi(bool pMaxi)
{
  if (pMaxi)
  {
        mPortNrPWM = PORT_L_PWM[mLampeNr];
  }
  else
  {
        if(mLampeNr % 2 == 0) //Lampen 0 und 2 werden durch Pins der PWM-Reihe angesteuert
        {
          mPortNrPWM = PORT_L_PWM[mLampeNr/2];
        }
        else  //Lampen 1 und 3 werden durch Pins der DIR-Reihe angesteuert
        {
          mPortNrPWM = PORT_M_DIR[mLampeNr/2];
        }
  }
  
  mHelligkeit = 0;

  //Zuweisen des PWM-Generators an einen Port entsprechend der Lampennummer...
  ledcAttachPin(mPortNrPWM, mLampeNr);  //Pin-Nr, PWM-Generator Nr
  ledcSetup(mLampeNr, 21700, 8);  //PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)

  // Ausgang ausschalten
  ledcWrite(mLampeNr, 0); //frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
}


void Lampe::setValues(unsigned int helligkeit)
{
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

  if(mLampeNr%2 == 1) //bei den Lampen 1,3,5,7 sind laut Datenblatt die PWM invertiert (255 = min, 0 = max)
  {
    helligkeit_pwm = 255 - helligkeit_pwm;
  }
  
  //Zuweisen des PWM-Werts an den richtigen Port entsprechend der Lampennummer
  ledcWrite(mLampeNr, helligkeit_pwm);
  //digitalWrite(PORT_M_DIR[mLampeNr], HIGH); //Richtungsangabe, siehe Beschreibung im Konstruktor

  digitalWrite(PIN_L_INH, HIGH);  //Einschalten Lampentreiber
  
  Serial.print("PWM: ");
  Serial.print(mPortNrPWM);
  Serial.print(" Val: ");
  Serial.println(helligkeit_pwm);
}

void Lampe::reRun()
{
  if(mHelligkeit > 0)
  {
    setValues(mHelligkeit);
  }
}

DigitalAnalogIn::DigitalAnalogIn()
{
  mInputNummer = 0;
}

DigitalAnalogIn::DigitalAnalogIn(unsigned int inputNummer)
{
  mInputNummer = inputNummer;

//  if (ISMAXI)
//  {
//    mInputPortNr = SX1509_PORT_DIO_PWMO[mInputNummer];
//  }
//  else
//  {
//    mInputPortNr = PORT_IN[mInputNummer];  
//  }
}

void DigitalAnalogIn::setMaxi(bool pMaxi)
{
  if (pMaxi)
  {
    mInputPortNr = SX1509_PORT_DIO_PWMO[mInputNummer];
  }
  else
  {
    mInputPortNr = PORT_IN[mInputNummer];  
  }
  
}

unsigned int DigitalAnalogIn::getValueDigital()
{
  bool eingabe;
  if (ISMAXI)
  {
      sx1509Object.pinMode(mInputPortNr, INPUT_PULLUP);  //Pin-Modus einrichten: Input mit Pull-Up Widerstand
      eingabe = !sx1509Object.digitalRead(mInputPortNr);  //Inverse Logik: Schalter gedrückt = 1 (= Port liegt auf Masse)
  }
  else
  {
	  //ledcDetachPin(mInputPortNr);	//für den Fall, dass eine PWM auf dem Pin eingerichtet ist, wird diese vor einer Abfrage von dem Pin getrennt
      pinMode(mInputPortNr, INPUT_PULLUP);  //Pin-Modus einrichten: Input mit Pull-Up Widerstand
      eingabe = !digitalRead(mInputPortNr);  //Inverse Logik: Schalter gedrückt = 1 (= Port liegt auf Masse)
  } 
  return (unsigned int) eingabe;
}

unsigned int DigitalAnalogIn::getValueAnalog()
{
  unsigned int eingabe;

  if (ISMAXI)
  {
      sx1509Object.pinMode(mInputPortNr, INPUT); //Pin-Modus einrichten: Input ohne Pull-Up Widerstand
      eingabe = sx1509Object.digitalRead(mInputPortNr);
  }
  else
  {
      pinMode(mInputPortNr, INPUT); //Pin-Modus einrichten: Input ohne Pull-Up Widerstand
      eingabe = analogRead(mInputPortNr);
  }
  return eingabe;
}

void DigitalAnalogIn::setValueDigital(bool ledLevel)
{
  if (ISMAXI)
  {
    sx1509Object.pinMode(mInputPortNr, OUTPUT);  //Pin_Modus einrichten: Output
  }
  else
  {
    pinMode(mInputPortNr, OUTPUT);  //Pin_Modus einrichten: Output
  }
  
  Serial.print("Setze LED ");
  Serial.print(mInputNummer);
  Serial.print(" auf ");
  if (ledLevel)
  {
      if (ISMAXI)
      {
        sx1509Object.digitalWrite(mInputPortNr, HIGH); //Pin auf HIGH setzen
      }
      else
      {
        digitalWrite(mInputPortNr, HIGH); //Pin auf HIGH setzen
      }
    Serial.println(" HIGH");
  }
  else
  {
      if (ISMAXI)
      {
            sx1509Object.digitalWrite(mInputPortNr, LOW); //Pin auf LOW setzen
      }
      else
      {
            digitalWrite(mInputPortNr, LOW); //Pin auf LOW setzen
      }
    Serial.println(" LOW");
  }
}



/***
 * 
 * 
 * FOLGENDER ABSCHNITT IST NICHT IMPLEMENTIERT IN DER QUEUE
 * 
 * 
 */
//#ifdef MAXI
//DigitalIO_PWMout::DigitalIO_PWMout()
//{
//#ifdef DEBUG
//  Serial.println("Ctor DIO_PWMO N/A");
//#endif // DEBUG
//
//
//  //Serial.println("DigitalIO_PWMout mit parameterlosem Ctor initialisiert");
//  mIONumber = 0;
//}
//
//DigitalIO_PWMout::DigitalIO_PWMout(byte io, byte inOut)
//{
//#ifdef DEBUG
//  Serial.println("Ctor DIO_PWMO " + io);
//#endif // DEBUG
//
//
//  mIONumber = io;
//  mDirection = inOut;
//
//  mIOPin = SX1509_PORT_DIO_PWMO[io];
//
//  sx1509Object.pinMode(mIOPin, mDirection);
//}
//
//unsigned int DigitalIO_PWMout::getValue()
//{
//  //if input is configured as INPUT (no pull up) return value as is
//  if (mDirection == INPUT) {
//    unsigned int ret = sx1509Object.digitalRead(mIOPin);
//
//    return ret;
//  }
//
//  //if input is configured as INPUT_PULLUP return inverted value
//  if (mDirection != INPUT_PULLUP) {
//    sx1509Object.pinMode(mIOPin, INPUT);
//    delay(20);
//    sx1509Object.digitalWrite(mIOPin, HIGH);
//    mDirection = INPUT_PULLUP;
//    Serial.println("SX1509 IO Output wurde in 'getValue' zu Input (Pullup) geändert, IONumber: " + mIONumber);
//    Serial.println("IOPin: " + mIOPin);
//  }
//  if (sx1509Object.digitalRead(mIOPin) != 0) {
//    return 0;
//  }
//  else {
//    return 1;
//  }
//  return sx1509Object.digitalRead(mIOPin);
//}
//
//
//void DigitalIO_PWMout::setValueDig(bool val)
//{
//  if (mDirection != OUTPUT) {
//    sx1509Object.pinMode(mIOPin, OUTPUT);
//    mDirection = OUTPUT;
//    Serial.println("SX1509 IO Input wurde in 'setValueDig' zu Output geändert, IONumber: " + mIONumber);
//    Serial.println("IOPin: " + mIOPin);
//  }
//
//  if (val)
//    sx1509Object.digitalWrite(mIOPin, HIGH);
//  else
//    sx1509Object.digitalWrite(mIOPin, LOW);
//}
//
//void DigitalIO_PWMout::setPWM(unsigned char pwmVal)
//{
//  if (mDirection != OUTPUT) {
//    sx1509Object.pinMode(mIOPin, OUTPUT);
//    mDirection = OUTPUT;
//    Serial.println("SX1509 IO Input wurde in 'setPWM' zu Output geändert, IONumber: " + mIONumber);
//    Serial.println("IOPin: " + mIOPin);
//  }
//
//  sx1509Object.analogWrite(mIOPin, pwmVal);
//}
//
//#endif
