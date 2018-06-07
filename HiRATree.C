#include <iostream>

TTree *tree;
TTree *kTree;
TFile *iFile;
TFile *kFile;
TString path = "/mnt/analysis/e15190/HiRAEVTMapper_rootfiles";

Long64_t kTime = 0;
Long64_t hTime = 0;
ULong64_t kkTime = 0;
ULong64_t kkTime2 = 0;


Int_t fmulti;
Int_t fmultiK;

ULong64_t ievt = 0;
ULong64_t ievtK = 0;


void load(int runNum)
{
  runNum = 4046;
  int krunNum = 819;
  iFile = new TFile( Form("%s/run-%d.root", path.Data(), runNum));
  kFile = new TFile( Form("mappedData/mappedRun_%d.root", krunNum));
  if( !(iFile->IsOpen() && kFile->IsOpen()) )
  {
    std::cout << "File not opened" << std::endl;
    return;
  }

  tree =  (TTree*)iFile->Get("E15190");
  kTree = (TTree*)kFile->Get("NWB");
  ievt = 0;
  
  //Set branch addresses
  tree->SetMakeClass(1);
  tree->SetBranchAddress("Timestamp1.fTimestampKoreans", &kTime);
  tree->SetBranchAddress("Timestamp1.fTimestamp", &hTime);
  tree->SetBranchAddress("NWB.fmulti", &fmulti);
  tree->SetBranchStatus("*", false);
  tree->SetBranchStatus("Timestamp1.fTimestampKoreans", true);
  tree->SetBranchStatus("Timestamp1.fTimestamp", true);
  tree->SetBranchStatus("NWB.fmulti", true);

    
  kTree->SetBranchAddress("fTimestamp", &kkTime);
  kTree->SetBranchAddress("fTimestamp2", &kkTime2);
  kTree->SetBranchAddress("fmulti", &fmultiK);


  tree->GetEntry(0);
  kTree->GetEntry(0);
  
  std::cout << Form("H entries: %llu K entries: %llu", tree->GetEntries(), kTree->GetEntries())
	    << std::endl;
}

void next()
{
  if(ievt < tree->GetEntries() - 1)
  {
    tree->GetEntry(++ievt);
    while(fmulti < 1)
      tree->GetEntry(++ievt);
  }
  
  if(ievtK < kTree->GetEntries() - 1)
  {
    kTree->GetEntry(++ievtK);
    while(fmultiK < 1)
      kTree->GetEntry(++ievtK);
  }

  std::cout << Form("For entry %llu/%llu the timestamps are K:%llu H:%llu KK:%llu KK2:%llu", ievt, ievtK, kTime, hTime, kkTime, kkTime2)
	    << std::endl;
}
