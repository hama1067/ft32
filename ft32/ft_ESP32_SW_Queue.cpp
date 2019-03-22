#include "ft_ESP32_SW_Queue.h"
#include "CSpiffsStorage.h"

SW_queue::SW_queue()
{
  //anlegen aller IO-Objekte (Schnittstelle zur Hardware)
  Serial.println("[Q] IO-Objekte anlegen...");

  for (unsigned int i = 0; i < mMotorArray.size(); i++)	//4 Motoren anlegen (Standardkonstruktor)
  {
    mMotorArray[i] = Motor(i);	//Motor mit "richtigem" Konstruktor erzeugen, in Array speichern
  }
  for (unsigned int i = 0; i < mLampeArray.size(); i++)
  {
    mLampeArray[i] = Lampe(i);
  }
  for (unsigned int i = 0; i < mServoArray.size(); i++)
  {
    mServoArray[i] = CServoMotor(i, 100);
  }
  for (unsigned int i = 0; i < mDAIn.size(); i++)
  {
    mDAIn[i] = DigitalAnalogIn(i);
  }
  Serial.println("IO-Objekte angelegt\n");

  startPtr = new commonElement;	//Anlegen des Start-Elements
  endPtr = new commonElement;	//Anlegen des End-Elements
}


void SW_queue::setBoardType(bool pMaxi)
{
  Serial.println("[io] adjusting IO-Objects ...");
  for (unsigned int i = 0; i < mMotorArray.size(); i++) //4 Motoren anlegen (Standardkonstruktor)
  {
    mMotorArray[i].setMaxi(pMaxi);
  }
  for (unsigned int i = 0; i < mLampeArray.size(); i++)
  {
    mLampeArray[i].setMaxi(pMaxi);
  }
  for (unsigned int i = 0; i < mServoArray.size(); i++)
  {
    mServoArray[i].setMaxi(pMaxi);
  }
  for (unsigned int i = 0; i < mDAIn.size(); i++)
  {
    mDAIn[i].setMaxi(pMaxi);
  }
  Serial.println("[io] IO-Objects adjusted.\n");
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
  uebergabestr = mSHM->webData.data;
  //uebergabestr = mSHM->webData.data;                    // nicht mehr benötigt (tobias) //oder vieleicht doch? (JM)

  //Übernehmen des Übergabestrings aus dem SPIFFS-Speicher
  //String uebergabestr = "";
  /*
  int counter = 0;
  bool loop_var = true;
  while (loop_var)
  {
    if (mSHM->mSpeicher.getStr(&uebergabestr))
      loop_var = false;
    else if (counter > 20)
      ESP.restart();
    else
      counter++;
  }
  */

  //Erstellen der Queue:
  if(uebergabestr != "")	//create queue if string is not empty
  {
    queueCreator();// startPtr, endPtr, uebergabestr);
  }
  else	//set error if string is empty
  {
    qCreateError = true;
	qCreateErrorID = 9;
  }
  //Testausgabe, ob Queue korrekt erstellt wurde (nur für Debugzwecke)
  Serial.println("[Q] Testausgabe der Queue:");
  Serial.println("[Q] String: " + uebergabestr);

  commonElement* workPtr = startPtr->nextElement;

  Serial.print("[Q] Startelement: ");
  Serial.println(startPtr->ID); //Ausgabe StartID: 0

  while (workPtr != endPtr)
  {
    Serial.print(char(workPtr->ID));  //Ausgabe ID (z.B. 'M', 'I', etc)
    Serial.print(" ");  //Leerzeichen, bessere Lesbarkeit
    Serial.print(workPtr->rootID);  //Ausgabe rootID (Zahl gleich bei If/EndIf bzw. While/EndWhile, sonst 0)
    Serial.print(" ");
    Serial.print(workPtr->time_s);  //Ausgabe Wartezeit Sekunden, sonst 0
    Serial.print(" ");
    Serial.println(workPtr->val_pwm_timems_loop); //Ausgabe Wert
    workPtr = workPtr->nextElement; //Zeiger auf nächstes Element der Queue legen
  }
  Serial.print("[Q] Endelement: ");
  Serial.println(endPtr->ID); //Ausgabe EndID: 0
  Serial.println("[Q] Testausgabe der Queue Ende.\n");
  
  //Abarbeiten des Programms
  if (!qCreateError)	//Wenn kein Fehler beim Erstellen der Queue
  {
	  queueWorker(mSHM);  //Uebergabe von: StartPtr, EndPtr, MotorArray, LampenArray, InputArray, Gemeinsamer Speicher
  }
  
  //print any errors that occurred on creating the queue or on runtime
  printErrors();

  //Queue löschen
  queueDelete();// startPtr, endPtr);

  //reset Errors
  qCreateError = false;
  qCreateErrorID = 0;
  qWorkError = false;  
  qWorkErrorID = 0;

  mSHM->commonStart=false;
  mSHM->commonPause=false;
  mSHM->running = false;  //Über das SHM der HMI Bescheid geben, dass die Queue nicht mehr laeuft  
}

