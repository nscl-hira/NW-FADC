//Class to handle the analysis of waveforms of type T
//T is expected to be an unsigned int-like value


//All of these functions assume that the data at T* remains constant
//unless the user tells the class using the function ArrayChange();
#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include "TObject.h"

class SignalProcessor : public TObject
{
  
public:
  SignalProcessor();
  ~SignalProcessor();

  void SetWave(Short_t* wave, UInt_t length);
  void ChangeWave();

  //These fucntions checks _flags and if the waveform has been updated
  //they redo the calculation, otherwise they just return the stored value
  Double_t GetPedistal();
  Double_t GetFastQDC();
  Double_t GetQDC();
  Double_t GetTime();

  void SetFastLength(Int_t len)      { fastLen = len;  _flags &= ~_fQDC; }
  void SetLongLength(Int_t len)      { longLen = len;  _flags &= ~_fQDC; }
  void SetPedLength(Int_t len)       { pedLen  = len;  _flags  = 0; }
  void SetGateOffset(Int_t off)      { gateOff = off;  _flags &= ~_fQDC; }
  void SetCFDFraction(Double_t frac) { cfdFrac = frac; _flags &= ~_fTime; }

  Int_t GetFastLength()     { return fastLen; }
  Int_t GetLongLength()     { return longLen; }
  Int_t GetPedLength()      { return pedLen;  }
  Int_t GetGateOffset()     { return gateOff; }
  Double_t GetCFDFraction() { return cfdFrac; }
  
private:

  /** Functions to actually do the signal processing **/
  void doPed();  //Updates waveSub and sets pedVal
  void doQDC();  //Updates both longVal and fastVal
  void doCFD();  //Updates timeVal
  void doPeak(); //Find the peak value and store the location of the peak

  
  /** Variables to hold what is currently being processed **/
  Short_t *wave;
  UInt_t waveLen;
  
  /** Variables to controll the behavior of the analysis **/
  Int_t fastLen;        //The length of the fast QDC gate
  Int_t longLen;        //The length of the long QDC gate. -1 => Go to end of waveform
  Int_t pedLen;         //Number of samples to average for the pedistal  
  Int_t gateOff;        //Number of samples to take before the start time
  Double_t cfdFrac;     //Faction to use for CFD
    
  /** Variable to hold the state of the analysis **/
  // These bits are set to 1 when they are up-to-date
  // Flags can be turned on like  _flags |= _fPed
  // Flags can be turned off like _flags &= ~_fPed
  // Flags can be checked like    _flags & _fPed
  // A bit can be flipped by _flags ^= _fPed
  UShort_t _flags;
  const static UShort_t _fPed  = 1 << 0;
  const static UShort_t _fQDC  = 1 << 1;
  const static UShort_t _fTime = 1 << 2;
  const static UShort_t _fPeak = 1 << 3;
  const static UShort_t _fFail = 1 << 4;
  

  /** Variables to hold the last calculated value **/
  Double_t pedVal;
  Double_t fastVal;
  Double_t longVal;
  Double_t timeVal;
  Double_t peakVal;
  UInt_t   peakPos;
  ClassDef( SignalProcessor, 1);
};

#endif

//TODO: Write class implimentation
