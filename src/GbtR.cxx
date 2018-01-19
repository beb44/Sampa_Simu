#include <iostream>
#include <cstdint>
#include <sys/types.h>
#include <sys/sem.h>
#include "GbtR.h"  
using namespace std;

GbtElink::GbtElink(int port,GbtGen& gbt): mGbt(gbt), mPort(port)
{ 
  mSample = 0;
  sem_init(&mWaitSem, 0, 0);
}
bool GbtElink::SerialAvailable()
{
bool res = mGbt.fetch(mPort,mSample);
  return res;
}

uint8_t  GbtElink::GetSerial()
{
  return mGbt.read(mPort,mSample++);
}


void GbtElink::lock()
{
  sem_wait(&mWaitSem);
}

void GbtElink::unlock()
{
  sem_post(&mWaitSem);
}


gbt_r::gbt_r(gbtlink &provider) :mDataProvider(provider)
{
  mElinkMap.clear();
  mStarted = false;
  mCurSample = -1;
}

Elink &gbt_r::get_elink(int const port)
{
  if (mStarted) throw;
  if (mElinkMap.find(port) != mElinkMap.end()) throw;
  mElinkMap[port] = new GbtElink(port,*this);
  mNbLinks++;
  mNbSampleReaders = mNbLinks;
  return *mElinkMap[port];
}

bool gbt_r::fetch(int const port,int const sample) 
{
  mMutex.lock();
  if ((sample/2) == mCurSample) {
    mMutex.unlock();
    return true;
  }
  
  mNbSampleReaders--;
  if (mNbSampleReaders != 0) {
    // suspend task
    mMutex.unlock();
    mElinkMap[port]->lock() ;
    return mDataAvailable;
  }
  else {
    //
    // fecth a new word
    //
    mDataAvailable= mDataProvider.GbtWordAvailable();
    if (mDataAvailable) {
      mCurWord = mDataProvider.GetWord();
      mCurSample++;
      mNbSampleReaders = mNbLinks;
    }
    //
    // free all waiting thread
    //
    for (int i=0; i< mNbLinks;i++) { 
      if (i != port) 
        mElinkMap[i]->unlock() ; 
    }
    mMutex.unlock(); // end on critical section
    return mDataAvailable;
  }
}

uint8_t gbt_r::read(int const port,int const sample)
{
  return mCurWord.Get(port*2+(sample & 1));
}
