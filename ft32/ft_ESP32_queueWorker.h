#ifndef FT_ESP32_QUEUEWORKER_H
#define FT_ESP32_QUEUEWORKER_H

#include "ft_ESP32_queueCreator.h"
#include "ft_ESP32_IOobjects.h"
#include "ft_ESP32_SHM.h"
#include <array>

using namespace std;

bool queueWorker(commonElement*& startPtr, commonElement*& endPtr, array<Motor,MOTOR_QTY>& mMotorArray, array<Lampe,LAMP_QTY>& mLampeArray, array<DigitalAnalogIn,DAIN_QTY>& mDAIn, SHM* mSHM);

#endif
