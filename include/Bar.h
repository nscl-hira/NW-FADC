#ifndef BAR__H__
#define BAR__H__
#include <iostream>
#include "TObject.h"

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
