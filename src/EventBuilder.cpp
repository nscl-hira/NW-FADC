#include "EventBuilder.h"

ClassImp(EventBuilder)

EventBuilder::EventBuilder()
{
  ;
}
EventBuilder::~EventBuilder()
{
  ;
}

//Load in all the files and create tree to store output
//If something failes returns false
bool EventBuilder::init(Int_t runNum)
{
  this->runNum = runNum;
  std::cout << "Loading files for " << this->runNum << "..." << std::endl;

  
  //Load all of the FADC files and link the trees
  for(int iFile = 0; iFile < 28; iFile++)
  {
    fADC[iFile] = new TFile(Form("unpackedData/RUN_%d_fadc_%d_conv.root", runNum, iFile+1));
    if(!fADC[iFile]->IsOpen())
    {
      std::cout << Form("Failed to open unpackedData/RUN_%d_fadc_%d_conv.root", runNum, iFile+1)
		<< std::endl;
      return false;
    }
    for(int i = 0; i < 4; i++)
    {
      int iTree = iFile*4 + 1;
      trFADC[iTree] = (TTree*)fADC[iFile]->Get(Form("Tree_%d",i));

      //Set branch addresses for tree
      fadc[iTree] = new FADC();
      if( trFADC[iTree] == nullptr) continue;
      trFADC[iTree]->SetBranchAddress("wave", &(fadc[iTree]));
      if(trFADC[iTree]->GetEntries() > 0) trFADC[iTree]->GetEntry(0);
    } //End loop over trees in file
  } //End loop over files
  
  
  //Load the TCB file and link the tree
  fTCB = new TFile(Form("unpackedData/RUN_%d_tcb_conv.root", runNum));
  if(!fTCB->IsOpen())
  {
    std::cout << Form("Failed to open unpackedData/RUN_%d_tcb_conv.root", runNum)
	      << std::endl;
    return false;
  }
  trTCB = (TTree*)fTCB->Get("TCBTree");
  trTCB->SetBranchAddress("tcb", &tcb);
  
  std::cout << "Loaded files and set branch addresses." << std::endl;


  /*** Create output tree ***/
  outFile = new TFile(Form("mappedData/mappedRun_%d.root", runNum), "recreate");

  outTree = new TTree("E15190", "E15190");

  nwEvent[0] = new NWEvent();
  nwEvent[1] = new NWEvent();
  outTree->Branch("NWA", "NWEvent", &(nwEvent[0]), 32000, 1);
  outTree->Branch("NWB", "NWEvent", &(nwEvent[0]), 32000, 1);

  //Successfuly loaded so return true
  validConfig = true;
  return true;
}




