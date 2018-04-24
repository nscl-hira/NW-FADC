FADC ****fBar;
TFile **fFile;
TTree ***fTree;
TTree ****fBarTree;
vector<vector<pair<Int_t, Int_t>>> *fEvent;
Long64_t fCurrentEvent;
Long64_t fLastEvent;
Int_t counter[112] = {0};

Bool_t isValid[112] = {0};

Int_t channelBarMap[112] = {
  //NWA R
  24, 1, 2, 3, 4, 5, 6, 7,
  8 , 9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,
  //NWA L
  74,51,52,53,54,55,56,57,
  58,59,60,61,62,63,64,65,
  66,67,68,69,70,71,72,73,

  -1,-1,-1,-1, 0, 0, 0, 0,

  //NWB R
  49,26,27,28,29,30,31,32,
  33,34,35,36,37,38,39,40,
  41,42,43,44,45,46,47,48,

  //NWB L
  99,76,77,78,79,80,81,82,
  83,84,85,86,87,88,89,90,
  91,92,93,94,95,96,97,98,

  -1,-1,-1,-1,-1,-1,-1,-1};

//NW barID
Int_t barIDMap[112] = {
  24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
  8 ,9 ,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,
  24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
  8 ,9 ,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,
  -1,-1,-1,-1, 0, 0, 0, 0,
  24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
  8 ,9 ,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,
  24,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
  8 ,9 ,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,
  -1,-1,-1,-1,-1,-1,-1,-1};

/// NWABMap
Int_t NWABMap[112] = {
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  0,0,0,0,1,1,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  0,0,0,0,0,0,0,0};
/// RL position ID
Int_t NWRL[112] = {
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,      
  0,0,0,0,2,1,2,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,      
  0,0,0,0,0,0,0,0};

//Special Channel Map
// 0 : FA timing or signal
Int_t SPCHMap[112] = {
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  0 ,1 ,2 ,3 ,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,
  8 ,9 ,10,11,12,13,14,15};

void load(Int_t runNo);
void analyze();

//void event(Long64_t eventid = -1);
void event();

void environment() {
  fCurrentEvent = 0;
  fLastEvent = -1;
  fEvent = new vector<vector<pair<Int_t, Int_t>>>;

  fBar = new FADC***[2];
  fBarTree = new TTree***[2];
  for (Int_t iLayer = 0; iLayer < 2; iLayer++) {
    fBar[iLayer] = new FADC**[25];
    fBarTree[iLayer] = new TTree**[25];
    for (Int_t iBar = 0; iBar < 25; iBar++) {
      fBar[iLayer][iBar] = new FADC*[2];
      fBarTree[iLayer][iBar] = new TTree*[2];
      for (Int_t iRL = 0; iRL < 2; iRL++) {
        fBar[iLayer][iBar][iRL] = new FADC();
        fBarTree[iLayer][iBar][iRL] = NULL;
      }
    }
  }

  cout << endl;
  cout << " == To load the data, type 'load(RUN NUMBER)'" << endl;
  cout << "    You must have unpacked data of the specified run in unpackedData folder." << endl;
  cout << endl;
}

void load(Int_t runNo) {
  fFile = new TFile*[28];
  fTree = new TTree**[28];
  for (Int_t iFile = 0; iFile < 28; iFile++) {
    fFile[iFile] = new TFile(Form("unpackedData/RUN_%d_fadc_%d_conv.root", runNo, iFile + 1));
    fTree[iFile] = new TTree*[4];
    for (Int_t iTree = 0; iTree < 4; iTree++)
      fTree[iFile][iTree] = (TTree *) fFile[iFile] -> Get(Form("Tree_%d", iTree));
  }

  //Set current event to the lowest non-zero number;
  fCurrentEvent = 0;
  for (Int_t iCh = 0; iCh < 112; iCh++) {
    Int_t fileNum = iCh/4;
    Int_t treeNum = iCh%4;

    Int_t barNum = barIDMap[iCh];
    Int_t rlNum = NWRL[iCh] - 1; // 0: R, 1:L
    Int_t layerNum = NWABMap[iCh] - 1; // 0: A, 1: B

//    cout << fileNum << " " << treeNum << " " << barNum << " " << rlNum << " " << layerNum << endl;

    if (barNum == -1 || rlNum == -1 || layerNum == -1)
      continue;

    fTree[fileNum][treeNum] -> SetBranchAddress("wave", &fBar[layerNum][barNum][rlNum]);
    fBarTree[layerNum][barNum][rlNum] = fTree[fileNum][treeNum];

    //set current event
    fBarTree[layerNum][barNum][rlNum]->GetEntry(0);
    if(fBar[layerNum][barNum][rlNum]-> tnum > 0)
    {
	if(fCurrentEvent == 0)
	    fCurrentEvent = fBar[layerNum][barNum][rlNum]-> tnum;
	else
	    fCurrentEvent = fBar[layerNum][barNum][rlNum]->tnum < fCurrentEvent ? fBar[layerNum][barNum][rlNum]->tnum : fCurrentEvent;
    }
    for(int i = 0; i < 10; i++) {

      fBarTree[layerNum][barNum][rlNum]->GetEntry(i);
      cout << fBar[layerNum][barNum][rlNum]->tnum << endl;
    }
    cout << endl << endl;
    isValid[iCh] = kTRUE;
  }
  cout << "fCurrentEvent: " << fCurrentEvent << endl;

  cout << endl;
  cout << " == RUN " << runNo << " data is successfully loaded." << endl;
  cout << endl;
  cout << " == You can access each bar using fBar[layer(A=0,B=1)][bar(0~24)][R=0,L=1] array." << endl;
  cout << " == For example, NW layer B, Bar 11, Left signal is fBar[1][11][1]." << endl;
  cout << " == Make sure to access it after fBarTree[1][11][1] -> GetEntry(entryNum)." << endl;
  cout << endl;
  cout << " == Example Code:" << endl;
  cout << "  fBarTree[1][11][1] -> GetEntry(0);" << endl;
  cout << "  TH1D *hist = new TH1D(\"hist\", \"\", 240, 0, 240);" << endl;
  cout << "  for (Int_t i = 0; i < 240; i++) {" << endl;
  cout << "    hist -> Fill(i, fBar[1][11][1] -> ADC[i]);" << endl;
  cout << "  }" << endl;
  cout << "  hist -> Sumw2(kFALSE);" << endl;
  cout << "  hist -> Draw();" << endl;
  cout << endl;
  cout << " == Loop over all the bars:" << endl;
  cout << "  for (Int_t iLayer = 0; iLayer < 2; iLayer++) {" << endl;
  cout << "    for (Int_t iBar = 0; iBar < 25; iBar++) {" << endl;
  cout << "      for (Int_t iRL = 0; iRL < 2; iRL++) {" << endl;
  cout << "        // Your code here" << endl;
  cout << endl;
  cout << "      }" << endl;
  cout << "    }" << endl;
  cout << "  }" << endl;
  cout << endl;

  analyze();
}

