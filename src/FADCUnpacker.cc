//Program to convert the binary root files into fadc_conv.root files
//Based on the code provided by the Korean group.
#include <iostream>

#include "TFile.h"
#include "TString.h"


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
  tcbOutF = new TFile( Form("%s%s", argv[2], Form(tcbFileName, runNum) ));
  if(!tcbFile->IsOpen())
  {
    std::cout << Form("Unable to open %s%s", argv[1], Form(tcbFileName, runNum))
	      << std::endl;
    return -3;
  }
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 7; j++)
    {
      fadcFile[i] = new TFile(Form("%s%s", argv[1], Form(fadcFileName, i, runNum, j+7*i + 1 ) )  );
    if(!fadcFile[i]->IsOpen())
    {
      std::cout << Form("Unable to open %s%s", argv[1], Form(fadcFileName, i, runNum, j+7*i+1 ))
		<< std::endl;
      return -4;
    }
  }

  //All of the input files are open

  //Open TCB out file
  TFile *oFile;
  oFile = new TFile(Form("%s/run-%04d", argv[2], runNum), "RECREATE");
  if(!oFile->IsOpen())
  {
    std::cout << Form("Failed to open output file: %s/run-%04d", argv[2], runNum) << std::endl;
    return -5;
  }
  
  int tcbStatus  = unpackTCB(tcbFile, oFile);
  int fadcStatus = unpackFADC(fadcFile. oFile);

  if(tcbStatus == 0 && fadcStatus == 0)
    return 0;

  std::cout << "Failed to unpack data" << std::endl;
  return -6;
}

int unpackTCB(TFile *tcbFile, TFile *oFile)
{
  if(!tcbFile->IsOpen() || !oFile->IsOpen())
    return -1;

  TTree *inTree, *oTree;

  //Load input tree
  inTree = (TTree*)tcbFile->Get("TCBTree");
  UShort_t TCBData[8192];
  inTree->SetBranchAddress("TCBData", TCBData);

  //Create output tree
  oFile->cd();
  oTree = new TTree("TCBTree","");
  TTree *tcbTree = new TTree();
  tcbTree->Branch("TCB");
}


void printUsage()
{
  std::cout << Form("Expected: FADCUnpacker dataDir/ outDir/ runNumber") << std::endl;
}
