#include "GbtElink.h"
using namespace std;

/*!
*  \brief Constructeur
*
*  \param port : port number on GbtR where the Elink is connected
*  \param gbt : reference of the GbtR where the Elink is connected
*/

GbtElink::GbtElink(int port,GbtRInterface& gbt): mGbt(gbt), 
                                                 mPort(port),
						 mSample(0)
{ 
  mMutex.lock();
}
/*!
*  \brief return the availability of data on the serial link
*
*  \return True is the data is available, false otherwise
*/

bool GbtElink::SerialAvailable()
{
bool res = mGbt.Fetch(mPort,mSample);
  return res;
}

/*!
*  \brief return next bit on Elink stream
*
*  The reciever must first check the data is available otherwise
*  the returned data may be not be consistant
*
*  \return next bit
*/

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

