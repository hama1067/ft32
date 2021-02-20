/*
 * Ausgangstreiber für ESP32-Fischertechnik-Anbindung
 * Autor: Johannes Marquart
 * 
 * modified by: F.J.P
 * date: 2020-10-27
 */

#include "ft_ESP32_IOobjects.h"

int PORT_M_PWM[MOTOR_QTY];    // = {};//Output-Pins Motor-Drehzahl
int PORT_IN[DAIN_QTY];        //Input-Pins Ditital/Analog
bool ISMAXI = false;

/* ====================================================================================================================== */

/* i2c SDA = PIN 21, SCL = PIN 22 */
SX1509 sx1509Object;

/* led strip, connected to pin LED_PIN */
Adafruit_NeoPixel * led_strip = new Adafruit_NeoPixel(LED_QTY, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ====================================================================================================================== */

CheckMaxiExtension::CheckMaxiExtension() {
  mAddress = SX1509_I2C_ADDRESS;
  mBoard = "";
}

CheckMaxiExtension::CheckMaxiExtension(byte address) {
  mAddress = address;
  mBoard = "";
}

bool CheckMaxiExtension::CheckMaxi() {
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
		for (int i = 0; i < DAIN_QTY; i++)
		{
			PORT_IN[i] = MAXI_PORT_IN[i];
		}
		return true;
	}
}

/* ====================================================================================================================== */

Motor::Motor() {
	//Abschalten des Motortreibers, welcher von diesem Objekt versorgt wird.
	//Evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_M_INH, OUTPUT);
	digitalWrite(PIN_M_INH, LOW);

	mMotorNr = 0;
	mPortNrPWM = 0;
	mPortNrDir = 0;
	mDirectionMode = 1;
	mDrehzahl = 0;
}

