#ifndef GBTELINK
#define GBTELINK
#include <cstdint>
#include <mutex>
#include "GbtRInterface.h"
#include "Elink.h"
/*!
 * \brief 'Virtual" Elink interface between GbtR and Receiver
 */
class GbtElink : public Elink
{
public:
  GbtElink(int port,GbtRInterface& gbt);
  bool SerialAvailable(); 
  uint8_t GetSerial(); 
  void  lock();
  void  unlock();
private:
  
  /*! \brief Reference of the GBTR the Elink belongs to              */
  GbtRInterface&     mGbt;
  
  /*! \brief Port number on the parent GBTR                          */
  int                mPort;
  
  /*! \brief Index number current bit on the Elink stream            */
  int                mSample;
  
  /*! \brief Synchronisation semaphore                               */
  std::mutex         mMutex;
};

#endif
