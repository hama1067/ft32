#include "ft_ESP32_SW_Queue.h"
#include "OledHandler.h"
#include <time.h>

/**/
extern Adafruit_NeoPixel * led_strip;

/**/
extern int encPosAbs0;
extern int encPosAbs1;

void SW_queue::queueWorker(SHM* mSHM)
{	
	Serial.println("[QW] Queue abarbeiten...");	//Debug-Ausgabe... alle Seriellen Ausgaben sind zur Pruefung des Programmablaufs in der Konsole
	
	commonElement* workPtr = startPtr->nextElement;

	unsigned long starttime, endtime, runtime;
	

	while ((workPtr != endPtr) && mSHM->commonStart) {	//wiederholen bis das letzte QE erreicht oder Stopp gedrueckt wurde
		starttime = millis();
		//starttime = clock() / (CLOCKS_PER_SEC / 1000);
		int waitTime = 0;	//Variable zur Wartezeit [ms]

		switch (workPtr->ID) {
		case MOTOR:
			mMotorArray.at(workPtr->portNr).setValues(workPtr->compare_direction, workPtr->val_pwm_timems_loop);	//Motordaten.set, Ausgangspins werden gesetzt
			mSHM->motorVal[workPtr->portNr] = (1 == workPtr->compare_direction ? workPtr->val_pwm_timems_loop : -workPtr->val_pwm_timems_loop);
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
     
			break;
    case ENCODER:  // Encoder
      {
        /* TODO: Encodersteuerung: Fahren einer vorgegebenen Strecke, Interrupts und Motoren starten */
        mEncoderStrecke.enableInterrupts();
        mEncoderStrecke.enableMotors();
        bool ende = false;
  
        // Schleife ausführen, bis Sollposition erreicht
        while(!ende) {
          // Reglerfunktion aufrufen, Sollposition und Drehrichtung übergeben
          ende = mEncoderStrecke.reglerStrecke(workPtr->val_distance_m0, workPtr->compare_direction);
  
          // Ausgabe auf seriellen Monitor
          Serial.print(encPosAbs0);
          Serial.print(" | ");
          Serial.println(encPosAbs1);
  
          // Ausgabe auf OLED-Display
          /*String str = "Pos 0: ";
          str.concat(encPosAbs0);
          str.concat("\nPos 1: ");
          str.concat(encPosAbs1);
          cOledHandler::getInstance()->printOledMessage(str);*/
          delay(10);
        }
  
        // Motoren und Encoder stoppen
        mEncoderStrecke.disableMotors();
        mEncoderStrecke.disableInterrupts();
  
        // Nächstes Element in der Queue starten
        workPtr = workPtr->nextElement; 
      }
      break;
    case ROTATE: //
      /**/

      break;
		case LED:
			mLedArray.at((workPtr->multi_digit_identifier).toInt()).setValues((workPtr->multi_digit_value).toInt(), workPtr->val_pwm_timems_loop); //Leddaten.set -> sollte sofort ausgefuehrt werden
			mSHM->ledVal[workPtr->portNr] = workPtr->val_pwm_timems_loop;
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
     
			break;
    case LED_RESET:
      led_reset(workPtr->multi_digit_identifier, mLedArray);
      workPtr = workPtr->nextElement;  //Zeiger auf naechstes QE legen
      
      break;
		case SERVO:
			mServoArray.at(workPtr->portNr).setValues(workPtr->val_pwm_timems_loop); //Servo.set -> sollte sofort ausgefuehrt werden
			mSHM->servoVal[workPtr->portNr] = workPtr->val_pwm_timems_loop;
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
     
			break;
		case INPUT_DIGITAL:	//Handling der Digitalen Anschluesse
			if (workPtr->type == 'O')	//Digitalen Ausgang setzen
			{
				mDAIn.at(workPtr->portNr).setValueDigital(workPtr->compare_direction);	//Ein-/Ausschalten des Pins
			}
			else	//Digitalen Eingang lesen, wird von der HMI noch nicht verwendet
			{
				mSHM->digitalVal[workPtr->portNr] = mDAIn.at(workPtr->portNr).getValueDigital();	//Wert in Container schreiben
				//Wenn das Display zur Analoganzeige benutzt wird:
				/*
				display.setCursor(0,0);
				display.print("Digital In ");
				display.println(workPtr->portNr);
				display.print(mDAIn.at(workPtr->portNr).getValueDigital());
				*/
				//Ende Display
			}
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
     
			break;
		case INPUT_ANALOG:	//wird von der HMI noch nicht verwendet
			mSHM->analogVal[workPtr->portNr] = mDAIn.at(workPtr->portNr).getValueAnalog()/32;	//Wert in Container schreiben, 12Bit-Wert auf 7Bit-Wert skalieren (Beschraenkung wegen HMI)
			//Wenn das Display zur Digitalanzeige benutzt wird:
			/*
			display.setCursor(0,16);
			display.print("Analog In ");
			display.println(workPtr->portNr);
			display.print(mDAIn.at(workPtr->portNr).getValueAnalog());
			*/
			//Ende Display
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
     
      break;
		case SLEEP:
			waitTime = workPtr->time_s * 1000;	//Sekunden in warten [ms] speichern
			waitTime += workPtr->val_pwm_timems_loop * 10;	//Millisekunden in warten [ms] speichern
			Serial.print("[QW] Warte ");
			Serial.print(waitTime);
			Serial.println(" ms");
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
     
			break;
		case IF:
			if (workPtr->type == 'D')	//Digitalen Eingang abfragen
			{
				unsigned int logicState = mDAIn.at(workPtr->portNr).getValueDigital();
				mSHM->digitalVal[workPtr->portNr] = (bool)logicState;
				Serial.print("[QW] If-Verzweigung, digital Abfragen: D" + (String)workPtr->portNr + "==" + (String)workPtr->val_pwm_timems_loop + "? ");
				if ((bool)logicState == (bool)workPtr->val_pwm_timems_loop)	//Wenn Taster gedrueckt wurde, in if reingehen
				{
					Serial.println("true ->Betrete if-Teil");
					workPtr = workPtr->nextElement;	//in If reingehen
				}
				else	//Taster wurde nicht gedrueckt, springen
				{
					Serial.println("false ->Ueberspringe if-Teil");
					if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pruefung nur fuer debugging)
					{
						Serial.println("[QW] Gehe in Else");
					}
					workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
				}
			}
			else	//Analogen Eingang abfragen
			{
				unsigned int measure_val = mDAIn.at(workPtr->portNr).getValueAnalog()/32;	//Einlesen des Analogwertes, 12Bit-Wert auf 7Bit-Wert skalieren (Beschraenkung wegen HMI)
				mSHM->analogVal[workPtr->portNr] = measure_val;
				Serial.print("[QW] If-Verzweigung, analog Abfragen: A" + (String)workPtr->portNr + ": " + (String)measure_val);
				switch (workPtr->compare_direction)	//Auswahl, welcher Vergleichsoperator durchgefuehrt wird
				{
				case '<':	//Ain < Vergleichswert
					Serial.print("<" + (String)workPtr->val_pwm_timems_loop + "? ");
					if (measure_val < workPtr->val_pwm_timems_loop)	//Wenn Ain < Vergleichswert, in if reingehen
					{
						Serial.println("true ->Betrete if-Teil");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht < Vergleichswert
					{
						Serial.println("false ->Ueberspringe if-Teil");
						if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pruefung nur fuer debugging)
						{
							Serial.println("[QW] Gehe in Else");
						}
						workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
						//workPtr = workPtr->jumpElement;	//Zeiger auf zugehoeriges Endif legen
					}
					break;
				case '>':	//Ain > Vergleichswert
					Serial.print(">" + (String)workPtr->val_pwm_timems_loop + "? ");
					if (measure_val > workPtr->val_pwm_timems_loop)	//Wenn Ain > Vergleichswert, in if reingehen
					{
						Serial.println("true ->Betrete if-Teil");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht > Vergleichswert
					{
						Serial.println("false ->Ueberspringe if-Teil");
						if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pruefung nur fuer debugging)
						{
							Serial.println("[QW] Gehe in Else");
						}
						workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
						//workPtr = workPtr->jumpElement;	//Zeiger auf zugehoeriges Endif legen
					}
					break;
				case '=':
					Serial.print(">" + (String)workPtr->val_pwm_timems_loop + "? ");
					if (measure_val == workPtr->val_pwm_timems_loop)	//Wenn Ain gleich Vergleichswert, in if reingehen
					{
						Serial.println("true ->Betrete if-Teil");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain ungleich Vergleichswert
					{
						Serial.println("false ->Ueberspringe if-Teil");
						if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pruefung nur fuer debugging)
						{
							Serial.println("[QW] Gehe in Else");
						}
						workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
						//workPtr = workPtr->jumpElement;	//Zeiger auf zugehoeriges Endif legen
					}
					break;
				default:
					break;
				}
			}
     
			break;
		case ELSE:	//Else
			workPtr = workPtr->jumpElement->nextElement;	//nach If-Pfad wird Else-Pfad uebersprungen. Zeiger auf eins nach zugehoerigem Endif legen
      
			break;
		case ENDIF:	//Endif
			workPtr = workPtr->nextElement;	//hier passiert nix, Endif wird uebersprungen
     
			break;
		case WHILE:
			if (workPtr->type == 'Z')
			{
				Serial.print("[QW] Zaehlschleife: ");
				if (workPtr->val_pwm_timems_loop > 0) {	//Anzahl der Durchlaeufe > 0
					Serial.print("noch ");
					Serial.print(workPtr->val_pwm_timems_loop);
					Serial.println(" Durchlaeufe. Betrete Schleife");
					workPtr->val_pwm_timems_loop--;	//Anzahl der noch offenen Schleifendurchlaeufe um 1 reduzieren 
					workPtr = workPtr->nextElement;	//in While Reingehen 
				}
				else
				{
					Serial.println("Ueberspringe Schleife");
					workPtr = workPtr->jumpElement->nextElement;
				}
			}
			else if (workPtr->type == 'D')	//Digitalen Eingang abfragen
			{
				unsigned int logicState = mDAIn.at(workPtr->portNr).getValueDigital();
				mSHM->digitalVal[workPtr->portNr] = (bool)logicState;
				Serial.print("[QW] Schleife, digital Abfragen: D" + (String)workPtr->portNr + "==" + (String)workPtr->val_pwm_timems_loop + "? ");
				if ((bool)logicState == (bool)workPtr->val_pwm_timems_loop)	//Wenn Taster gedrueckt wurde, in if reingehen
				{
					Serial.println("true ->Betrete Schleife");
					workPtr = workPtr->nextElement;	//in While reingehen
				}
				else	//Taster wurde nicht gedroeckt, springen
				{
					Serial.println("false ->Ueberspringe Schleife");
					workPtr = workPtr->jumpElement->nextElement; //Zeiger auf Element nach zugehoerigem EndWhile legen
				}
			}
			else	//Analogen Eingang abfragen
			{
				unsigned int measure_val = mDAIn.at(workPtr->portNr).getValueAnalog()/32;	//Einlesen des Analogwertes, 12Bit-Wert auf 7Bit-Wert skalieren (Beschraenkung wegen HMI)
				mSHM->analogVal[workPtr->portNr] = measure_val;
				Serial.print("[QW] Schleife, analog Abfragen: A" + (String)workPtr->portNr + ": " + (String)measure_val);
				switch (workPtr->compare_direction)	//Auswahl, welcher Vergleichsoperator durchgefuehrt wird
				{
				case '<':	//Ain < Vergleichswert
					Serial.print("<" + (String)workPtr->val_pwm_timems_loop + "? ");
					if (measure_val < workPtr->val_pwm_timems_loop)	//Wenn Ain < Vergleichswert, in if reingehen
					{
						Serial.println("true ->Betrete Schleife");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht < Vergleichswert
					{
						Serial.println("false ->Ueberspringe Schleife");
						workPtr = workPtr->jumpElement->nextElement;	//Zeiger auf zugehoeriges Endif legen
					}
					break;
				case '>':	//Ain > Vergleichswert
					Serial.print(">" + (String)workPtr->val_pwm_timems_loop + "? ");
					if (measure_val > workPtr->val_pwm_timems_loop)	//Wenn Ain > Vergleichswert, in if reingehen
					{
						Serial.println("true ->Betrete Schleife");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht > Vergleichswert
					{
						Serial.println("false ->Ueberspringe Schleife");
						workPtr = workPtr->jumpElement->nextElement;	//Zeiger auf zugehoeriges Endif legen
					}
					break;
				case '=':
					Serial.print("==" + (String)workPtr->val_pwm_timems_loop + "? ");
					if (measure_val == workPtr->val_pwm_timems_loop)	//Wenn Ain gleich Vergleichswert, in if reingehen
					{
						Serial.println("true ->Betrete Schleife");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain ungleich Vergleichswert
					{
						Serial.println("false ->Ueberspringe Schleife");
						workPtr = workPtr->jumpElement->nextElement;	//Zeiger auf zugehoeriges Endif legen
					}
					break;
				default:
					break;
				}
			}
     
			break;
		case ENDWHILE:	//EndWhile
			Serial.println("[QW] Wiederhole Schleife");
			workPtr = workPtr->jumpElement;	//Springt zurueck zu While und dortiger Pruefung
      
			break;
		default:
    
			break;
		}
		
		cOledHandler::getInstance()->printQueueWindow(mSHM);
		
		/* debug
    //DEBUG MIT DISPLAY - Anzeige aller Eingaenge (digital und analog)
    //cOledHandler::getInstance()->printOledMessage("T1:" + (String)mDAIn.at(0).getValueDigital());
    
    Serial.println("State: ");
		for (int i = 0; i < 8; i++)
		{
			Serial.print("T" + (String)i + ":" + (String)mSHM->digitalVal[i]);
		}
		for(int i = 0; i < DAIN_QTY; i++)
		{
			display.setCursor((i%4)*32,(i/4)*16);
			display.print(PORT_IN[i]);
			display.print(" ");
			display.print(!mDAIn.at(i).getValueDigital());
			display.setCursor((i%4)*32,((i/4)*16)+8);
			//display.print(mDAIn.at(i).getValueAnalog());
		}
		display.display();
		display.clearDisplay();
		//~DEBUG MIT DISPLAY */
    
    led_update(mLedArray);

		do {
			if (waitTime > 0)	//wenn Wartezeit vorhanden
			{
				//Serial.println("Warte 10 ms");
				waitTime -= 10;	//Wartezeit um 10ms reduzieren
				delay(10);
			}
			
			if (mSHM->commonPause)	//Pause-Button wurde gedrueckt
			{
				Serial.println("[Q_work] Pause gestartet");
				Motor mMotorSave[MOTOR_QTY];// mMotorArray.size()];	//Sicherungsobjekte fuer Motoren anlegen
				Led mLedSave[LED_QTY];// mLedArray.size()];	//Sicherungsobjekte fuer Ledn anlegen
				for (int i = 0; i < mMotorArray.size(); i++)
				{
					mMotorSave[i] = mMotorArray[i];	//Motor i sichern
					mMotorArray[i].setValues(true, 0);	//Ausgang zur 0 setzen
				}

        for (int i = 0; i < mLedArray.size(); i++)
        {
          mLedSave[i] = mLedArray[i]; //Led i sichern
          mLedArray[i].setValues(0, 0);  //Ausgang zur 0 setzen
        }
       
				do {	//in Schleife auf Aufhebung des Pausezustandes warten
					delay(10);	//10ms warten (schohnt CPU)
				} while (mSHM->commonPause && mSHM->commonStart);	//warten auf Aufhebung der Pause oder auf Stopp
       
				Serial.println("[Q_work] Pause beendet");
				//IO-Objekte auf kopierte Werte zuruecksetzen
				for (int i = 0; i < mMotorArray.size(); i++)
				{
					mMotorArray[i] = mMotorSave[i];	//Ausgangswerte zuruecksetzen
					mMotorArray[i].reRun();	//Ausgang neu setzen
					mLedArray[i] = mLedSave[i];
					//mLedArray[i].reRun();
				}
			}
		} while (waitTime > 0 && mSHM->commonStart);	//warten auf Ende der Pausenzeit oder auf Stopp
		
		//endtime = clock() / (CLOCKS_PER_SEC / 1000);
		endtime = millis();
		runtime = endtime - starttime;
		//Serial.println("[Q_work] Endtime: " + (String)endtime + " Diff: " + (String)(runtime));
		runtime = runtime <= cycleTime ? runtime : cycleTime;	//limit runtime to max. 10ms for delay();
		//delay(cycleTime - runtime);	//ensures a constant cycle time for queue-thread while running
		delay(1);
  }

	Serial.println("[Q_work] Queue abarbeiten Ende.\n");
	
	Serial.println("[Q_work] Ausgaenge zuruecksetzen...");
	
	for (int i = 0; i < mMotorArray.size(); i++)	//Am Ende des Programms zuruecksetzen der Ausgaenge auf 0
	{
		mMotorArray[i].setValues(true, 0);
	}

  for (int i = 0; i < mLedArray.size(); i++)  //Am Ende des Programms zuruecksetzen der Ausgaenge auf 0
  {
    mLedArray[i].setValues(0,0);
    led_update(mLedArray);
  }

	mServoArray[0].setValues(100);

	delay(10);

  //mSHM->commonStart=false;
  //mSHM->commonPause=false;
  //mSHM->commonStopp=false;
  //mSHM->running=false;
  
	Serial.println("[Q_work] Ausgaenge zurueckgesetzt.");
	
	//return false;
}
