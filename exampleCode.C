//Code to give an example of how to use the EventBuilder and SignalProcessor classes
//Adam Anthony 5/29/2018

//Variables
TFile *inFile;        //File that the tree is loaded from
TTree *NW[2];         //The NWA/B trees A:0 B:1
NWEvent event[2];     //Place the data is stored 
SignalProcessor sigP; //Thing to process the signals


//Load in a mapped run and set the branches to eventA and eventB
void loadRun(int runNum)
{
  //Open the file and load the trees
  inFile = new TFile(Form("mappedData/mappedRun_%d.root",runNum));
  if(!inFile->IsOpen())
  {
    std::cout << Form("Failed to open mappedData/mappedRun_%d.root",runNum) << std::endl;
    return;
  }
  NW[0] = (TTree*)inFile->Get("NWA");
  NW[1] = (TTree*)inFile->Get("NWB");

  //Set the ADCs, barNums timestamp and multiplicity
  for(int iB = 0; iB < 2; iB++)
  {
    NW[iB]->SetBranchAddress("fmulti", &event[iB].fmulti);
    NW[iB]->SetBranchAddress("fTimestamp", &event[iB].fTimestamp);
    NW[iB]->SetBranchAddress("fBarNum", event[iB].fBarNum);
    NW[iB]->SetBranchAddress("ADCRight", event[iB].ADCRight);
    NW[iB]->SetBranchAddress("ADCLeft", event[iB].ADCLeft);
  }
}

void PSD(bool iB, Int_t bar)
{
  
}


void printEvent(bool iB, ULong64_t evt)
{
  evt = evt < NW[iB]->GetEntries() ? evt : NW[iB]->GetEntries()-1;

  NW[iB]->GetEntry(evt);

  if(event[iB].fmulti < 1)
  {
    std::cout << Form("Invalid fmulti of %d", event[iB].fmulti) << std::endl;
    return;
  }

  //while(event[iB].fmulti < 2)
  // NW[iB]->GetEntry(++evt);

  TCanvas *canv = new TCanvas("c1", "c1", 900, 700);

  //Get the number of bars triggered and split the canvas
  canv->Divide(2,event[iB].fmulti);
  //Create histograms
  Int_t numADCs = 2*event[iB].fmulti;
  TH1F **hist = new TH1F*[numADCs];
  
  for(int i = 0; i < event[iB].fmulti; i++)
  {
    hist[2*i] = new TH1F(Form("hist%d", 2*i), Form("Bar %d Right", event[iB].fBarNum[i]),
			 240,0,240);
    hist[2*i + 1] = new TH1F(Form("hist%d", 2*i+1), Form("Bar %d Left", event[iB].fBarNum[i]),
			 240,0,240);
    
    for(int ip = 0; ip < 240; ip++)
    {
      hist[2*i]->Fill(ip, event[iB].ADCRight[i][ip]);
      hist[2*i+1]->Fill(ip, event[iB].ADCLeft[i][ip]);
      if(event[iB].ADCRight[i][ip] == event[iB].ADCLeft[i][ip])
	std::cout << "?" << std::endl;

    }
    canv->cd(2*(i+1));
    hist[2*i]->Draw("hist");
    canv->cd(2*i+1);
    hist[2*i+1]->Draw("hist");
  }//End loop over bars
}
  