//Take individual detectors and turn into Bars
//Take bars that happened at the same time and turn into Events
void EventBuilder::buildEvents()
{
  //make sure stuff is loaded
  if(!validConfig)
  {
    std::cout << "Invalid configuration. Did you run EventBuilder.init(runNum)?"
	      << std::endl;
    return;
  }


  //Get the start event number for this run, and the max event number
  trTCB->GetEntry(trTCB->GetEntries()-1);
  ULong64_t MaxEvt = tcb->tcb_trigger_number;
  auto tEnd = tcb->tcb_time;
  trTCB->GetEntry(0);
  ULong64_t MinEvt = tcb->tcb_trigger_number;

  std::cout << Form("Looking for events %llu to %llu (t0 = %llu tend = %llu sec)",
		    MinEvt, MaxEvt, tcb->tcb_time, tEnd) << std::endl;

  //Variables to track what entry we're at in the trees
  ULong64_t fadcEntry[112] = {0};
  ULong64_t tcbEntry = 0;
  
  //Loop over all triggers and look for possible events
  for(auto ievt = MinEvt; ievt < MaxEvt; ievt++)
  {
    if((ievt-MinEvt) % ( (MaxEvt-MinEvt)/250 ) == 0)
      std::cout << Form("%2.1f%%", ((double)(ievt-MinEvt))/(MaxEvt-MinEvt)*100.)
		<< std::endl;

    //Just truncate the ammount of data read in
    if( (double)(ievt-MinEvt)/(MaxEvt-MinEvt)*100 > 5)
      break;

    ULong64_t evtTime = 0;
    
    //Reset the tree variables and tmp variables
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 24; j++)
	for(int k = 0; k < 2; k++)
	  tempFADC[i][j][k] == nullptr;

    nwEvent[0]->fmult = 0;
    nwEvent[1]->fmult = 0;

    //Look for the next trigger in TCB
    while(tcb->tcb_trigger_number < ievt)
    {
      tcbEntry++;
      if(tcbEntry >= trTCB->GetEntries())
	break;
      trTCB->GetEntry(tcbEntry);
    }

    if(tcb->tcb_trigger_number == ievt)
      evtTime = tcb->tcb_ttime;

    //Loop though FADCs and look for those that triggered
    int nWaves[2][24] = {0};
    for(int ifadc = 0; ifadc < 112; ifadc++)
    {
      if(trFADC[ifadc] == nullptr)
	continue;

      //Advance to the next event at or after ievt
      while(fadc[ifadc]->tnum < ievt)
      {
	fadcEntry[ifadc]++;
	if(fadcEntry[ifadc] >= trFADC[ifadc]->GetEntries())
	  break;
	trFADC[ifadc]->GetEntry(fadcEntry[ifadc]);
      }
      
      //If this FADC actually triggered and it isn't a special channel
      if(fadc[ifadc]->tnum == ievt && SPCHMap[ifadc] < 0)
      {
	//std::cout << "Found a recorded thing at " << ievt << std::endl;
	//Get R/L A/B and bar number
	bool bLeft = NWRL[ifadc] - 1;
	bool bB = NWABMap[ifadc] - 1;
	Int_t barNum = BarIDMap[ifadc] - 1; //Zero index BarNum ie in [0,23]

	//Record this in a temporary array
	tempFADC[bB][barNum][bLeft] = fadc[ifadc];
	nWaves[bB][barNum]++;
      }
    }//End loop over FADCs

    //Loop over and look at temporary variables, if both ends of the bar are there
    //Then construct and store an event
    bool foundEvent = false;
    for(int iB = 0; iB < 2; iB++)
    {
      for(int iBar = 0; iBar < 24; iBar++)
	if(nWaves[iB][iBar] > 1)
	{
	  foundEvent = true;
	  //std::cout << Form("There was actually an event in Wall %c and bar %d",
	  //iB ? 'B':'A', iBar) << std::endl;

	  //There was a hit so record all of the information
	  for(int i = 0; i < 240; i++)
	  { //Save the waves
	    std::cout << nwEvent[iB]->ADCRight << Form(" %d %d %d",iB,nwEvent[iB]->fmult, i) << std::endl;
	    nwEvent[iB]->ADCRight[nwEvent[iB]->fmult][i] = tempFADC[iB][iBar][0]->ADC[i];
	    nwEvent[iB]->ADCLeft[nwEvent[iB]->fmult][i]  = tempFADC[iB][iBar][1]->ADC[i];
	  }
	  
	  nwEvent[iB]->fBarNum[nwEvent[iB]->fmult]      = iBar;
	  nwEvent[iB]->fRight[nwEvent[iB]->fmult]       = tempFADC[iB][iBar][0]->ADCSum;
	  nwEvent[iB]->fLeft[nwEvent[iB]->fmult]        = tempFADC[iB][iBar][1]->ADCSum;
	  nwEvent[iB]->fFastRight[nwEvent[iB]->fmult]   = tempFADC[iB][iBar][0]->ADCPart;
	  nwEvent[iB]->fFastLeft[nwEvent[iB]->fmult]    = tempFADC[iB][iBar][1]->ADCPart;
	  nwEvent[iB]->fTimeRight[nwEvent[iB]->fmult]   = tempFADC[iB][iBar][0]->ADCTime; //in ns 
	  nwEvent[iB]->fTimeLeft[nwEvent[iB]->fmult]    = tempFADC[iB][iBar][1]->ADCTime; //in ns
	  nwEvent[iB]->fGeoMean[nwEvent[iB]->fmult]     =
	    TMath::Sqrt(tempFADC[iB][iBar][0]->ADCSum*tempFADC[iB][iBar][1]->ADCSum);
	  nwEvent[iB]->fFastGeoMean[nwEvent[iB]->fmult] =
	    TMath::Sqrt(tempFADC[iB][iBar][0]->ADCPart*tempFADC[iB][iBar][1]->ADCPart);
	  
	  nwEvent[iB]->fTimestamp = evtTime;
	  nwEvent[iB]->fmult++;

	} //End loop over all bars
    }// End loop over wall
    if(foundEvent)
    {
      std::cout << "Filling event" << std::endl;
      outTree->Fill();
      if(outTree->GetEntries() < 3)
	outTree->Show(outTree->GetEntries()-1);
    }
  } // End loop over all events    
  
    /*for(int iBar = 0; iBar < 24; iBar++)
    {
      if(nWaves[0][iBar] > 1)
      {
	//std::cout << Form("There was actually an event in Wall %c and bar %d",
	//iB ? 'B':'A', iBar) << std::endl;
	
	//There was a hit so record all of the information
	for(int i = 0; i < 240; i++)
	{ //Save the waves
	  nwAEvent.ADCRight[nwAEvent.fmult][i] = tempFADC[0][iBar][0]->ADC[i];
	  nwAEvent.ADCLeft[nwAEvent.fmult][i]  = tempFADC[0][iBar][1]->ADC[i];
	}
	
	nwAEvent.fBarNum[nwAEvent.fmult]      = iBar;
	nwAEvent.fRight[nwAEvent.fmult]       = tempFADC[0][iBar][0]->ADCSum;
	nwAEvent.fLeft[nwAEvent.fmult]        = tempFADC[0][iBar][1]->ADCSum;
	nwAEvent.fFastRight[nwAEvent.fmult]   = tempFADC[0][iBar][0]->ADCPart;
	nwAEvent.fFastLeft[nwAEvent.fmult]    = tempFADC[0][iBar][1]->ADCPart;
	nwAEvent.fTimeRight[nwAEvent.fmult]   = tempFADC[0][iBar][0]->ADCTime; //in ns 
	nwAEvent.fTimeLeft[nwAEvent.fmult]    = tempFADC[0][iBar][1]->ADCTime; //in ns
	nwAEvent.fGeoMean[nwAEvent.fmult]     =
	  TMath::Sqrt(tempFADC[0][iBar][0]->ADCSum*tempFADC[0][iBar][1]->ADCSum);
	nwAEvent.fFastGeoMean[nwAEvent.fmult] =
	  TMath::Sqrt(tempFADC[0][iBar][0]->ADCPart*tempFADC[0][iBar][1]->ADCPart);
	
	nwAEvent.fTimestamp = evtTime;
	nwAEvent.fmult++;
      }
    } //End loop over wall A

    
  for(int iBar = 0; iBar < 24; iBar++)
  {
    if(nWaves[1][iBar] > 1)
    {
      //std::cout << Form("There was actually an event in Wall %c and bar %d",
      //iB ? 'B':'A', iBar) << std::endl;
      
      //There was a hit so record all of the information
      for(int i = 0; i < 240; i++)
      { //Save the waves
	nwBEvent.ADCRight[nwBEvent.fmult][i] = tempFADC[1][iBar][0]->ADC[i];
	nwBEvent.ADCLeft[nwBEvent.fmult][i]  = tempFADC[1][iBar][1]->ADC[i];
      }
      
      nwBEvent.fBarNum[nwBEvent.fmult]      = iBar;
      nwBEvent.fRight[nwBEvent.fmult]       = tempFADC[1][iBar][0]->ADCSum;
      nwBEvent.fLeft[nwBEvent.fmult]        = tempFADC[1][iBar][1]->ADCSum;
      nwBEvent.fFastRight[nwBEvent.fmult]   = tempFADC[1][iBar][0]->ADCPart;
      nwBEvent.fFastLeft[nwBEvent.fmult]    = tempFADC[1][iBar][1]->ADCPart;
      nwBEvent.fTimeRight[nwBEvent.fmult]   = tempFADC[1][iBar][0]->ADCTime; //in ns 
      nwBEvent.fTimeLeft[nwBEvent.fmult]    = tempFADC[1][iBar][1]->ADCTime; //in ns
      nwBEvent.fGeoMean[nwBEvent.fmult]     =
	TMath::Sqrt(tempFADC[1][iBar][0]->ADCSum*tempFADC[1][iBar][1]->ADCSum);
      nwBEvent.fFastGeoMean[nwBEvent.fmult] =
	TMath::Sqrt(tempFADC[1][iBar][0]->ADCPart*tempFADC[1][iBar][1]->ADCPart);
      
      nwBEvent.fTimestamp = evtTime;
      nwBEvent.fmult++;
    }
  } //End loop over Wall B
    
  outTree->Fill();
  if(outTree->GetEntries() < 3)
    outTree->Show(outTree->GetEntries()-1);
   
    } //End loop over events*/
  
  std::cout << "Finished loop over events: saving file" << outFile->GetName() << std::endl;
  outFile->Write();
  outFile->Close();
}
