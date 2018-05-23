//TreeTest

//Lets try to fix this bug

#include "include/NWEvent.h"

void treeTest()
{
  NWEvent *ev = new NWEvent();
  NWEvent *ev2 = new NWEvent();
  ev->reset();

  TFile *file = new TFile("test.root", "recreate");
  TTree *tree = new TTree("test", "Test Tree");
  tree->Branch("NWA", "NWEvent", &ev, 8000,99);
  tree->Branch("NWB", "NWEvent", &ev2, 8000,99);

  for(int i = 0; i < 10; i++)
  {
    ev->reset();
    ev2->reset();
    ev->fLeft[0] = i;
    for(int iii =0; iii < 240; iii++)
    {
      ev->ADCLeft[0][iii] = iii;
      ev2->ADCRight[0][iii] = 1;
    }
    
    ev2->fRight[0] = i;
    ev->fmult++;
    ev->fTimestamp++;
    ev2->fmult++;
    ev2->fTimestamp++;
      
    
    tree->Fill();
    //if(i<3)
    //  tree->Show(i);
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
  for(int i = 0; i < 240; i++)
    hist->Fill(i, ev.ADCLeft[0][i]);

  hist->Draw("hist");
    
}
  
