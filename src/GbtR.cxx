#include <iostream>
#include <cstdint>
#include "GbtR.h"  
using namespace std;

/*!
*  \brief Constructeur
*
*  \param provider : reference of the 'remote' GbtS
*/

GbtR::GbtR(gbtlink &provider) :mDataProvider(provider),
			       mCurSample(-1),
			       mNbLinks(0),
			       mNbSampleReaders(0), 
			       mCurWord(Bits128()),
			       mDataAvailable(false)
{
  mElinkMap.clear();
}

/*!
 *  \brief Allocates a new virtual Elink port on the GbtR
 *
 *  \return the allocated Elink 
 */

Elink &GbtR::GetElink(int const port)
{
  //
  // Check ports availability
  //
  if (mElinkMap.find(port) != mElinkMap.end()) throw;
  //
  // add the new ports
  //
  mElinkMap[port] = new GbtElink(port,*this);
  //
  // update internal variables
  mNbLinks++;
  mNbSampleReaders = mNbLinks;
  //
  // return allocated Elink
  //
  return *mElinkMap[port];
}

/*!
 *  \brief Get a new Gbt 128 bit word from the remote GBT
 *
 *  This routine must be called by a receiver to get sure that
 *  a word is available. \n
 *  All receivers will be suspended as long as some receiver is
 *  no synchronised (i.e. fetching the same GBT word).
 *
 *  \return true if a new word is available, false otherwise 
 */

bool GbtR::Fetch(int const port,int const sample) 
{
  
  mMutex.lock();
  if ((sample/2) == mCurSample) {
    //
    // a Gbt word carries two bits of information
    // in this case, the current sample is already available
    // and has been 'fetch' along the preceeding one.
    //
    mMutex.unlock();
    return true;
  }
  //
  // At the stage, the GBT word has not been read yet, check if all
  // readers are synchronised before fetching it
  //
  
  mNbSampleReaders--;
  if (mNbSampleReaders != 0) {
    // 
    // some readers are out of synchronisation, let' wait for them
    // suspend task
    //
    mMutex.unlock();
    mElinkMap[port]->lock() ;
    //
    // returns data availability when all readers are synchronised
    //
    return mDataAvailable;
  }
  else {
    //
    // The last readers is now synchronised, let's get the GBT word
    // from the distant party and release all pending readers
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
    // free all waiting thread (but ourself)
    //
    for (int i=0; i< mNbLinks;i++) { 
      if (i != port) 
        mElinkMap[i]->unlock() ; 
    }
    mMutex.unlock(); // end on critical section
    return mDataAvailable;
  }
}
/*!
 *  \brief Read a bit on a given port
 *
 *  Return the next bit of the GBT stream. \n
 *  Receiver is assumed to have check the availibilty of the
 *  date through the Fetch method, otherwise returned information
 *  validity is not guarantied.
 *
 *  \return bit information 
 */

uint8_t GbtR::Read(int const port,int const sample)
{
  return mCurWord.Get(port*2+(1-(sample & 1)));
}