void analyze() {
  Long64_t largest = 0;
  for (Int_t iCh = 0; iCh < 112; iCh++) {
    if (!isValid[iCh])
      continue;

    Int_t fileNum = iCh/4;
    Int_t treeNum = iCh%4;

    Int_t barNum = barIDMap[iCh];
    Int_t rlNum = NWRL[iCh] - 1; // 0: R, 1:L
    Int_t layerNum = NWABMap[iCh] - 1; // 0: A, 1: B

    Long64_t current = fTree[fileNum][treeNum] -> GetEntries();
    largest = (largest > current ? largest : current);

//    cout << Form("Layer:%s, Bar:%2d, LR:%s: %ld", (layerNum == 0 ? "A" : "B"), barNum, (rlNum == 0 ? "R" : "L"), current) << endl;
  }

  cout << " == There are " << largest << " events in this run." << endl;
}

//void event(Long64_t eventid) {
void event() {

  //Clear fBar
  for (Int_t iLayer = 0; iLayer < 2; iLayer++)
    for (Int_t iBar = 0; iBar < 25; iBar++)
      for (Int_t iRL = 0; iRL < 2; iRL++)
        fBar[iLayer][iBar][iRL] -> Clear();

/*
  if (eventid == -1) {
    fLastEvent = eventid;
    eventid = fCurrentEvent;
    fCurrentEvent++;
  } else if (eventid > fCurrentEvent) {
    fCurrentEvent = eventid;
    fCurrentEvent++;
  }
  */

  for (Int_t iEvent = fCurrentEvent; iEvent < fCurrentEvent + 1; iEvent++) {
    vector<pair<Int_t, Int_t>> vec;

    for (Int_t iCh = 0; iCh < 112; iCh++) {
      if (!isValid[iCh])
        continue;

      Int_t barNum = barIDMap[iCh];
      Int_t rlNum = NWRL[iCh] - 1; // 0: R, 1:L
      Int_t layerNum = NWABMap[iCh] - 1; // 0: A, 1: B

      fBarTree[layerNum][barNum][rlNum] -> GetEntry(counter[iCh]);
      if (fBar[layerNum][barNum][rlNum] -> tnum == iEvent) {
	std::cout << "Adding " << barNum << " at " << iEvent << std::endl;
        pair<Int_t, Int_t> element(iCh, counter[iCh]);
        vec.push_back(element);
        counter[iCh]++;
      }
    }

    fEvent -> push_back(vec);
  }

  vector<pair<Int_t, Int_t>> vec = fEvent -> at(fCurrentEvent);
  Int_t vecSize = vec.size();
  for (Int_t iAt = 0; iAt < vecSize; iAt++) {
    pair<Int_t, Int_t> myPair = vec.at(iAt);

    Int_t barNum = barIDMap[myPair.first];
    Int_t rlNum = NWRL[myPair.first] - 1; // 0: R, 1:L
    Int_t layerNum = NWABMap[myPair.first] - 1; // 0: A, 1: B

    fBarTree[layerNum][barNum][rlNum] -> GetEntry(myPair.second);

    cout << Form("Layer:%s, Bar:%2d, LR:%s: %d", (layerNum == 0 ? "A" : "B"), barNum, (rlNum == 0 ? "R" : "L"), myPair.second) << endl;
  }

  fCurrentEvent++;
}
