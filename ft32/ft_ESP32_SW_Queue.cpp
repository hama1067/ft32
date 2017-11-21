#include "ft_ESP32_SW_Queue.h"

SW_queue::SW_queue()
{
	//anlegen aller IO-Objekte (Schnittstelle zur Hardware)
	Serial.println("IO-Objekte anlegen...");
	
	for (unsigned int i = 0; i < mMotorArray.size(); i++)	//4 Motoren anlegen (Standardkonstruktor)
	{
		mMotorArray[i] = Motor(i);	//Motor mit "richtigem" Konstruktor erzeugen, in Array speichern
	}
	for (unsigned int i = 0; i < mLampeArray.size(); i++)
	{
		mLampeArray[i] = Lampe(i);
	}
	for (unsigned int i = 0; i < mDAIn.size(); i++)
	{
		mDAIn[i] = DigitalAnalogIn(i);
	}
	Serial.println("IO-Objekte angelegt\n");
	
	startPtr = new commonElement;	//Anlegen des Start-Elements
	endPtr = new commonElement;	//Anlegen des End-Elements
}

SW_queue::~SW_queue()
{
	delete startPtr;
	delete endPtr;
}

void SW_queue::SW_work(SHM* mSHM)
{
	mSHM->running = true;  //über SHM der HMI Bescheid geben, dass das Programm läuft
  Serial.print("QUEUE common start ");
	Serial.println(mSHM->commonStart);
	//Übernehmen des Übergabestrings aus dem SHM als Referenz
	String& uebergabestr = mSHM->webData.data;
	
	//Erstellen der Queue:
	qCreateError = queue_creator(startPtr, endPtr, uebergabestr);
	
	//Testausgabe, ob Queue korrekt erstellt wurde (nur für Debugzwecke)
	Serial.println("Testausgabe der Queue...");
	commonElement* workPtr = startPtr->nextElement;

	Serial.println(startPtr->ID); //Ausgabe StartID: 0
	Serial.println(endPtr->ID); //Ausgabe EndID: 0

	while (workPtr != endPtr)
	{
		Serial.print(workPtr->ID);  //Ausgabe ID (z.B. 'M', 'I', etc)
		Serial.print(" ");  //Leerzeichen, bessere Lesbarkeit
		Serial.print(workPtr->rootID);  //Ausgabe rootID (Zahl gleich bei If/EndIf bzw. While/EndWhile, sonst 0)
		Serial.print(" ");
		Serial.print(workPtr->time_s);  //Ausgabe Wartezeit Sekunden, sonst 0
		Serial.print(" ");
		Serial.println(workPtr->val_pwm_timems_loop); //Ausgabe Wert
		workPtr = workPtr->nextElement; //Zeiger auf nächstes Element der Queue legen
	}
	Serial.println("Testausgabe der Queue Ende.\n");

	//Abarbeiten des Programms
	qWorkError = queueWorker(startPtr, endPtr, mMotorArray, mLampeArray, mDAIn, mSHM);  //Übergabe von: StartPtr, EndPtr, MotorArray, LampenArray, InputArray, Gemeinsamer Speicher
  
	//Queue löschen
	queue_delete(startPtr, endPtr);
	
	mSHM->running = false;  //Über das SHM der HMI Bescheid geben, dass die Queue nicht mehr läuft
}
