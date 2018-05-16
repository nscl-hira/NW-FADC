#ifndef FADC__H__
#define FADC__H__
#include <iostream>
#include "TObject.h"

class FADC : public TObject {
 public:
  FADC();
  ~FADC();

  UInt_t        data_length;
  UShort_t      ttype;
  UInt_t        tnum;
  UInt_t        lnum;
  ULong64_t     ttime;
  ULong64_t     ltime;

  UInt_t        t0;
  UShort_t      rid;
  UShort_t      mid;
  UShort_t      cid;
  Int_t         evtn;
  UShort_t      tpattern;

  UShort_t         ped;
  Int_t         nADC;
  Int_t         nTDC;
  Short_t       ADC[240];
  Int_t         TDC[60];

  //Int_t    ADCT[16384];
  Double_t      ADCSum;
  Double_t      ADCPeak;
  Double_t      ADCPart;//partial ADC sum//
  Double_t      ADCTime;
  Double_t      ADCPed;

  virtual void Clear();
  void Convert(UShort_t* data,int off=0);

  
  ClassDef( FADC, 1 )
};  

#endif 
