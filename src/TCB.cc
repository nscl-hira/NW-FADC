#include "TCB.h"

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
