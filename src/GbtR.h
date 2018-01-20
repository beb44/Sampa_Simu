#ifndef GBTR
#define GBTR
#include <bitset>
#include <map>
#include <mutex>
#include <condition_variable>
#include "GbtRInterface.h"
#include "Bits128.h"
#include "Elink.h"
#include "GbtElink.h"
#include "GbtLink.h"

class GbtR: public GbtRInterface
{
public:
  GbtR(gbtlink &provider);
  Elink   &GetElink(int const port);
  bool    Fetch(int const port,int const sample) ;
  uint8_t Read(int const port,int const sample);
private:
  bool                         mStarted;
  gbtlink                      &mDataProvider;
  std::map<int,GbtElink *>     mElinkMap;
  int                          mCurSample;
  int                          mNbSampleReaders;
  int                          mNbLinks;
  Bits128  	               mCurWord;
  bool                         mDataAvailable;
  std::mutex		       mMutex;
 
};
#endif
