#include "FADC.h"
#include "TCB.h"
#include "Bar.h"


#include <iomanip>
ClassImp( FADC )

FADC::FADC() :
data_length(0),ttype(0),tnum(0),lnum(0),ttime(0),ltime(0),t0(0),
  rid(0),mid(0),cid(0),tpattern(0),ped(0),nADC(0),nTDC(0)
{
  for( int i = 0; i< 240; i++){
    ADC[i] = 0;
  }
  for( int i = 0; i< 60; i++){
    TDC[i] = 0;
  }
}
FADC::~FADC(){
  ;
}
void FADC::Clear(){  

  data_length=0;
  ttype    = 0;
  rid      = 0;
  mid      = 0;
  cid      = 0; 
  tnum     = 0;
  lnum     = 0;
  tpattern = 0;
  ttime    = 0;
  ltime    = 0;
  ped             =0;
  nADC            =0;
  nTDC            =0;
  for( int i = 0; i< 240; i++){
    ADC[i]=0;
  }
  for( int i = 0; i< 60; i++){
    TDC[i]=0;
  }
  ADCSum=0;
  ADCPeak=0;
  ADCPart=0;//partial ADC sum//
  ADCTime=0;
  ADCPed=0;
}

void FADC::Convert(UShort_t* data, int off){
  ////////////////////////////////////////
  data_length  = data[0+off];
  data_length += (UInt_t)(data[1+off] << 16);
  rid          = (UShort_t)data[2+off];
  ttype        = (UShort_t)(data[3+off] & 0x00FF);

  tnum  = ((UInt_t)(data[3+off] & 0xFF00) >> 8);
  tnum += ((UInt_t)(data[4+off]) << 8 );
  tnum += ((UInt_t)(data[5+off] & 0x00FF) << 24);

  ttime  = ((ULong64_t)(data[5+off] & 0xFF00 ) >> 8 )*8;
  ttime += ((ULong64_t)(data[6+off]))*1000;
  ttime += (((ULong64_t)(data[7+off] & 0x00FF)) << 16)*1000;
  mid    = ((UShort_t)(data[7+off] & 0xFF00 ) >> 8);      
  ////////////////////////////////////////
  cid    = (UShort_t)(data[8+off] & 0x00FF);

  lnum = ((UInt_t)(data[8+off] & 0xFF00)) >> 8;
  lnum+= ((UInt_t)(data[9+off] << 8 ));
  lnum+= ((UInt_t)(data[10+off] & 0x00FF )) << 24;

  tpattern = (UShort_t)(data[10+off] & 0xFF00) >> 8;
  tpattern+= (UShort_t)(data[11+off] & 0x00FF) << 8;

  ped = (UShort_t)(data[11+off] & 0xFF00 ) >> 8;
  ped +=(UShort_t)(data[12+off] & 0x00FF ) << 8;

  ltime = (((ULong64_t)(data[12+off] & 0xFF00 )) >> 8 )*8;
  ltime+= ((ULong64_t)(data[13+off]))*1000;
  ltime+= ((ULong64_t)(data[14+off]) << 16)*1000;
  ltime+= ((ULong64_t)(data[15+off]) << 32)*1000;

  //nADC = (data_length -32)/2;
  //nTDC = nADC/4;
  nADC = 240;
  nTDC = 60; 
  ////////////////////////////////////////
  for( int ip = 0; ip < nADC; ip++){
    UShort_t iadc = (UShort_t)(data[16+ip+off]&0x0FFF);// 12bit
    ADC[ip] = 4096 - iadc;
  }
  
  for( int ip = 0; ip < nTDC; ip++){
    UShort_t itdc = ((data[16+ip*4+off] & 0xF000) >> 24);
    itdc += ((data[17+ip*4+off] & 0xF000) >> 16);
    itdc += ((data[18+ip*4+off] & 0xF000) >> 8 );
    TDC[ip] = itdc;
  }
  
  ADCSum = 0.;
  ADCPeak = 0.;
  ADCPart = 0.;
  ADCPed  = 0.;
  ADCTime = 0.;

  //Find the peak
  int ADCPeakP = 0;
  for( int ip = 0; ip < nADC; ip++){
    if( ADCPeak < ADC[ip] ){
      ADCPeak = ADC[ip];
      ADCPeakP= ip;
    }
  }

  //Make sure peak isn't in the pedistal or at the end
  if( ADCPeak < 20 || ADCPeakP >= nADC - 50 ){
    ADCTime=-1;
  }
  
  /// pedestal
  for( int ip = 0; ip < 20; ip++){
    ADCPed += ADC[ip];
  }
  ADCPed = ADCPed/20.;
  ADCPeak = ADCPeak-ADCPed;
  
  /// ADCTime
  // Get's time in ns of the half height (each block is 2ns)
  for( int ip = ADCPeakP; ip >=20; ip--){
    if( ADC[ip] -ADCPed == 0.5*ADCPeak ){
      ADCTime = ip*2;
      break;
    }
    if( ADC[ip] -ADCPed < 0.5*ADCPeak ){ //DO interpolation
      ADCTime = ip*2 + 2*(0.5*ADCPeak -(ADC[ip]-ADCPed))/(ADC[ip+1]-ADC[ip]);
      break;
    }
  }
  
  //// ADCSum / ADCPart
  Int_t trig_point       = (int)(ADCTime/2);
  Int_t LowLimit         = trig_point - 10;
  Int_t HighLimitforPart = trig_point + 20;
  Int_t HighLimitforSum  = trig_point + 70;
  if( LowLimit < 0 ){
    LowLimit = 0;
    HighLimitforPart = 30;
  }
  if( HighLimitforSum >= nADC ){
    HighLimitforSum  = nADC-1; 
    HighLimitforPart = nADC-51;
    LowLimit = nADC-1-80;
  }
  for( int ip = LowLimit; ip < HighLimitforPart; ip++){
    ADCPart += ADC[ip]-ADCPed;
  }
  for( int ip = LowLimit; ip < HighLimitforSum; ip++){
    ADCSum += ADC[ip]-ADCPed;
  }
}


