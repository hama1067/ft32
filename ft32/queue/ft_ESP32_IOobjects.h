/*
 * Ausgangstreiber für ESP32-Fischertechnik-Anbindung
 * Autor: Johannes Marquart
 *
 * Editiert von SS2020 by F.J.P
 *
 *
 * Hinweis: MaxiBoard kann keine Analogdaten lesen
 */
#ifndef FT_ESP32_IOOBJECTS_H
#define FT_ESP32_IOOBJECTS_H

#include <Arduino.h>
#include <SparkFunSX1509.h>
#include <Wire.h>
#include <analogWrite.h>
#include <Adafruit_NeoPixel.h>

/*  */
#include <array>
using namespace std;

//#include <SX1509_IO_Expander/src/SparkFunSX1509.h>

/* ports */
constexpr size_t MOTOR_QTY = 2;	    // max. Anzahl der Motoren
constexpr size_t LED_QTY = 11;      // max. Anzahl der Leds
constexpr size_t DAIN_QTY = 8;      // max. Anzahl der Eingänge (digital/analog) (Normal: 8, Wettbewerb: 2)
constexpr size_t SERVO_QTY = 1;	    // max. Anzahl der Servomotoren
constexpr size_t DIO_PWMO_QTY = 8;

#define LED_PIN 14                                                            // Output-Pin Led strip
extern int PORT_IN[DAIN_QTY];                                                 // Input-Pins Ditital/Analog

const int PORT_M_0[2] = {17, 16};                                             // Motor 0, AIN1=17, AIN2=16
const int PORT_M_1[2] = {2, 4};                                               // Motor 1, AIN1=4, AIN2=2
const int PIN_M_INH = 27;                                                     // Output-Pin Einschalten Motortreiber

extern int PORT_M_PWM[MOTOR_QTY];                                             // Output-Pins Motor-Drehzahl
const int PORT_M_DIR[MOTOR_QTY] = { 16, 2 };                                  // ,16, 2};	//Output-Pins Motor-Richtung !! NUR 17 und 4 sind Verbunden, Rest ist Dummy
const int PORT_M_ENCODER[MOTOR_QTY] = { 36, 39 };                             // ,25, 26};//InputPins Motor Encoder -> MOMENTAN NICHT VERWENDET
const int PIN_ENCODER_0 = 36;                // Pin für Encoder 0
const int PIN_ENCODER_1 = 39;               // Pin für Encoder 1

extern bool ISMAXI;

const int MINI_PORT_M_PWM[MOTOR_QTY] = { 17, 4};//, 17, 4 };                  // Output-Pins Motor-Drehzahl !! NUR 16 und 2 sind Verbunden, Rest ist Dummy
const int MINI_PORT_IN[DAIN_QTY] = { 32, 25, 33, 32, 25, 33, 32, 25 };

const int MAXI_PORT_M_PWM[MOTOR_QTY] = { 4, 2}; //, 13, 15 };                 // Output-Pins Motor-Drehzahl { 16, 2, 13, 15 }
const int MAXI_PORT_IN[DAIN_QTY] = { 34, 35, 34, 35, 34, 35, 34, 35 };        // Input-Pins Digital/Analog   NUR 34 und 35 in Hardware     MOMENTAN NICHT VERWENDET

const byte SX1509_PORT_DIO_PWMO[DIO_PWMO_QTY] = { 8, 9, 10, 11, 12, 13, 14, 15 };
const byte SX1509_PORT_M_DIR[MOTOR_QTY] = { 0, 1};                            //, 2, 3 }; //Motor Richtung - Output am SX1509

const byte SX1509_I2C_ADDRESS = 0x3E;    //wenn keine ADD-Jumper auf SX1509 aktiviert sind
const byte SX1509_I2C_PIN_SDA = 21;
const byte SX1509_I2C_PIN_SCL = 22;
//const byte SX1509_PIN_RESET = 16;
//const byte SX1509_PIN_EXTI = 17;

extern SX1509 sx1509Object;        //i2c SDA = PIN 21, SCL = PIN 22

/* ====================================================================================================================== */

class CheckMaxiExtension {

public:
	/** default constructor, stores public SX1509_I2C_ADDRESS in mAddress
	 *
	 *  @param ---
	 *  @return ---
	*/
	CheckMaxiExtension();
	
