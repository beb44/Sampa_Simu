#ifndef GBRINTERFACE
#define GBRINTERFACE
#include <cstdint>
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
};

#endif
