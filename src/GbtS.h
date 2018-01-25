#ifndef GBTS
#define GBTS
#include <cstdint>
#include <bitset>
#include <list>
#include "GbtLink.h"
#include "Elink.h"
#include "Bits128.h"

/*!
 * \brief Emission GBT side emulation
 */


class GbtS: public gbtlink
{
  

struct GBits128
{
  Bits128    word;
  GBits128    *next;
};

public:
  GbtS();
  void PlugElink(const uint8_t socket,Elink *peer);
  void Process();
  
  bool              GbtWordAvailable();
  Bits128           GetWord();
private:
  /*! \brief Max number of Elinks ports                              */
  static const int    mMaxSocket = 40;
  /*! \brief Address of the Elink data provider connected to a port  */
  Elink               *mElinkMap[mMaxSocket];
  /*! \brief Number of active connected ports                        */
  int                 mNbRec;
  /*! \brief Sending queue head                                      */
  GBits128            *QueueH;
  /*! \brief Sending queue tail                                      */
  GBits128            *QueueT;
};
#endif
