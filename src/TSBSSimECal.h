#ifndef _TSBSSIMECAL_H
#define _TSBSSIMECAL_H

#include "TSBSSimDetector.h"
#include "TSBSSimAuxi.h"

class TF1;
class TF1Convolution;
class TTree;
class TFile;

class TSBSSimECal : public TSBSSimDetector {
public:
  TSBSSimECal(const char* name, short id);
  virtual ~TSBSSimECal();
  // This loads the simulation event data
  virtual void LoadEventData(const std::vector<g4sbshitdata*> &evbuffer);
  virtual void Digitize(TSBSSimEvent &event);
  
  virtual void Clear();
  
  // Initialize
  void Init();
  
 private:
  
  TSPEModel *fSPE;
  std::vector<TPMTSignal> fSignals;
  
  ClassDef(TSBSSimECal,1)
};

#endif //_TSBSSIMECAL_H