void SW_queue::queueDelete()//commonElement*& startPtr, commonElement*& endPtr)	//method to delete the queue
{
	Serial.println("[Q] Queue loeschen...");
	commonElement* workPtr = startPtr->nextElement;
	while (workPtr != endPtr)
	{
		workPtr = workPtr->nextElement;
		delete workPtr->prevElement;
	}
	Serial.println("[Q] Queue geloescht...\n");
}

int SW_queue::stoi_ft(String& uestring, unsigned int& strcounter)
{
	/* 
	String blockstr = "";	//Hilfsstring, speichert die einzulesende Zahl
	blockstr += uestring.charAt(strcounter);	//Erste Ziffer einlesen
	while (isDigit(uestring.charAt(strcounter + 1)))	//naechstes Zeichen ist eine Ziffer?
	{
		strcounter++;	//counter auf dieses Zeichen legen
		blockstr += uestring.charAt(strcounter);	//Zeichen an den Hilfsstring anhaengen
	}
	return blockstr.toInt();	//gibt es unter Arduino
	*/
	 
	String blockstr = "";	//stores number-to-read as string
	while (strCounter < uestring.length())	//repeat while strCounter points on valid char of uestring
	{
		if(isDigit(uestring.charAt(strCounter)))	//check if character is a digit
		{
			blockstr += uestring.charAt(strCounter);	//add digit to string
			strCounter++;	//increase counter onto next character, may exceed string-length
		}
		else	//if character is a letter
		{
			strCounter = uestring.length();	//counter will exceed string, loop will be exited
		}
	}
	if ("" == blockstr)	//if blockstr is empty fill it with a zero to prevent errors in return line
	{
		blockstr = "0";
	}
	return blockstr.toInt();
}

void SW_queue::checkChar(int& strCounter, char zeichen)
{
	if (!qCreateError)
	{
		if (uebergabestr.charAt(strCounter) != zeichen)	//auf Zeichen pruefen
		{
			qCreateError = true;
			switch (zeichen)
			{
			case '#':
				qCreateErrorID = 1; //Fehler, keine Raute an der gesuchten Stelle
				break;
			case ',':
				qCreateErrorID = 2; //Fehler, kein Komma an der gesuchten Stelle
				break;
			case '.':
				qCreateErrorID = 3; //Fehler, kein Punkt an der gesuchten Stelle
				break;
			case ';':
				qCreateErrorID = 4; //Fehler, kein Strichpunkt an der gesuchten Stelle
				break;
			default:
				break;
			}
		}
	}
}

void SW_queue::printErrors()
{
	Serial.print("[Q creator] qCreateErrorID = " + String(qCreateErrorID) + ": ");
	switch (qCreateErrorID)
	{
	case 0:	//no errors occurred
		Serial.println("Keine Fehler beim Erstellen der Queue");
		break;
	case 1:
		Serial.println("Keine Raute an der gesuchten Stelle");
		break;
	case 2:
		Serial.println("Kein Komma an der gesuchten Stelle");
		break;
	case 3:
		Serial.println("Kein Punkt an der gesuchten Stelle");
		break;
	case 4:
		Serial.println("Kein Strichpunkt an der gesuchten Stelle");
		break;
	case 5:
		Serial.println("Kein passendes If zum EndIf gefunden");
		break;
	case 6:
		Serial.println("Kein passendes If oder Else zum EndIf gefunden");
		break;
	case 7:
		Serial.println("Kein passendes While zum EndWhile gefunden");
		break;
	case 8:
		Serial.println("Falscher Identifier");
		break;
	case 9:
		Serial.println("No data recieved");
	default:
		Serial.println("Unbekannter Fehler, qCreateErrorID: " + qCreateErrorID);
		break;
	}

	Serial.print("[Q worker] qWorkErrorID = " + String(qWorkErrorID) + ": ");
	switch (qWorkErrorID)
	{
	case 0:	//no errors occurred
		Serial.println("Kein Fehler beim Abarbeiten der Queue");
		break;
	default:
		Serial.println("Unbekannter Fehler, qWorkErrorID: " + qWorkErrorID);
		break;
	}
}
