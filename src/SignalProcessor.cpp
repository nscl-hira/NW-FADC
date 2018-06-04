#include "SignalProcessor.h"

ClassImp(SignalProcessor)

SignalProcessor::SignalProcessor()
{
  //Set default values for processing
  fastLen = 20;
  longLen = 70; 
  pedLen = 20;
  gateOff = 10;
  cfdFrac = 0.5;

  _flags = 0;
}

SignalProcessor::~SignalProcessor()
{
  ;
}

void SignalProcessor::SetWave(Short_t *waveIn, UInt_t length)
{
  wave = waveIn;
  waveLen = length;
  ChangeWave();
}

void SignalProcessor::ChangeWave()
{
  _flags = 0;
}

//Return the pedistal value
Double_t SignalProcessor::GetPedistal()
{
  if(!(_flags & _fPed))
  {
    doPed();
    _flags |= _fPed;
  }
  
  return pedVal;
}

//Return the Fast QDC Value
Double_t SignalProcessor::GetFastQDC()
{
  //Make sure everything is up to date
  if(!(_flags & _fPed))
    doPed();
  if(!(_flags & _fPeak))
     doPeak();
  if(!(_flags & _fTime))
    doCFD();
  _flags |= (_fPed | _fPeak | _fTime);

  if(!(_flags & _fQDC))
  {
    doQDC();
    _flags |= _fQDC;
  }
  return fastVal;
}

Double_t SignalProcessor::GetQDC()
{
  //Make sure everything is up to date
  if(!(_flags & _fPed))
    doPed();
  if(!(_flags & _fPeak))
     doPeak();
  if(!(_flags & _fTime))
    doCFD();
  _flags |= (_fPed | _fPeak | _fTime);

  if(!(_flags & _fQDC))
  {
    doQDC();
    _flags |= _fQDC;
  }
  return longVal;
}

Double_t SignalProcessor::GetTime()
{
  if(!(_flags & _fPed))
    doPed();
  if(!(_flags & _fPeak))
    doPeak();
  _flags |= (_fPed | _fPeak);
  
  if(!(_flags & _fTime))
  {
    doCFD();
    _flags |= _fTime;
  }
  return timeVal;
}


//Find and set the pedistal value
void SignalProcessor::doPed()
{
  Int_t pedTot = 0;
  Int_t sumTo = (pedLen < waveLen) ? pedLen : waveLen;
  for(int i = 0; i < sumTo; i++)
    pedTot += wave[i];

  pedVal = (Double_t)pedTot/sumTo;
}

//Find the location of the peak and the peak value
//Does no validation on the result
//Assumes that the pedistal as been properly set
void SignalProcessor::doPeak()
{
  //Loop through and look for the peak
  peakVal = 0.;
  peakPos = 0;

  for(int ip = 0; ip < waveLen; ip++)
    if(peakVal <= wave[ip] - pedVal)
    {
      peakVal = wave[ip] - pedVal;
      peakPos = ip;
    }

  if(peakPos < pedLen ||
     peakPos >= waveLen - (longLen - fastLen) ||
     peakVal < minHeight)
    _flags |= _fFail;
}


//Find crossing point of CFD and set timeVal
//Does no validation on the data
//Assumed that the pedistal, and peak are up to date
void SignalProcessor::doCFD()
{

  //Loop backward from peak to the pedistal and look for crossing
  Double_t dPeakFrac= peakVal * cfdFrac;
  for(int ip = peakPos; ip >= pedLen; ip--)
  {
    //If it happens at a sample record that sample
    if( (wave[ip] - pedVal) == peakVal)
    {
      timeVal = ip;
      break;
    }
    //If it happens between some samples, do a linear interpolation
    if( (wave[ip] - pedVal) < peakVal)
    {
      timeVal = (peakVal - (wave[ip] - pedVal) )/
	(wave[ip+1] - wave[ip]);
      timeVal += ip;
      break;
    }
    
  }//End loop to look for time pos
}

//Find the QDC values for the short and long gate
//Assumes pedistal, timing, and peak are all up to date
void SignalProcessor::doQDC()
{
  Int_t startPos = timeVal - gateOff;
  Int_t fastEnd  = timeVal + fastLen;
  Int_t longEnd  = timeVal + longLen;

  //Check edge case where the start position is negative
  if(startPos < 0)
  {
    startPos = 0;
    fastEnd = gateOff + fastLen;
    longEnd = gateOff + longLen;
  }

  //Check if the end-point is too high
  if(longEnd >= waveLen)
  {
    startPos = waveLen - longLen - gateOff - 1;
    fastEnd  = waveLen - longLen - fastLen - 1;
    longEnd  = waveLen - 1;
  }

  //Find the QDC values by integrating
  fastVal = 0;
  longVal = 0;
  for(int ip = startPos; ip < longEnd; ip++)
  {
    if( ip < fastEnd)
      fastVal += wave[ip] - pedVal;
    longVal += wave[ip] - pedVal;
  }

  if(fastVal < 0 || longVal < 0)
    _flags |= _fFail;
  
}
