//Script to generate a plot of partial vs total sum

//Run just after an event it loaded

//histograms to hold
//Adam/Korean PSD/timing L/R
TH2D *hist[2][2][2];
TH1D *form;

void PSD(int layerNum, int barNum)
{
 
  for(Int_t iLR = 0; iLR < 2; iLR++) {

      int nDiff = 0;
      int nDiff2 = 0;
      Int_t nFailedTimes = 0;
      hist[0][0][iLR] = new TH2D(Form("hist%d%d%d",0,0,iLR),Form("Korean%d%s-%s",barNum,
							 layerNum == 0 ? "A" : "B",
							 iLR == 0 ? "R" : "L"),
				 1000,0,5000,
				 1000,0,5000);
      
      hist[1][0][iLR] = new TH2D(Form("hist%d%d%d",1,0,iLR),Form("Adam%d%s-%s",barNum,
							   layerNum == 0 ? "A" : "B",
							   iLR == 0 ? "R" : "L"),
				  1000,0,5000,
				  1000,0,5000);

      hist[0][1][iLR] = new TH2D(Form("hist%d%d%d",0,1,iLR),Form("KoreanTiming%d%s-%s",barNum,
							 layerNum == 0 ? "A" : "B",
							 iLR == 0 ? "R" : "L"),
				 240,0,240,
				 1000,0,5000);
      hist[1][1][iLR] = new TH2D(Form("hist%d%d%d",1,1,iLR),Form("AdamTiming%d%s-%s",barNum,
							 layerNum == 0 ? "A" : "B",
							 iLR == 0 ? "R" : "L"),
				 240,0,240,
				 1000,0,5000);

      
      TTreeReader reader(fBarTree[layerNum][barNum][iLR]);
    
      TTreeReaderValue<FADC> wave(reader, "wave");
      //For each hit, find 
      while(reader.Next())
      {
	  //Constants used in finding QDC values and timing resolution
	  const UChar_t cPedSize = 20;
	  const UChar_t cStartOff = 10;
	  const UChar_t cFastLength = 20;
	  const UChar_t cLongLength = 70;
	  const Double_t dFraction = 0.5;


          //Find the pedistal, peak, and trigger point (50% CFD)
	  Double_t dPed = 0;
	  Int_t iPeak = 0;
	  UChar_t cPeakPos = 0;
	  Double_t dStartTime = 0;
	  UChar_t cFractPos = 0;
	  
	  //find the pedistal
	  for(int ip = 0; ip < cPedSize; ip++)
	      dPed += wave->ADC[ip];
	  dPed /= cPedSize;

	  //Find and validate peak
	  for(int ip = 0; ip < wave->nADC; ip++)
	       if(iPeak <= wave->ADC[ip]-dPed)
	       {
		   iPeak =  wave->ADC[ip]-dPed;
		   cPeakPos = ip;
	       }
	  //Need to validate?
	  if(cPeakPos < cPedSize || cPeakPos >= wave->nADC - 50) //Used 50 because that's what the Koreans used
	      nFailedTimes++;

	  //Find the crossover
	  Double_t dPeakh = iPeak*dFraction;
	  for(int cFractPos = cPeakPos; cFractPos >= cPedSize; cFractPos--)
	      if(wave->ADC[cFractPos] <= dPeakh)
		  break;
	  //cFractPos is one below the half height
	  //Check if interpolation is needed
	  if(wave->ADC[cFractPos] == dPeakh)
	  {
	      dStartTime = cFractPos;
	  }
	  else
	  {
	      dStartTime = cFractPos + (0.5*iPeak - wave->ADC[cFractPos])/
		  (wave->ADC[cFractPos]-wave->ADC[cFractPos+1]);
	  }

	  cout << dStartTime << endl;
	  Double_t dFastGate = 0;
	  Double_t dLongGate = 0;


	  //Set the gate positions
	  UChar_t trig_point = (int) (wave->ADCTime/2);//trigger point (0-240)
	  UChar_t cStartPos = trig_point - cStartOff;
	  UChar_t cFastPos = trig_point + cFastLength;
	  UChar_t cLongPos = trig_point + cLongLength;
	  
	  //Deal with edge cases (too close to end or start
	  if(cStartPos < 0) {
	      cStartPos = 0;
	      cFastPos = cStartOff + cFastLength;
	  }
	  //TODO: Be smarter about setting the conditions here
	  //If close to the edge, still use the start time
	  //Either shorten the long gate, or expand them as a ratio
	  if(cLongPos >= wave->nADC) {
	      cStartPos = wave->nADC - cLongLength - cStartOff -1;
	      cFastPos  = wave->nADC - cLongLength + cFastLength -1;
	      cLongPos  = wave->nADC -1;
	  }

	  //Find the QDC values
	  for(int ip = cStartPos; ip < cLongPos; ip++)
	  {
	      if(ip < cFastPos)
		  dFastGate += wave->ADC[ip]-dPed;
	      dLongGate += wave->ADC[ip]-dPed;
	  }

	  //Fill the histograms
	  hist[1][0][iLR]->Fill(dLongGate, dFastGate);
	  hist[0][0][iLR]->Fill(wave->ADCSum, wave->ADCPart);
	  hist[1][1][iLR]->Fill(dStartTime, iPeak);
	  hist[0][1][iLR]->Fill(wave->ADCTime/2, wave->ADCPeak);

	  //Compare with korean values
	  if(dLongGate != wave->ADCSum)
	      nDiff++;
	  if(dFastGate != wave->ADCPart)
	  {
	      nDiff2++;
	      if(form == nullptr)
	      {
		  form = new TH1D("form", "Waveform", wave->nADC,0,wave->nADC);
		  for(int i = 0; i < wave->nADC; i++)
		      form->Fill(i,wave->ADC[i]);
	      }
	  }
      }

      cout << Form("Number different: %d\n",nDiff);
      cout << Form("Number of failed times: %d\n", nFailedTimes);
  }	  
  
}
//rlNum = 0:R :L
//layerNum = 0:A 1:B
