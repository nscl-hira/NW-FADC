#ifndef EVENTBUILDER_H
#define EVENTBUILDER_H

#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"

#include "FADC.h"
#include "Bar.h"
#include "TCB.h"

class EventBuilder : public TObject {

public:
  EventBuilder();
  virtual ~EventBuilder();
  
  bool init(Int_t runNum);
  
  void buildEvents();

  void writeTree(TString fileName);
  
private:
  //Input files
  TFile *fTCB;
  TFile *fADC[28];

  //Input data structures
  TTree *trTCB;
  TCB *tcb;
  TTree *trFADC[112];
  FADC *fadc[24];

  //Temp data structures
  FADC *tempFADC[2][24][2]; //A/B BarNum R/L


  //Output variables to be written to a tree
  int fMult;

  //Arrays of length fMult
  short  *ADC[240];
  int    *fBarNum;
  double *fLeft;
  double *fRight;
  double *fFastLeft;
  double *fFastRight;
  double *fTimeLeft;
  double *fTimeRight;
  double *fGeoMean;
  double *fFastGeoMean;
    
  unsigned long fTimestamp;
  
  
  
  TTree *outTree;

  ClassDef( EventBuilder, 1)

};

#endif
