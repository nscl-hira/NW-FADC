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
#include "NWEvent.h"
#include "TCB.h"

class EventBuilder : public TObject {

public:
  EventBuilder();
  virtual ~EventBuilder();
  
  bool init(Int_t runNum);
  
  void buildEvents();
  
private:
  const int NBAR  = 24;
  const int NCH   = 112;
  const int NSMPL = 240;

  bool validConfig = false;
  int runNum;
  

  //Input files
  TFile *fTCB;
  TFile *fADC[28];

  //Input data structures
  TTree *trTCB;
  TCB *tcb;
  TTree *trFADC[112];
  FADC *fadc[112];

  //Temp data structures
  FADC *tempFADC[2][24][2]; //A/B BarNum R/L

  NWEvent event[2];
  TTree *outTree[2];
  TFile *outFile;
  
  //Channel Mappings
  Int_t ChannelBarMap[112] =
    {
      //NWA R
      24, 1, 2, 3, 4, 5, 6, 7,
      8 , 9,10,11,12,13,14,15,
      16,17,18,19,20,21,22,23,
      //NWA L
      74,51,52,53,54,55,56,57,
      58,59,60,61,62,63,64,65,
      66,67,68,69,70,71,72,73,

      -1,-1,-1,-1, 0, 0, 0, 0,

      //NWB R
      49,26,27,28,29,30,31,32,
      33,34,35,36,37,38,39,40,
      41,42,43,44,45,46,47,48,
      
      //NWB L
      99,76,77,78,79,80,81,82,
      83,84,85,86,87,88,89,90,
      91,92,93,94,95,96,97,98,
      
      -1,-1,-1,-1,-1,-1,-1,-1};
  
  //NW barID
  Int_t BarIDMap[112]=
    {
      24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
      8 ,9 ,10,11,12,13,14,15,
      16,17,18,19,20,21,22,23,
      
      24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
      8 ,9 ,10,11,12,13,14,15,
      16,17,18,19,20,21,22,23,
      
      -1,-1,-1,-1, 0, 0, 0, 0,
      
      24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
      8 ,9 ,10,11,12,13,14,15,
      16,17,18,19,20,21,22,23,
      
      24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
      8 ,9 ,10,11,12,13,14,15,
      16,17,18,19,20,21,22,23,
      
      -1,-1,-1,-1,-1,-1,-1,-1};
  
  /// NWABMap
  Int_t  NWABMap[112] = //1:A 2:B
    {
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      0,0,0,0,1,1,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      0,0,0,0,0,0,0,0};
  /// RL position ID
  Int_t  NWRL[112] = //1:R 2:L
    {
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,      
      0,0,0,0,2,1,2,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,      
      0,0,0,0,0,0,0,0};

  //Special Channel Map
  // 0 : FA timing or signal
  Int_t SPCHMap[112] =
    {
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      0 ,1 ,2 ,3 ,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,
      8 ,9 ,10,11,12,13,14,15};
  
  
  ClassDef( EventBuilder, 1)
    
};

#endif
