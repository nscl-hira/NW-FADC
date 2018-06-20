//Program to convert the binary root files into fadc_conv.root files
//Based on the code provided by the Korean group.
//Adam Anthony

#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TTree.h"

#include "FADC.h"
#include "TCB.h"

//Function definitions
void printUsage();
int  unpackTCB(TFile *inTCB, TFile *outTCB);
int  unpackFADC(TFile *inFADC[28], TFile *oFile);

//Should be called like
//FADCUnpacker dataDir outDir runNumber
//The run number should be the korean's run number, not HiRAs
int main(int argc, char *argv[])
{
  //Validate input
  if(argc != 4)
  {
    printUsage();
    return -1;
  }
  int runNum = TString(argv[3]).Atoi();
  if(runNum <= 0)
  {
    printUsage();
    return -2;
  }

  //Try to open all of the input files and output files
  TFile *tcbFile;
  TFile *fadcFile[28];
  
  TString tcbFileName = "TCBRawData/RUN_%d_tcb_bin.root";
  TString fadcFileName = "Data%d/RUN_%d_fadc_%d_bin.root";

  tcbFile = new TFile( Form("%s%s", argv[1], Form(tcbFileName, runNum) ));
  if(!tcbFile->IsOpen())
  {
    std::cout << Form("Unable to open %s%s", argv[1], Form(tcbFileName, runNum))
	      << std::endl;
    return -3;
  }
  std::cout << Form("Opened %s%s", argv[1], Form(tcbFileName, runNum))
	      << std::endl;
    
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 7; j++)
    {
      fadcFile[j+7*i] = new TFile(Form("%s%s", argv[1], Form(fadcFileName, i, runNum, j+7*i + 1 ) )  );
      if(!fadcFile[j+7*i]->IsOpen())
      {
	std::cout << Form("Unable to open %s%s", argv[1], Form(fadcFileName, i, runNum, j+7*i+1 ))
		  << std::endl;
	return -4;
      }
      std::cout << Form("Opened %s%s", argv[1], Form(fadcFileName, i, runNum, j+7*i+1 ))
		  << std::endl;
      
    }//End loop over fadc files

  //Open the output file
  TFile *oFile;
  oFile = new TFile(Form("%s/run-%04d.root", argv[2], runNum), "RECREATE");
  if(!oFile->IsOpen())
  {
    std::cout << Form("Failed to open output file: %s/run-%04d", argv[2], runNum) << std::endl;
    return -5;
  }
  
  int tcbStatus  = unpackTCB(tcbFile, oFile);
//  int fadcStatus = unpackFADC(fadcFile, oFile);
  int fadcStatus = 0;

  //Close all files
  oFile->Close();

  tcbFile->Close();
  for(int i = 0; i < 28; i++)
      fadcFile[i]->Close();



  //Return
  if(tcbStatus == 0 && fadcStatus == 0)
  {
    return 0;
    std::cout << "Finished unpacking all data" << std::endl;
  }

  std::cout << "Failed to unpack data" << std::endl;
  return -6;
}

int unpackTCB(TFile *tcbFile, TFile *oFile)
{
  if(!tcbFile->IsOpen() || !oFile->IsOpen())
    return -1;

  TTree *inTree, *oTree;

  //Load input tree
  inTree = (TTree*)tcbFile->Get("TCBBTree");
  if(inTree == NULL)
  {
    std::cout << "Filed to load tree from file" << std::endl;
    tcbFile->Print();
    return -1;
  }
  
  UShort_t TCBData[8192];
  inTree->SetBranchAddress("TCBData", TCBData);

  //Create output tree and TCB to fill

  oFile->cd();
  oTree = new TTree("TCBTree","");
  TCB   *tcb = new TCB();
  oTree->Branch("TCB",&tcb,16000,99);
  tcb->Init();
  //Create debug histograms
  TH1D *histHitMap = new TH1D("hisHitMap","TDC Hit Map",150,0,150);
  TH1D *histRate   = new TH1D("hisRate","TDC Rate",150,0,150);
  TH1D *histAccept = new TH1D("hisAccept","TDC Accept",150,0,150);
  
  //Variables for unpacking and validating
  Long64_t  ptrigNum = -1;
  ULong64_t nEven    = 0;
  ULong64_t nMissed  = 0;
  ULong64_t nInverse = 0;
  ULong64_t nTrig    = inTree->GetEntries() * 256;
  ULong64_t evtn;
  Bool_t    bTime0 = true;

  std::cout << "Total TCB Events: " << inTree->GetEntries() << std::endl;

  //Loop through all events
  for(ULong64_t ievt = 0; ievt < inTree->GetEntries(); ievt++)
  {
    inTree->GetEntry(ievt);
    int off = 0;
    while(off < 8192)
    {
      tcb->Convert(TCBData, off);

      //Set the zero time
      if(bTime0) { tcb->tcb_ttime0 = tcb->tcb_ttime; bTime0 = false; }

      //Not sure why?
      if(tcb->tcb_trigger_number == 0 && tcb->tcb_ttime == 0) { break; }

      //Fill the histograms
      histAccept->Fill(tcb->tcb_trigger_number);
      histRate->Fill( (tcb->tcb_ttime - tcb->tcb_ttime0)/1000./1000./1000.);

      //Check for even, reversed, and missed triggers
      if(ptrigNum == tcb->tcb_trigger_number)
	nEven++;
      if(ptrigNum > tcb->tcb_trigger_number && ptrigNum != -1)
      {
	std::cout << "Inversed trigger: " << ptrigNum << "\t"
		  << tcb->tcb_trigger_number << std::endl;
	nInverse++;
      }
      if(ptrigNum + 1 != tcb->tcb_trigger_number && ptrigNum != -1)
	nMissed += tcb->tcb_trigger_number - ptrigNum;

      //Set previous trigger and fill tree
      ptrigNum = tcb->tcb_trigger_number;
      evtn++;
      oTree->Fill();
      off += 32;

      if( tcb->tcb_trigger_number %1000000 == 0 )
	std::cout<< tcb->tcb_trigger_number << "/"<< nTrig << std::endl;
    }//End loop over offset
  }//end loop over evt entries
  
  std::cout << "Even    trig. number: " << nEven    << std::endl
	   << "Missing trig. number: " << nMissed  << std::endl
	   << "Inverse trig. number: " << nInverse << std::endl
	   << "Total   trig. number: " << ptrigNum << std::endl
	   << "Bad data ratio      : " << (double)(nMissed + nEven)/ptrigNum
	   << std::endl;

  histAccept->Write();
  histHitMap->Write();
  histRate->Write();
  oTree->Write();

}//End unpackTDC

int unpackFADC(TFile *fadcFile[28], TFile *oFile)
{
  return 0;
}

void printUsage()
{
  std::cout << Form("Expected: FADCUnpacker dataDir/ outDir/ runNumber")
	    << "Example: FADCUnpacker /mnt/spirit/rawdata/misc/KoreanElectronics/Mar18-25 /mnt/spirit/rawdata/misc/KoreanElectronics/unpacker/unpackedData 1470"
	    << std::endl;
}
