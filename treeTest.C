//TreeTest

//Lets try to fix this bug

#include "include/NWEvent.h"

void treeTest()
{
  NWEvent *ev = new NWEvent();
  NWEvent *ev2 = new NWEvent();

  Short_t ADCRight[24][240];
  Int_t fmult = 0;
  
  TFile *file = new TFile("test.root", "recreate");
  TTree *tree = new TTree("test", "Test Tree");
  //tree->Branch("NWA", "NWEvent", &ev, 8000,99);
  //tree->Branch("NWB", "NWEvent", &ev2, 8000,99);
  tree->Branch("NWA.fmult", &fmult, "fmult/I");
  tree->Branch("NWA.ADCRight", ADCRight, "ADCRight[fmult][240]/S");
  
  for(int i = 0; i < 10; i++)
  {
    for(int iii =0; iii < 240; iii++)
    {
      ADCRight[0][iii] = 1;
    }
    fmult = 1;
    
    tree->Fill();
    if(i<3)
      tree->Show(i);
  }
  
  tree->Write();
  tree->Print();
  file->Close();
  
}

void readTree()
{
  TFile *file = new TFile("test.root");
  TTree *tree = (TTree*)file->Get("test");

  NWEvent ev;
  tree->SetMakeClass(1);
  tree->SetBranchAddress("NWB", &ev);

  tree->GetEntry(1);
  TH1F *hist = new TH1F("testHist", "test", 240, 0, 240);


  hist->Draw("hist");
    
}
  
