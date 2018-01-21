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
  /*! \brief Sending queue                                           */
  std::list<Bits128>  mSendList;
  /*! \brief Number of active connected ports                        */
  int                 mNbRec;
};
#endif
