#ifndef CBUTTON_H_
#define CBUTTON_H_

#include "ft_ESP32_SHM.h"

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
