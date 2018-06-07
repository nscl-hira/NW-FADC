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
      int iTree = iFile*4 + i;
      //std::cout << Form("iTree: %d = %d*4 + %d",iTree, iFile, i) << std::endl;
      trFADC[iTree] = (TTree*)fADC[iFile]->Get(Form("Tree_%d",i));

      //Set branch addresses for tree
      fadc[iTree] = new FADC();
      if( trFADC[iTree] == nullptr)
      {
	std::cout << Form("No tree %d", iTree) << std::endl;
	continue;
      }
      trFADC[iTree]->SetBranchAddress("wave", &(fadc[iTree]));
      if(trFADC[iTree]->GetEntries() > 0)
      {
#ifdef DEBUG
	std::cout << Form("Have %d %d %d at %d", BarIDMap[iTree], NWABMap[iTree], NWRL[iTree], iTree)
		  << std::endl;
#endif
	trFADC[iTree]->GetEntry(0);
      }
#ifdef DEBUG
      else
	std::cout << Form("Missing %d %d %d at %d", BarIDMap[iTree], NWABMap[iTree], NWRL[iTree], iTree)
		  << std::endl;
#endif
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
  
  outTree[0] = new TTree("NWA", "NWA");
  outTree[1] = new TTree("NWB", "NWB");
  
  //Create output variables
  for(int iB = 0; iB < 2; iB++)
  {
    outTree[iB]->Branch("fmulti",     &event[iB].fmulti,     "fmulti/I");
    outTree[iB]->Branch("fTimestamp", &event[iB].fTimestamp, "fTimestamp/l");
    outTree[iB]->Branch("fTimestamp2", &event[iB].fTimestamp2, "fTimestamp/l");

    outTree[iB]->Branch("fBarNum", event[iB].fBarNum, "fBarNum[fmulti]/I");
      
    outTree[iB]->Branch("ADCRight", event[iB].ADCRight, "ADCRight[fmulti][240]/S");
    outTree[iB]->Branch("ADCLeft", event[iB].ADCLeft, "ADCLeft[fmulti][240]/S");

    outTree[iB]->Branch("fRight", event[iB].fRight, "fRight[fmulti]/D");
    outTree[iB]->Branch("fLeft", event[iB].fLeft, "fLeft[fmulti]/D");
    
    outTree[iB]->Branch("fFastRight", event[iB].fFastRight, "fFastRight[fmulti]/D");
    outTree[iB]->Branch("fFastLeft", event[iB].fFastLeft, "fFastLeft[fmulti]/D");
    
    outTree[iB]->Branch("fTimeRight", event[iB].fTimeRight, "fTimeRight[fmulti]/D");
    outTree[iB]->Branch("fTimeLeft", event[iB].fTimeLeft, "fTimeLeft[fmulti]/D");

    outTree[iB]->Branch("fGeoMean",     event[iB].fGeoMean,     "fGeoMean[fmulti]/D");
    outTree[iB]->Branch("fFastGeoMean", event[iB].fFastGeoMean, "fFastGeoMean[fmulti]/D");

    
  }

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

  std::cout << Form("Looking for %d events from %llu to %llu (t0 = %llu tend = %llu sec)",
		    trTCB->GetEntries(), MinEvt, MaxEvt, tcb->tcb_time, tEnd) << std::endl;

  //Variables to track what entry we're at in the trees
  ULong64_t fadcEntry[112] = {0};
  ULong64_t tcbEntry = 0;
  
  //Loop over all triggers and look for possible events
  for(auto ievt = MinEvt; ievt < MaxEvt; ievt++)
  {
    if((ievt-MinEvt) % ( (MaxEvt-MinEvt)/250 ) == 0)
      std::cout << Form("\r%2.1f%%", ((double)(ievt-MinEvt))/(MaxEvt-MinEvt)*100.)
      	<< std::flush;


    //Just truncate the ammount of data read in
//    if( (double)(ievt-MinEvt)/(MaxEvt-MinEvt)*100 > 5)
//      break;

    ULong64_t evtTime = 0;
    ULong64_t evtTime2 = 0;
    
    //Reset the tree variables and tmp variables
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 24; j++)
	for(int k = 0; k < 2; k++)
	  tempFADC[i][j][k] == nullptr;


    //Look for the next trigger in TCB
    while(tcb->tcb_trigger_number < ievt)
    {
      tcbEntry++;
      if(tcbEntry >= trTCB->GetEntries())
	break;
      trTCB->GetEntry(tcbEntry);
    }

    if(tcb->tcb_trigger_number == ievt)
    {
      //evtTime = tcb->tcb_ttime;
      //std::cout << tcb->tcb_ttime - tcb->tcb_ttime0 << std::endl;
      evtTime = tcb->tcb_ttime;
      evtTime2 = tcb->tcb_trigger_number;
    }
    
    //Loop though FADCs and look for those that triggered
    int nWaves[2][24] = {0};
    for(int ifadc = 0; ifadc < 112; ifadc++)
    {
      //If the fadc is not a bar or the FA continue
      if(BarIDMap[ifadc] <= 0 && SPCHMap[ifadc] != 0)
	continue;
      
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

    //Loop through each bar and see if there was an event
    for(int iB = 0; iB < 2; iB++)
    {
      bool foundEvent = false;
      event[iB].Clear();
      
      for(int iBar = 0; iBar < 24; iBar++)
	if(nWaves[iB][iBar] >= 2)
	{
	  event[iB].fTimestamp = evtTime;
	  event[iB].fTimestamp2 = evtTime2;
	  event[iB].AddBar(iBar, tempFADC[iB][iBar][0], tempFADC[iB][iBar][1]);
	  foundEvent = true;
	}//End loop over all bars in a wall

      //Fill the tree if this wall saw an event
      if(foundEvent)
      {
	outTree[iB]->Fill();
      }
      
    }//End loop over walls
  } //End loop over events
  
  std::cout << std::endl << Form("Finished loop over events: saving file %s", outFile->GetName()) << std::endl;
  outFile->Write();
  outFile->Close();

  //Close out of all of the other files
  fTCB->Close();
  for(int i = 0; i < 28; i++)
    fADC[i]->Close();
  validConfig = false;  
}
