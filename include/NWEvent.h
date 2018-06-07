#ifndef NWEVENT_H
#define NWEVENT_H

#include <iostream>
#include "TObject.h"
#include "TMath.h"

#include "FADC.h"
#include "SignalProcessor.h"

class NWEvent : public TObject {
private:
  const static int NBAR = 24;
  const static int NCH = 112;
  const static int NSMPL = 240;
  static SignalProcessor sigP;
 
public:
  NWEvent();
  virtual ~NWEvent();
  
  Int_t fmulti;
  ULong64_t fTimestamp;
  ULong64_t fTimestamp2;

  Int_t   fBarNum[NBAR];
  Double_t fGeoMean[NBAR];
  Double_t fFastGeoMean[NBAR];
  
  Short_t  ADCRight[NBAR][NSMPL];
  Double_t fRight[NBAR];
  Double_t fFastRight[NBAR];
  Double_t fTimeRight[NBAR];

  Short_t  ADCLeft[NBAR][NSMPL];
  Double_t fLeft[NBAR];
  Double_t fFastLeft[NBAR];
  Double_t fTimeLeft[NBAR];
  
  void AddBar(Int_t barNum, FADC *fadcRight, FADC *fadcLeft);
  void Clear();
  ClassDef( NWEvent, 1);
  
};

#endif
