#ifndef FT_ESP32_SW_QUEUE_H
#define FT_ESP32_SW_QUEUE_H

#include <string.h> //für String-Operationen
#include <Arduino.h>  //für String-Operationen
#include <array>  //Library erlaubt Zugriffssicherung bei Arrays

#include "ft_ESP32_SHM.h" //gemeinsamer Speicher (SHared Memory)
#include "ft_ESP32_IOobjects.h" //Klassen der Ausgänge (Hardwaretreiber)

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
	/**
	 *
	 *  @param
	 *  @return
	*/
	SW_queue();
	
	/** destructor, deletes startPtr and endPtr
	 *
	 *  @param ---
	 *  @return ---
	*/
	~SW_queue();
	
	/** main method creates queue from SHM-data, runs program, communicates with HMI
	 *
	 *  @param pointer on shared-memory for "uebergabestr"
	 *  @return ---
	*/
	void SW_work(SHM* mSHM);
	
	/** creates queue on heap, checks for syntax and logic errors
	 *
	 *  @param ---
	 *  @return ---
	*/
	void queueCreator();// commonElement*& startPtr, commonElement*& endPtr, String& uebergabestr);
	
	/** runs program according on queue. communicates with HMI (for pause and stop)
	 *
	 *  @param pointer on shared-memory for pause, stop etc.
	 *  @return ---
	*/
	void queueWorker(SHM* mSHM);
	
	/** deletes queue after program finished
	 *
	 *  @param ---
	 *  @return ---
	*/
	void queueDelete();// commonElement*& startPtr, commonElement*& endPtr);
	
	/**
	 *
	 *  @param
	 *  @return ---
	*/
	void setBoardType(bool pMaxi);

	/** extracts a number from a string starting at a given position 
	 *  necessary, as string.toInt() from Arduino only supports numbers at the beginning of a string.
	 *  Note: this method does not check for valid positions of strCounter.
	 *		In fact it's even required that there is a non-digit key after the number...
	 *  
	 *  @param uestring - string the number will be extracted from
	 *  @param strCounter - position of the first digit
	 *  @return extracted number as integer
	*/
	int stoi_ft(String& uestring, unsigned int& strCounter);
	
	/** checks for specific char in "uebergabestr" at given counter-position. 
	 *  Drops error if character is not on this position: qCreateError & qCreateErrorID
	 *
	 *  @param strCounter - position where the char should be
	 *  @param zeichen - char that should be checked for
	 *  @return ---
	*/
	void checkChar(int& strCounter, char zeichen);
	
	/** prints errors via serial that caused creator or worker to stop
	 *
	 *  @param ---
	 *  @return ---
	*/
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
