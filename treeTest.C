//TreeTest

//Lets try to fix this bug

#include "include/NWEvent.h"

void writeTree()
{
  NWEvent *ev = new NWEvent();
  NWEvent *ev2 = new NWEvent();

  TFile *file = new TFile("test.root", "recreate");
  TTree *NWTree[2];
  NWTree[0] = new TTree("NWA", "NWA");
  NWTree[1] = new TTree("NWB", "NWB");

  NWEvent event[2];
  Short_t ADCRight[2][24][240];
  Int_t fmulti[2];

  //Create tree structure
  for(int iB = 0; iB < 2; iB++)
  {
    NWTree[iB]->Branch("fmulti", &event[iB].fmulti, "fmulti/I");
    NWTree[iB]->Branch("ADCRight", event[iB].ADCRight, "ADCRight[fmulti][240]/S");
  }
  
  for(int ievt = 0; ievt < 10; ievt++)
  for(int iB = 0; iB < 2; iB++)
  {
    event[iB].fmulti = 0;
    for(int smpl =0; smpl < 240; smpl++)
    {
      event[iB].ADCRight[event[iB].fmulti][smpl] =
	iB == 0 ? smpl : 240-smpl;
    }
    event[iB].fmulti++;
    NWTree[iB]->Fill();
  }
  
  NWTree[0]->Write();
  NWTree[1]->Write();
  file->Close();
  
}

void readTree()
{
  TFile *file = new TFile("test.root");
  TTree *NW[2];
  NW[0] = (TTree*)file->Get("NWA");
  NW[1] = (TTree*)file->Get("NWB");

  Int_t fmulti[2];
  Short_t ADCRight[2][24][240];
  for(int iB = 0; iB < 2; iB++)
  {
    NW[iB]->SetBranchAddress("fmulti", &fmulti[iB]);
    NW[iB]->SetBranchAddress("ADCRight", &ADCRight[iB]);
  }
  
  NW[0]->GetEntry(0);
  NW[1]->GetEntry(0);
  
  std::cout << Form("fmulti: %d %d", fmulti[0], fmulti[1] ) << std::endl;
  
  TH1F *hist = new TH1F("testHist", "test", 240, 0, 240);
  TH1F *hist2 = new TH1F("testHist2", "test", 240, 0, 240);

  for(int i = 0; i < 240; i++)
  {
    
    hist->Fill(i, ADCRight[0][fmulti[0]-1][i]);
    hist2->Fill(i, ADCRight[1][fmulti[1]-1][i]);
  }


  hist->Draw("hist");
  hist2->Draw("hist");    
}
  
