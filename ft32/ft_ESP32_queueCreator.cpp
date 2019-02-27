#include "ft_ESP32_queueCreator.h"

bool queue_creator(commonElement*& startPtr, commonElement*& endPtr, String& uebergabestr)
{
	commonElement* createPtr = startPtr;
	commonElement* searchPtr;

	bool qCreateError = false;	//Fehlerflag f�r Erstellung der Queue
	int qCreateErrorID = 0;	//FehlerID des ersten Fehlers der gefunden wurde

	int ustrPos = 0;

	checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, '#');	//Pr�fen ob erstes Zeichen der �bergabe ein '#'

	while ((uebergabestr.charAt(ustrPos) == '#') && !qCreateError)	//solange das n�chste Zeichen '#' ist und kein Fehler verursacht wurde
	{
		createPtr->nextElement = new commonElement;	//neues QE erstellen, Nachfolgezeiger legen
		createPtr->nextElement->prevElement = createPtr;	//Zeiger zum Vorg�nger legen
		createPtr = createPtr->nextElement;	//in neu angelegtes Element springen
		
		ustrPos++;	//Z�hler auf erste Info nach '#' legen, sollte ID sein
		createPtr->ID = uebergabestr.charAt(ustrPos);	//ID wird in allen F�llen gespeichert
		ustrPos++;	//Z�hler auf Komma nach ID legen
		checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
		ustrPos++;	//z�hler auf erste Info nach ID legen

		bool foundRootID = false;	//zur Suche bei EndIf und EndWhile, wird true wenn zugeh�riges While oder If gefunden wurde

		switch (createPtr->ID)	//Auswahl entsprechend der Element-ID ('M', 'A', 'D', etc.)
		{
		case 'M':	//Motor
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer eintragen
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Richtung legen
			createPtr->compare_direction = uebergabestr.charAt(ustrPos) - '0';	//Richtung
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Drehzahl legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Drehzahl (0..8, vgl. RoboPro)
			break;
		case 'L':	//Input Lampe
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Helligkeit legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Helligkeit (0..8)
			break;
		case 'D':	//Input Digital
			if('O' == uebergabestr.charAt(ustrPos))
			{
				createPtr->type = uebergabestr.charAt(ustrPos);	//Art eintragen ('O' wenn Output, nichts wenn Input)
				ustrPos++;	//Z�hler auf Komma legen
				checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
				ustrPos++;	//Z�hler auf Port-Nummer legen
				createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
				ustrPos++;	//Z�hler auf Komma legen
				checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
				ustrPos++;	//Z�hler auf Level legen
				createPtr->compare_direction = uebergabestr.charAt(ustrPos); //Level eintragen (0=low - Lampe aus, 1=high - Lampe ein)
			}
			else
			{
				createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
			}
			break;
		case 'A':	//Input Analog
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
			break;
		case 'S':	//Warten
			createPtr->time_s = stoi_ft(uebergabestr, ustrPos);	//Sekunden auslesen, Z�hler auf letzte Ziffer legen
			ustrPos++;	//Z�hler auf Punkt legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, '.');	//auf Punkt pr�fen
			ustrPos++;	//Z�hler auf Millisekunden legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Sekunden auslesen, Z�hler auf letzte Ziffer legen
			break;
		case 'I':	//If
			createPtr->type = uebergabestr.charAt(ustrPos);	//Art (D/A-Pr�fung) eingtragen
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Port-Nummer legen
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Vergleichsoperator legen
			createPtr->compare_direction = uebergabestr.charAt(ustrPos);	//Vergleichsoperator (< > =)
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Vergleichswert legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Vergleichswert auslesen (Analogwert oder 0/1 beim DIn), Z�hler auf letzte Ziffer legen
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf If-ID legen
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//If-ID
			break;
		case 'E':	//Else
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//If-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufw�rts nach dem zugeh�rigen If
			foundRootID = false;
			while (!foundRootID && (searchPtr != startPtr))	//solange kein passendes If gefunden und der Anfang nicht erreicht wurde
			{
				if ((searchPtr->ID == 'I') && (searchPtr->rootID == createPtr->rootID))	//If gefunden und passende rootID
				{
					searchPtr->jumpElement = createPtr;	//Sprungzeiger in If-Element legen
					foundRootID = true;	//passendes If wurde gefunden
				}
				else
				{
					searchPtr = searchPtr->prevElement;	//weitersuchen beim Vorg�nger
				}
			}
			if (!foundRootID)	//Kein passendes If gefunden
			{
				qCreateError = true;
				qCreateErrorID = 5; //Fehler, kein passendes If gefunden
				Serial.println("Fehler, kein passendes If gefunden");
			}
			break;
		case 'J':	//EndIf
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//If-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufw�rts nach dem zugeh�rigen Else oder If
			foundRootID = false;
			while (!foundRootID && (searchPtr != startPtr))	//solange kein passendes If gefunden und der Anfang nicht erreicht wurde
			{
				if (((searchPtr->ID == 'I') || (searchPtr->ID == 'E')) && (searchPtr->rootID == createPtr->rootID))	//If oder Else gefunden und passende rootID
				{
					searchPtr->jumpElement = createPtr;	//Sprungzeiger in If- oder Else-Element legen
					foundRootID = true;	//passendes If oder Else wurde gefunden
				}
				else
				{
					searchPtr = searchPtr->prevElement;	//weitersuchen beim Vorg�nger
				}
			}
			if (!foundRootID)	//Kein passendes If oder Else gefunden
			{
				qCreateError = true;
				qCreateErrorID = 5; //Fehler, kein passendes If oder Else gefunden
				Serial.println("Fehler, kein passendes If oder Else gefunden");
			}
			break;
		case 'W':	//While
			createPtr->type = uebergabestr.charAt(ustrPos);	//Art (D/A-Pr�fung oder Z-Z�hlschleife) eintragen
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf Z�hlwert (Z�hlschleife) oder Port-Nummer (normale Schleife) legen
			if (createPtr->type != 'Z')	//Schleife ist keine Z�hlschleife
			{
				createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
				ustrPos++;	//Z�hler auf Komma legen
				checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
				ustrPos++;	//Z�hler auf Vergleichsoperator legen
				createPtr->compare_direction = uebergabestr.charAt(ustrPos);	//Vergleichsoperator (< > =)
				ustrPos++;	//Z�hler auf Komma legen
				checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
				ustrPos++;	//Z�hler auf Vergleichswert legen
			}
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Vergleichswert auslesen (Analogwert oder 0/1 beim DIn), Z�hler auf letzte Ziffer legen
			ustrPos++;	//Z�hler auf Komma legen
			checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ',');	//auf Komma pr�fen
			ustrPos++;	//Z�hler auf While-ID legen
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//While-ID
			break;
		case 'X':	//EndWhile
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//While-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufw�rts nach dem zugeh�rigen While
			foundRootID = false;
			while (!foundRootID && (searchPtr != startPtr))	//solange kein passendes While gefunden und der Anfang nicht erreicht wurde
			{
				if ((searchPtr->ID == 'W') && (searchPtr->rootID == createPtr->rootID))	//While gefunden und passende rootID
				{
					searchPtr->jumpElement = createPtr;	//Sprungzeiger in While-Element legen
					createPtr->jumpElement = searchPtr;	//Sprungzeiger in EndWhile-Element legen
					foundRootID = true;	//passendes If wurde gefunden
				}
				else
				{
					searchPtr = searchPtr->prevElement;	//weitersuchen beim Vorg�nger
				}
			}
			if (!foundRootID)	//kein passendes While gefunden
			{
				qCreateError = true;
				qCreateErrorID = 5; //Fehler, kein passendes While gefunden
				Serial.println("Fehler, kein passendes While gefunden");
			}
			break;
		default:
			Serial.println("Falscher Identifier");
			break;
		}

		ustrPos++;	//Z�hler zeigt auf Strichpunkt am Ende eines Blocks
		checkChar(qCreateError, qCreateErrorID, uebergabestr, ustrPos, ';'); //auf Strichpunkt pr�fen
		ustrPos++;	//Z�hler zeigt auf '#' des n�chsten Blocks oder auf ';' am Ende der �bertragung
	}

	//letztes abgefragte Zeichen ist kein '#', also vermutlich ';' (wird hier nicht extra gepr�ft, k�nnte man aber machen)
	//Zeiger mit End-QE abpassen
	createPtr->nextElement = endPtr;
	endPtr->prevElement = createPtr;

	return qCreateError;
}

