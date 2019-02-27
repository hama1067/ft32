//#include "ft_ESP32_queueCreator.h"
#include "ft_ESP32_SW_Queue.h"

void SW_queue::queueCreator()// commonElement*& startPtr, commonElement*& endPtr, String& uebergabestr)
{
	commonElement* createPtr = startPtr;
	commonElement* searchPtr;

	bool qCreateError = false;	//Fehlerflag fuer Erstellung der Queue
	int qCreateErrorID = 0;	//FehlerID des ersten Fehlers der gefunden wurde

	int ustrPos = 0;

	checkChar(ustrPos, '#');	//Pruefen ob erstes Zeichen der uebergabe ein '#'

	while ((uebergabestr.charAt(ustrPos) == '#') && !qCreateError)	//solange das naechste Zeichen '#' ist und kein Fehler verursacht wurde
	{
		createPtr->nextElement = new commonElement;	//neues QE erstellen, Nachfolgezeiger legen
		createPtr->nextElement->prevElement = createPtr;	//Zeiger zum Vorgaenger legen
		createPtr = createPtr->nextElement;	//in neu angelegtes Element springen

		ustrPos++;	//Zaehler auf erste Info nach '#' legen, sollte ID sein
		createPtr->ID = uebergabestr.charAt(ustrPos);	//ID wird in allen Faellen gespeichert
		ustrPos++;	//Zaehler auf Komma nach ID legen
		checkChar(ustrPos, ',');	//auf Komma pruefen
		ustrPos++;	//zaehler auf erste Info nach ID legen

		bool foundRootID = false;	//zur Suche bei EndIf und EndWhile, wird true wenn zugehoeriges While oder If gefunden wurde

		switch (createPtr->ID)	//Auswahl entsprechend der Element-ID ('M', 'A', 'D', etc.)
		{
		case 'M':	//Motor
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer eintragen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Richtung legen
			createPtr->compare_direction = uebergabestr.charAt(ustrPos) - '0';	//Richtung
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Drehzahl legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Drehzahl (0..8, vgl. RoboPro)
			break;
		case 'L':	//Input Lampe
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Helligkeit legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Helligkeit (0..8)
			break;
		case 'N':	//Servo Motor
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer eintragen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Position legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//relative Servoposition (%) auslesen, Zaehler auf letzte Ziffer legen
			break;
		case 'D':	//Input Digital
			if ('O' == uebergabestr.charAt(ustrPos))
			{
				createPtr->type = uebergabestr.charAt(ustrPos);	//Art eintragen ('O' wenn Output, nichts wenn Input)
				ustrPos++;	//Zaehler auf Komma legen
				checkChar(ustrPos, ',');	//auf Komma pruefen
				ustrPos++;	//Zaehler auf Port-Nummer legen
				createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
				ustrPos++;	//Zaehler auf Komma legen
				checkChar(ustrPos, ',');	//auf Komma pruefen
				ustrPos++;	//Zaehler auf Level legen
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
			createPtr->time_s = stoi_ft(uebergabestr, ustrPos);	//Sekunden auslesen, Zaehler auf letzte Ziffer legen
			ustrPos++;	//Zaehler auf Punkt legen
			checkChar(ustrPos, '.');	//auf Punkt pruefen
			ustrPos++;	//Zaehler auf Millisekunden legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Sekunden auslesen, Zaehler auf letzte Ziffer legen
			break;
		case 'I':	//If
			createPtr->type = uebergabestr.charAt(ustrPos);	//Art (D/A-Pruefung) eingtragen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Port-Nummer legen
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Vergleichsoperator legen
			createPtr->compare_direction = uebergabestr.charAt(ustrPos);	//Vergleichsoperator (< > =)
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Vergleichswert legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Vergleichswert auslesen (Analogwert oder 0/1 beim DIn), Zaehler auf letzte Ziffer legen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf If-ID legen
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//If-ID
			break;
		case 'E':	//Else
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//If-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufwaerts nach dem zugehoerigen If
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
					searchPtr = searchPtr->prevElement;	//weitersuchen beim Vorgaenger
				}
			}
			if (!foundRootID)	//Kein passendes If gefunden
			{
				qCreateError = true;
				qCreateErrorID = 5; //Fehler, kein passendes If gefunden
			}
			break;
		case 'J':	//EndIf
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//If-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufwaerts nach dem zugehoerigen Else oder If
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
					searchPtr = searchPtr->prevElement;	//weitersuchen beim Vorgaenger
				}
			}
			if (!foundRootID)	//Kein passendes If oder Else gefunden
			{
				qCreateError = true;
				qCreateErrorID = 6; //Fehler, kein passendes If oder Else gefunden
			}
			break;
		case 'W':	//While
			createPtr->type = uebergabestr.charAt(ustrPos);	//Art (D/A-Pruefung oder Z-Zaehlschleife) eintragen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Zaehlwert (Zaehlschleife) oder Port-Nummer (normale Schleife) legen
			if (createPtr->type != 'Z')	//Schleife ist keine Zaehlschleife
			{
				createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
				ustrPos++;	//Zaehler auf Komma legen
				checkChar(ustrPos, ',');	//auf Komma pruefen
				ustrPos++;	//Zaehler auf Vergleichsoperator legen
				createPtr->compare_direction = uebergabestr.charAt(ustrPos);	//Vergleichsoperator (< > =)
				ustrPos++;	//Zaehler auf Komma legen
				checkChar(ustrPos, ',');	//auf Komma pruefen
				ustrPos++;	//Zaehler auf Vergleichswert legen
			}
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Vergleichswert auslesen (Analogwert oder 0/1 beim DIn), Zaehler auf letzte Ziffer legen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf While-ID legen
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//While-ID
			break;
		case 'X':	//EndWhile
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//While-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufwaerts nach dem zugehoerigen While
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
					searchPtr = searchPtr->prevElement;	//weitersuchen beim Vorgaenger
				}
			}
			if (!foundRootID)	//kein passendes While gefunden
			{
				qCreateError = true;
				qCreateErrorID = 7; //Fehler, kein passendes While gefunden
			}
			break;
		default:
			qCreateError = true;
			qCreateErrorID = 8;	//Fehler, falscher Identifier
			break;
		}

		if (!qCreateError)
		{
			ustrPos++;	//Zaehler zeigt auf Strichpunkt am Ende eines Blocks
			checkChar(ustrPos, ';'); //auf Strichpunkt pruefen
			ustrPos++;	//Zaehler zeigt auf '#' des naechsten Blocks oder auf ';' am Ende der uebertragung
		}
	}

	//letztes abgefragte Zeichen ist kein '#', also vermutlich ';' (wird hier nicht extra geprueft, koennte man aber machen)
	//Zeiger mit End-QE abpassen
	createPtr->nextElement = endPtr;
	endPtr->prevElement = createPtr;

	//return qCreateErrorID;
}

