#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "DataStruct.h"
#include <iomanip>

int main( int argc, char** argv ){
  ////////////////////////////////////////////////////
  //// EnvSetting
  char* TCBRawData;
  char* TCBConvData;
  char* RawData[4];
  char* ConvData[4];
  char* MonData;

  TCBRawData=std::getenv("TCBRAWDATA"); //TODO: Does this do anything????
  TCBConvData=std::getenv("TCBCONVDATA");
  for( int i = 0; i< 4; i++){
    RawData[i] = std::getenv(Form("RAWDATA_%d",i));
    ConvData[i]= std::getenv(Form("CONVDATA_%d",i));
  }
  MonData = std::getenv("MONDATA");
  ///////////////////////////////////////////////////

  if( argc != 5 ){
    std::cout<< "<Command> <input filename> <output filename> <rl> <currn>" << std::endl;
    return -1;
  }  
  std::string ifilename = argv[1];
  std::string ofilename = argv[2];
  int   rl              = std::atoi( argv[3] );
  int   currn           = std::atoi( argv[4] );
  
  if( rl != 1 && rl != 2 && rl != 4 && rl != 8 && rl != 16 && rl != 32 ){
    std::cout<< "Wrong Recording length " << rl << std::endl; 
    return -1; 
  }
  if( currn < 0 ){
    std::cout<< "Wrong RunNumber " << currn << std::endl;
    return -1; 
  }
  unsigned int data_length  = rl*128;

  //Get raw FADC data
  TFile* tf = new TFile(ifilename.c_str());
  TTree* tr_fadc = (TTree*)tf->Get("FADCBTree");
  if( tr_fadc == NULL ){
    std::cout << ifilename.c_str() << " has Error" << std::endl; 
    return -1;
  }
  
  UShort_t FADCData[8192]={0};
  int status = tr_fadc->SetBranchAddress("FADCData",FADCData);  

  ULong64_t           evtn;  
  TFile* otf = new TFile(ofilename.c_str(),"recreate");
  TTree* otr[4];
  FADC* wave = new FADC();
  Int_t BinEventNumber = 0; 
  for( int i = 0; i< 4; i++){
    otr[i] = new TTree(Form("Tree_%d",i),"");
    otr[i]->Branch("BinEventNumber",&BinEventNumber,"BinEventNumber/I");
    otr[i]->Branch("wave",&wave,16000,99);
  }
  
  TH2D* hisWave[4];
  TH2D* hisTimeH[4];
  TH2D* hisSumPart[4]; 
  TH1D* hisChannel = new TH1D("hisChannel","hisChannel",4,0,4);
  TH1D* hisCHData  = new TH1D("hisCHData","hisCHData",1000,0,1000);
  TH1D* hisPeak[4];
  for( int i = 0; i< 4; i++){
    hisWave[i]    = new TH2D(Form("hisWave_%d",i),
			     Form("hisWave_%d",i),
			     256,0,512,4000,0,4000);
    hisTimeH[i]   = new TH2D(Form("hisTimeH_%d",i),
			     Form("hisTimeH_%d",i),
			     256,0,512,400,0,4000);
    hisSumPart[i] = new TH2D(Form("hisSumPart_%d",i),
			     Form("hisSumPart_%d;sum;part",i),
			     1000,0,5000,1000,0,5000);
    hisPeak[i]    = new TH1D(Form("hisPeak_%d",i),
			     Form("hisPeak_%d",i),
			     400,0,4000); 
  }
  
  int rtn;
  int off = 0;
  unsigned int toff = 0;
  unsigned int ddata_length;
  unsigned int itmp;
  evtn = 0;
  Int_t ich = -1;
  long int Position = 0;
  int      Offset = 0;
  bool     bOffset = false;
  ULong64_t Entries = 0; 
  Int_t     NRetry = 0;
  Int_t    ConversionError=0;

  bool GoodData = true;
  Int_t nSkipped   = 0;
  Int_t nSkippedCH = 0; 
  for( int ievt = 0; ievt < tr_fadc->GetEntries(); ievt++){
    tr_fadc->GetEntry(ievt);
    off = 0;    
    BinEventNumber++;
    while( off < 8192 && GoodData ){
      wave->Clear();
      wave->Convert(FADCData, off );
      wave->evtn = evtn;
      off += 256;
      evtn++;
      if( wave->rid != currn ){
	//std::cout<< currn << "\t" << wave->rid << std::endl;
	nSkipped++;
	continue;
      }
      
      hisChannel->Fill( wave->cid );
      
      if( wave->cid < 1 || wave->cid > 4){
	nSkippedCH++; 
	continue;
      }
      otr[wave->cid - 1]->Fill();
      
      for( int ip = 0; ip< wave->nADC; ip++){
	hisWave[ wave->cid-1]->Fill(ip*2, wave->ADC[ip]);
      }
      hisTimeH  [ wave->cid-1]->Fill( wave->ADCTime, wave->ADCPeak );
      hisSumPart[ wave->cid-1]->Fill( wave->ADCSum , wave->ADCPart );
      hisPeak   [ wave->cid-1]->Fill( wave->ADCPeak);
      hisCHData->Fill( BinEventNumber );
      //if( evtn % 100000 == 0 ){ otr[wave->cid-1]->AutoSave("SaveSelf Overwrite");}
    }
  }
  //std::cout<< "nSkipped  : " << nSkipped << std::endl;
  //std::cout<< "nSkippedCH: " << nSkippedCH << std::endl;
  ULong64_t Estimated = tr_fadc->GetEntries()*8192/256;
  ULong64_t Recorded  = 0;
  ULong64_t Aborted   = nSkipped + nSkippedCH;
  for( int i = 0; i< 4; i++){
    Recorded += otr[i]->GetEntries();
    otr[i]->Write();
    hisWave[i]->Write();
    hisTimeH[i]->Write();
    hisSumPart[i]->Write();
    hisPeak[i]->Write(); 
  }
  hisCHData->Write();
  hisChannel->Write();
  if( !GoodData ){
    std::cout<< ifilename << " is bad Data. " << std::endl;
  }
  
  otf->Close();

  if( Estimated == Recorded + Aborted ){
    std::cout<< ifilename << " was succesfully converted.\n"
	     << "Total  : " << Estimated << "\n"
	     << "Record : " << Recorded  << "\n"
	     << "Aborted: " << Aborted   << "\n";
  }else{
    std::cout<< ifilename << " had conversion trouble.\n"
	     << "Total  : " << Estimated << "\n"
	     << "Record : " << Recorded  << "\n"
	     << "Aborted: " << Aborted   << "\n";    
  }
}
