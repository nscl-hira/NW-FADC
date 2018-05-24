#ifndef BAR__H__
#define BAR__H__
#include <iostream>
#include "TObject.h"
#include "FADC.h"
class Bar : public TObject {
 public:
  Bar(Int_t barNum = -1);
  virtual ~Bar();


  Int_t fnumBar;
  Short_t ADCLeft[240];
  Short_t ADCRight[240];
  Double_t fLeft;
  Double_t fRight;
  Double_t fFastLeft;
  Double_t fFastRight;
  Double_t fGeoMean;
  Double_t fFastGeoMean;
  Double_t fTimeLeft;
  Double_t fTimeRight;

  //Take the two ends and turn it into a bar
  //Fills everything but the barNumber
  void Convert(FADC *fadcRight, FADC *fadcLeft);
  void Convert(FADC *fadcRight, FADC *fadcLeft,
	       Short_t pedLength, Short_t fastWindow, Short_t longWindow,
	       Double_t CFDHeight)
  {
    Convert(fadcRight, fadcLeft);
  }
  
  ClassDef( Bar, 1 )
};

#endif 
