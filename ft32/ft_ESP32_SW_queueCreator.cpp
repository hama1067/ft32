//#include "ft_ESP32_queueCreator.h"
#include "ft_ESP32_SW_Queue.h"

void SW_queue::queueCreator()// commonElement*& startPtr, commonElement*& endPtr, String& uebergabestr)
{
	commonElement* createPtr = startPtr;
	commonElement* searchPtr;

	//bool qCreateError = false;	//Fehlerflag fuer Erstellung der Queue
	//int qCreateErrorID = 0;	//FehlerID des ersten Fehlers der gefunden wurde

	int ustrPos = 0;
  String tmpBrightness = "";
  String tmpPortNumber = "";
  String tmpMode = "";
  String tmpID = "";

	checkChar(ustrPos, '#');	//Pruefen ob erstes Zeichen der uebergabe ein '#'

	while ((uebergabestr.charAt(ustrPos) == '#') && !qCreateError)	//solange das naechste Zeichen '#' ist und kein Fehler verursacht wurde
	{
		createPtr->nextElement = new commonElement;	//neues QE erstellen, Nachfolgezeiger legen
		createPtr->nextElement->prevElement = createPtr;	//Zeiger zum Vorgaenger legen
		createPtr = createPtr->nextElement;	//in neu angelegtes Element springen

    ustrPos++; //Zaehler auf erste Info nach '#' legen, sollte ID sein (M, D, oder auch zweistellig LR, ...)

    while(uebergabestr.charAt(ustrPos) != ',') { // zur Ueberpruefung zweistelliger ID Bloecke
      tmpID+=uebergabestr.charAt(ustrPos);
      ustrPos++;  //Zaehler auf XX legen
    }
    createPtr->ID = getQueueElementID(tmpID);  //ID wird in allen Faellen gespeichert
    tmpID = "";

		checkChar(ustrPos, ',');	//auf Komma pruefen
		ustrPos++;	//zaehler auf erste Info nach ID legen
		bool foundRootID = false;	//zur Suche bei EndIf und EndWhile, wird true wenn zugehoeriges While oder If gefunden wurde

		switch (createPtr->ID)	//Auswahl entsprechend der Element-ID ('M', 'A', 'D', etc.)
		{
		case MOTOR:	// Motor
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer eintragen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Richtung legen
			createPtr->compare_direction = uebergabestr.charAt(ustrPos) - '0';	//Richtung
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Drehzahl legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//Drehzahl (0..50)
     
			break;
    case ENCODER:  // Encoder
      /* TODO: create encoder data from queue string */

      /* not used? */
      createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';  //Port-Nummer eintragen
      
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen

      /* used */
      ustrPos++;  //Zaehler auf Richtung legen
      createPtr->compare_direction = uebergabestr.charAt(ustrPos) - '0';  //Richtung
      
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen

      /* not used? */
      ustrPos++;  //Zaehler auf Drehzahl legen
      createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);  //Drehzahl (0..8, vgl. RoboPro)
      
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen

      /* used */
      ustrPos++; ////Zaehler auf Distanz legen
      createPtr->val_distance_m0 = stoi_ft(uebergabestr, ustrPos);

      /* ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen
      ustrPos++; ////Zaehler auf Port-Nr legen
      createPtr->portNr1 = uebergabestr.charAt(ustrPos) - '0';  //Port-Nummer eintragen
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen
      ustrPos++;  //Zaehler auf Richtung legen
      createPtr->compare_direction1 = uebergabestr.charAt(ustrPos) - '0';  //Richtung
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen
      ustrPos++;  //Zaehler auf Drehzahl legen
      createPtr->val_pwm_timems_loop1 = stoi_ft(uebergabestr, ustrPos);  //Drehzahl (0..8, vgl. RoboPro)
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen
      ustrPos++; ////Zaehler auf Distanz legen
      createPtr->val_distance_m1 = stoi_ft(uebergabestr, ustrPos);*/

      break;
		case LED:	// LED strip pixel
      while(uebergabestr.charAt(ustrPos) != ',') {
        tmpPortNumber+=uebergabestr.charAt(ustrPos) - '0';
        ustrPos++;  //Zaehler auf XX legen
      }
			createPtr->multi_digit_identifier = tmpPortNumber;  //Port-Nummer eintragen
      tmpPortNumber = "";
   
      checkChar(ustrPos, ',');  //auf Komma pruefen
      ustrPos++;  //Zaehler auf XX legen

      while(uebergabestr.charAt(ustrPos) != ',') {
        tmpBrightness+=uebergabestr.charAt(ustrPos) - '0';
        ustrPos++;  //Zaehler auf XX legen
      }
      createPtr->multi_digit_value = tmpBrightness;  // XX
      tmpBrightness = "";
      checkChar(ustrPos, ',');  //auf Komma pruefen

      ustrPos++;  //Zaehler auf  legen
      createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);  //XX
      
			break;
    case LED_RESET:  // reset LED strip pixel
      while(uebergabestr.charAt(ustrPos) != ';') {
        if(uebergabestr.charAt(ustrPos) == 'A') {
          tmpMode+=uebergabestr.charAt(ustrPos);
        } else {
          tmpMode+=uebergabestr.charAt(ustrPos) - '0';
        }       
        ustrPos++;  //Zaehler auf XX legen
      }
      createPtr->multi_digit_identifier = tmpMode;  // Port-Nummer oder Modus eintragen (0-10 oder A fuer alle LEDs) eintragen
      tmpMode = "";

      ustrPos--;
      
      break;
		case SERVO:	// Servo Motor
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer eintragen
			ustrPos++;	//Zaehler auf Komma legen
			checkChar(ustrPos, ',');	//auf Komma pruefen
			ustrPos++;	//Zaehler auf Position legen
			createPtr->val_pwm_timems_loop = stoi_ft(uebergabestr, ustrPos);	//relative Servoposition (%) auslesen, Zaehler auf letzte Ziffer legen
      
			break;
    case ROTATE:  // rotate around itself by an angle
      createPtr->compare_direction = uebergabestr.charAt(ustrPos) - '0';  //Richtung
      ustrPos++;  //Zaehler auf Komma legen
      checkChar(ustrPos, ',');  //auf Komma pruefen
      ustrPos++;  //Zaehler auf Winkel legen
      createPtr->val_angle = stoi_ft(uebergabestr, ustrPos);
      
      break;
    case CIRCLE:  // rotate around one wheel multiple times

      break;
		case INPUT_DIGITAL:	//Input Digital
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
		case INPUT_ANALOG:	//Input Analog
			createPtr->portNr = uebergabestr.charAt(ustrPos) - '0';	//Port-Nummer
     
			break;
		case SLEEP:	//Warten
			createPtr->time_s = stoi_ft(uebergabestr, ustrPos);	//read seconds, counter on last digit
			ustrPos++;	//counter on dot
			checkChar(ustrPos, '.');	//check for dot, throw error if not present
			ustrPos++;	//counter on milliseconds
			{
				int _tempUstrPos = ustrPos;	//remember position for check of # of digits
				int _tempTimems = 0;	//temp save for milliseconds
				_tempTimems = stoi_ft(uebergabestr, ustrPos);	//read milliseconds, counter on last digit
				if (1 >= (ustrPos - _tempTimems))	//if # of digits is only 1, assume it has to be 1/10 second (not 1/100)
				{
					_tempTimems *= 10;	//increase from 1/100 sec to 1/10 sec
				}
				while (_tempTimems >= 100)	//in case milliseconds have more than 3 digits
				{
					_tempTimems /= 10;	//shrink to exactly 2 digits (my format precious, gollum  :-) )
				}
				createPtr->val_pwm_timems_loop = _tempTimems;	//store millisecs to queue
			}
     
			break;
		case IF:	//If
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
		case ELSE:	//Else
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
		case ENDIF:	//EndIf
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
		case WHILE:	//While
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
		case ENDWHILE:	//EndWhile
			createPtr->rootID = uebergabestr.charAt(ustrPos);	//While-ID
			searchPtr = createPtr->prevElement;	//Zeiger sucht ab hier aufwaerts nach dem zugehoerigen While
			foundRootID = false;
			while (!foundRootID && (searchPtr != startPtr))	//solange kein passendes While gefunden und der Anfang nicht erreicht wurde
			{
				if ((searchPtr->ID == WHILE) && (searchPtr->rootID == createPtr->rootID))	//While gefunden und passende rootID
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

/*//void queue_delete(commonElement*& startPtr, commonElement*& endPtr)	//method to delete the queue
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
//}*/
