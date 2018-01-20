#include <iostream>
#include <cstdint>
#include "GbtR.h"  
using namespace std;

GbtR::GbtR(gbtlink &provider) :mDataProvider(provider)
{
  mElinkMap.clear();
  mStarted = false;
  mCurSample = -1;
}

Elink &GbtR::GetElink(int const port)
{
  if (mStarted) throw;
  if (mElinkMap.find(port) != mElinkMap.end()) throw;
  mElinkMap[port] = new GbtElink(port,*this);
  mNbLinks++;
  mNbSampleReaders = mNbLinks;
  return *mElinkMap[port];
}

bool GbtR::Fetch(int const port,int const sample) 
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

uint8_t GbtR::Read(int const port,int const sample)
{
  return mCurWord.Get(port*2+(1-(sample & 1)));
}
