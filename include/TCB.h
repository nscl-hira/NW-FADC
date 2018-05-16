#ifndef TCB__H__
#define TCB__H__
#include <iostream>
#include "TObject.h"

class TCB : public TObject {
 public:
  TCB();
  virtual ~TCB();
  void Init();
  void Convert(UShort_t* data,int off = 0);

  ULong64_t tcb_trigger_number;
  Int_t     tcb_trigger_type;
  ULong64_t tcb_ntime;
  ULong64_t tcb_time;
  ULong64_t tcb_ttime;
  ULong64_t tcb_ttime0;
  UShort_t  tcb_trigger_pattern[20];

  ClassDef( TCB, 1 )
};


#endif 
