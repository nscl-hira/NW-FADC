#include "NWEvent.h"

ClassImp(NWEvent)

SignalProcessor NWEvent::sigP;

NWEvent::NWEvent()
{
//  Bars = new Bar*[NBAR];
}

NWEvent::~NWEvent()
{
//  delete [] Bars;
}

void NWEvent::Clear()
{
  fmulti = 0;
  fTimestamp = 0;
}


void NWEvent::AddBar(Int_t barNum, FADC *fadcRight, FADC *fadcLeft)
{
  bool validBar = true;
  fBarNum[fmulti] = barNum;

  //Copy over the waveforms
  for(int smpl = 0; smpl < NSMPL; smpl++)
  {
    ADCRight[fmulti][smpl] = fadcRight->ADC[smpl];
    ADCLeft[fmulti][smpl]  = fadcLeft->ADC[smpl];
  }
  sigP.SetWave(ADCRight[fmulti], NSMPL);
  fRight[fmulti]     = sigP.GetQDC();
  fFastRight[fmulti] = sigP.GetFastQDC();
  fTimeRight[fmulti] = sigP.GetTime() * 2; //Get time in nanoseconds
  validBar &= sigP.GetStatus();
  
  sigP.SetWave(ADCLeft[fmulti], NSMPL);
  fLeft[fmulti]     = sigP.GetQDC();
  fFastLeft[fmulti] = sigP.GetFastQDC();
  fTimeLeft[fmulti] = sigP.GetTime();
  validBar &= sigP.GetStatus();
  
  fGeoMean[fmulti]     = TMath::Sqrt(fLeft[fmulti]*fRight[fmulti]);
  fFastGeoMean[fmulti] = TMath::Sqrt(fFastLeft[fmulti]*fFastRight[fmulti]);

  //Make sure this bar is valid
  if( validBar &&
      fRight[fmulti]     > 0 &&
      fLeft[fmulti]      > 0 &&
      fFastRight[fmulti] > 0 &&
      fFastLeft[fmulti]  > 0 &&
      fTimeRight[fmulti] > 0 &&
      fTimeLeft[fmulti]  > 0 )
    fmulti++;
}