	/** constructor, stores given address in mAddress
	 *
	 *  @param address-byte, I2C-Address of SX1509-extension board
	 *  @return ---
	*/
	CheckMaxiExtension(byte);
	
	/** Checks if extension board is connected, does init, manages ports depending on board-type
	 *
	 *  @param ---
	 *  @return Flag (bool) showing if extension board is connected (true) or not (false)
	*/
	bool CheckMaxi();
private:
	String mBoard;
	byte mAddress;
};

/* ====================================================================================================================== */

class Motor {

public:
	/** default constructor, resets all values to 0
	 *
	 *  @param ---
	 *  @return ---
	*/
	Motor();	//Standardkonstruktor, setzt alles auf 0;
	
	/** constructor, sets pins (PWM, direction) according to board-type. 
	 *
	 *  @param motorNr 0..3
	 *  @return ---
	*/
	Motor(unsigned int motorNr);	//Konstruktor, Motor-Nr (0..3), weist zu: Pin-Nr für PWM, Pin-Nr für Richtung
	//Destructor is missing -> detach PWM-Generator from pin!
	
	/** sets new motor-values (mode, speed)
	 *
	 *  @param direction mode (char) eg. 2-brake right-1 left-0
	 *  @param speed (unsigned int) qualitative value -8..0..8 manipulates internal pwm-signal
	 *  @return ---
	*/
	void setValues(unsigned char, unsigned int);	//neue Motorwerte setzen (Richtung, Drehzahl)
	
	/** sets pins and pwm value with attributes
	 *
	 *  @param ---
	 *  @return ---
	*/
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
	//evtl. eine Methode Stop einbauen
	
	/** attaches pins to pwm-generators according to maxi- or mini-board
	 *
	 *  @param pMaxi (bool) manipulates attachment of direction pins
	 *  @return ---
	*/
	void setMaxi(bool pMaxi);
private:
	unsigned int mMotorNr;	//Motornummer 0..3, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mPortNrDir;	//PortNr für Richtung, wird bei Erstellung des Objekts zugewiesen
	unsigned char mDirectionMode;	//Drehrichtung: rechts = 1, links = 0, bremsen=2
	unsigned int mDrehzahl;	//aktuelle Geschwindigkeit (von 0 bis 8)
	//unsigned int mLedcChannel;	//PWM-Kanal (0..15(max))

};

/* ====================================================================================================================== */

class CServoMotor {

public:
	CServoMotor();	//default constructor, puts servomotor into neutral position
	CServoMotor(unsigned int motorNr, unsigned int dutyCycle = 50);	//contsructor, motor-Nr(0..3) - gives pin-nr for PWM, allows init of position - default is neutral, no pin override here
	//Destructor is missing -> detach PWM-Generator from pin!

	void setValues(unsigned int dutyCycle);	//new values to be set (servo-position between 0%..100%)
	void reRun();	//pins will be set with stored values

	void setMaxi(bool pMaxi);
private:
	unsigned int mMotorNr;	//motor-nr 0..3, will be set in constructor
	unsigned int mPortNrPWM;	//pin-nr for PWM, will be set in constructor
	unsigned int mMinDuty, mMaxDuty;	//minimum and maximum duty-cycle of servo (normally 4%..11%)
	unsigned int mRelDuty;	//current relative duty-cycle of servo (0%..100% between duty-boarders)
	unsigned int mLedcChannel;	//Servos get channels 4,5,6 (only for Mini-board)
};

/* ====================================================================================================================== */

class Led {

public:
	Led();
	Led(unsigned int ledNr);	//Konstruktor, Led-Nr (0..LAMP_QTY-1)
	void setValues(int brightness, int ledColor);	// set LED values: brightness(0-255), color as value (0:off, 1: red, 2:green; 3:blue ...)
	void reRun();	//bei Aufruf werden erneut die Pins gesetzt
	void setMaxi(bool pMaxi);
  void setLedRGBColorByIdentifier(int colorNumber, int brigthness);
  String getRGBColorName();
  
private:
	unsigned int mLedNr;	//LednNr 0..7, wird bei Erstellung des Objekts angelegt	
  int mBrightness; // brightness between 0 ... 255
  int mColor; // siehe (0: red, 1:green; 2:blue ...)
  int mColorRGB[3];
  String mColorName;
};