ClassImp( TCB )
TCB::TCB(){
  ;
}
TCB::~TCB(){
  ;
}
void TCB::Init(){
  tcb_trigger_number=0;
  tcb_trigger_type=0;
  tcb_ttime=0;
  for( int i = 0; i< 20; i++){
    tcb_trigger_pattern[i]=0;
  }
}

void TCB::Convert(UShort_t* TCBData, int off){

  tcb_trigger_number = TCBData[0+off] + (unsigned int)(TCBData[1+off] << 16);    
  tcb_trigger_type = (TCBData[2+off] & 0x00FF);
  
  tcb_ttime = ((TCBData[2+off] & 0xFF00) >> 5);
  tcb_ttime += (ULong64_t)(TCBData[3+off])*1000;
  tcb_ttime += (ULong64_t)(TCBData[4+off] << 16)*1000;
  tcb_ttime += (ULong64_t)(TCBData[5+off] << 32)*1000; 
      
  tcb_ntime = tcb_ttime % 1000000000;/// ns
  tcb_time  = tcb_ttime/1000000000;/// sec  

  for( int ch = 0; ch < 20; ch++){
    tcb_trigger_pattern[ch] = TCBData[6+ch+off];
  }  
}

ClassImp( Bar )
Bar::Bar(){
  ;
}
Bar::~Bar(){
  ;
}

void Bar::Init(){
  ID       = -1;
  ABID     = -1;
  nHit     = 0.;
  TimeMean = 0.;
  TimeDiff = 0.;
  Time[0]  = 0.;
  Time[1]  = 0.;
  Peak[0]  = 0.;
  Peak[1]  = 0.;
  Ped[0]   = 0.;
  Ped[1]   = 0.;
  Sum[0]   = 0.;
  Sum[1]   = 0.;
  Part[0]  = 0.;
  Part[1]  = 0.;
  ChID[0]  = -1;
  ChID[1]  = -1;
}
  
  
