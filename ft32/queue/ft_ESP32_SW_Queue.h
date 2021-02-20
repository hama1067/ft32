#ifndef FT_ESP32_SW_QUEUE_H
#define FT_ESP32_SW_QUEUE_H

#include <string.h> //für String-Operationen
#include <Arduino.h>  //für String-Operationen
#include <array>  //Library erlaubt Zugriffssicherung bei Arrays

#include "ft_ESP32_SHM.h" //gemeinsamer Speicher (Shared Memory)
#include "ft_ESP32_IOobjects.h" //Klassen der Ausgänge (Hardwaretreiber)

using namespace std;  //zum Handling der Arrays (für Zugriffssicherung)

/*
 * queue elment ID, defined by ft_ESP32_queueCreator.cpp and SHM content, 
 * will be used by queueCreator by calling getQueueElementID()
 */
enum queueElementID {
  MOTOR,          /*  M  */
  ENCODER,        /*  EN */
  LED,            /*  L  */
  LED_RESET,      /*  LR */
  SERVO,          /*  N  */
  ROTATE,         /*  R  */
  CIRCLE,         /*  C  */
  INPUT_DIGITAL,  /*  D  */
  INPUT_ANALOG,   /*  A  */
  SLEEP,          /*  S  */
  IF,             /*  I  */
  ELSE,           /*  E  */
  ENDIF,          /*  J  */
  WHILE,          /*  W  */
  ENDWHILE,       /*  X  */
  UNKNOWN_ID
};

struct commonElement {
	queueElementID ID = UNKNOWN_ID;	        //'M', 'A', 'D', 'LR', ...
	
	commonElement* nextElement;	            // naechstes regulaeres Element der Kette
	commonElement* prevElement;	            // vorhergehendes regulaeres Element der Kette
	commonElement* jumpElement = NULL;	    // zeigt bei: If -> Else+1/EndIf+1, Else -> EndIf+1, While -> EndWhile+1, EndWhile -> While
	
	unsigned char type = 0;	                // Art des Inputs "A"/"D" bei If und (normalem) While, "Z" bei Zaehlschleifen
	unsigned char portNr = 0;	              // 0..7 bei Input (auch in If/While), 0..3 bei Aktor
  String multi_digit_identifier = "";     // uebergabe parameter, welcher mehr als eine Stelle aufweist (z.B. fuer die LED-Funktionsauswahl)
	unsigned char compare_direction = 0;	  // Vergleichsoperator bei If/While, Richtung(0/1) Motor, Bremsen(2)
  String multi_digit_value = "";          // uebergabe parameter, welcher mehr als eine Stelle aufweist (z.B. fuer die LED Helligkeit: 255)
	unsigned int val_pwm_timems_loop = 0;	  // Vgl-Wert If/While, PWM Motor/Led, Wartezeit [ms], Wert Zaehlschleife
	unsigned int time_s = 0;	              // Zeitwert in Sekunden (nur von Wartezeit benutzt)
	
	unsigned int val_angle = 0;             // Zwischenspeicher fuer uebergebenen Winkelwert
  unsigned int val_distance_m0 = 0;       // Laufstrecke fuer Motor M0
  double val_circle = 0;                  // Zwischenspeicher fuer Anzahl der Umdrehungen um ein fetstehendes Rad
  unsigned int val_distance_m1 = 0;       // Laufstrecke fuer Motor M0 (fuer Block distance, da Parameter fuer 2 Motoren uebergeben werden)
  unsigned char portNr1 = 0;              // 0..7 bei Input (auch in If/While), 0..3 bei Aktor
  unsigned char compare_direction1 = 0;   // Vergleichsoperator bei If/While, Richtung(0/1) Motor, Bremsen(2)
  unsigned int val_pwm_timems_loop1 = 0;  // Vgl-Wert If/While, PWM Motor/Lampe, Wartezeit [ms], Wert Zaehlschleife
	
	unsigned short rootID = 0;	            // fortlaufende ID fuer If/While (um die EndIf und EndWhile korrekt zuordnen zu koennen)
};

class SW_queue {
  
public:
	/**
	 *
	 *  @param ---
	 *  @return ---
	*/
	SW_queue();
	
	/** destructor, deletes startPtr and endPtr
	 *
	 *  @param ---
	 *  @return ---
	*/
	~SW_queue();

	/**	main task method, waits on flag (commonStart) and runs qWorker
	 *	Note: method should normally never end
	 *	
	 *	@param pointer on shared_memory
	 *	@return ---
	*/
	void startQueueTask(SHM* ptrSHM);

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
	 *  Note: this method modifies strCounter. It points onto the last digit after the extraction
	 *  Note: the method can be used outside this class independantely as a function
	 *	
	 *  @param uestring - string the number will be extracted from
	 *  @param strCounter - position of the first digit
	 *  @return extracted number as integer
	*/
	int stoi_ft(String& uestring, int& strCounter);
	
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

  /*
   * 
   */
  queueElementID getQueueElementID(String strID);

private:
	array<Motor, MOTOR_QTY> mMotorArray;  //Anlegen aller Motoren in einen Array
	array<Led, LED_QTY> mLedArray; //Anlegen aller Ledn in einen Array
	array<CServoMotor, SERVO_QTY> mServoArray;	//Anlegen aller Servos in einen Array
	array<DigitalAnalogIn, DAIN_QTY> mDAIn; //Anlegen aller Eingänge in einen Array
 
  //Encoder mEncoder;
  //CAngle mAngle;
  //EncoderKreis mEncoderKreis;
  
  /* TODO: Neue Encoderfunktionen */
  CEncoderStrecke mEncoderStrecke;
  
	//SHM* mSHM;	//Pointer on Shared memory
	String uebergabestr;

	commonElement* startPtr;  //Pointer auf Startobjekt der Queue
	commonElement* endPtr;  //Pointer auf Endobjekt der Queue
	
	bool qCreateError;	//Fehlerflag für Erstellung der Queue
	int qCreateErrorID;
	bool qWorkError;	//Fehlerflag für fehlerhaft ablaufende Queue
	int qWorkErrorID;

	unsigned int cycleTime;	//Refresh time for queue-thread while running in [ms]
};



#endif
