#include "NWEvent.h"

ClassImp(NWEvent)

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
  /*for(int iBar = 0; iBar < NBAR; iBar++)
  {
    fBarNum[iBar] = 0;
    fGeoMean[iBar] = 0;
    fFastGeoMean[iBar] = 0;

    fLeft[iBar] = 0;
    fFastLeft[iBar] = 0;
    fTimeLeft[iBar] = 0;

    fRight[iBar] = 0;
    fFastRight[iBar] = 0;
    fTimeRight[iBar] = 0;

    for(int smpl = 0; smpl<NSMPL; smpl++)
    {
      ADCRight[iBar][smpl] = 0;
      ADCLeft[iBar][smpl] = 0;
    }
  }
  */
}


void NWEvent::AddBar(Int_t barNum, FADC *fadcRight, FADC *fadcLeft)
{
  fBarNum[fmulti] = barNum;

  //Copy over the waveforms
  for(int smpl = 0; smpl < NSMPL; smpl++)
  {
    ADCRight[fmulti][smpl] = fadcRight->ADC[smpl];
    ADCLeft[fmulti][smpl]  = fadcLeft->ADC[smpl];
  }

  fRight[fmulti]     = fadcRight->ADCSum;
  fFastRight[fmulti] = fadcRight->ADCPart;
  fTimeRight[fmulti] = fadcRight->ADCTime;

  fLeft[fmulti]     = fadcLeft->ADCSum;
  fFastLeft[fmulti] = fadcLeft->ADCPart;
  fTimeLeft[fmulti] = fadcLeft->ADCTime;

  fGeoMean[fmulti]     = TMath::Sqrt(fLeft[fmulti]*fRight[fmulti]);
  fFastGeoMean[fmulti] = TMath::Sqrt(fFastLeft[fmulti]*fFastRight[fmulti]);
  fmulti++;
}
