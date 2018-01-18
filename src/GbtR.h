#ifndef GBTR
#define GBTR
#include <bitset>
#include <map>
#include <mutex>
#include <condition_variable>
#include <sys/types.h>
#include <semaphore.h>
#include "Elink.h"
#include "gbtlink.h"
class GbtGen
{
public:
   virtual bool    fetch(int port,int sample)=0;
   virtual uint8_t read(int port,int sample)=0;
};
class GbtElink : public Elink
{
public:
  GbtElink(int port,GbtGen& gbt);
  bool SerialAvailable(); 
  uint8_t GetSerial(); 
  void  lock();
  void  unlock();
private:
  GbtGen&     mGbt;
  int         mPort;
  int         mSample;
  sem_t	      mWaitSem;
};

class gbt_r: public GbtGen
{
public:
  gbt_r(gbtlink &provider);
  Elink   &get_elink(int const port);
  bool    fetch(int const port,int const sample) ;
  uint8_t read(int const port,int const sample);
private:
  bool                         mStarted;
  gbtlink                      &mDataProvider;
  std::map<int,GbtElink *>     mElinkMap;
  int                          mCurSample;
  int                          mNbSampleReaders;
  int                          mNbLinks;
  std::bitset<128>  	       mCurWord;
  bool                         mDataAvailable;
  std::mutex		       mMutex;
 
};
#endif
