//Class to handle the analysis of waveforms of type T
//T is expected to be an unsigned int-like value


//All of these functions assume that the data at T* remains constant
//unless the user tells the class using the function ArrayChange();

template <class T>
class SignalProcessor : public TObject
{
  
public:
  SignalProcessor();
  ~SignalProcessor();

  void SetWave(T* wave, UInt_t length);
  void ChangeWave();

  //These functions call SetWave and then their agumentless counterpart
  Double_t GetPedistal(T* wave, UInt_t len);
  Double_t GetFastQDC(T* wave, UInt_t len);
  Double_t GetQDC(T* wave, UInt_t len);
  Double_t GetTime(T* wave, UInt_t len);

  //These fucntions checks _flags and if the waveform has been updated
  //they redo the calculation, otherwise they just return the stored value
  Double_t GetPedistal();
  Double_t GetFastQDC();
  Double_t GetQDC();
  Double_t GetTime();

  void SetFastLength(Int_t len)      { fastLen = len;  _flags &= ~_fFast; }
  void SetLongLength(Int_t len)      { longLen = len;  _flags &= ~_fLong; }
  void SetPedLength(Int_t len)       { pedLen  = len;  _flags &= ~_fPed;  }
  void SetGateOffset(Int_t off)      { gateOff = off;  _flags &= ~(_fFast | _fLong); }
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
  void doTime(); //Updates timeVal

  
  /** Variables to hold what is currently being processed **/
  T *wave;
  T *waveSub;
  UInt_t waveLen;
  
  /** Variables to controll the behavior of the analysis **/
  Int_t fastLen;        //The length of the fast QDC gate
  Int_t longLen;        //The length of the long QDC gate
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
  const static UShort_t _fFast = 1 << 1;
  const static UShort_t _fLong = 1 << 2;
  const static UShort_t _fTime = 1 << 3;
  

  /** Variables to hold the last calculated value **/
  Double_t pedVal;
  Double_t fastVal;
  Double_t longVal;
  Double_t timeVal;
  
  
}
