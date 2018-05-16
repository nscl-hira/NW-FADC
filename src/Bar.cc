#include "Bar.h"


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
  
  
