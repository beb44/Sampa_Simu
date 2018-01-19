#ifndef GBTS
#define GBTS
#include <cstdint>
#include <bitset>
#include <list>
#include "GbtLink.h"
#include "Elink.h"
#include "Bits128.h"

class GbtS: public gbtlink
{
public:
  GbtS();
  void PlugElink(const uint8_t socket,Elink *peer);
  void Process();
  
  bool              GbtWordAvailable();
  Bits128           GetWord();
private:
  static const int    mMaxSocket = 40;
  Elink               *mElinkMap[mMaxSocket];
  Bits128             mCurWord;
  std::list<Bits128>  mSendList;
  int                 mNbRec;
   
};
#endif