void led_init();

void led_clear();

void led_reset(String &option, array<Led, LED_QTY> &ledArray);

void led_update(array<Led, LED_QTY> &ledArray);

void led_update(Led ledArray[]);

/* ====================================================================================================================== */

// ===== ENCODERSTEUERUNG
// Interrupt-Service-Routinen ==> Hochzählen der Encoder-Zähler
void ISREnc0();
void ISREnc1();

class CEncoderStrecke {
public:
  // Konstruktor
  CEncoderStrecke();

  // Beide Encoder aktivieren ==> Impulse werden gezählt
  // Zusätzlich alle erforderlichen Variablen zurücksetzen
  void enableInterrupts();

  // Beide Encoder deaktivieren ==> es werden keine weiteren Impulse gezählt
  void disableInterrupts();

  // den Motortreiber aktivieren und PWM einrichten
  void enableMotors();

  // Motoren stoppen und Motortreiber deaktivieren
  void disableMotors();

  // Reglerfunktion: Ausgehend von der aktuellen Rotationsgeschwindigkeit wird die nächste
  // Stellgröße für den jeweiligen Motor berechnet und anschließend auf den Motor gegeben.
  // Die Sollposition (in cm) wird übergeben. Ist der Return-Wert true, so wurde die Sollposition
  // erreicht. Bei false wurde die Position noch nicht erreicht.
  // richtung == 0: vorwärts, richtung == 1: rückwärts
  bool reglerStrecke(int posSoll, int richtung);

  // Reglerfunktion für die Winkelfahrt: Der Roboter dreht sich um einen bestimmten Winkel nach
  // rechts (1) oder links (0)
  bool reglerWinkel(int winkelSoll, int richtung);

  // setMotorSpeed: setzt die Rotationsgeschwindigkeit für beide Motoren (zwischen -255 und +255)
  // ein negatives Vorzeichen bedeutet eine Umkehr der Drehrichtung
  void setMotorSpeed(float speedM0, float speedM1);

private:
    float integrierer;          // Wert des Integrierers (I-Anteil)
    float RegAbweichung[2];     // Reglerabweichung des aktuellen (0) und letzten (1) Abtastschritts
    float RegStellgr[2];        // Stellgröße von Motor0 und Motor1
    float diff[2];              // Differenzialanteil (aktueller (0) und letzter (1) Abtastschritt)

    // Reglerparameter für den PID-Regler; der D-Anteil wird bisher nicht verwendet.
    float PWM0 = 114;           // PWM-Wert für die Basis-Ansteuerung der Motoren
    float P_ENC = 15;           // Proportionalanteil
    float TN_ENC = 0.1;         // Integralanteil
    float TV_ENC = 0.06;        // Differenzialanteil
    float TF_ENC = 0.5*TV_ENC;  // Filterzeitkonstante

    int64_t time_ReglerInt[2];  // Abtastzeit des Integrierers: aktueller und letzter Zeitpunkt der Abtastung
};

/* ====================================================================================================================== */

class CAngle {
  public:
    CAngle();      //aggregation: class CAngle can control Motor_0 and Motor_1 
                                                                               
    void setAngle();
    void driveAngleRel(float pAngleRel, unsigned char pTurnRight);                                         //transfer parameter is the relative angle of the actual position, if pTurnRight is one --> CW, else if pTurnRight is cero --> CCW
  private:
    float mAngleAbs;
    bool finish_flag;
    Motor mMotor_0;
    Motor mMotor_1;
};

/* ====================================================================================================================== */

class DigitalAnalogIn {

public:
	DigitalAnalogIn();
	DigitalAnalogIn(unsigned int);
	unsigned int getValueAnalog();
	unsigned int getValueDigital();
	void setValueDigital(bool);	//Digitalen Ausgang setzen (HIGH/LOW)
	void setMaxi(bool pMaxi);
private:
	unsigned int mInputNummer;
	unsigned int mInputPortNr;
	unsigned char currentPinMode;
};

/* ====================================================================================================================== */

#endif
