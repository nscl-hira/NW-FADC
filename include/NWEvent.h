#ifndef NWEVENT_H
#define NWEVENT_H

#include <iostream>
#include "TObject.h"

#include "FADC.h"
#include "Bar.h"
#include "TCB.h"

class NWEvent {

public:
  NWEvent();
  virtual ~NWEvent();

  void reset();

  
  Int_t fmult;
  ULong_t fTimestamp;
  
  //Arrays of size [fmult][NSMPL]
  Short_t  **ADCRight;   //[fmult][240]
  Short_t  **ADCLeft;    //[fmult][240]

  //Arrays of size fmult
  Int_t    *fBarNum;      //[fmult]
  Double_t *fLeft;        //[fmult]
  Double_t *fRight;       //[fmult]
  Double_t *fFastLeft;    //[fmult]
  Double_t *fFastRight;   //[fmult]
  Double_t *fTimeLeft;    //[fmult]
  Double_t *fTimeRight;   //[fmult]
  Double_t *fGeoMean;     //[fmult]
  Double_t *fFastGeoMean; //[fmult]
  
  
private:
  const static int NBAR = 24;
  const static int NCH = 112;
  const static int NSMPL = 240;
  
  ClassDef( NWEvent, 1);
  
};

#endif
