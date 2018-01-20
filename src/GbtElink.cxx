#include "GbtElink.h"
using namespace std;

GbtElink::GbtElink(int port,GbtRInterface& gbt): mGbt(gbt), 
                                                 mPort(port),
						 mSample(0)
{ 
  mMutex.lock();
}
bool GbtElink::SerialAvailable()
{
bool res = mGbt.Fetch(mPort,mSample);
  return res;
}

uint8_t  GbtElink::GetSerial()
{
  return mGbt.Read(mPort,mSample++);
}


void GbtElink::lock()
{
  mMutex.lock();
}

void GbtElink::unlock()
{
  mMutex.unlock();
}

