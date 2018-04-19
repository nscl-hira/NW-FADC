#ifndef DataStruct__H__
#define DataStruct__H__
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


class TCB : public TObject {
 public:
  TCB();
  virtual ~TCB();
  void Init();
  void Convert(UShort_t* data,int off = 0);

  ULong64_t tcb_trigger_number;
  Int_t     tcb_trigger_type;
  ULong64_t tcb_ntime;
  ULong64_t tcb_time;
  ULong64_t tcb_ttime;
  ULong64_t tcb_ttime0;
  UShort_t  tcb_trigger_pattern[20];

  ClassDef( TCB, 1 )
};

class Bar : public TObject {
 public:
  Bar();
  virtual ~Bar();
  Int_t ID;
  Int_t ABID;// 0 : A, 1 : B
  Double_t nHit;
  Double_t TimeMean;
  Double_t TimeDiff;
  Double_t GeoMeanSum;// Geometric mean sum
  Double_t GeoMeanPrt;// Geometric mean part 
  Double_t Time[2];
  Double_t Peak[2];
  Double_t Ped[2];
  Double_t Sum[2];
  Double_t Part[2];
  Int_t    ChID[2];

  void Init();
  
  ClassDef( Bar, 1 )
};

  

#endif 
