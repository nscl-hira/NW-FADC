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
#include "DataStruct.h"
#include "TStyle.h"
int main( int argc, char** argv ){
  gStyle->SetOptStat(0);
  //////////////////////////////////////////////////
  //// EnvSetting
  char* TCBRawData;
  char* TCBConvData;
  char* RawData[4];
  char* ConvData[4];
  char* MonData;
  TCBRawData=std::getenv("TCBRAWDATA");
  TCBConvData=std::getenv("TCBCONVDATA");
  for( int i = 0; i< 4; i++){
    RawData[i] = std::getenv(Form("RAWDATA_%d",i));
    ConvData[i]= std::getenv(Form("CONVDATA_%d",i));
  }
  MonData = std::getenv("MONDATA");
  //////////////////////////////////////////////////

  std::string head = argv[1];
  TFile* tftcb = new TFile(Form("%s/%s_tcb_conv.root",TCBConvData,head.c_str()));
  if( tftcb == NULL ){
    std::cout << "No File : "
	      << Form("%s_tcb_conv.root", head.c_str())
	      << std::endl; 
    return -1;
  }
  //TApplication* app = new TApplication("app",&argc, argv );
  TCanvas* can = new TCanvas("can","",800,800);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLogz();
  TTree* trtcb = (TTree*)tftcb->Get("TCBTree");
  TFile* tffadc[28]={NULL};
  TTree* trfadc[112]={NULL};
  for( int i = 0; i< 28; i++){
    int diskID = (int)(i/7);
    tffadc[i] = new TFile(Form("%s/%s_fadc_%d_conv.root",ConvData[diskID],head.c_str(),i+1)); 
    std::cout << i << "\t" << Form("%s/%s_fadc_%d_conv.root",ConvData[diskID],head.c_str(),i+1) << std::endl;
    if( tffadc[i] == NULL ){
      std::cout << "No File : "
		<< Form("%s_fadc_%d_conv.root", head.c_str(),i+1)
		<< std::endl; 
      continue;
    }
    for( int j = 0; j< 4; j++){
      trfadc[i*4+j] = (TTree*)tffadc[i]->Get(Form("Tree_%d",j));
    }
  }
  
  std::cout<< "TTree Setting" << std::endl;
  /////// TCB tree setting //////

  TCB* tcb = new TCB();
  trtcb->SetBranchAddress("tcb",&tcb);
  ULong64_t TCBEntries = trtcb->GetEntries();
  trtcb->GetEntry( TCBEntries -1 );
  ULong64_t MAXEVT = tcb->tcb_trigger_number;
  trtcb->GetEntry(0);
  ULong64_t MINEVT = tcb->tcb_trigger_number;
  ULong64_t tcb_ttime0 = tcb->tcb_ttime;

  std::cout<< "Minimum : " << MINEVT << std::endl;
  std::cout<< "Maximum : " << MAXEVT << std::endl;
  //////////////////////////////
  
  FADC* fadc[112];
  for( int i = 0; i< 112; i++){
    fadc[i] = new FADC();
    if( trfadc[i] == NULL ){ continue; }
    //wave[i] = new FADC();
    std::cout << i << std::endl;
    trfadc[i]->SetBranchAddress("wave",&(fadc[i]));
    if( trfadc[i]->GetEntries() > 0 ){
      trfadc[i]->GetEntry(0);
    }
  }

  std::cout<< "Set End" << std::endl;

  ULong64_t TCBEntry=0;
  ULong64_t FADCEntry[112] = {0};

  std::cout<< "ReadFile setting end" << std::endl;
  ///////////////////////////////////////////////////////////////////////

  TFile* tfMerge = new TFile(Form("%s_event.root",head.c_str()),"recreate");
  TTree* MergeTree = new TTree("EventTree","EventTree");
  MergeTree->SetMaxTreeSize(1000000000);//1GB
  const Int_t NCH  = 112;   
  const Int_t NBAR = 25;
  const Int_t NSMPL= 240;
  ULong64_t Trig_num;
  ULong64_t Trig_ttime;

  Int_t     nWAVE;
  UShort_t  Channel[NCH];
  ULong64_t CH_ttime[NCH];
  UShort_t  WAVE[NCH][NSMPL]; 
  Int_t     nSPWAVE;
  UShort_t  SPChannel[16];
  UShort_t  SPWAVE[NCH][NSMPL];
  Double_t  SPWAVETime[NCH];
  MergeTree->Branch("Trig_num"  ,&Trig_num  ,"Trig_num/l");
  MergeTree->Branch("Trig_ttime",&Trig_ttime,"Trig_ttime/l");
  MergeTree->Branch("nWAVE"     ,&nWAVE     ,"nWAVE/I");
  MergeTree->Branch("Channel"   ,Channel    ,"Channel[nWAVE]/s");
  MergeTree->Branch("CH_ttime"  ,CH_ttime   ,"CH_ttime[nWAVE]/l");
  MergeTree->Branch("WAVE"      ,WAVE       ,Form("WAVE[nWAVE][%d]/s",NSMPL));
  MergeTree->Branch("nSPWAVE"   ,&nSPWAVE   ,"nSPWAVE/I");
  MergeTree->Branch("SPChannel" ,Channel    ,"SPChannel[nSPWAVE]/s");
  MergeTree->Branch("SPWAVE"    ,SPWAVE     ,Form("SPWAVE[nSPWAVE][%d]/s",NSMPL));
  MergeTree->Branch("SPWAVETime",SPWAVETime ,"SPWAVETime[nSPWAVE]/D");
  
  Int_t NA;
  Int_t AID[NBAR];
  Double_t AGeoMeanSum[NBAR];  
  Double_t AGeoMeanPart[NBAR]; 
  Double_t AMeanTime[NBAR];
  Double_t ASubTime[NBAR];
  Double_t AModSum[NBAR][2];
  Double_t AModPart[NBAR][2];
  Double_t AModTime[NBAR][2];
  Double_t AModPeak[NBAR][2];
  Double_t AModPed[NBAR][2];
  Int_t    AModCH[NBAR][2];
  
  Int_t NB;
  Int_t BID[NBAR];
  Double_t BGeoMeanSum[NBAR];  
  Double_t BGeoMeanPart[NBAR]; 
  Double_t BMeanTime[NBAR];
  Double_t BSubTime[NBAR];
  Double_t BModSum[NBAR][2];
  Double_t BModPart[NBAR][2];
  Double_t BModTime[NBAR][2];
  Double_t BModPeak[NBAR][2];
  Double_t BModPed[NBAR][2]; 
  Int_t    BModCH[NBAR][2];

  Int_t    TMPAID[NBAR];
  Int_t    TMPANHIT[NBAR];
  Double_t TMPAGeoMeanSum[NBAR];  
  Double_t TMPAGeoMeanPart[NBAR]; 
  Double_t TMPAMeanTime[NBAR];
  Double_t TMPASubTime[NBAR];
  Double_t TMPAModSum[NBAR][2];
  Double_t TMPAModPart[NBAR][2];
  Double_t TMPAModTime[NBAR][2];
  Double_t TMPAModPeak[NBAR][2];
  Double_t TMPAModPed[NBAR][2]; 
  Int_t    TMPAModCH[NBAR][2];
  
  Int_t    TMPBID[NBAR];
  Int_t    TMPBNHIT[NBAR];
  Double_t TMPBGeoMeanSum[NBAR];  
  Double_t TMPBGeoMeanPart[NBAR]; 
  Double_t TMPBMeanTime[NBAR];
  Double_t TMPBSubTime[NBAR];
  Double_t TMPBModSum[NBAR][2];
  Double_t TMPBModPart[NBAR][2];
  Double_t TMPBModTime[NBAR][2];
  Double_t TMPBModPeak[NBAR][2];
  Double_t TMPBModPed[NBAR][2];
  Int_t    TMPBModCH[NBAR][2];
  
  MergeTree->Branch("NA"          ,&NA         ,"NA/I");
  MergeTree->Branch("AID"         ,AID         ,"AID[NA]/I");
  MergeTree->Branch("AGeoMeanSum" ,AGeoMeanSum ,"AGeoMeanSum[NA]/D");
  MergeTree->Branch("AGeoMeanPart",AGeoMeanPart,"AGeoMeanPart[NA]/D");
  MergeTree->Branch("AMeanTime"   ,AMeanTime   ,"AMeanTime[NA]/D");
  MergeTree->Branch("ASubTime"    ,ASubTime    ,"ASubTime[NA]/D");
  MergeTree->Branch("AModSum"     ,AModSum     ,"AModSum[NA][2]/D");
  MergeTree->Branch("AModPart"    ,AModPart    ,"AModPart[NA][2]/D");
  MergeTree->Branch("AModTime"    ,AModTime    ,"AModTime[NA][2]/D");
  MergeTree->Branch("AModPeak"    ,AModPeak    ,"AModPeak[NA][2]/D");
  MergeTree->Branch("AModPed"     ,AModPed     ,"AModPed[NA][2]/D");
  MergeTree->Branch("AModCH"      ,AModCH      ,"AModCH[NA][2]/I");
  
  MergeTree->Branch("NB"          ,&NB         ,"NB/I");
  MergeTree->Branch("BID"         ,BID         ,"BID[NB]/I");
  MergeTree->Branch("BGeoMeanSum" ,BGeoMeanSum ,"BGeoMeanSum[NB]/D");
  MergeTree->Branch("BGeoMeanPart",BGeoMeanPart,"BGeoMeanPart[NB]/D");
  MergeTree->Branch("BMeanTime"   ,BMeanTime   ,"BMeanTime[NB]/D");
  MergeTree->Branch("BSubTime"    ,BSubTime    ,"BSubTime[NB]/D");
  MergeTree->Branch("BModSum"     ,BModSum     ,"BModSum[NB][2]/D");
  MergeTree->Branch("BModPart"    ,BModPart    ,"BModPart[NB][2]/D");
  MergeTree->Branch("BModTime"    ,BModTime    ,"BModTime[NB][2]/D");
  MergeTree->Branch("BModPeak"    ,BModPeak    ,"BModPeak[NB][2]/D");
  MergeTree->Branch("BModPed"     ,BModPed     ,"BModPed[NB][2]/D");
  MergeTree->Branch("BModCH"      ,BModCH      ,"BModCH[NB][2]/I");
  
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
  Int_t  NWABMap[112]=
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
  Int_t  NWRL[112]=
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
  can->Draw();
  TH2D* hisWave = new TH2D("hisWave","hisWave",240,0,480,112,0,112);
  std::cout << "Loop" << std::endl;
  for( int ievt = MINEVT ; ievt <= MAXEVT; ievt++){
  //for( int ievt = MINEVT ; ievt <= MINEVT+100000; ievt++){    
    hisWave->Reset();
    
    //std::cout<< ievt << std::endl;
    Trig_num   = ievt;
    Trig_ttime = 0; 
    nWAVE      = 0;
    nSPWAVE    = 0;
    NA         = 0;
    NB         = 0;
    
    for( int ibar = 0; ibar < NBAR; ibar++ ){
      TMPAID         [ibar]=0;
      TMPANHIT       [ibar]=0;
      TMPAGeoMeanSum [ibar]=0;  
      TMPAGeoMeanPart[ibar]=0; 
      TMPAMeanTime   [ibar]=0;
      TMPASubTime    [ibar]=0;
      TMPBID         [ibar]=0;
      TMPBNHIT       [ibar]=0;
      TMPBGeoMeanSum [ibar]=0;  
      TMPBGeoMeanPart[ibar]=0; 
      TMPBMeanTime   [ibar]=0;
      TMPBSubTime    [ibar]=0;
      for( int irl = 0; irl < 2; irl++){
	TMPAModSum [ibar][irl]=0;
	TMPAModPart[ibar][irl]=0;
	TMPAModTime[ibar][irl]=0;
	TMPAModPeak[ibar][irl]=0;
	TMPAModPed [ibar][irl]=0; 	
	TMPBModCH  [ibar][irl]=0;
	TMPBModSum [ibar][irl]=0;
	TMPBModPart[ibar][irl]=0;
	TMPBModTime[ibar][irl]=0;
	TMPBModPeak[ibar][irl]=0;
	TMPBModPed [ibar][irl]=0;
	TMPBModCH  [ibar][irl]=0;
      }
    }
    
    while( tcb->tcb_trigger_number < ievt ){
      TCBEntry++;
      if( TCBEntry >= trtcb->GetEntries() ){ break; }
      trtcb->GetEntry( TCBEntry );
      if( tcb->tcb_trigger_number >= ievt ){ break; }
    }
    if( tcb->tcb_trigger_number == ievt ){
      Trig_ttime = tcb->tcb_ttime;
    }
    
    // Don't think about tcb_trig_num > ievt
    nSPWAVE = 0;
    for( int ifadc = 0; ifadc < 112; ifadc++){
      if( trfadc[ifadc] == NULL ){ continue; }
      while( fadc[ifadc]->tnum < ievt ){
	FADCEntry[ifadc]++;
	if( FADCEntry[ifadc] >= trfadc[ifadc]->GetEntries() ){ break; }
	trfadc[ifadc]->GetEntry( FADCEntry[ifadc] );
      }
      if( fadc[ifadc]->tnum == ievt ){
	//int ch = (fadc[ifadc]->mid-1)*4 + (fadc[ifadc]->cid-1);
	int ch = ifadc;

	bool bcrossed = false; 
	if ( SPCHMap[ch] >= 0 ){ // Special Channel 
	  SPChannel[nSPWAVE] = ch;
	  for( int ip = 0; ip < NSMPL; ip++){
	    SPWAVE[nSPWAVE][ip]	= fadc[ifadc]->ADC[ip];
	    if( ip > 0 && 
		SPWAVE[nSPWAVE][ip]  >= 1400 &&
		SPWAVE[nSPWAVE][ip-1]<  1400 &&
		!bcrossed ){
	      SPWAVETime[nSPWAVE]  = ip*2 - 2*(SPWAVE[nSPWAVE][ip]-1400)/(SPWAVE[nSPWAVE][ip]-SPWAVE[nSPWAVE][ip-1]);
	      bcrossed = true;	      
	    }
	  }
	  nSPWAVE++;
	}else{
	  if( ch < 0 || ch >= 112 ){ continue; }
	  Channel[nWAVE] = ch;	  
	  CH_ttime[nWAVE] = fadc[ifadc]->ttime;
	  for( int ip = 0; ip < NSMPL; ip++){
	    WAVE[nWAVE][ip]	= fadc[ifadc]->ADC[ip];
	  }
	  nWAVE++;
	  Int_t RLID = NWRL[ifadc]-1;   // 0 : R, 1 :L
	  Int_t ABID = NWABMap[ifadc]-1;// 0 : A, 1 :B 
	  Int_t BARID= BarIDMap[ifadc]; // From 0 to 24;
	  if( ABID == 0 ){
	    
	    TMPAID[BARID]=BARID;
	    TMPANHIT[BARID]++;
	    TMPAModSum [BARID][RLID] =fadc[ifadc]->ADCSum;
	    TMPAModPart[BARID][RLID] =fadc[ifadc]->ADCPart;
	    TMPAModTime[BARID][RLID] =fadc[ifadc]->ADCTime;
	    TMPAModPeak[BARID][RLID] =fadc[ifadc]->ADCPeak;
	    TMPAModPed [BARID][RLID] =fadc[ifadc]->ADCPed; 	
	    
	  }else if( ABID == 1 ){

	    TMPBID[BARID]=BARID;
	    TMPBNHIT[BARID]++;
	    TMPBModSum [BARID][RLID] =fadc[ifadc]->ADCSum;
	    TMPBModPart[BARID][RLID] =fadc[ifadc]->ADCPart;
	    TMPBModTime[BARID][RLID] =fadc[ifadc]->ADCTime;
	    TMPBModPeak[BARID][RLID] =fadc[ifadc]->ADCPeak;
	    TMPBModPed [BARID][RLID] =fadc[ifadc]->ADCPed; 	
	    
	  }	  
	}
      }
    }

    /// Evaluate Hit
    NA = 0; NB = 0;
    for( int ibar = 0; ibar < 25; ibar++){
      if( TMPANHIT[ibar] == 2 ){
	AID[NA]=ibar;
	for( int irl = 0; irl < 2; irl++){
	  AModSum [NA][irl] = TMPAModSum [ibar][irl]; 
	  AModPart[NA][irl] = TMPAModPart[ibar][irl];
	  AModTime[NA][irl] = TMPAModTime[ibar][irl];
	  AModPeak[NA][irl] = TMPAModPeak[ibar][irl];
	  AModPed [NA][irl] = TMPAModPed [ibar][irl]; 	
	}
	if( AModSum[NA][0] > 0 &&
	    AModSum[NA][1] > 0 ){
	  AGeoMeanSum[NA] = TMath::Sqrt(AModSum[NA][0]*AModSum[NA][1]);
	}else{
	  AGeoMeanSum[NA] = -1;
	}
	if( AModPart[NA][0] > 0 &&
	    AModPart[NA][1] > 0 ){
	  AGeoMeanPart[NA] = TMath::Sqrt(AModPart[NA][0]*AModPart[NA][1]);
	}
	AMeanTime[NA] = (AModTime[NA][0]+AModTime[NA][1])/2.;
	ASubTime[NA]  = (AModTime[NA][0]-AModTime[NA][1]);
	NA++;
      }
    }

    for( int ibar = 0; ibar < 25; ibar++){
    if( TMPBNHIT[ibar] == 2 ){
      BID[NB]=ibar;
      for( int irl = 0; irl < 2; irl++){
	BModSum [NB][irl] = TMPBModSum[ibar][irl]; 
	BModPart[NB][irl] = TMPBModPart[ibar][irl];
	BModTime[NB][irl] = TMPBModTime[ibar][irl];
	BModPeak[NB][irl] = TMPBModPeak[ibar][irl];
	BModPed [NB][irl] = TMPBModPed[ibar][irl]; 	
      }
      if( BModSum[NB][0] > 0 &&
	  BModSum[NB][1] > 0 ){
	BGeoMeanSum[NB] = TMath::Sqrt(BModSum[NB][0]*BModSum[NB][1]);
      }else{
	BGeoMeanSum[NB] = -1;
      }
      if( BModPart[NB][0] > 0 &&
	  BModPart[NB][1] > 0 ){
	BGeoMeanPart[NB] = TMath::Sqrt(BModPart[NB][0]*BModPart[NB][1]);
      }
      BMeanTime[NB] = (BModTime[NB][0]+BModTime[NB][1])/2.;
      BSubTime[NB]  = (BModTime[NB][0]-BModTime[NB][1]);
      NB++;
    }
  }    

    //std::cout<< NA << "\t" << NB << std::endl; 
    if( ievt % 10000 == 0 ){ std::cout<< ievt << "/" << MAXEVT << std::endl; }


    /*
    if( NA > 0 || NB > 0 ){
      int nOut = 0;
      for( int ia = 0; ia < NA; ia++){
        if( TMath::Abs( ASubTime[ia] ) > 40 ){
          nOut++;
        }
      }
      for( int ib = 0; ib < NB; ib++){
        if( TMath::Abs( BSubTime[ib] ) > 40 ){
          nOut++;
        }
      }
    */
    //if( nOut == 0 ){ continue; }
    /*
      hisWave->GetZaxis()->SetRangeUser(10,100);
      hisWave->Draw("colz");
      hisWave->GetZaxis()->SetRangeUser(10,100);
      can->Update();
      can->Modified();
      gSystem->ProcessEvents();
      getchar();
    */
    //}
    MergeTree->Fill();    
  }
  std::cout<< "Minimum : " << MINEVT << std::endl;
  std::cout<< "Maximum : " << MAXEVT << std::endl;
  std::cout<< "ESTIMATE: " << MAXEVT - MINEVT << std::endl; 
  std::cout<< "Recorded: " << MergeTree->GetEntries() << std::endl; 
  MergeTree->Write();
  tfMerge->Close();
  //app->Run();
  //////////////////////////////
}
