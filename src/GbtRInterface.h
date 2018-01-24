#ifndef GBRINTERFACE
#define GBRINTERFACE
#include <cstdint>
#include "Bits128.h"
/*!
 *  \brief GbtR Interface class
 *
 *  This class provides interfacing method of Gbt Receiver class GbtR.\n
 */
class GbtRInterface
{
public:
   virtual bool    Fetch(int port,int sample)=0;
   virtual uint8_t Read(int port,int sample)=0;
   virtual void    Push(Bits128 word)=0;
};

#endif
