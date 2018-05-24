#ifndef NWEVENT_H
#define NWEVENT_H

#include <iostream>
#include "TObject.h"
#include "TString.h"


#include "FADC.h"
#include "Bar.h"
#include "TCB.h"

class NWEvent : public TObject {

public:
  NWEvent();
  virtual ~NWEvent();

  void reset();
  void addBar(Bar *bar);
  
  Int_t fmult;
  ULong_t fTimestamp;
  
  Bar fBars[24]; //[fmult]
  
  
private:
  const static int NBAR = 24;

  ClassDef( NWEvent, 1);
};

#endif
