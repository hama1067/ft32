#ifndef DISABLEMOTORBOARD_H_
#define DISABLEMOTORBOARD_H_

void DisableMotorBoard()
{
  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);
}

#endif
