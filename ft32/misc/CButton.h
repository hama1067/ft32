#ifndef CBUTTON_H_ESP32
#define CBUTTON_H_

#include "../shm/ft_ESP32_SHM.h"

class CButton
{
  public:
      CButton();
      ~CButton();
      void initButton(const int pPin, SHM * pSHM);
      void handleButton();
  private:
      SHM *ptrSHM;
      int mPin;
};

#endif
