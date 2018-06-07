//Program to convert the binary root files into fadc_conv.root files
//Based on the code provided by the Korean group.
#include <iostream>

#include "TFile.h"
#include "TString.h"


//Function definitions
void printUsage();
int  unpackTCB(TFile *inTCB, TFile *outTCB);

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
  TFile *tcbOutF;

  TFile *fadcFile[28];
  TFile *fadcOutF[28];
  
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
  tcbOutF = new TFile( Form( "%s%s", argv[2], Form(
  unpackTCB();
  unpackFADC();

  
  
  return 0;
}

void printUsage()
{
  std::cout << Form("Expected: FADCUnpacker dataDir/ outDir/ runNumber") << std::endl;
}
