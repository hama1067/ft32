#include "CButton.h"

CButton::CButton()
{
  ptrSHM = NULL;
  mPin = 0;
}

CButton::~CButton(){};

void CButton::initButton(const int pPin, SHM * pSHM)
{
  ptrSHM = pSHM;
  mPin = pPin;
  pinMode(pPin, INPUT_PULLUP);
}

void CButton::handleButton() {
	if (digitalRead(mPin)==LOW) {

		delay(50);

		if (digitalRead(mPin)==LOW) {

			while(digitalRead(mPin)==LOW) { };

			if(ptrSHM->mSpeicher.fileExists("/spiffs-cody-storage.txt") == true) {

				if(ptrSHM->commonStart == false) {
					ptrSHM->commonStart = true;
					ptrSHM->commonPause = false;
					Serial.println("");
					Serial.println("Taster betätigt: START");
					Serial.println("");
				} else	{
					ptrSHM->commonStart = false;
					ptrSHM->commonPause = true;
					Serial.println("");
					Serial.println("Taster betätigt: PAUSE");
					Serial.println("");
				}
			}
		}
	}
}
