#include "NWEvent.h"

ClassImp(NWEvent)

NWEvent::NWEvent()
{
  ADCLeft = new Short_t*[NBAR];
  ADCRight = new Short_t*[NBAR];
  for(int i =0; i < NBAR; i++)
  {
    ADCLeft[i] = new Short_t[NSMPL];
    ADCRight[i] = new Short_t[NSMPL];
  }

  fBarNum      = new Int_t[NBAR];
  fLeft        = new Double_t[NBAR];
  fRight       = new Double_t[NBAR];
  fFastLeft    = new Double_t[NBAR];
  fFastRight   = new Double_t[NBAR];
  fTimeLeft    = new Double_t[NBAR];
  fTimeRight   = new Double_t[NBAR];
  fGeoMean     = new Double_t[NBAR];
  fFastGeoMean = new Double_t[NBAR];
  
}

NWEvent::~NWEvent()
{
  for(int i = 0; i < NBAR; i++)
  {
    delete [] ADCLeft[i];
    delete [] ADCRight[i];
  }

  delete [] fBarNum;
  delete [] fLeft;
  delete [] fRight;
  delete [] fFastLeft;
  delete [] fFastRight;
  delete [] fTimeLeft;
  delete [] fTimeRight;
  delete [] fGeoMean;
  delete [] fFastGeoMean;
}

void NWEvent::reset()
{
  fmult = 0;
  fTimestamp = 0;
  for(int i =0; i < NBAR; i++)
  {
    for(int j = 0; j < NSMPL; j++)
    {
      ADCLeft[i][j]  = 0;
      ADCRight[i][j] = 0;
    }
    
    fBarNum[i] = 0;
    fLeft[i] = 0;
    fRight[i] = 0;
    fFastLeft[i] = 0;
    fFastRight[i] = 0;
    fTimeLeft[i] = 0;
    fTimeRight[i] = 0;
    fGeoMean[i] = 0;
    fFastGeoMean[i] = 0;
  }
  
}

