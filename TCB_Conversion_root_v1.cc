#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TThread.h"
#include "TH1.h"

#include "DataStruct.h"

#define PC_DRAM_SIZE    4
#define DATA_ARRAY_SIZE PC_DRAM_SIZE*1024*1024
#define CHUNK_SIZE      PC_DRAM_SIZE*1024 


int main( int argc, char** argv ){
  ////////////////////////////////////////////////////
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
  /////////////////////////////////////////////////////
  if( argc != 3 ){
    std::cout<< "Wrong Arguments: <command> <inputfile> <outputfile>" << std::endl;
    return -1; 
  }

  std::string ifilename = argv[1];
  std::string ofilename = argv[2];
  
  TFile* itf = new TFile(ifilename.c_str());
  TTree* itr = (TTree*)itf->Get("TCBBTree");
  UShort_t TCBData[8192];
  itr->SetBranchAddress("TCBData",TCBData);  
  
  TFile* otf = new TFile(ofilename.c_str(),"recreate");
  TTree* otr = new TTree("TCBTree","");
  TCB* tcb = new TCB();
  otr->Branch("tcb",&tcb,16000,99);
  tcb->Init();
  
  int       rtn; 
  int       k    = 0;
  int       ch   = 0;
  int       itmp = 0;
  ULong64_t ltmp = 0;
  ULong64_t evtn;
  Bool_t    bTime = true; 

  
  TH1D* hisHitMap = new TH1D("hisHitMap","hisHitMap",150,0,150);
  TH1D* hisRate   = new TH1D("hisRate","hisRate",7200,0,7200);// two hour 
  TH1D* hisAccept = new TH1D("hisAccept","hisAccept",1000,0,1000000);
  Bool_t bTime0 = kTRUE;


  //// Checking TCB Data //// 
  Long64_t ptrigNum =-1;
  ULong64_t nEvenTNum=0;
  ULong64_t nMissed  =0;
  ULong64_t nInverse =0;
  ULong64_t TotalTNum=itr->GetEntries()*256;
  std::cout<< "Total Event : " << itr->GetEntries() << std::endl; 
  for( int ievt = 0; ievt < itr->GetEntries(); ievt++){
    itr->GetEntry(ievt);
    int off = 0;
    while( off < 8192 ){    
      tcb->Convert( TCBData, off);
      if( bTime0 ){
	tcb->tcb_ttime0 = tcb->tcb_ttime;
	bTime0 = kFALSE;
      }
      if( tcb->tcb_trigger_number == 0 && tcb->tcb_ttime == 0 ){ break; }
      hisAccept->Fill( tcb->tcb_trigger_number );
      hisRate->Fill( ( tcb->tcb_ttime - tcb->tcb_ttime0)/1000./1000./1000. );

      if( ptrigNum == tcb->tcb_trigger_number ){ nEvenTNum++; }
      if( ptrigNum > tcb->tcb_trigger_number ){
	if( ptrigNum != -1 ){ 
	  std::cout<<"INVERSE:" <<ptrigNum << "\t"
		   << tcb->tcb_trigger_number << std::endl;
	  nInverse++;
	}
      }
      if( ptrigNum + 1 != tcb->tcb_trigger_number ){ nMissed  += tcb->tcb_trigger_number - ptrigNum; }
      ptrigNum = tcb->tcb_trigger_number;
      if( tcb->tcb_trigger_number %1000000 == 0 ){ std::cout<< tcb->tcb_trigger_number << "/"<< TotalTNum << std::endl;}
      evtn++;
      otr->Fill();
      off += 32;
    }
  }
  std::cout<<"Even Trigger Number : " <<  nEvenTNum  << "\n"
	   <<"Absent trig. Number : " <<  nMissed    << "\n"
	   <<"Invse. trig. Number : " <<  nInverse   << "\n"
	   <<"Total. trig. Number : " <<  ptrigNum   << "\n"
	   <<"Bad Data Ratio      : " <<  (double)(nMissed + nEvenTNum)/ptrigNum
	   << std::endl;

  
  hisAccept->Write();
  hisHitMap->Write();
  hisRate->Write();
  otr->Write();
  otf->Close();
}

  
