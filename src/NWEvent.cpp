#include "NWEvent.h"

ClassImp(NWEvent)

NWEvent::NWEvent()
{
  fmult = 0;
  fTimestamp = 0;
  fBars = new Bar[NBAR];
}

NWEvent::~NWEvent()
{
  if(fBars)  delete [] fBars;
}

void NWEvent::reset()
{
  fmult = 0;
  fTimestamp = 0;
  if(fBars) delete [] fBars;
  fBars = new Bar[NBAR];
}

void NWEvent::addBar(Bar* bar)
{
  if(fmult >= NBAR)
  {
    std::cout << "Failed to add bar with multiplicity: "
	      << fmult << std::endl;
    return;
  }

  fBars[fmult] = *bar;
  fmult++;
}