void queue_delete(commonElement*& startPtr, commonElement*& endPtr)
{
	Serial.println("Queue loeschen...");
	commonElement* workPtr = startPtr->nextElement;
	while (workPtr != endPtr)
	{
		workPtr = workPtr->nextElement;
		delete workPtr->prevElement;
	}
	Serial.println("Queue geloescht...\n");
}

int stoi_ft(String& uestring, int& strcounter)
{
	String blockstr = "";	//Hilfsstring, speichert die einzulesende Zahl
	blockstr += uestring.charAt(strcounter);	//Erste Ziffer einlesen
	while (isDigit(uestring.charAt(strcounter+1)))	//n�chstes Zeichen ist eine Ziffer?
	{
		strcounter++;	//counter auf dieses Zeichen legen
		blockstr += uestring.charAt(strcounter);	//Zeichen an den Hilfsstring anh�ngen
	}
	return blockstr.toInt();	//gibt es unter Arduino
}

//Nur f�r Debugging beim Erstellen der Queue:
void checkChar(bool& errorFlag, int& errorID, String& uestring, int& strCounter, char zeichen)
{
	if (!errorFlag)
	{
		if (uestring.charAt(strCounter) != zeichen)	//auf Zeichen pr�fen
		{
			errorFlag = true;
			switch (zeichen)
			{
			case '#':
				errorID = 1; //Fehler, keine Raute an der gesuchten Stelle
				Serial.println("Fehler, keine Raute an der gesuchten Stelle");
				break;
			case ',':
				errorID = 2; //Fehler, kein Komma an der gesuchten Stelle
				Serial.println("Fehler, kein Komma an der gesuchten Stelle");
				break;
			case '.':
				errorID = 3; //Fehler, kein Punkt an der gesuchten Stelle
				Serial.println("Fehler, kein Punkt an der gesuchten Stelle");
				break;
			case ';':
				errorID = 4; //Fehler, kein Strichpunkt an der gesuchten Stelle
				Serial.println("Fehler, kein Strichpunkt an der gesuchten Stelle");
				break;
			default:
				break;
			}	
		}
	}
}