Motor::Motor(unsigned int motorNr) {
	//Abschalten des Motortreibers, evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_M_INH, OUTPUT);
	digitalWrite(PIN_M_INH, LOW);
  
	//Initialisieren des Motorobjektes
  mMotorNr = motorNr;
  mPortNrPWM = 0;
  mPortNrDir = 0;
  mDirectionMode = 2;
  mDrehzahl = 0;

/*
  if (ISMAXI)
  {
    mPortNrDir = SX1509_PORT_M_DIR[mMotorNr];
  }
  else
  {
    mPortNrDir = PORT_M_DIR[mMotorNr];
  }
*/
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

void Motor::setMaxi(bool pMaxi) {
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

void Motor::setValues(unsigned char directionMode, unsigned int drehzahl) {
  /*
   *  mDirectionMode: 
   *  0 -> right
   *  1 -> left
   *  2 -> brake
  */
  mDirectionMode = directionMode;
  mDrehzahl = drehzahl;

  digitalWrite(PIN_M_INH, LOW);

  Serial.print("[io] Motor " + (String)mMotorNr);
  Serial.print(" dreht in Richtung " + (String)mDirectionMode);
  Serial.println(" mit Drehzahl " + (String)mDrehzahl);

  //Berechnen der PWM-Werte
  int drehzahl_pwm;
  if (mDrehzahl < 1) {
    drehzahl_pwm = 0;                         //170
  } else if (mDrehzahl > 49) {
    drehzahl_pwm = 255;
  } else {
    drehzahl_pwm = 170 + mDrehzahl * (85 / 50);         //170+mDrehzahl*85/8
    Serial.println("Drehzahl PWM: " + (String)drehzahl_pwm);
  }

  //Zuweisen der Richtung an den richtigen Pin entsprechend der Motornr.
  // mDirectionMode == 1 -> right
  if (mDirectionMode == 1) {
    //digitalWrite(mPortNrDir, HIGH);
    if (ISMAXI) {
      sx1509Object.digitalWrite(mPortNrDir, 1);  //Richtungspin wird auf HIGH - Rechtslauf gesetzt
    }

    if(mMotorNr==0 || mMotorNr==2) {
      analogWrite(PORT_M_0[1],0);
      analogWrite(PORT_M_0[0],drehzahl_pwm);
    } else if (mMotorNr==1 || mMotorNr==3) {
      analogWrite(PORT_M_1[1],0);
      analogWrite(PORT_M_1[0],drehzahl_pwm);
    }   
  } 
  
  // mDirectionMode == 1 -> right
  else if (mDirectionMode == 0) {
    //digitalWrite(mPortNrDir, LOW);
    if (ISMAXI) {
      sx1509Object.digitalWrite(mPortNrDir, 0);  //Richtungspin wird auf LOW - Linkslauf gesetzt
    }

    if (mMotorNr==0 ||mMotorNr==2) {
      analogWrite(PORT_M_0[0],0);
      analogWrite(PORT_M_0[1],drehzahl_pwm);
    } else if (mMotorNr==1 || mMotorNr==3) {
      analogWrite(PORT_M_1[0],0);
      analogWrite(PORT_M_1[1],drehzahl_pwm);
    }
  } 
  
  // mDirectionMode == 2 -> brake
  else {
    digitalWrite(PIN_M_INH, HIGH);  //Einschalten Motortreiber
    
    if(mMotorNr==0 || mMotorNr==2) {
      analogWrite(PORT_M_0[1], -1);  
      analogWrite(PORT_M_0[0], -1);
    } else if (mMotorNr==1 || mMotorNr==3) {
      analogWrite(PORT_M_1[1], -1);  
      analogWrite(PORT_M_1[0], -1);   
    }
  }
  
  digitalWrite(PIN_M_INH, HIGH);  //Einschalten Motortreiber
  //Serial.print("  raw: DirPin: " + (String)mPortNrDir);
  //Serial.print(mPortNrDir);
  //Serial.print(" PWMPin: " + (String)mPortNrPWM);
  //Serial.print(mPortNrPWM);
  //Serial.println(" Val: " + (String)drehzahl_pwm);
  //Serial.println(drehzahl_pwm);
}

void Motor::reRun() {
  if(mDrehzahl > 0)
  {
    setValues(mDirectionMode, mDrehzahl);
  }
}

/* ====================================================================================================================== */

CServoMotor::CServoMotor() {
	mMotorNr = 0;
	mPortNrPWM = 0;
	mMinDuty = 4;
	mMaxDuty = 11;
	mRelDuty = 0;
}

CServoMotor::CServoMotor(unsigned int motorNr, unsigned int dutyCycle) {
  mPortNrPWM = 13;
	//init of servomotor-object
	mMotorNr = motorNr;
	//mPinNrPWM = PIN_M_PWM[motorNr];
	mMinDuty = 11;	//minimal real DutyCycle in %
	mMaxDuty = 4;	//maximal real DutyCycle in %
	mRelDuty = dutyCycle < 100 ? dutyCycle : 100;	//limiting maximum value to 100(%)
	//mLedcChannel = 4;	//channel 4 - servo only used at mini-board -> only channels 0..3 are used by motors
  mLedcChannel=10;
  ledcAttachPin(mPortNrPWM, mLedcChannel);  //Pin-Nr für Drehzahl, PWM-Generator Nr
  ledcSetup(mLedcChannel, 50, 12);  //PWM-Generator Nr, 50 Hz PWM, 12-bit resolution (0..255)
  
}

void CServoMotor::setMaxi(bool pMaxi) {
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

void CServoMotor::setValues(unsigned int dutyCycle) {
	if (!ISMAXI)
	{
		mRelDuty = dutyCycle < 100 ? dutyCycle : 100;	//limiting maximum value to 100(%)
		Serial.print("[io] Servo ");
		Serial.print(mMotorNr);
		Serial.print(" auf Pos ");
		Serial.println(mRelDuty);
    
    //analogWrite(mPortNrPWM, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);
		ledcWrite(mLedcChannel, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);	//calculate real duty-cycle for servos: 0%rel = 4%, 100%rel = 11%
	}
}

void CServoMotor::reRun() {
	if (!ISMAXI)
	{
    //analogWrite(mPortNrPWM, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);
		ledcWrite(mLedcChannel, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);
	}
}

/* ====================================================================================================================== */

void led_init() {
  /* initialize led strip */
  pinMode(LED_PIN,OUTPUT);
  led_strip->begin();
  led_strip->show();
}

void led_clear() {
  /* clear buffer (pixel information) and update through show() */
  led_strip->clear();
  led_strip->show();
}

void led_reset(String &option, array<Led, LED_QTY> &ledArray) {
  if(option == "A") {
    for(int i=0; i < LED_QTY; i++) {
      ledArray.at(i).setValues(0, 0);  //Ausgang zur 0 setzen
    }
    Serial.println("[io] Setze alle LEDs zurueck ...");
  } else {
     ledArray.at(option.toInt()).setValues(0, 0);
     Serial.println("[io] Setze LED" + option + " zurueck ...");
  }
}

void led_update(array<Led, LED_QTY> &ledArray) {    
    for(int i=0; i < LED_QTY; i++) {
      ledArray[i].reRun();
    }    
    
    /* update led strip */
    led_strip->show();
}

void led_update(Led ledArray[]) {
    /* update led strip */
    led_strip->clear();
    
    for(int i=0; i < LED_QTY; i++) {
      ledArray[i].reRun();  
    }    

    led_strip->show();
}

Led::Led() {
  //Abschalten des Ledntreibers, welcher von diesem Objekt versorgt wird und setzen der default Werte
  
  mLedNr = 0;
  mColor = 0; // -> RED
  mColorName = "RED";
  mBrightness = 0;
  
  mColorRGB[0] = 255;
  mColorRGB[1] = 0;
  mColorRGB[2] = 0;
}

Led::Led(unsigned int ledNr) {
  //Initialisieren des Led-objektes
  mLedNr = ledNr;
  mColor = 0; // -> Off
  mColorName = "RED";
  mBrightness = 0;

  mColorRGB[0] = 255;
  mColorRGB[1] = 0;
  mColorRGB[2] = 0;
}

void Led::setMaxi(bool pMaxi) {
  /* TODO */
}

void Led::setValues(int brightness, int ledColor) {
  mColor = ledColor;
  mBrightness = brightness;

  setLedRGBColorByIdentifier(ledColor, brightness);
  
  Serial.print("[io] led " + (String)mLedNr + " leuchtet mit Helligkeit ");
  Serial.print(mBrightness);
  Serial.println(" und Farbe " + getRGBColorName());
  
  led_strip->setPixelColor(mLedNr,mColorRGB[0],mColorRGB[1],mColorRGB[2]); 
}

void Led::reRun() {
  if(mBrightness > 0) {
    led_strip->setPixelColor(mLedNr,mColorRGB[0],mColorRGB[1],mColorRGB[2]); 
  }
}

void Led::setLedRGBColorByIdentifier(int colorNumber, int brigthness) {

  mColorRGB[0] = 0;
  mColorRGB[1] = 0;
  mColorRGB[2] = 0;

  if(brigthness >= 0 && brigthness < 256) {
    switch(colorNumber) {
      case 0:
        mColorName = "Off";
        mColorRGB[0] = 0;
        mColorRGB[1] = 0;
        mColorRGB[2] = 0;
      break;
      case 1:
        mColorName = "RED";
        mColorRGB[0] = brigthness;
      break;
      case 2:
        mColorName = "GREEN";
        mColorRGB[1] = brigthness;
      break;
      case 3:
        mColorName = "BLUE";
        mColorRGB[2] = brigthness;
      break;
      default:
        mColorName = "RED";
        mColorRGB[0] = 255;
      break;
    }
  } else {
    mColorName = "RED";
    mColorRGB[0] = 255;    
  }
}

String Led::getRGBColorName() {
  return mColorName;
}

/* ====================================================================================================================== */

// ===== ENCODERSTEUERUNG
// Für ISR müssen einige Elemente global definiert werden:
// Interrupt-Service-Routinen ==> Hochzählen der Encoder-Zähler
// Folgende Variablen müssen (leider) in der c++-Datei deklariert werden.
// Andernfalls kommt es zu include-Fehlern.
// Die Absolut-Encoderzähler werden zur Bestimmung der Reglerabweichung verwendet
// und zur Erkennung, ob der Roboter die gewünschte Strecke gefahren ist.
int encPosAbs0;
int encPosAbs1;
// Variablen zur Entprellung der Lichtschrankensignale
int64_t time_ISREnc0[2];
int64_t time_ISREnc1[2];

// Interrupt-Service-Routinen zur Flankenzählung der Encoder
void ISREnc0() {
    // Entprellen: Prüfen, ob seit letztem Schritt mind. 5ms vergangen sind
    time_ISREnc0[0] = esp_timer_get_time();
    if(time_ISREnc0[0] - time_ISREnc0[1] > 2000) {
    // Zähler inkrementieren und Zeit speichern
      encPosAbs0++;
      time_ISREnc0[1] = time_ISREnc0[0];
    }
}
void ISREnc1() {
    // Entprellen: Prüfen, ob seit letztem Schritt mind. 5ms vergangen sind
    time_ISREnc1[0] = esp_timer_get_time();
    if(time_ISREnc1[0] - time_ISREnc1[1] > 2000) {
    // Zähler inkrementieren und Zeit speichern
      encPosAbs1++;
      time_ISREnc1[1] = time_ISREnc1[0];
    }
}

// Konstruktor
CEncoderStrecke::CEncoderStrecke() {}

// Beide Encoder aktivieren ==> Impulse werden gezählt
// Zusätzlich alle erforderlichen Variablen zurücksetzen
void CEncoderStrecke::enableInterrupts() {
    // Variablen zurücksetzen
    // Encoder-Zähler und Entprell-Zeiten zurücksetzen
    encPosAbs0 = 0;
    encPosAbs1 = 0;
    time_ISREnc0[1] = 0;
    time_ISREnc1[1] = 0;    

    // Abtastzeit für den Regler zurücksetzen
    time_ReglerInt[0] = esp_timer_get_time();

    // Reglervariablen zurücksetzen
    integrierer = 0;
    RegAbweichung[0] = 0;
    RegStellgr[0] = 0;
    RegStellgr[1] = 0;
    diff[0] = 0;

    // Encoderpins konfigurieren, Interrupts hinzufügen
    pinMode(PIN_ENCODER_0, INPUT);
    pinMode(PIN_ENCODER_1, INPUT);

    attachInterrupt(PIN_ENCODER_0, ISREnc0, RISING);
    attachInterrupt(PIN_ENCODER_1, ISREnc1, RISING);

    Serial.println("Beide Encoder aktiv");
}

// Beide Encoder deaktivieren ==> es werden keine weiteren Impulse gezählt
void CEncoderStrecke::disableInterrupts() {
    // Interrupts deaktivieren
    detachInterrupt(PIN_ENCODER_0);
    detachInterrupt(PIN_ENCODER_1);

    Serial.println("Beide Encoder deaktiviert");
}

void CEncoderStrecke::enableMotors() {
  // Motortreiber muss bereits aktiv sein, bevor die Reglerfunktion gestartet wird: 
  pinMode(PIN_M_INH, OUTPUT);     // Pin konfigurieren
  digitalWrite(PIN_M_INH, HIGH);  // Einschalten Motortreiber

  // PWM-Basiswert setzen (Basisgeschwindigkeit des Roboters)

  // Motoren zunächst stoppen
  setMotorSpeed(0.0f, 0.0f);

  // PWM-Kanäle einrichten: Kanal0 für Motor0, Kanal1 für Motor1
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
}

void CEncoderStrecke::disableMotors() {
  // Motoren stoppen
  setMotorSpeed(0.0f, 0.0f);

  // Motortreiber ausschalten
  digitalWrite(PIN_M_INH, LOW);  //Ausschalten Motortreiber
}

// Reglerfunktion: Ausgehend von der aktuellen Rotationsgeschwindigkeit wird die nächste
// Stellgröße für den jeweiligen Motor berechnet und anschließend auf den Motor gegeben.
// Die Sollposition (in cm) wird übergeben. Ist der Return-Wert true, so wurde die Sollposition
// erreicht. Bei false wurde die Position noch nicht erreicht.
bool CEncoderStrecke::reglerStrecke(int posSoll, int richtung) {
    // Sollposition von cm in Impulse umrechnen
    int anzImpulse = lround(posSoll*1.818);

    // Prüfen, ob Sollposition erreicht ==> Regler stoppen
    if(encPosAbs0 >= anzImpulse || encPosAbs1 >= anzImpulse) {
        // Motoren stoppen
        RegStellgr[0] = 0.0f;
        RegStellgr[1] = 0.0f;
        setMotorSpeed(RegStellgr[0], RegStellgr[1]);

        // Die Sollposition der Encoder ist erreicht ==> true zurückgeben und
        // Reglerfunktion sofort beenden
        return true;
    }

    // alte Proportionalanteile weiterschieben (für D-Anteil)
    RegAbweichung[1] = RegAbweichung[0];

    // PROPORTIONALANTEIL der Reglerabweichung bestimmen
    RegAbweichung[0] = encPosAbs0 - encPosAbs1;

    // Abtastzeiten für die Integrierer weiterschieben: time_ReglerInt[neueZeit,alteZeit]
    time_ReglerInt[1] = time_ReglerInt[0];        // alte Zeit weiterschieben
    time_ReglerInt[0] = esp_timer_get_time();     // aktuelle Zeit zwischenspeichern
    // Zeitdifferenz des Abtastschritts berechnen (in sec)
    double timediff = (time_ReglerInt[0] - time_ReglerInt[1])/1000000.0;

    // Fehlerüberprüfung: die Zeitdifferenz wird nur verwendet, wenn sie im richtigen Bereich
    // (0 bis 2 Sekunden) liegt. Andernfalls ignorieren.
    if(timediff < 2.0 && timediff > 0.0) {
        // Bei Stellgrößenbeschränkung: Integrierer anhalten (Anti-Windup)
        if(RegStellgr[0] > -254.9 && RegStellgr[0] < 254.9 && RegStellgr[1] > -254.9 && RegStellgr[1] < 254.9) {
          integrierer += (timediff*RegAbweichung[0]);
        }

        // DIFFERENZIALANTEIL: Regelabweichung ableiten
        diff[1] = diff[0];
        diff[0] = (RegAbweichung[0] - RegAbweichung[1] + (TF_ENC - timediff)*diff[1])/TF_ENC;
    }
    else {    // Fehlerausgabe
        Serial.print("Abtastzeitdifferenz fehlerhaft: ");
        Serial.println(timediff);
    }

    // P-I-D-Anteile summieren und Stellgröße berechnen (aktuell wird der D-Anteil nicht verwendet, 
    // da er zu erhöhten Schwingungen führt)
    if(richtung == 0) {   // vorwärts fahren
      RegStellgr[0] = PWM0 - P_ENC*RegAbweichung[0] - 1.0/TN_ENC*integrierer;// - TV_ENC*diff[0];
      RegStellgr[1] = PWM0 + P_ENC*RegAbweichung[0] + 1.0/TN_ENC*integrierer;// + TV_ENC*diff[0];
    }
    else if(richtung == 1) {    // rückwärts fahren
      RegStellgr[0] = -PWM0 + P_ENC*RegAbweichung[0] + 1.0/TN_ENC*integrierer;// + TV_ENC*diff[0];
      RegStellgr[1] = -PWM0 - P_ENC*RegAbweichung[0] - 1.0/TN_ENC*integrierer;// - TV_ENC*diff[0];
    }

    // PWM-Signal auf die Motoren geben
    setMotorSpeed(RegStellgr[0], RegStellgr[1]);

    // die Sollposition der Encoder ist momentan noch nicht erreicht
    // ==> false zurückgeben
    return false;
}

// Reglerfunktion für die Winkelfahrt: Der Roboter dreht sich um einen bestimmten Winkel nach
// rechts oder links
bool CEncoderStrecke::reglerWinkel(int winkelSoll, int richtung) {
  // Winkel in Encoderimpulse umrechnen: 2 Umdrehungen (720° entsprechen 172 Radimpulsen)
  int anzImpulse = lround(winkelSoll*0.23889);

  // Prüfen, ob Sollposition erreicht
  if(encPosAbs0 >= anzImpulse || encPosAbs1 >= anzImpulse) {
        // Motoren stoppen
        RegStellgr[0] = 0.0f;
        RegStellgr[1] = 0.0f;
        setMotorSpeed(RegStellgr[0], RegStellgr[1]);

        // Die Sollposition der Encoder ist erreicht ==> true zurückgeben und
        // Reglerfunktion sofort beenden
        return true;
    }

    // alte Proportionalanteile weiterschieben (für D-Anteil)
    RegAbweichung[1] = RegAbweichung[0];

    // PROPORTIONALANTEIL der Reglerabweichung bestimmen
    RegAbweichung[0] = encPosAbs0 - encPosAbs1;

    // Abtastzeiten für die Integrierer weiterschieben: time_ReglerInt[neueZeit,alteZeit]
    time_ReglerInt[1] = time_ReglerInt[0];        // alte Zeit weiterschieben
    time_ReglerInt[0] = esp_timer_get_time();     // aktuelle Zeit zwischenspeichern
    // Zeitdifferenz des Abtastschritts berechnen (in sec)
    double timediff = (time_ReglerInt[0] - time_ReglerInt[1])/1000000.0;

    // Fehlerüberprüfung: die Zeitdifferenz wird nur verwendet, wenn sie im richtigen Bereich
    // (0 bis 2 Sekunden) liegt. Andernfalls ignorieren.
    if(timediff < 2.0 && timediff > 0.0) {
        // Bei Stellgrößenbeschränkung: Integrierer anhalten (Anti-Windup)
        if(RegStellgr[0] > -254.9 && RegStellgr[0] < 254.9 && RegStellgr[1] > -254.9 && RegStellgr[1] < 254.9) {
          integrierer += (timediff*RegAbweichung[0]);
        }

        // DIFFERENZIALANTEIL: Regelabweichung ableiten
        diff[1] = diff[0];
        diff[0] = (RegAbweichung[0] - RegAbweichung[1] + (TF_ENC - timediff)*diff[1])/TF_ENC;
    }
    else {    // Fehlerausgabe
        Serial.print("Abtastzeitdifferenz fehlerhaft: ");
        Serial.println(timediff);
    }

    // P-I-D-Anteile summieren und Stellgröße berechnen (aktuell wird der D-Anteil nicht verwendet, 
    // da er zu erhöhten Schwingungen führt)
    // Die Variable "richtung" ist 0 bei links und 1 bei rechts
    // bei links: M1 positive Drehrichtung und M0 negative Drehrichtung
    if(richtung == 0) { // M1 positiv, M0 negative Drehrichtung
        RegStellgr[0] = -PWM0 + P_ENC*RegAbweichung[0] + 1.0/TN_ENC*integrierer;// + TV_ENC*diff[0];
        RegStellgr[1] = PWM0 + P_ENC*RegAbweichung[0] + 1.0/TN_ENC*integrierer;// + TV_ENC*diff[0];
    }
    else if(richtung == 1) {  // M1 negativ, M0 positive Drehrichtung
        RegStellgr[0] = PWM0 - P_ENC*RegAbweichung[0] - 1.0/TN_ENC*integrierer;// - TV_ENC*diff[0];
        RegStellgr[1] = -PWM0 - P_ENC*RegAbweichung[0] - 1.0/TN_ENC*integrierer;// - TV_ENC*diff[0];
    }
    else {
      Serial.println("Fehler: falsche Richtungsvorgabe!");
    }

    // PWM-Signal auf die Motoren geben
    setMotorSpeed(RegStellgr[0], RegStellgr[1]);

    // die Sollposition der Encoder ist momentan noch nicht erreicht
    // ==> false zurückgeben
    return false;
}

// setMotorSpeed: setzt die Rotationsgeschwindigkeit für beide Motoren (zwischen -255 und +255)
// ein negatives Vorzeichen bedeutet eine Umkehr der Drehrichtung
void CEncoderStrecke::setMotorSpeed(float speedM0, float speedM1) {
  // Motortreiber muss bereits aktiv sein: Vor dem ersten Aufruf dieser Funktion:
  // pinMode(PIN_M_INH, OUTPUT);
  // digitalWrite(PIN_M_INH, HIGH);  //Einschalten Motortreiber
  // // und bereits PWM-Kanäle einrichten:
  // ledcSetup(0, 1000, 8);
  // ledcSetup(1, 1000, 8);

  if (speedM0 > 0.0f) {
    if (speedM0 >= 254.9f) {
      // Motor läuft mit maximaler Geschwindigkeit
      ledcDetachPin(PORT_M_0[0]);
      ledcDetachPin(PORT_M_0[1]);
      pinMode(PORT_M_0[0], OUTPUT);
      pinMode(PORT_M_0[1], OUTPUT);
      digitalWrite(PORT_M_0[0], LOW);
      digitalWrite(PORT_M_0[1], HIGH);
    }
    else if(speedM0 >= 0.01f) {
      // PWM konfigurieren
      ledcDetachPin(PORT_M_0[0]);
      ledcAttachPin(PORT_M_0[1], 0);            
      ledcWrite(0, speedM0);
      pinMode(PORT_M_0[0], OUTPUT);
      digitalWrite(PORT_M_0[0], LOW);
    }
    else {
      // Motor ausschalten
      ledcDetachPin(PORT_M_0[0]);
      ledcDetachPin(PORT_M_0[1]);
      digitalWrite(PORT_M_0[0], LOW);
      digitalWrite(PORT_M_0[1], LOW);
    }
  } else {
    if (speedM0 <= -254.9f) {
      // Motor läuft mit maximaler Geschwindigkeit
      ledcDetachPin(PORT_M_0[0]);
      ledcDetachPin(PORT_M_0[1]);
      pinMode(PORT_M_0[0], OUTPUT);
      pinMode(PORT_M_0[1], OUTPUT);
      digitalWrite(PORT_M_0[0], HIGH);
      digitalWrite(PORT_M_0[1], LOW);
    }
    else if(speedM0 <= -0.01f) {
      // PWM konfigurieren
      ledcDetachPin(PORT_M_0[1]);
      ledcAttachPin(PORT_M_0[0], 0);            
      ledcWrite(0, -speedM0);
      pinMode(PORT_M_0[1], OUTPUT);
      digitalWrite(PORT_M_0[1], LOW);
    }
    else {
      // Motor ausschalten
      ledcDetachPin(PORT_M_0[0]);
      ledcDetachPin(PORT_M_0[1]);
      digitalWrite(PORT_M_0[0], LOW);
      digitalWrite(PORT_M_0[1], LOW);
    }
  }
  if (speedM1 > 0.0f) {
    if (speedM1 >= 254.9f) {
      // Motor läuft mit maximaler Geschwindigkeit
      ledcDetachPin(PORT_M_1[0]);
      ledcDetachPin(PORT_M_1[1]);
      pinMode(PORT_M_1[0], OUTPUT);
      pinMode(PORT_M_1[1], OUTPUT);
      digitalWrite(PORT_M_1[0], LOW);
      digitalWrite(PORT_M_1[1], HIGH);
    }
    else if(speedM1 >= 0.01f) {
      // PWM konfigurieren
      ledcDetachPin(PORT_M_1[0]);
      ledcAttachPin(PORT_M_1[1], 1);            
      ledcWrite(1, speedM1);
      pinMode(PORT_M_1[0], OUTPUT);
      digitalWrite(PORT_M_1[0], LOW);
    }
    else {
      // Motor ausschalten
      ledcDetachPin(PORT_M_1[0]);
      ledcDetachPin(PORT_M_1[1]);
      digitalWrite(PORT_M_1[0], LOW);
      digitalWrite(PORT_M_1[1], LOW);
    }
  } else {
    if (speedM1 <= -254.9f) {
      // Motor läuft mit maximaler Geschwindigkeit
      ledcDetachPin(PORT_M_1[0]);
      ledcDetachPin(PORT_M_1[1]);
      pinMode(PORT_M_1[0], OUTPUT);
      pinMode(PORT_M_1[1], OUTPUT);
      digitalWrite(PORT_M_1[0], HIGH);
      digitalWrite(PORT_M_1[1], LOW);
    }
    else if(speedM1 <= -0.01f) {
      // PWM konfigurieren
      ledcDetachPin(PORT_M_1[1]);
      ledcAttachPin(PORT_M_1[0], 1);            
      ledcWrite(1, -speedM1);
      pinMode(PORT_M_1[1], OUTPUT);
      digitalWrite(PORT_M_1[1], LOW);
    }
    else {
      // Motor ausschalten
      ledcDetachPin(PORT_M_1[0]);
      ledcDetachPin(PORT_M_1[1]);
      digitalWrite(PORT_M_1[0], LOW);
      digitalWrite(PORT_M_1[1], LOW);
    }
  }
}

/* ====================================================================================================================== */

DigitalAnalogIn::DigitalAnalogIn() {
  mInputNummer = 0;
}

DigitalAnalogIn::DigitalAnalogIn(unsigned int inputNummer) {
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

void DigitalAnalogIn::setMaxi(bool pMaxi) {
  if (pMaxi)
  {
    mInputPortNr = SX1509_PORT_DIO_PWMO[mInputNummer];
	currentPinMode = INPUT_PULLUP;
	sx1509Object.pinMode(mInputPortNr, currentPinMode);
  }
  else
  {
    mInputPortNr = PORT_IN[mInputNummer];
	currentPinMode = INPUT_PULLUP;
	pinMode(mInputPortNr, currentPinMode);
  }
  
}

unsigned int DigitalAnalogIn::getValueDigital() {
  bool eingabe;
  if (ISMAXI)
  {
	  if (INPUT_PULLUP != currentPinMode)
	  {
		  sx1509Object.pinMode(mInputPortNr, INPUT_PULLUP);  //Pin-Modus einrichten: Input mit Pull-Up Widerstand
	  }
      eingabe = !sx1509Object.digitalRead(mInputPortNr);  //Inverse Logik: Schalter gedrückt = 1 (= Port liegt auf Masse)
  }
  else
  {
	  ledcDetachPin(mInputPortNr);	//für den Fall, dass eine PWM auf dem Pin eingerichtet ist, wird diese vor einer Abfrage von dem Pin getrennt
	  if (INPUT_PULLUP != currentPinMode)
	  {
		  pinMode(mInputPortNr, INPUT_PULLUP);  //Pin-Modus einrichten: Input mit Pull-Up Widerstand
	  }
      eingabe = !digitalRead(mInputPortNr);  //Inverse Logik: Schalter gedrückt = 1 (= Port liegt auf Masse)
  } 
  return (unsigned int) eingabe;
}

unsigned int DigitalAnalogIn::getValueAnalog() {
  unsigned int eingabe;

  if (ISMAXI)
  {
	  if (INPUT != currentPinMode)
	  {
		  sx1509Object.pinMode(mInputPortNr, INPUT); //Pin-Modus einrichten: Input ohne Pull-Up Widerstand
	  }
      eingabe = sx1509Object.digitalRead(mInputPortNr);
  }
  else
  {
	  if (INPUT != currentPinMode)
	  {
		  pinMode(mInputPortNr, INPUT); //Pin-Modus einrichten: Input ohne Pull-Up Widerstand
	  }
      eingabe = analogRead(mInputPortNr);
  }
  return eingabe;
}

void DigitalAnalogIn::setValueDigital(bool ledLevel) {
  if (ISMAXI)
  {
    sx1509Object.pinMode(mInputPortNr, OUTPUT);  //Pin_Modus einrichten: Output
  }
  else
  {
    pinMode(mInputPortNr, OUTPUT);  //Pin_Modus einrichten: Output
  }
  
  Serial.print("[io] Setze LED ");
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

/* ====================================================================================================================== */

/***
 * 
 * 
 * FOLGENDER ABSCHNITT IST NICHT IMPLEMENTIERT IN DER QUEUE
 * 
 * 
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
*/