//void queue_delete(commonElement*& startPtr, commonElement*& endPtr)	//method to delete the queue
//{
//	Serial.println("[Q] Queue loeschen...");
//	commonElement* workPtr = startPtr->nextElement;
//	while (workPtr != endPtr)
//	{
//		workPtr = workPtr->nextElement;
//		delete workPtr->prevElement;
//	}
//	Serial.println("[Q] Queue geloescht...\n");
//}

//int stoi_ft(String& uestring, int& strcounter)
//{
//	String blockstr = "";	//Hilfsstring, speichert die einzulesende Zahl
//	blockstr += uestring.charAt(strcounter);	//Erste Ziffer einlesen
//	while (isDigit(uestring.charAt(strcounter+1)))	//naechstes Zeichen ist eine Ziffer?
//	{
//		strcounter++;	//counter auf dieses Zeichen legen
//		blockstr += uestring.charAt(strcounter);	//Zeichen an den Hilfsstring anhaengen
//	}
//	return blockstr.toInt();	//gibt es unter Arduino
//}

//Nur fuer Debugging beim Erstellen der Queue:
//void checkChar(bool& errorFlag, int& errorID, String& uestring, int& strCounter, char zeichen)
//{
//	if (!errorFlag)
//	{
//		if (uestring.charAt(strCounter) != zeichen)	//auf Zeichen pruefen
//		{
//			errorFlag = true;
//			switch (zeichen)
//			{
//			case '#':
//				errorID = 1; //Fehler, keine Raute an der gesuchten Stelle
//				Serial.println("[Q creator] Fehler, keine Raute an der gesuchten Stelle");
//				break;
//			case ',':
//				errorID = 2; //Fehler, kein Komma an der gesuchten Stelle
//				Serial.println("[Q creator] Fehler, kein Komma an der gesuchten Stelle");
//				break;
//			case '.':
//				errorID = 3; //Fehler, kein Punkt an der gesuchten Stelle
//				Serial.println("[Q creator] Fehler, kein Punkt an der gesuchten Stelle");
//				break;
//			case ';':
//				errorID = 4; //Fehler, kein Strichpunkt an der gesuchten Stelle
//				Serial.println("[Q creator] Fehler, kein Strichpunkt an der gesuchten Stelle");
//				break;
//			default:
//				break;
//			}	
//		}
//	}
//}