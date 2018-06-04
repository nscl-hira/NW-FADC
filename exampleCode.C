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
    NW[iB]->SetBranchAddress("fGeoMean", event[iB].fGeoMean);
    NW[iB]->SetBranchAddress("fFastGeoMean", event[iB].fFastGeoMean);
  }
}

void PrintGeoMean(bool iB, Int_t bar)
{
  TH1F *geoMean = new TH1F("geoMean", Form("Geometric Mean Bar %d", bar),
			   500, -100, 6000);
  for(ULong64_t evt = 0; evt < NW[iB]->GetEntries(); evt++)
  {
    if( evt % (NW[iB]->GetEntries()/250) == 0)
      std::cout << Form("\r%2.1f%%", ((double)evt/NW[iB]->GetEntries()*100.))
		<< std::flush;

    NW[iB]->GetEntry(evt);
    for(Int_t i = 0; i < event[iB].fmulti; i++)
      if(event[iB].fBarNum[i] == bar)
	geoMean->Fill(event[iB].fGeoMean[i]);
  }

  TCanvas *canv = new TCanvas("c1", "GeoMean", 900,700);
  geoMean->Draw("hist");
}

//Print out the nth waveform
void PrintWave(bool iB, ULong64_t evt)
{
  evt = (evt < NW[iB]->GetEntries()) ? evt : NW[iB]->GetEntries() - 1;

  NW[iB]->GetEntry(evt);
  while(event[iB].fmulti < 1)
    NW[iB]->GetEntry(++evt);
  
  TH1F *wave[4];
  wave[0] = new TH1F("wave0", Form("Wall %c Bar %d Right", iB ? 'A' : 'B', event[iB].fBarNum[0]), 240,0,240);
  wave[1] = new TH1F("wave0", Form("Wall %c Bar %d Right Sub", iB ? 'A' : 'B', event[iB].fBarNum[0]), 240,0,240);
  wave[2] = new TH1F("wave0", Form("Wall %c Bar %d Left", iB ? 'A' : 'B', event[iB].fBarNum[0]), 240,0,240);
  wave[3] = new TH1F("wave1", Form("Wall %c Bar %d Left Sub", iB ? 'A' : 'B', event[iB].fBarNum[0]), 240,0,240);
  SignalProcessor sigR, sigL;
  sigR.SetWave(event[iB].ADCRight[0], 240);
  sigL.SetWave(event[iB].ADCLeft[0], 240);
  for(int ip = 0; ip < 240; ip++)
  {
    wave[0]->Fill(ip, event[iB].ADCRight[0][ip]);
    wave[1]->Fill(ip, event[iB].ADCRight[0][ip] - sigR.GetPedistal());
    wave[2]->Fill(ip, event[iB].ADCLeft[0][ip]);
    wave[3]->Fill(ip, event[iB].ADCLeft[0][ip] - sigL.GetPedistal());
  }

  TCanvas *canv = new TCanvas("c1", "Waveforms", 900, 700);
  canv->Divide(2,2);
  canv->cd(1);
  wave[0]->Draw("hist");
  canv->cd(3);
  wave[1]->Draw("hist");

  canv->cd(2);
  wave[2]->Draw("hist");
  canv->cd(4);
  wave[3]->Draw("hist");
  
}


//Show PSD plot
void PSD(bool iB, Int_t bar)
{
  TH2D *psdHist = new TH2D(Form("psd%d", bar+1), Form("PSD Bar %d",bar+1),
			   1000,0,5000,
			   1000,0,5000);

  for(ULong64_t evt = 0; evt < NW[iB]->GetEntries(); evt++)
  {
    if( evt % (NW[iB]->GetEntries()/250) == 0)
      std::cout << Form("\r%2.1f%%", ((double)evt/NW[iB]->GetEntries()*100.))
		<< std::flush;


    NW[iB]->GetEntry(evt);
    //Loop through each multilpicity and look for the right bar
    for(int i = 0; i < event[iB].fmulti; i++)
    {
      if(event[iB].fBarNum[i] != bar)
	continue;

      //This is the bar we are looking for
      sigP.SetWave(event[iB].ADCRight[i], 240);
      Double_t fastGateR = sigP.GetFastQDC();
      Double_t longGateR = sigP.GetQDC();

      sigP.SetWave(event[iB].ADCLeft[i], 240);
      Double_t fastGateL = sigP.GetFastQDC();
      Double_t longGateL = sigP.GetQDC();

      psdHist->Fill(TMath::Sqrt(longGateR * longGateL),
		    TMath::Sqrt(fastGateR * fastGateL));

      //psdHist->Fill(event[iB].fGeoMean[i],
      //	    event[iB].fFastGeoMean[i]);
    }
  }
  //Draw the filled histogram
  psdHist->Draw("colz");
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
  
