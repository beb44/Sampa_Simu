#ifndef GBTELINK
#define GBTELINK
#include <cstdint>
#include <mutex>
#include "GbtRInterface.h"
#include "Elink.h"

class GbtElink : public Elink
{
public:
  GbtElink(int port,GbtRInterface& gbt);
  bool SerialAvailable(); 
  uint8_t GetSerial(); 
  void  lock();
  void  unlock();
private:
  GbtRInterface&     mGbt;
  int                mPort;
  int                mSample;
  std::mutex         mMutex;
};

#endif
